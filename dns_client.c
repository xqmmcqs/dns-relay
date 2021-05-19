//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include "dns_conversion.h"
#include "dns_print.h"
#include "dns_client.h"
#include "query_pool.h"
#include "util.h"

static uv_udp_t client_socket;
struct sockaddr_in local_addr;
struct sockaddr send_addr;
extern uv_loop_t * loop;
extern Query_Pool * qpool;

static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
{
    buf->base = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    buf->len = DNS_STRING_MAX_SIZE;
}

static void
on_read(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
{
    if (nread <= 0)
    {
        free(buf->base);
        return;
    }
    log_info("从服务器接收到消息");
//    print_dns_string(buf->base, nread);
    Dns_Msg * msg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
    string_to_dnsmsg(msg, buf->base);
    print_dns_message(msg);
    qpool_finish(qpool, msg);
    destroy_dnsmsg(msg);
    free(buf->base);
}

static void on_send(uv_udp_send_t * req, int status)
{
    free(req);
    // TODO: status异常处理
}

void init_client()
{
    log_info("启动client")
    uv_udp_init(loop, &client_socket);
    uv_ip4_addr("0.0.0.0", 0, &local_addr);
    uv_udp_bind(&client_socket, (const struct sockaddr *) &local_addr, UV_UDP_REUSEADDR);
    uv_udp_set_broadcast(&client_socket, 1);
    uv_ip4_addr("10.3.9.4", 53, (struct sockaddr_in *) &send_addr);
    uv_udp_recv_start(&client_socket, alloc_buffer, on_read);
}

void send_to_remote(const Dns_Msg * msg)
{
    char * str = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    unsigned int len = dnsmsg_to_string(msg, str);
    
    uv_udp_send_t * req = malloc(sizeof(uv_udp_send_t));
    uv_buf_t send_buf = uv_buf_init((char *) malloc(len), len);
    memcpy(send_buf.base, str, len);
    log_info("向服务器发送消息");

//    Dns_Msg * chkmsg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
//    print_dns_string(send_buf.base,len);
//    string_to_dnsmsg(chkmsg, send_buf.base);
//    log_debug("Now printing msg");
    print_dns_message(msg);
//    log_debug("Now printing chkmsg");
//    print_dns_message(chkmsg);
//    destroy_dnsmsg(chkmsg);
    
    uv_udp_send(req, &client_socket, &send_buf, 1, &send_addr, on_send);
    free(str);
}