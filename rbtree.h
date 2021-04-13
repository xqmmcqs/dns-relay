//
// Created by xqmmcqs on 2021/4/11.
//

#ifndef DNSR_RBTREE_H
#define DNSR_RBTREE_H

#include <time.h>
#include "dns_structure.h"

typedef enum
{
    BLACK, RED
} Color;

typedef struct
{
    Dns_RR * rr;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
    uint8_t type;
} Rbtree_Value;

typedef struct dns_rr_linklist
{
    Rbtree_Value * value;
    time_t ttl;
    struct dns_rr_linklist * next;
} Dns_RR_LinkList;

typedef struct rbtree_node
{
    unsigned int key;
    Dns_RR_LinkList * rr_list;
    Color color;
    struct rbtree_node * left;
    struct rbtree_node * right;
    struct rbtree_node * parent;
} Rbtree_Node;

typedef struct
{
    Rbtree_Node * root;
} Rbtree;

void rbtree_init(Rbtree * tree);

void rbtree_insert(Rbtree * tree, unsigned int key, Rbtree_Value * value, time_t ttl);

Dns_RR_LinkList * rbtree_query(Rbtree * tree, unsigned int data);

#endif //DNSR_RBTREE_H
