/**
 * @file      dns_server.h
 * @brief     DNS服务器
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS服务器的接口，包括服务器初始化函数、向本地发送回复的函数和DNS查询超时的回调函数
 *
 * DNS服务器通过server_socket与本地53端口通信，将收到的DNS查询报文加入查询池
 *
 * 当一个DNS查询的计时器超时的时候，调用超时回调函数销毁这个查询请求
*/

#ifndef DNSR_DNS_SERVER_H
#define DNSR_DNS_SERVER_H

#include <uv.h>

#include "dns_structure.h"

/**
 * @brief 服务器初始化
 */
void init_server(uv_loop_t * loop);

/**
 * @brief 将DNS回复报文发送至本地
 * @param addr 本地地址
 * @param msg DNS回复报文
 */
void send_to_local(const struct sockaddr * addr, const Dns_Msg * msg);

#endif //DNSR_DNS_SERVER_H
