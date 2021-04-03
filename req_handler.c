//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include <uv.h>
#include "dns_server.h"
#include "req_handler.h"
#include "dns_structure.h"
#include "dns_convesion.h"

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    buf->len = DNS_STRING_MAX_SIZE;
}

static void on_send(uv_udp_send_t *handle, int status)
{
    printf("Send successful!");
    free(handle);
}

static void on_read(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{
    printf("Received!");
    uv_udp_send_t *req = malloc(sizeof(uv_udp_send_t));
    uv_udp_recv_stop(handle);
    uv_buf_t *res_buf = redirect(buf);
    uv_buf_t send_buf = uv_buf_init(res_buf->base, res_buf->len);
    Dns_Msg *msg = (Dns_Msg *)calloc(1, sizeof(Dns_Msg));
    string_to_dnsmsg(msg, res_buf->base);
    print_dns_string(res_buf->base, res_buf->len);
    print_dns_message(msg);
    uv_udp_send(req, handle, &send_buf, 1, addr, on_send);
    free(buf->base);
}

void init_handler()
{
    uv_udp_init(loop, &recv_socket);
    struct sockaddr_in recv_addr;
    uv_ip4_addr("0.0.0.0", 53, &recv_addr);
    uv_udp_bind(&recv_socket, (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);
}
