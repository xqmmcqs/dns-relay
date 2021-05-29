//
// Created by xqmmcqs on 2021/4/13.
//

#ifndef DNSR_QUERY_POOL_H
#define DNSR_QUERY_POOL_H

#include <stdbool.h>
#include <uv.h>

#include "dns_structure.h"
#include "udp_pool.h"
#include "cache.h"

#define QUERY_POOL_SIZE 256

typedef struct
{
    uint16_t id;
    uint16_t prev_id;
    struct sockaddr addr;
    Dns_Msg * msg;
    uv_timer_t timer;
} Dns_Query;

typedef struct query_pool
{
    Dns_Query * pool[QUERY_POOL_SIZE];
    unsigned short count;
    Queue * queue;
    Udp_Pool * upool;
    uv_loop_t * loop;
    Cache * cache;
    
    bool (* full)(struct query_pool * this);
    
    void (* insert)(struct query_pool * this, const struct sockaddr * addr, const Dns_Msg * msg);
    
    void (* finish)(struct query_pool * this, const Dns_Msg * msg);
    
    void (* delete)(struct query_pool * this, uint16_t id);
    
    void (* destroy)(struct query_pool * this);
} Query_Pool;

Query_Pool * qpool_init(uv_loop_t * loop, Cache * cache);

#endif //DNSR_QUERY_POOL_H
