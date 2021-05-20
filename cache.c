//
// Created by xqmmcqs on 2021/4/12.
//

#include "cache.h"

#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "dns_conversion.h"
#include "util.h"

static unsigned int BKDRHash(uint8_t * str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)
        hash = hash * seed + (*str++);
    return (hash & 0x7FFFFFFF);
}

Rbtree * init_cache(FILE * keep_file)
{
    log_debug("初始化cache");
    Rbtree * tree = rbtree_init();
    if (keep_file != NULL)
    {
        char ip[DNS_RR_NAME_MAX_SIZE], domain[DNS_RR_NAME_MAX_SIZE];
        while (fscanf(keep_file, "%s %s", domain, ip) != EOF)
        {
            Dns_RR * rr = (Dns_RR *) calloc(1, sizeof(Dns_RR));
            rr->name = (uint8_t *) calloc(DNS_RR_NAME_MAX_SIZE, sizeof(uint8_t));
            memcpy(rr->name, domain, strlen(domain) + 1);
            rr->name[strlen(domain) + 1] = 0;
            rr->name[strlen(domain)] = '.';
            rr->class = DNS_CLASS_IN;
            rr->ttl = -1;
            if (strchr(ip, '.') != NULL) // ipv4
            {
                rr->type = DNS_TYPE_A;
                rr->rdlength = 4;
                rr->rdata = (uint8_t *) calloc(4, sizeof(uint8_t));
                uv_inet_pton(AF_INET, ip, rr->rdata);
//                for (int iip = 0, ird = 0; iip < strlen(ip); ++iip)
//                {
//                    if (ip[iip] == ':')
//                    {
//                        ++ird;
//                        continue;
//                    }
//                    rr->rdata[ird] = rr->rdata[ird] * 10 + ip[iip] - '0';
//                }
            }
            else // ipv6
            {
                rr->type = DNS_TYPE_AAAA;
                rr->rdlength = 16;
                rr->rdata = (uint8_t *) calloc(16, sizeof(uint8_t));
                uv_inet_pton(AF_INET6, ip, rr->rdata);
            }
            Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            value->rr = rr;
            value->ancount = 1;
            value->type = rr->type;
            log_debug("插入记录 %s", rr->name);
            tree->insert(tree, BKDRHash(rr->name), value, -1);
        }
    }
    return tree;
}

static uint32_t get_min_ttl(Dns_RR * prr)
{
    if (prr == NULL)
        return 0;
    uint32_t ttl = prr->ttl;
    prr = prr->next;
    while (prr != NULL)
    {
        if (prr->ttl < ttl)
            ttl = prr->ttl;
        prr = prr->next;
    }
    return ttl;
}

void insert_cache(Rbtree * tree, const Dns_Msg * msg)
{
    log_debug("插入cache");
    if (msg->rr == NULL) return;
    Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
    value->rr = copy_dnsrr(msg->rr);
    value->ancount = msg->header->ancount;
    value->nscount = msg->header->nscount;
    value->arcount = msg->header->arcount;
    value->type = msg->que->qtype;
    tree->insert(tree, BKDRHash(value->rr->name), value, time(NULL) + get_min_ttl(value->rr));
}

Rbtree_Value * query_cache(Rbtree * tree, const Dns_Que * que)
{
    log_debug("查询cache");
    Dns_RR_LinkList * list = tree->query(tree, BKDRHash(que->qname));
    while (list != NULL)
    {
        if (strcmp(list->value->rr->name, que->qname) == 0 && list->value->type == que->qtype)
        {
            Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            memcpy(value, list->value, sizeof(Rbtree_Value));
            value->rr = copy_dnsrr(list->value->rr);
            return value;
        }
        list = list->next;
    }
    return NULL;
}
