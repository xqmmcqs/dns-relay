/**
 * @file      dns_conversion.h
 * @brief     DNS格式转换
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS报文数据结构与字节序列之间的转换，以及释放报文空间和复制报文的函数。
*/

#ifndef DNSR_DNS_CONVERSION_H
#define DNSR_DNS_CONVERSION_H

#include "dns_structure.h"

void string_to_dnsmsg(Dns_Msg * pmsg, const char * pstring);

unsigned int dnsmsg_to_string(const Dns_Msg * pmsg, char * pstring);

void destroy_dnsrr(Dns_RR * prr);

void destroy_dnsmsg(Dns_Msg * pmsg);

Dns_RR * copy_dnsrr(const Dns_RR * src);

Dns_Msg * copy_dnsmsg(const Dns_Msg * src);

#endif //DNSR_DNS_CONVERSION_H
