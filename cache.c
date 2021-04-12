//
// Created by xqmmcqs on 2021/4/12.
//

#include <stdlib.h>
#include <string.h>
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
            rbtree_insert(tree, tree->root, BKDRHash(rr->name), rr, rr->ttl, NULL);
        }
    }
    return tree;
}

void insert_cache(Rbtree * tree, Dns_Msg * msg)
{
    Dns_RR * new_rr = (Dns_RR *) calloc(1, sizeof(Dns_RR)), * rr = new_rr;
    Dns_RR * old_rr = msg->rr;
    memcpy(new_rr, old_rr, sizeof(Dns_RR));
    while (old_rr->next)
    {
        new_rr->next = (Dns_RR *) calloc(1, sizeof(Dns_RR));
        old_rr = old_rr->next;
        new_rr = new_rr->next;
        memcpy(new_rr, old_rr, sizeof(Dns_RR));
    }
    rbtree_insert(tree, tree->root, BKDRHash(rr->name), rr, time(NULL) + rr->ttl, NULL);
}

Dns_RR * query_cache(Rbtree * tree, Dns_Que * que)
{
    Dns_RR_LinkList * list = rbtree_query(tree, BKDRHash(que->qname));
    while (list != NULL)
    {
        if (strcmp(list->rr->name, que->qname) == 0)
        {
            Dns_RR * new_rr = (Dns_RR *) calloc(1, sizeof(Dns_RR)), * rr = new_rr;
            Dns_RR * old_rr = list->rr;
            memcpy(new_rr, old_rr, sizeof(Dns_RR));
            while (old_rr->next)
            {
                new_rr->next = (Dns_RR *) calloc(1, sizeof(Dns_RR));
                old_rr = old_rr->next;
                new_rr = new_rr->next;
                memcpy(new_rr, old_rr, sizeof(Dns_RR));
            }
            return rr;
        }
        list = list->next;
    }
    return NULL;
}
