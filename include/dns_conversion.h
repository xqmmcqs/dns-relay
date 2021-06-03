/**
 * @file      dns_conversion.h
 * @brief     DNS报文格式转换
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS报文结构体与字节流之间的转换、释放报文结构体空间和复制报文结构体的接口。
*/

#ifndef DNSR_DNS_CONVERSION_H
#define DNSR_DNS_CONVERSION_H

#include "dns_structure.h"

/**
 * @brief DNS报文字节流转换到结构体
 *
 * @param pmsg DNS报文结构体
 * @param pstring DNS报文字节流
 * @note 为Header Section、Question Section和Resource Record分配了空间
 */
void string_to_dnsmsg(Dns_Msg * pmsg, const char * pstring);

/**
 * @brief DNS报文结构体转换到字节流
 *
 * @param pmsg DNS报文结构体
 * @param pstring DNS报文字节流
 * @return 报文字节流的长度
 */
unsigned dnsmsg_to_string(const Dns_Msg * pmsg, char * pstring);

/**
 * @brief 释放DNS报文RR结构体的空间
 *
 * @param prr DNS报文RR结构体
 */
void destroy_dnsrr(Dns_RR * prr);

/**
 * @brief 释放DNS报文结构体的空间
 *
 * @param pmsg DNS报文结构体
 */
void destroy_dnsmsg(Dns_Msg * pmsg);

/**
 * @brief 复制DNS报文RR结构体
 *
 * @param src 源RR结构体
 * @return 复制后的RR结构体
 * @note 为新的RR结构体分配了空间
 */
Dns_RR * copy_dnsrr(const Dns_RR * src);

/**
 * @brief 复制DNS报文结构体
 *
 * @param src 源结构体
 * @return 复制后的结构体
 * @note 为新的结构体分配了空间
 */
Dns_Msg * copy_dnsmsg(const Dns_Msg * src);

#endif //DNSR_DNS_CONVERSION_H
