//
// Created by xqmmcqs on 2021/4/5.
//

#include <stdlib.h>
#include "udp_pool.h"

Udp_Pool * upool_init()
{
    Udp_Pool * upool = (Udp_Pool *) calloc(1, sizeof(Udp_Pool));
    upool->count = 0;
    upool->index_que = queue_init();
    for (uint16_t i = 0; i < UDP_POOL_SIZE; ++i)
        queue_push(upool->index_que, i);
    return upool;
}

bool upool_full(Udp_Pool * upool)
{
    return upool->count == UDP_POOL_SIZE;
}

uint16_t upool_insert(Udp_Pool * upool, Udp_Req * req)
{
    uint16_t id = queue_pop(upool->index_que);
    upool->p[id % UDP_POOL_SIZE] = req;
    upool->count++;
    return id;
}

bool upool_query(Udp_Pool * upool, uint16_t id)
{
    return upool->p[id % UDP_POOL_SIZE] != NULL && upool->p[id % UDP_POOL_SIZE]->id == id;
}

Udp_Req * upool_delete(Udp_Pool * upool, uint16_t id)
{
    Udp_Req * req = upool->p[id % UDP_POOL_SIZE];
    queue_push(upool->index_que, id + UDP_POOL_SIZE);
    upool->p[id % UDP_POOL_SIZE] = NULL;
    upool->count--;
    return req;
}

void upool_destroy(Udp_Pool * upool)
{
    queue_destroy(upool->index_que);
    free(upool);
}
