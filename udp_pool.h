//
// Created by xqmmcqs on 2021/4/5.
//

#ifndef DNSR_UDP_POOL_H
#define DNSR_UDP_POOL_H

#include <uv.h>
#include <stdbool.h>
#include "queue.h"

#define UDP_POOL_SIZE 256

typedef struct
{
    uint16_t id;
    uint16_t prev_id;
} Udp_Req;

typedef struct
{
    Udp_Req * p[UDP_POOL_SIZE];
    unsigned short count;
    Queue * index_que;
} Udp_Pool;

Udp_Pool * upool_init();

bool upool_full(Udp_Pool * upool);

uint16_t upool_insert(Udp_Pool * upool, Udp_Req * req);

bool upool_query(Udp_Pool * upool, uint16_t id);

Udp_Req * upool_delete(Udp_Pool * upool, uint16_t index);

void upool_destroy(Udp_Pool * upool);

#endif //DNSR_UDP_POOL_H
