//
// Created by xqmmcqs on 2021/4/5.
//

#ifndef DNSR_QUEUE_H
#define DNSR_QUEUE_H

#define QUEUE_SIZE 256

typedef struct
{
    uint16_t q[QUEUE_SIZE];
    unsigned char head;
    unsigned char tail;
} Queue;

Queue * queue_init();

void queue_push(Queue * queue, uint16_t num);

uint16_t queue_pop(Queue * queue);

void queue_destroy(Queue * queue);

#endif //DNSR_QUEUE_H
