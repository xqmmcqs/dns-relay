/**
 * @file      dns_server.c
 * @brief     DNS服务器
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是DNS服务器的实现
 *
 * 文件中实现了初始化函数和与本地通信的函数，包括接收DNS查询报文和发送DNS回复报文的函数
 *
 * 另外文件中包含数个工具函数，如分配缓冲区和发送回调函数
*/

#include "../include/dns_server.h"

#include <stdlib.h>

#include "../include/util.h"
#include "../include/dns_conversion.h"
#include "../include/dns_print.h"
#include "../include/query_pool.h"

static uv_udp_t server_socket; ///< 服务器与本地通信的socket
static struct sockaddr_in recv_addr; ///< 服务器收取DNS查询报文的地址
extern Query_Pool * qpool; ///< 查询池，定义在main.c中

/**
 * @brief 为缓冲区分配空间
 * @param handle 分配句柄
 * @param suggested_size 期望缓冲区大小
 * @param buf 缓冲区
 *
 * 分配大小固定为DNS_STRING_MAX_SIZE的缓冲区，用于从本地接收DNS查询报文
 */
static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
{
    buf->base = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char));
    if (!buf->base)
    {
        log_fatal("内存分配错误");
        exit(1);
    }
    buf->len = DNS_STRING_MAX_SIZE;
}

/**
 * @brief 向本地发送回复报文的回调函数
 * @param req 发送句柄
 * @param status 发送状态
 */
static void on_send(uv_udp_send_t * req, int status)
{
    log_debug("向本地发送回复，状态 %d", status);
    free(req);
    // TODO: status异常处理
}

/**
 * @brief 从本地接收查询报文的回调函数
 * @param handle 查询句柄
 * @param nread 收到报文的字节数
 * @param buf 缓冲区，存放收到的报文
 * @param addr 本地发送方的地址
 * @param flags 标志
 */
static void
on_read(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
{
    if (nread <= 0) // 收到无效报文
    {
        free(buf->base);
        return;
    }
    
    log_debug("收到本地DNS查询报文");
//    uv_buf_t send_buf = uv_buf_init((char *) malloc(buf->len), nread);
//    memcpy(send_buf.base, buf->base, nread);
    Dns_Msg * msg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
    if (!msg)
    {
        log_fatal("内存分配错误");
        exit(1);
    }
    string_to_dnsmsg(msg, buf->base); // 将字节序列转化为结构体
    print_dns_message(msg);
//    print_dns_string(send_buf.base, nread);
    
    qpool->insert(qpool, addr, msg); // 将DNS查询加入查询池
    destroy_dnsmsg(msg);
    free(buf->base);
}

void init_server(uv_loop_t * loop)
{
    log_info("启动server");
    uv_udp_init(loop, &server_socket); // 将server_docket绑定到事件循环
    uv_ip4_addr("0.0.0.0", 53, &recv_addr); // 初始化recv_addr为0.0.0.0:53
    uv_udp_bind(&server_socket, (struct sockaddr *) &recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&server_socket, alloc_buffer, on_read); // 当收到DNS查询报文时，分配缓冲区并调用回调函数
}

void send_to_local(const struct sockaddr * addr, const Dns_Msg * msg)
{
    log_info("发送DNS回复报文到本地");
    print_dns_message(msg);
    char * str = (char *) calloc(DNS_STRING_MAX_SIZE, sizeof(char)); // 将DNS结构体转化成字节序列
    if (!str)
    {
        log_fatal("内存分配错误");
        exit(1);
    }
    unsigned int len = dnsmsg_to_string(msg, str);
    uv_udp_send_t * req = malloc(sizeof(uv_udp_send_t));
    if (!req)
    {
        log_fatal("内存分配错误");
        exit(1);
    }
    
    uv_buf_t send_buf = uv_buf_init((char *) malloc(len), len);
    memcpy(send_buf.base, str, len); // 将字节序列存入发送缓冲区中
//    Dns_Msg * chkmsg = (Dns_Msg *) calloc(1, sizeof(Dns_Msg));
//    print_dns_string(send_buf.base,len);
//    string_to_dnsmsg(chkmsg, send_buf.base);
//    log_debug("Now printing chkmsg");
//    print_dns_message(chkmsg);
//    destroy_dnsmsg(chkmsg);
    
    uv_udp_send(req, &server_socket, &send_buf, 1, addr, on_send);
    free(str);
}
