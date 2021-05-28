//
// Created by xqmmcqs on 2021/4/5.
//

#include "udp_pool.h"

#include <stdlib.h>

static bool upool_full(Udp_Pool * upool)
{
    return upool->count == UDP_POOL_SIZE;
}

static uint16_t upool_insert(Udp_Pool * upool, Udp_Req * req)
{
    uint16_t id = upool->queue->pop(upool->queue);
    upool->pool[id] = req;
    upool->count++;
    return id;
}

static bool upool_query(Udp_Pool * upool, uint16_t id)
{
    return upool->pool[id] != NULL;
}

static Udp_Req * upool_delete(Udp_Pool * upool, uint16_t id)
{
    Udp_Req * req = upool->pool[id];
    upool->queue->push(upool->queue, id);
    upool->pool[id] = NULL;
    upool->count--;
    return req;
}

static void upool_destroy(Udp_Pool * upool)
{
    upool->queue->destroy(upool->queue);
    free(upool);
}

Udp_Pool * upool_init()
{
    Udp_Pool * upool = (Udp_Pool *) calloc(1, sizeof(Udp_Pool));
    upool->count = 0;
    upool->queue = queue_init();
    for (uint16_t i = 0; i < UDP_POOL_SIZE; ++i)
        upool->queue->push(upool->queue, i);
    
    upool->full = &upool_full;
    upool->insert = &upool_insert;
    upool->query = &upool_query;
    upool->delete = &upool_delete;
    upool->destroy = &upool_destroy;
    return upool;
}
