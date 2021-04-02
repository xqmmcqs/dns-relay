#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <uv.h>

typedef struct dns_header
{
    uint16_t id;
    uint8_t qr;
    uint8_t opcode;
    uint8_t aa;
    uint8_t tc;
    uint8_t rd;
    uint8_t ra;
    uint16_t rcode;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}DNS_Header;

uv_loop_t *client_loop;
uv_udp_t send_socket;
struct sockaddr_in addr, send_addr;

static void alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    static char slab[6000];
    buf->base = slab;
    buf->len = sizeof(slab);
}

static void close_cb(uv_handle_t *handle)
{
    uv_is_closing(handle);
}

static void cl_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *rcvbuf, const struct sockaddr *addr, unsigned flags)
{
    if (nread <= 0)
    {
        return;
    }
    FILE * temp = fopen("./out.txt", "w");
    uv_close((uv_handle_t *)handle, close_cb);
    for(int i=0;i<100;++i)
        fprintf(temp, "%c", *(rcvbuf->base+i));
    fclose(temp);
}

void cl_send_cb(uv_udp_send_t *req, int status)
{
    printf("call back");
    uv_udp_recv_start(req->handle, alloc_cb, cl_recv_cb);
}

int main()
{
    client_loop = uv_default_loop();
    uv_udp_init(client_loop, &send_socket);
    uv_ip4_addr("0.0.0.0", 0, &addr);
    uv_udp_bind(&send_socket, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
    uv_udp_set_broadcast(&send_socket, 1);
    uv_ip4_addr("10.3.9.4", 53, &send_addr);
    uv_udp_send_t send_req;
    
    uv_buf_t cl_buf = uv_buf_init("\x4b\xcc\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x03\x77\x77\x77\x04\x62\x75\x70\x74\x03\x65\x64\x75\x02\x63\x6e\x00\x00\x01\x00\x01", 33);
    uv_udp_send(&send_req, &send_socket, &cl_buf, 1, (const struct sockaddr*)&send_addr, cl_send_cb);
    
    uv_run(client_loop, UV_RUN_DEFAULT);
    return 0;
}
