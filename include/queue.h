//
// Created by xqmmcqs on 2021/4/5.
//

#include <stdint.h>

#ifndef DNSR_QUEUE_H
#define DNSR_QUEUE_H

#define QUEUE_SIZE 65536

typedef struct queue
{
    uint16_t q[QUEUE_SIZE];
    unsigned short head;
    unsigned short tail;
    
    void (* push)(struct queue * this, uint16_t num);
    
    uint16_t (* pop)(struct queue * this);
    
    void (* destroy)(struct queue * this);
} Queue;

Queue * queue_init();

#endif //DNSR_QUEUE_H
