//
// Created by xqmmcqs on 2021/4/12.
//

#ifndef DNSR_CACHE_H
#define DNSR_CACHE_H

#include <stdio.h>
#include "dns_structure.h"
#include "rbtree.h"

Rbtree * init_cache(FILE * keep_file);

void insert_cache(Rbtree * tree, Dns_Msg * msg);

Rbtree_Value * query_cache(Rbtree * tree, Dns_Que * que);

#endif //DNSR_CACHE_H
