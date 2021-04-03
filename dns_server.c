//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdlib.h>
#include "dns_structure.h"
#include "dns_server.h"

uv_udp_t send_socket;
struct sockaddr_in addr, send_addr;
char recv_packet[DNS_STRING_MAX_SIZE];
int recv_packet_size;
uv_buf_t res;

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    buf->len = DNS_STRING_MAX_SIZE;
}

static void close_cb(uv_handle_t *handle)
{
    uv_is_closing(handle);
}

static void on_read(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{
//    FILE * temp = fopen("./out.txt", "w");
//    for(int i=0;i<100;++i)
//        fprintf(temp, "%c", *(rcvbuf->base+i));
//    fclose(temp);
    recv_packet_size = nread;
    memcpy(recv_packet, buf->base, recv_packet_size);
    uv_close((uv_handle_t *)handle, close_cb);
    free(buf->base);
}

static void on_send(uv_udp_send_t *req, int status)
{
    printf("call back");
    uv_udp_recv_start(req->handle, alloc_buffer, on_read);
}

void run_server(const uv_buf_t *buf)
{
    uv_udp_init(server_loop, &send_socket);
    uv_ip4_addr("0.0.0.0", 0, &addr);
    uv_udp_bind(&send_socket, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
    uv_udp_set_broadcast(&send_socket, 1);
    uv_ip4_addr("10.3.9.4", 53, &send_addr);
    uv_buf_t cl_buf = uv_buf_init(buf->base, buf->len);
    uv_udp_send_t req;
    uv_udp_send(&req, &send_socket, &cl_buf, 1, (const struct sockaddr*)&send_addr, on_send);
    uv_run(server_loop, UV_RUN_DEFAULT);
}

uv_buf_t *redirect(const uv_buf_t *buf)
{
    uv_thread_t thread_id;
    uv_thread_create(&thread_id, run_server, buf);
    uv_sleep(2000);

    res = uv_buf_init(recv_packet,recv_packet_size);
    printf("response!");
    return &res;
}