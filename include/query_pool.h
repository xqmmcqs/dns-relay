/**
 * @file      query_pool.h
 * @brief     查询池
 * @author    Ziheng Mao
 * @date      2021/4/13
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件中定义了一个查询池，池内是目前活跃的DNS查询。
 *
 * 用户可以通过一个DNS查询报文在查询池内创建一个查询，并可以通过一个回复报文结束一个DNS查询。
 *
 * 创建查询时，查询池会首先在缓存中查找是否已有对应的回复，如果找到了回复会立即发送给本地。
 *
 * 收到结束查询时，查询池会将支持的回复报文插入缓存，并且将回复发送给本地。
 *
 * DNS查询会在一段时间后超时并且自动销毁。
*/

#ifndef DNSR_QUERY_POOL_H
#define DNSR_QUERY_POOL_H

#include <stdbool.h>
#include <uv.h>

#include "dns_structure.h"
#include "index_pool.h"
#include "cache.h"

#define QUERY_POOL_MAX_SIZE 256

/// DNS查询结构体
typedef struct dns_query
{
    uint16_t id; ///< 查询ID
    uint16_t prev_id; ///< 原本DNS查询报文的ID
    struct sockaddr addr; ///< 请求方地址
    Dns_Msg * msg; ///< DNS查询报文报文
    uv_timer_t timer; ///< 计时器
} Dns_Query;

/// DNS查询池
typedef struct query_pool
{
    Dns_Query * pool[QUERY_POOL_MAX_SIZE]; ///< 查询池
    unsigned short count; ///< 池内查询数量
    Queue * queue; ///< 未分配的查询ID的队列
    Index_Pool * ipool; ///< 序号池
    uv_loop_t * loop; ///< 事件循环
    Cache * cache; ///< 缓存
    
    /**
     * @brief 判断查询池是否已满
     *
     * @param qpool 查询池
     * @return 如果查询池已满，返回true
     */
    bool (* full)(struct query_pool * qpool);
    
    /**
     * @brief 向查询池中加入新查询
     *
     * @param qpool 查询池
     * @param addr 请求方地址
     * @param msg 查询报文
     */
    void (* insert)(struct query_pool * qpool, const struct sockaddr * addr, const Dns_Msg * msg);
    
    /**
     * @brief 结束查询
     *
     * @param qpool 查询池
     * @param msg 查询报文
     */
    void (* finish)(struct query_pool * qpool, const Dns_Msg * msg);
    
    /**
     * @brief 删除查询
     *
     * @param qpool 查询池
     * @param id 待删除的查询ID
     */
    void (* delete)(struct query_pool * qpool, uint16_t id);
} Query_Pool;

/**
 * @brief 创建查询池
 *
 * @param loop 事件循环
 * @param cache 缓存
 * @return 新的查询池
 */
Query_Pool * new_qpool(uv_loop_t * loop, Cache * cache);

#endif //DNSR_QUERY_POOL_H
