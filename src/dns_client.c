/**
 * @file      dns_client.c
 * @brief     DNS客户端
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是DNS客户端的实现。
*/

#include "../include/dns_client.h"

#include <stdlib.h>

#include "../include/util.h"
#include "../include/dns_conversion.h"
#include "../include/dns_print.h"
#include "../include/query_pool.h"

static uv_udp_t client_socket; ///< 客户端与远程通信的socket
static struct sockaddr_in local_addr; ///< 本地地址
static struct sockaddr send_addr; ///< 远程服务器地址
extern Query_Pool * qpool; ///< 查询池

/**
 * @brief 为缓冲区分配空间
 *
 * @param handle 分配句柄
 * @param suggested_size 期望缓冲区大小
 * @param buf 缓冲区
 *
 * 分配大小固定为DNS_STRING_MAX_SIZE的缓冲区，用于从远程接收DNS回复报文
 */
static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
{
    buf->base = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    if (!buf->base)
        log_fatal("内存分配错误")
    buf->len = DNS_STRING_MAX_SIZE;
}

/**
 * @brief 从远程接收回复报文的回调函数
 *
 * @param handle 查询句柄
 * @param nread 收到报文的字节数
 * @param buf 缓冲区，存放收到的报文
 * @param addr 发送方的地址
 * @param flags 标志
 */
static void
on_read(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
{
    if (nread < 0)
    {
        if (buf->base)
            free(buf->base);
        log_debug("传输错误")
        return;
    }
    if (nread == 0)
    {
        if (buf->base)
            free(buf->base);
        return;
    }
    log_info("从服务器接收到消息")
    print_dns_string(buf->base, nread);
    Dns_Msg * msg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
    if (!msg)
        log_fatal("内存分配错误")
    string_to_dnsmsg(msg, buf->base);
    print_dns_message(msg);
    qpool->finish(qpool, msg);
    destroy_dnsmsg(msg);
    if (buf->base)
        free(buf->base);
}

/**
 * @brief 向远程发送查询报文的回调函数
 *
 * @param req 发送句柄
 * @param status 发送状态
 */
static void on_send(uv_udp_send_t * req, int status)
{
    free(*(char **) req->data);
    free(req->data);
    free(req);
    if (status)
        log_error("发送状态异常 %d", status)
}

void init_client(uv_loop_t * loop)
{
    log_info("启动client")
    uv_udp_init(loop, &client_socket);
    uv_ip4_addr("0.0.0.0", CLIENT_PORT, &local_addr);
    uv_udp_bind(&client_socket, (const struct sockaddr *) &local_addr, UV_UDP_REUSEADDR);
    uv_udp_set_broadcast(&client_socket, 1);
    uv_ip4_addr(REMOTE_HOST, 53, (struct sockaddr_in *) &send_addr);
    uv_udp_recv_start(&client_socket, alloc_buffer, on_read);
}

void send_to_remote(const Dns_Msg * msg)
{
    char * str = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    if (!str)
        log_fatal("内存分配错误")
    unsigned int len = dnsmsg_to_string(msg, str);
    
    uv_udp_send_t * req = malloc(sizeof(uv_udp_send_t));
    if (!req)
        log_fatal("内存分配错误")
    uv_buf_t send_buf = uv_buf_init((char *) malloc(len), len);
    memcpy(send_buf.base, str, len);
    req->data = (char **) malloc(sizeof(char **));
    *(char **) (req->data) = send_buf.base;
    
    log_info("向服务器发送消息")
    print_dns_message(msg);
    print_dns_string(send_buf.base, len);
    uv_udp_send(req, &client_socket, &send_buf, 1, &send_addr, on_send);
    free(str);
}