//
// Created by xqmmcqs on 2021/4/3.
//

#ifndef DNSR_DNS_STRUCTURE_H
#define DNSR_DNS_STRUCTURE_H

#include <stdint.h>

#define DNS_STRING_MAX_SIZE 5120
#define DNS_RR_NAME_MAX_SIZE 256

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
#define DNS_TYPE_MX 15
#define DNS_TYPE_AAAA 28

typedef struct
{
    uint16_t id;
    uint8_t qr: 1;
    uint8_t opcode: 4;
    uint8_t aa: 1;
    uint8_t tc: 1;
    uint8_t rd: 1;
    uint8_t ra: 1;
    uint8_t z : 3;
    uint8_t rcode : 4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}Dns_Header;

typedef struct dns_question
{
    uint8_t *qname;
    uint16_t qtype;
    uint16_t qclass;
    struct dns_question * next;
}Dns_Que;

typedef struct dns_rr
{
    uint8_t *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t *rdata;
    struct dns_rr * next;
}Dns_RR;

typedef struct
{
    Dns_Header * header;
    Dns_Que * que;
    Dns_RR * rr;
}Dns_Msg;

#endif //DNSR_DNS_STRUCTURE_H
