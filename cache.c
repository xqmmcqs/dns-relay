//
// Created by xqmmcqs on 2021/4/12.
//

#include <stdlib.h>
#include <string.h>
#include "dns_conversion.h"
#include "cache.h"

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
    Rbtree * tree = (Rbtree *) calloc(1, sizeof(Rbtree));
    rbtree_init(tree);
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
            rr->type = DNS_TYPE_A;
            rr->class = DNS_CLASS_IN;
            rr->ttl = -1;
            rr->rdlength = 4;
            rr->rdata = (uint8_t *) calloc(4, sizeof(uint8_t));
            for (int iip = 0, ird = 0; iip < strlen(ip); ++iip)
            {
                if (ip[iip] == '.')
                {
                    ++ird;
                    continue;
                }
                rr->rdata[ird] = rr->rdata[ird] * 10 + ip[iip] - '0';
            }
            Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            value->rr = rr;
            value->ancount = 1;
            value->type = rr->type;
            rbtree_insert(tree, BKDRHash(rr->name), value, -1);
        }
    }
    return tree;
}

void insert_cache(Rbtree * tree, Dns_Msg * msg)
{
    Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
    value->rr = copy_dnsrr(msg->rr);
    value->ancount = msg->header->ancount;
    value->nscount = msg->header->nscount;
    value->arcount = msg->header->arcount;
    value->type = msg->que->qtype;
    rbtree_insert(tree, BKDRHash(value->rr->name), value, time(NULL) + value->rr->ttl);
}

Rbtree_Value * query_cache(Rbtree * tree, Dns_Que * que)
{
    Dns_RR_LinkList * list = rbtree_query(tree, BKDRHash(que->qname));
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
