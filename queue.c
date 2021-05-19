//
// Created by xqmmcqs on 2021/4/5.
//

#include <stdlib.h>
#include "queue.h"

Queue * queue_init()
{
    Queue * queue = (Queue *) calloc(1, sizeof(Queue));
    queue->head = 0;
    queue->tail = QUEUE_SIZE - 1;
    return queue;
}

void queue_push(Queue * queue, uint16_t num)
{
    queue->q[++queue->tail] = num;
}

uint16_t queue_pop(Queue * queue)
{
    return queue->q[queue->head++];
}

void queue_destroy(Queue * queue)
{
    free(queue);
}
