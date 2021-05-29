/**
 * @file      dns_print.h
 * @brief     DNS报文打印
 * @author    Ziheng Mao
 * @date      2021/4/6
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了打印DNS报文的接口，包括打印字节流和报文结构体。
 *
 * 如果设置了不打印DEBUG信息，则打印功能不会生效。
*/

#ifndef DNSR_DNS_PRINT_H
#define DNSR_DNS_PRINT_H

#include "dns_structure.h"

/**
 * @brief 打印DNS报文字节流
 *
 * @param pstring DNS报文字节流
 * @param len 字节流长度
 */
void print_dns_string(const char * pstring, unsigned int len);

/**
 * @brief 打印DNS报文结构体
 *
 * @param pmsg DNS报文结构体
 */
void print_dns_message(const Dns_Msg * pmsg);

#endif //DNSR_DNS_PRINT_H
