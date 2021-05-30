//
// Created by xqmmcqs on 2021/4/5.
//

#include "../include/queue.h"

#include <stdlib.h>

#include "../include/util.h"

static void queue_push(Queue * this, uint16_t num)
{
    this->q[++this->tail] = num;
}

static uint16_t queue_pop(Queue * this)
{
    return this->q[this->head++];
}

static void queue_destroy(Queue * this)
{
    free(this);
}

Queue * queue_init()
{
    Queue * queue = (Queue *) calloc(1, sizeof(Queue));
    if (!queue)
        log_fatal("内存分配错误")
    queue->head = 0;
    queue->tail = QUEUE_SIZE - 1;
    
    queue->push = &queue_push;
    queue->pop = &queue_pop;
    queue->destroy = &queue_destroy;
    return queue;
}
