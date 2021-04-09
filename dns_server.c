//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include "dns_client.h"
#include "dns_server.h"
#include "dns_convesion.h"
#include "dns_print.h"
#include "util.h"

static uv_udp_t server_socket;
struct sockaddr_in recv_addr;

static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
{
    buf->base = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    buf->len = DNS_STRING_MAX_SIZE;
}

static void on_send(uv_udp_send_t * req, int status)
{
    print_log(DEBUG, "Sent to local, status %d", status);
    free(req);
    // TODO: status异常处理
}

static void
on_read(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
{
    // TODO : query池
    if (nread <= 0)
    {
        free(buf->base);
        return;
    }
    print_log(DEBUG, "Received request from local");
    uv_buf_t send_buf = uv_buf_init((char *) malloc(buf->len), nread);
    memcpy(send_buf.base, buf->base, nread);
    Dns_Msg * msg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
    string_to_dnsmsg(msg, buf->base);
//    print_dns_message(msg);
//    print_dns_string(send_buf.base, nread);
    send_to_remote(addr, msg);
    free(buf->base);
}

void send_to_local(const struct sockaddr * addr, Dns_Msg * msg)
{
    // TODO : msg to string
    print_log(DEBUG, "Sending response to local");
//    print_log(DEBUG, "Now printing msg");
//    print_dns_message(msg);
    char * str = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    unsigned int len = dnsmsg_to_string(msg, str);
    uv_udp_send_t * req = malloc(sizeof(uv_udp_send_t));
    uv_buf_t send_buf = uv_buf_init((char *) malloc(len), len);
    memcpy(send_buf.base, str, len);

//    Dns_Msg * chkmsg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
//    print_dns_string(send_buf.base,len);
//    string_to_dnsmsg(chkmsg, send_buf.base);
//    print_log(DEBUG, "Now printing chkmsg");
//    print_dns_message(chkmsg);
//    destroy_dnsmsg(chkmsg);
    
    uv_udp_send(req, &server_socket, &send_buf, 1, addr, on_send);
    free(str);
    destroy_dnsmsg(msg);
}

void init_server()
{
    print_log(DEBUG, "Starting server");
    uv_udp_init(loop, &server_socket);
    uv_ip4_addr("0.0.0.0", 53, &recv_addr);
    uv_udp_bind(&server_socket, (struct sockaddr *) &recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&server_socket, alloc_buffer, on_read);
}
