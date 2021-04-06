//
// Created by xqmmcqs on 2021/4/3.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "dns_structure.h"
#include "dns_convesion.h"

static uint16_t read_uint16(const char * pstring, unsigned * offset)
{
    uint16_t ret = ntohs(*(uint16_t *) (pstring + *offset));
    *offset += 2;
    return ret;
}

static uint32_t read_uint32(const char * pstring, unsigned * offset)
{
    uint32_t ret = ntohl(*(uint32_t *) (pstring + *offset));
    *offset += 4;
    return ret;
}

static void string_to_rrname(uint8_t * pname, const char * pstring, unsigned * offset)
{
    if(!*(pstring + *offset))return;
    while (true)
    {
        if((*(pstring + *offset) >> 6) & 0x3)
        {
            unsigned new_offset = read_uint16(pstring, offset) & 0x3fff;
            string_to_rrname(pname, pstring, &new_offset);
            return;
        }
        if(!*(pstring + *offset))
        {
            ++*offset;
            *(pname-1) = 0;
            return;
        }
        int cur_length = (int) *(pstring + *offset);
        ++*offset;
        memcpy(pname, pstring + *offset, cur_length);
        pname += cur_length;
        *offset += cur_length;
        *pname++ = '.';
    }
}

static void string_to_dnshead(Dns_Header * phead, const char * pstring, unsigned * offset)
{
    phead->id = read_uint16(pstring, offset);
    uint16_t flag = read_uint16(pstring, offset);
    phead->qr = flag>>15;
    phead->opcode = (flag>>11)&0xF;
    phead->aa = (flag>>10)*0x1;
    phead->tc = (flag>>9)*0x1;
    phead->rd = (flag>>8)*0x1;
    phead->ra = (flag>>7)*0x1;
    phead->z = (flag>>4)*0x7;
    phead->rcode = flag&0xF;
    phead->qdcount = read_uint16(pstring, offset);
    phead->ancount = read_uint16(pstring, offset);
    phead->nscount = read_uint16(pstring, offset);
    phead->arcount = read_uint16(pstring, offset);
}

static void string_to_dnsque(Dns_Que * pque, const char * pstring, unsigned * offset)
{
    pque->qname = (uint8_t *) calloc(DNS_RR_NAME_MAX_SIZE, sizeof(uint8_t));
    string_to_rrname(pque->qname, pstring, offset);
    pque->qtype = read_uint16(pstring, offset);
    pque->qclass = read_uint16(pstring, offset);
}

static void string_to_dnsrr(Dns_RR * prr, const char * pstring, unsigned * offset)
{
    prr->name = (uint8_t *) calloc(DNS_RR_NAME_MAX_SIZE, sizeof(uint8_t));
    string_to_rrname(prr->name, pstring, offset);
    prr->type = read_uint16(pstring, offset);
    prr->class = read_uint16(pstring, offset);
    prr->ttl = read_uint32(pstring, offset);
    prr->rdlength = read_uint16(pstring, offset);
    prr->rdata = (uint8_t *) calloc(prr->rdlength, sizeof(uint8_t));
    memcpy(prr->rdata, pstring, prr->rdlength);
    *offset += prr->rdlength;
    // TODO: display rdata A, AAAA, CNAME, NS, MX
}

void string_to_dnsmsg(Dns_Msg * pmsg, const char * pstring)
{
    unsigned offset = 0;
    pmsg->header = (Dns_Header *) calloc(1, sizeof(Dns_Header));
    string_to_dnshead(pmsg->header, pstring, &offset);
    // TODO: 链表是反的
    for (int i = 0; i < pmsg->header->qdcount; ++i)
    {
        Dns_Que * temp = (Dns_Que *) calloc(1, sizeof(Dns_Que));
        temp->next = pmsg->question;
        pmsg->question = temp;
        string_to_dnsque(pmsg->question, pstring, &offset);
    }
    for (int i = 0; i < pmsg->header->ancount; ++i)
    {
        Dns_RR * temp = (Dns_RR *) calloc(1, sizeof(Dns_RR));
        temp->next = pmsg->answer;
        pmsg->answer = temp;
        string_to_dnsrr(pmsg->answer, pstring, &offset);
    }
    for (int i = 0; i < pmsg->header->nscount; ++i)
    {
        Dns_RR * temp = (Dns_RR *) calloc(1, sizeof(Dns_RR));
        temp->next = pmsg->authority;
        pmsg->authority = temp;
        string_to_dnsrr(pmsg->authority, pstring, &offset);
    }
    for (int i = 0; i < pmsg->header->arcount; ++i)
    {
        Dns_RR * temp = (Dns_RR *) calloc(1, sizeof(Dns_RR));
        temp->next = pmsg->additional;
        pmsg->additional = temp;
        string_to_dnsrr(pmsg->additional, pstring, &offset);
    }
}

void print_dns_string(const char * pstring, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (i % 16 == 0)
            printf("\n%04x ", i);
        printf("%02hhx ", pstring[i]);
    }
    printf("\n");
}

void destroy_dnsmsg(Dns_Msg * pmsg)
{
    free(pmsg->header);
    free(pmsg->question);
    free(pmsg->answer);
    free(pmsg->authority);
    free(pmsg->additional);
    free(pmsg);
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
    for (Dns_Que * pque = pmsg->question; pque; pque = pque->next)
    {
        print_dns_question(pque);
        printf("\n");
    }
    printf("=======Answer=======\n");
    for (Dns_RR * prr = pmsg->answer; prr; prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
    printf("=======Authority=======\n");
    for (Dns_RR * prr = pmsg->authority; prr; prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
    printf("=======Additional=======\n");
    for (Dns_RR * prr = pmsg->additional; prr; prr = prr->next)
    {
        print_dns_rr(prr);
        printf("\n");
    }
}
