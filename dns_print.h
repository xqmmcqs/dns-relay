/**
 * @file      dns_print.h
 * @brief     DNS报文打印
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS报文的接口，包括打印原始字符序列和DNS
 *
 * DNS服务器通过server_socket与本地53端口通信，将收到的DNS查询报文加入查询池
 *
 * 当一个DNS查询的计时器超时的时候，调用超时回调函数销毁这个查询请求
*/


#ifndef DNSR_DNS_PRINT_H
#define DNSR_DNS_PRINT_H

#include "dns_structure.h"

void print_dns_string(const char * pstring, unsigned int len);

void print_dns_message(const Dns_Msg * pmsg);

#endif //DNSR_DNS_PRINT_H
