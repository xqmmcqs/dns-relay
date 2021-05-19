//
// Created by xqmmcqs on 2021/4/6.
//

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "dns_print.h"

void print_dns_string(const char * pstring, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        if (i % 16 == 0)
            printf("\n%04x ", i);
        printf("%02hhx ", pstring[i]);
    }
    printf("\n");
}

static void print_rr_A(const uint8_t * pdata)
{
    printf("%d.%d.%d.%d", pdata[0], pdata[1], pdata[2], pdata[3]);
}

static void print_rr_CNAME(const uint8_t * pdata)
{
    printf("%s", pdata);
}

static void print_rr_AAAA(const uint8_t * pdata)
{
    for (int i = 0; i < 16; i += 2)
    {
        if (i)printf(":");
        printf("%x", (pdata[i] << 8) + pdata[i + 1]);
    }
}

static void print_dns_header(const Dns_Header * phead)
{
    printf("ID = 0x%04" PRIx16 "\n", phead->id);
    printf("QR = %" PRIu8 "\n", phead->qr);
    printf("OPCODE = %" PRIu8 "\n", phead->opcode);
    printf("AA = %" PRIu8 "\n", phead->aa);
    printf("TC = %" PRIu8 "\n", phead->tc);
    printf("RD = %" PRIu8 "\n", phead->rd);
    printf("RA = %" PRIu8 "\n", phead->ra);
    printf("RCODE = %" PRIu16 "\n", phead->rcode);
    printf("QDCOUNT = %" PRIu16 "\n", phead->qdcount);
    printf("ANCOUNT = %" PRIu16 "\n", phead->ancount);
    printf("NSCOUNT = %" PRIu16 "\n", phead->nscount);
    printf("ARCOUNT = %" PRIu16 "\n", phead->arcount);
}

static void print_dns_question(const Dns_Que * pque)
{
    printf("QNAME = %s\n", pque->qname);
    printf("QTYPE = %" PRIu16 "\n", pque->qtype);
    printf("QCLASS = %" PRIu16 "\n", pque->qclass);
}

static void print_dns_rr(const Dns_RR * prr)
{
    printf("NAME = %s\n", prr->name);
    printf("TYPE = %" PRIu16 "\n", prr->type);
    printf("CLASS = %" PRIu16 "\n", prr->class);
    printf("TTL = %" PRIu32 "\n", prr->ttl);
    printf("RDLENGTH = %" PRIu16 "\n", prr->rdlength);
    printf("RDATA = ");
    if (prr->type == DNS_TYPE_A)
        print_rr_A(prr->rdata);
    else if (prr->type == DNS_TYPE_CNAME || prr->type == DNS_TYPE_NS)
        print_rr_CNAME(prr->rdata);
    else if (prr->type == DNS_TYPE_AAAA)
        print_rr_AAAA(prr->rdata);
    else if (prr->type == DNS_TYPE_SOA)
    {
        print_rr_CNAME(prr->rdata);
        printf(" ");
        print_rr_CNAME(prr->rdata + strlen(prr->rdata) + 1);
        printf(" ");
        printf("%" PRIu32 " ", ntohl(*(uint32_t *) (prr->rdata + prr->rdlength - 20)));
        printf("%" PRIu32 " ", ntohl(*(uint32_t *) (prr->rdata + prr->rdlength - 16)));
        printf("%" PRIu32 " ", ntohl(*(uint32_t *) (prr->rdata + prr->rdlength - 12)));
        printf("%" PRIu32 " ", ntohl(*(uint32_t *) (prr->rdata + prr->rdlength - 8)));
        printf("%" PRIu32, ntohl(*(uint32_t *) (prr->rdata + prr->rdlength - 4)));
    }
    else
        for (int i = 0; i < prr->rdlength; ++i)
            printf("%" PRIu8, *(prr->rdata + i));
    printf("\n");
}

void print_dns_message(const Dns_Msg * pmsg)
{
    printf("=======Header=======\n");
    print_dns_header(pmsg->header);
    printf("\n");
    printf("=======Question=======\n");
    for (Dns_Que * pque = pmsg->que; pque; pque = pque->next)
    {
        print_dns_question(pque);
        printf("\n");
    }
    Dns_RR * prr = pmsg->rr;
    printf("=======Answer=======\n");
    for (int i = 0; i < pmsg->header->ancount; ++i, prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
    printf("=======Authority=======\n");
    for (int i = 0; i < pmsg->header->nscount; ++i, prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
    printf("=======Additional=======\n");
    for (int i = 0; i < pmsg->header->arcount; ++i, prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
}
