/**
 * @file      dns_structure.h
 * @brief     DNS报文结构
 * @author    Ziheng Mao
 * @date      2021/4/3
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了DNS报文中部分字段的常量，以及DNS报文结构体。
 *
 * 报文结构体中每个字段长度均与RFC1035中规定的字段长度一致，但是采用小端法存储。
*/

#ifndef DNSR_DNS_STRUCTURE_H
#define DNSR_DNS_STRUCTURE_H

#include <stdint.h>

#define DNS_STRING_MAX_SIZE 8192
#define DNS_RR_NAME_MAX_SIZE 512

#define DNS_QR_QUERY 0
#define DNS_QR_ANSWER 1

#define DNS_OPCODE_QUERY 0
#define DNS_OPCODE_IQUERY 1
#define DNS_OPCODE_STATUS 2

#define DNS_TYPE_A 1
#define DNS_TYPE_NS 2
#define DNS_TYPE_CNAME 5
#define DNS_TYPE_SOA 6
#define DNS_TYPE_PTR 12
#define DNS_TYPE_HINFO 13
#define DNS_TYPE_MINFO 15
#define DNS_TYPE_MX 15
#define DNS_TYPE_TXT 16
#define DNS_TYPE_AAAA 28

#define DNS_CLASS_IN 1

#define DNS_RCODE_OK 0
#define DNS_RCODE_NXDOMAIN 3

/// 报文Header Section结构体
typedef struct dns_header
{
    uint16_t id;
    uint8_t qr: 1;
    uint8_t opcode: 4;
    uint8_t aa: 1;
    uint8_t tc: 1;
    uint8_t rd: 1;
    uint8_t ra: 1;
    uint8_t z: 3;
    uint8_t rcode: 4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} Dns_Header;

/// 报文Question Section结构体，以链表表示
typedef struct dns_question
{
    uint8_t * qname;
    uint16_t qtype;
    uint16_t qclass;
    struct dns_question * next;
} Dns_Que;

/// 报文Resource Record结构体，以链表表示
typedef struct dns_rr
{
    uint8_t * name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t * rdata;
    struct dns_rr * next;
} Dns_RR;

/// DNS报文结构体
typedef struct dns_msg
{
    Dns_Header * header; ///< 指向Header Section
    Dns_Que * que; ///< 指向Question Section链表的头节点
    Dns_RR * rr; ///< 指向Resource Record链表的头节点
} Dns_Msg;

#endif //DNSR_DNS_STRUCTURE_H
