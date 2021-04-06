//
// Created by xqmmcqs on 2021/4/5.
//

#ifndef DNSR_QUEUE_H
#define DNSR_QUEUE_H

#define QUEUE_SIZE 65536

typedef struct
{
    uint16_t q[QUEUE_SIZE];
    unsigned short head;
    unsigned short tail;
}Queue;

Queue *queue_init();

void queue_push(Queue * queue, uint16_t num);

uint16_t queue_pop(Queue * queue);

void queue_destroy(Queue *queue);

#endif //DNSR_QUEUE_H
