//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include "dns_structure.h"
#include "dns_convesion.h"
#include "dns_server.h"
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
//    Dns_Msg *msg = (Dns_Msg *)calloc(1, sizeof(Dns_Msg));
//    string_to_dnsmsg(msg, buf->base);
//    print_dns_message(msg);
//    destroy_dnsmsg(msg);
    uint16_t id = ntohs(*(uint16_t *)buf->base);
    if (!upool_query(upool, id))
    {
        return;
    }
    Udp_Req *ureq = upool_delete(upool, id);
    *(uint16_t *)buf->base = htons(ureq->prev_id);
    uv_buf_t send_buf = uv_buf_init((char *)malloc(buf->len), nread);
    memcpy(send_buf.base, buf->base, nread);
    send_to_local(&ureq->addr, &send_buf);
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

void send_to_remote(const struct sockaddr *addr, const uv_buf_t * buf)
{
    if(upool_full(upool))
    {
        print_log(FATAL, "UDP pool full");
        return;
    }
    Udp_Req * ureq = (Udp_Req *)calloc(1, sizeof(Udp_Req));
    ureq->addr = *addr;
    ureq->id = upool_insert(upool, ureq);
    ureq->prev_id = ntohs(*(uint16_t *)buf->base);
    uv_udp_send_t *req = malloc(sizeof(uv_udp_send_t));
    *(uint16_t *)buf->base = htons(ureq->id);
    uv_buf_t send_buf = uv_buf_init((char *)malloc(buf->len), buf->len);
    memcpy(send_buf.base, buf->base, buf->len);
    print_log(DEBUG, "Sending request to remote");
//    Dns_Msg *msg = (Dns_Msg *)calloc(1, sizeof(Dns_Msg));
//    string_to_dnsmsg(msg, buf->base);
//    print_dns_message(msg);
//    destroy_dnsmsg(msg);
    uv_udp_send(req, &client_socket, &send_buf, 1, &send_addr, on_send);
    free(buf->base);
}