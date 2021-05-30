/**
 * @file      dns_client.h
 * @brief     DNS客户端
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS客户端的接口，包括服务器初始化函数、向远程发送查询的函数。
 *
 * DNS客户端通过client_socket与远程服务器的53端口通信，收到DNS回复后在查询池中结束对应的查询。
 *
 * 查询池通过调用接口将向远程服务器发送请求。
*/

#ifndef DNSR_DNS_CLIENT_H
#define DNSR_DNS_CLIENT_H

#include <uv.h>

#include "dns_structure.h"

/**
 * @brief 客户端初始化
 *
 * @param loop 事件循环
 */
void init_client(uv_loop_t * loop);

/**
 * @brief 将DNS请求报文发送至远程
 *
 * @param msg DNS请求报文
 */
void send_to_remote(const Dns_Msg * msg);

#endif //DNSR_DNS_CLIENT_H
