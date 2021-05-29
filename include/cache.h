//
// Created by xqmmcqs on 2021/4/12.
//

#ifndef DNSR_CACHE_H
#define DNSR_CACHE_H

#include <stdio.h>

#include "dns_structure.h"
#include "rbtree.h"

#define CACHE_SIZE 30

typedef struct _cache
{
    Dns_RR_LinkList * head;
    Dns_RR_LinkList * tail;
    int size;
    Rbtree * tree;
    
    void (* insert)(struct _cache * cache, const Dns_Msg * msg);
    
    Rbtree_Value * (* query)(struct _cache * cache, const Dns_Que * que);
} Cache;

Cache * cache_init(FILE * keep_file);

#endif //DNSR_CACHE_H
