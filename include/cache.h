/**
 * @file      cache.h
 * @brief     缓存
 * @author    Ziheng Mao
 * @date      2021/4/12
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了缓存的结构体和接口。
 *
 * 缓存是一个大小为CACHE_SIZE的LRU，采用链表实现，每次命中时将链表中的节点移到链表末尾，当缓存满时，优先移出链表头的节点。
 *
 * 如果LRU未命中，将在红黑树中查询，并且根据QNAME和QTYPE筛选红黑树的查询结果
*/

#ifndef DNSR_CACHE_H
#define DNSR_CACHE_H

#include <stdio.h>

#include "rbtree.h"

#define CACHE_SIZE 30

/// 缓存结构体
typedef struct cache_
{
    Dns_RR_LinkList * head; ///< LRU头结点
    Dns_RR_LinkList * tail; ///< LRU尾节点
    int size; ///< LRU大小
    Rbtree * tree; ///< 红黑树
    
    /**
     * @brief 向缓存中插入DNS回复
     *
     * @param cache 缓存
     * @param msg DNS回复报文
     */
    void (* insert)(struct cache_ * cache, const Dns_Msg * msg);
    
    /**
     * @brief 在缓存中查询
     *
     * @param cache 缓存
     * @param que DNS Question Section
     * @return 如果查询到回复，则返回，否则返回NULL
     */
    Rbtree_Value * (* query)(struct cache_ * cache, const Dns_Que * que);
} Cache;

/**
 * @brief 创建缓存
 *
 * @param hosts_file hosts文件路径
 * @return 新的缓存结构体
 */
Cache * new_cache(FILE * hosts_file);

#endif //DNSR_CACHE_H
