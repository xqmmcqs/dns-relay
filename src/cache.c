//
// Created by xqmmcqs on 2021/4/12.
//

#include "../include/cache.h"

#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "../include/util.h"
#include "../include/dns_conversion.h"

static unsigned int BKDRHash(const uint8_t * str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)
        hash = hash * seed + (*str++);
    return (hash & 0x7FFFFFFF);
}

static uint32_t get_min_ttl(const Dns_RR * prr)
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

static void cache_insert(Cache * cache, const Dns_Msg * msg)
{
    log_debug("插入cache")
    if (msg->rr == NULL) return;
    
    Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
    if (!value)
        log_fatal("内存分配错误")
    value->rr = copy_dnsrr(msg->rr);
    value->ancount = msg->header->ancount;
    value->nscount = msg->header->nscount;
    value->arcount = msg->header->arcount;
    value->type = msg->que->qtype;
    Dns_RR_LinkList * new_list_node = linklist_init();
    new_list_node->value = value;
    new_list_node->expire_time = time(NULL) + get_min_ttl(value->rr);
    if (cache->size == CACHE_SIZE)
    {
        log_debug("去除最久未访问的元素")
        cache->head->delete_next(cache->head);
        --cache->size;
    }
    cache->tail->insert(cache->tail, new_list_node);
    cache->tail = cache->tail->next;
    ++cache->size;
    
    value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
    if (!value)
        log_fatal("内存分配错误")
    value->rr = copy_dnsrr(msg->rr);
    value->ancount = msg->header->ancount;
    value->nscount = msg->header->nscount;
    value->arcount = msg->header->arcount;
    value->type = msg->que->qtype;
    new_list_node = linklist_init();
    new_list_node->value = value;
    new_list_node->expire_time = time(NULL) + get_min_ttl(value->rr);
    cache->tree->insert(cache->tree, BKDRHash(value->rr->name), new_list_node);
}

static Rbtree_Value * cache_query(Cache * cache, const Dns_Que * que)
{
    log_debug("查询cache")
    Dns_RR_LinkList * list = cache->head->query_next(cache->head, que->qname, que->qtype);
    if (list != NULL)
    {
        log_debug("cache命中")
        Dns_RR_LinkList * temp = list->next;
        if (temp != cache->tail)
        {
            list->next = list->next->next;
            cache->tail->insert(cache->tail, temp);
            cache->tail = cache->tail->next;
        }
        
        Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
        if (!value)
            log_fatal("内存分配错误")
        memcpy(value, temp->value, sizeof(Rbtree_Value));
        value->rr = copy_dnsrr(temp->value->rr);
        return value;
    }
    
    log_debug("cache未命中")
    list = cache->tree->query(cache->tree, BKDRHash(que->qname));
    while (list != NULL)
    {
        if (strcmp(list->value->rr->name, que->qname) == 0 && list->value->type == que->qtype)
        {
            Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            if (!value)
                log_fatal("内存分配错误")
            memcpy(value, list->value, sizeof(Rbtree_Value));
            value->rr = copy_dnsrr(list->value->rr);
            Dns_RR_LinkList * new_list_node = linklist_init();
            new_list_node->value = value;
            new_list_node->expire_time = list->expire_time;
            if (cache->size == CACHE_SIZE)
            {
                log_debug("去除最久未访问的元素")
                cache->head->delete_next(cache->head);
                --cache->size;
            }
            cache->tail->insert(cache->tail, new_list_node);
            cache->tail = cache->tail->next;
            ++cache->size;
            
            value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            if (!value)
                log_fatal("内存分配错误")
            memcpy(value, list->value, sizeof(Rbtree_Value));
            value->rr = copy_dnsrr(list->value->rr);
            return value;
        }
        list = list->next;
    }
    return NULL;
}

Cache * cache_init(FILE * keep_file)
{
    log_debug("初始化cache")
    Cache * cache = (Cache *) malloc(sizeof(Cache));
    if (!cache)
        log_fatal("内存分配错误")
    Rbtree * tree = rbtree_init();
    if (keep_file != NULL)
    {
        char ip[DNS_RR_NAME_MAX_SIZE], domain[DNS_RR_NAME_MAX_SIZE];
        while (fscanf(keep_file, "%s %s", domain, ip) != EOF)
        {
            Dns_RR * rr = (Dns_RR *) calloc(1, sizeof(Dns_RR));
            if (!rr)
                log_fatal("内存分配错误")
            rr->name = (uint8_t *) calloc(DNS_RR_NAME_MAX_SIZE, sizeof(uint8_t));
            if (!rr->name)
                log_fatal("内存分配错误")
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
                if (!rr->rdata)
                {
                    log_fatal("内存分配错误")
                    exit(1);
                }
                uv_inet_pton(AF_INET, ip, rr->rdata);
            }
            else // ipv6
            {
                rr->type = DNS_TYPE_AAAA;
                rr->rdlength = 16;
                rr->rdata = (uint8_t *) calloc(16, sizeof(uint8_t));
                if (!rr->rdata)
                    log_fatal("内存分配错误")
                uv_inet_pton(AF_INET6, ip, rr->rdata);
            }
            Rbtree_Value * value = (Rbtree_Value *) calloc(1, sizeof(Rbtree_Value));
            if (!value)
                log_fatal("内存分配错误")
            value->rr = rr;
            value->ancount = 1;
            value->type = rr->type;
            log_debug("插入记录 %s", rr->name)
            Dns_RR_LinkList * list = linklist_init();
            list->value = value;
            list->expire_time = -1;
            tree->insert(tree, BKDRHash(rr->name), list);
        }
    }
    
    cache->tree = tree;
    cache->head = cache->tail = linklist_init();
    cache->size = 0;
    cache->query = &cache_query;
    cache->insert = &cache_insert;
    return cache;
}
