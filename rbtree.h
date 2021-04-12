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

typedef struct dns_rr_linklist
{
    Dns_RR * rr;
    time_t ttl;
    struct dns_rr_linklist * next;
} Dns_RR_LinkList;

typedef struct rbtree_node
{
    unsigned int key;
    Dns_RR_LinkList * value;
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

void rbtree_insert(Rbtree * tree, Rbtree_Node * node, unsigned int key, Dns_RR * value, time_t ttl, Rbtree_Node * fa);

Dns_RR_LinkList * rbtree_query(Rbtree * tree, unsigned int data);

#endif //DNSR_RBTREE_H
