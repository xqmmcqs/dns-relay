/**
 * @file      queue.h
 * @brief     队列
 * @author    Ziheng Mao
 * @date      2021/4/5
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件中定义了一个循环队列，包括初始化、入队、出队、销毁的接口。
 *
 * 队列最大长度为65536，队列编号采用自然溢出。
 *
 * 队列不检查是否已满，需要用户保证队列中元素个数不超过上限。
*/

#include <stdint.h>

#ifndef DNSR_QUEUE_H
#define DNSR_QUEUE_H

#define QUEUE_MAX_SIZE 65536

/// 循环队列
typedef struct queue
{
    uint16_t q[QUEUE_MAX_SIZE]; ///< 队列
    unsigned short head; ///< 队列头
    unsigned short tail; ///< 队列尾
    
    /**
     * @brief 将值加入队尾
     *
     * @param queue 队列
     * @param num 加入队列的值
     */
    void (* push)(struct queue * queue, uint16_t num);
    
    /**
     * @brief 队首出队
     *
     * @param queue 队列
     * @return 队首的值
     */
    uint16_t (* pop)(struct queue * queue);
    
    /**
     * @brief 销毁队列
     *
     * @param queue 队列
     */
    void (* destroy)(struct queue * queue);
} Queue;

/**
 * @brief 创建队列
 *
 * @return 新的队列
 */
Queue * new_queue();

#endif //DNSR_QUEUE_H
