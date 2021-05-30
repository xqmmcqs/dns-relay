/**
 * @file      index_pool.c
 * @brief     序号池
 * @author    Ziheng Mao
 * @date      2021/4/5
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是序号池的实现。
*/

#include "../include/index_pool.h"

#include <stdlib.h>

#include "../include/util.h"

static bool ipool_full(Index_Pool * ipool)
{
    return ipool->count == INDEX_POOL_MAX_SIZE;
}

static uint16_t ipool_insert(Index_Pool * ipool, Index * req)
{
    uint16_t id = ipool->queue->pop(ipool->queue);
    ipool->pool[id] = req;
    ipool->count++;
    return id;
}

static bool ipool_query(Index_Pool * ipool, uint16_t index)
{
    return ipool->pool[index] != NULL;
}

static Index * ipool_delete(Index_Pool * ipool, uint16_t index)
{
    Index * req = ipool->pool[index];
    ipool->queue->push(ipool->queue, index);
    ipool->pool[index] = NULL;
    ipool->count--;
    return req;
}

static void ipool_destroy(Index_Pool * ipool)
{
    ipool->queue->destroy(ipool->queue);
    free(ipool);
}

Index_Pool * new_ipool()
{
    Index_Pool * ipool = (Index_Pool *) calloc(1, sizeof(Index_Pool));
    if (!ipool)
        log_fatal("内存分配错误")
    ipool->count = 0;
    ipool->queue = new_queue();
    for (uint16_t i = 0; i < INDEX_POOL_MAX_SIZE; ++i)
        ipool->queue->push(ipool->queue, i);
    
    ipool->full = &ipool_full;
    ipool->insert = &ipool_insert;
    ipool->query = &ipool_query;
    ipool->delete = &ipool_delete;
    ipool->destroy = &ipool_destroy;
    return ipool;
}
