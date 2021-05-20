//
// Created by xqmmcqs on 2021/4/5.
//

#ifndef DNSR_UDP_POOL_H
#define DNSR_UDP_POOL_H

#include <stdbool.h>
#include <uv.h>

#include "queue.h"

#define UDP_POOL_SIZE 256

typedef struct
{
    uint16_t id;
    uint16_t prev_id;
} Udp_Req;

typedef struct udp_pool
{
    Udp_Req * p[UDP_POOL_SIZE];
    unsigned short count;
    Queue * queue;
    
    bool (* full)(struct udp_pool * this);
    
    uint16_t (* insert)(struct udp_pool * this, Udp_Req * req);
    
    bool (* query)(struct udp_pool * this, uint16_t id);
    
    Udp_Req * (* delete)(struct udp_pool * this, uint16_t index);
    
    void (* destroy)(struct udp_pool * this);
} Udp_Pool;

Udp_Pool * upool_init();

#endif //DNSR_UDP_POOL_H
