//
// Created by xqmmcqs on 2021/4/13.
//

#ifndef DNSR_QUERY_POOL_H
#define DNSR_QUERY_POOL_H

#include <stdbool.h>
#include <uv.h>
#include "dns_structure.h"
#include "udp_pool.h"

#define QUERY_POOL_SIZE 256

typedef struct
{
    uint16_t id;
    uint16_t prev_id;
    struct sockaddr addr;
    Dns_Msg * msg;
    uv_timer_t timer;
} Dns_Query;

typedef struct
{
    Dns_Query * p[QUERY_POOL_SIZE];
    unsigned short count;
    Queue * index_que;
    Udp_Pool * upool;
} Query_Pool;

Query_Pool * qpool_init();

bool qpool_full(Query_Pool * qpool);

void qpool_insert(Query_Pool * qpool, const struct sockaddr * addr, const Dns_Msg * msg);

void qpool_finish(Query_Pool * qpool, const Dns_Msg * msg);

void qpool_delete(Query_Pool * qpool, uint16_t id);

void qpool_destroy(Query_Pool * qpool);

#endif //DNSR_QUERY_POOL_H
