//
// Created by xqmmcqs on 2021/4/5.
//

#include <stdlib.h>
#include "udp_pool.h"

static bool upool_full(Udp_Pool * this)
{
    return this->count == UDP_POOL_SIZE;
}

static uint16_t upool_insert(Udp_Pool * this, Udp_Req * req)
{
    uint16_t id = this->queue->pop(this->queue);
    this->p[id % UDP_POOL_SIZE] = req;
    this->count++;
    return id;
}

static bool upool_query(Udp_Pool * this, uint16_t id)
{
    return this->p[id % UDP_POOL_SIZE] != NULL && this->p[id % UDP_POOL_SIZE]->id == id;
}

static Udp_Req * upool_delete(Udp_Pool * this, uint16_t id)
{
    Udp_Req * req = this->p[id % UDP_POOL_SIZE];
    this->queue->push(this->queue, id + UDP_POOL_SIZE);
    this->p[id % UDP_POOL_SIZE] = NULL;
    this->count--;
    return req;
}

static void upool_destroy(Udp_Pool * this)
{
    this->queue->destroy(this->queue);
    free(this);
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
