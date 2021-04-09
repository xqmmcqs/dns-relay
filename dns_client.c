//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include "dns_convesion.h"
#include "dns_server.h"
#include "dns_print.h"
#include "dns_client.h"
#include "udp_pool.h"
#include "util.h"

static uv_udp_t client_socket;
struct sockaddr_in local_addr;
struct sockaddr send_addr;
Udp_Pool * upool;

static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
{
    buf->base = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    buf->len = DNS_STRING_MAX_SIZE;
}

static void on_read(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
{
    if(nread<=0)
    {
        free(buf->base);
        return;
    }
    print_log(DEBUG, "Received response from remote");
    uint16_t id = ntohs(*(uint16_t *) buf->base);
    if (!upool_query(upool, id))
    {
        return;
    }
    Udp_Req * ureq = upool_delete(upool, id);
    *(uint16_t *) buf->base = htons(ureq->prev_id);
//    print_dns_string(buf->base, nread);
    Dns_Msg * msg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
    string_to_dnsmsg(msg, buf->base);
//    print_dns_string(buf->base, nread);
//    print_dns_message(msg);
    send_to_local(&ureq->addr, msg);
    free(buf->base);
}

static void on_send(uv_udp_send_t * req, int status)
{
    free(req);
    // TODO: status异常处理
}

void init_client()
{
    print_log(DEBUG, "Starting client")
    uv_udp_init(loop, &client_socket);
    uv_ip4_addr("0.0.0.0", 0, &local_addr);
    uv_udp_bind(&client_socket, (const struct sockaddr *) &local_addr, UV_UDP_REUSEADDR);
    uv_udp_set_broadcast(&client_socket, 1);
    uv_ip4_addr("10.3.9.4", 53, (struct sockaddr_in *)&send_addr);
    uv_udp_recv_start(&client_socket, alloc_buffer, on_read);
    upool = upool_init();
}

void send_to_remote(const struct sockaddr * addr, Dns_Msg * msg)
{
    if (upool_full(upool))
    {
        print_log(FATAL, "UDP pool full");
        return;
    }
    
    char * str = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    unsigned int len = dnsmsg_to_string(msg, str);
    
    Udp_Req * ureq = (Udp_Req *) calloc(1, sizeof(Udp_Req));
    ureq->addr = *addr;
    ureq->id = upool_insert(upool, ureq);
    ureq->prev_id = ntohs(*(uint16_t *) str);
    
    uv_udp_send_t * req = malloc(sizeof(uv_udp_send_t));
    uv_buf_t send_buf = uv_buf_init((char *) malloc(len), len);
    memcpy(send_buf.base, str, len);
    *(uint16_t *) send_buf.base = htons(ureq->id);
    print_log(DEBUG, "Sending request to remote");

//    Dns_Msg * chkmsg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
//    print_dns_string(send_buf.base,len);
//    string_to_dnsmsg(chkmsg, send_buf.base);
//    print_log(DEBUG, "Now printing msg");
//    print_dns_message(msg);
//    print_log(DEBUG, "Now printing chkmsg");
//    print_dns_message(chkmsg);
//    destroy_dnsmsg(chkmsg);
    
    uv_udp_send(req, &client_socket, &send_buf, 1, &send_addr, on_send);
    free(str);
    destroy_dnsmsg(msg);
}