/**
 * @file      query_pool.c
 * @brief     查询池
 * @author    Ziheng Mao
 * @date      2021/4/13
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是查询池的实现。
*/

#include "../include/query_pool.h"

#include <stdlib.h>

#include "../include/util.h"
#include "../include/dns_conversion.h"
#include "../include/dns_client.h"
#include "../include/dns_server.h"

/**
 * @brief 超时回调函数
 * @param timer 超时的计时器
 */
static void timeout_cb(uv_timer_t * timer)
{
    log_info("超时")
    uv_timer_stop(timer);
    Query_Pool * qpool = *(Query_Pool **) (timer->data + sizeof(uint16_t));
    qpool->delete(qpool, *(uint16_t *) timer->data);
}

static bool qpool_full(Query_Pool * this)
{
    return this->count == QUERY_POOL_MAX_SIZE;
}

static void qpool_insert(Query_Pool * qpool, const struct sockaddr * addr, const Dns_Msg * msg)
{
    log_debug("添加新查询请求")
    Dns_Query * query = (Dns_Query *) calloc(1, sizeof(Dns_Query));
    if (!query)
        log_fatal("内存分配错误")
    uint16_t id = qpool->queue->pop(qpool->queue);
    qpool->pool[id % QUERY_POOL_MAX_SIZE] = query;
    qpool->count++;
    
    query->id = id;
    query->prev_id = msg->header->id;
    query->addr = *addr;
    query->msg = copy_dnsmsg(msg);
    
    Rbtree_Value * value = qpool->cache->query(qpool->cache, query->msg->que);
    if (value != NULL)
    {
        query->msg->header->qr = DNS_QR_ANSWER;
        if (query->msg->header->rd == 1)query->msg->header->ra = 1;
        query->msg->header->ancount = value->ancount;
        query->msg->header->nscount = value->nscount;
        query->msg->header->arcount = value->arcount;
        query->msg->rr = value->rr;
        
        // 污染
        if (value->rr->type == 255 && (*(int *) value->rr->rdata) == 0)
        {
            query->msg->header->rcode = DNS_RCODE_NXDOMAIN;
            destroy_dnsrr(query->msg->rr);
            query->msg->rr = NULL;
            query->msg->header->ancount = 0;
        }
        
        send_to_local(addr, query->msg);
        free(value);
        qpool->delete(qpool, query->id);
    }
    else
    {
        if (qpool->ipool->full(qpool->ipool))
        {
            log_error("序号池满")
            qpool->delete(qpool, id);
            return;
        }
        Index * index = (Index *) calloc(1, sizeof(Index));
        if (!index)
            log_fatal("内存分配错误")
        index->id = qpool->ipool->insert(qpool->ipool, index);
        index->prev_id = id;
        query->msg->header->id = index->id;
        
        uv_timer_init(qpool->loop, &query->timer);
        query->timer.data = malloc(sizeof(uint16_t) + sizeof(Query_Pool *));
        if (!query->timer.data)
            log_fatal("内存分配错误")
        *(uint16_t *) query->timer.data = query->id;
        *(Query_Pool **) (query->timer.data + sizeof(uint16_t)) = qpool;
        uv_timer_start(&query->timer, timeout_cb, 5000, 5000);
        send_to_remote(query->msg);
    }
}

static bool qpool_query(Query_Pool * qpool, uint16_t id)
{
    return qpool->pool[id % QUERY_POOL_MAX_SIZE] != NULL && qpool->pool[id % QUERY_POOL_MAX_SIZE]->id == id;
}

static void qpool_finish(Query_Pool * qpool, const Dns_Msg * msg)
{
    uint16_t uid = msg->header->id;
    if (!qpool->ipool->query(qpool->ipool, uid))
    {
        log_error("序号池中不存在此序号")
        return;
    }
    Index * index = qpool->ipool->delete(qpool->ipool, uid);
    if (qpool_query(qpool, index->prev_id))
    {
        Dns_Query * query = qpool->pool[index->prev_id % QUERY_POOL_MAX_SIZE];
        log_debug("结束查询 ID: 0x%04x", query->id)
        
        if (strcmp(msg->que->qname, query->msg->que->qname) == 0)
        {
            destroy_dnsmsg(query->msg);
            query->msg = copy_dnsmsg(msg);
            query->msg->header->id = query->prev_id;
            if (msg->header->rcode == DNS_RCODE_OK &&
                (msg->que->qtype == DNS_TYPE_A || msg->que->qtype == DNS_TYPE_CNAME ||
                 msg->que->qtype == DNS_TYPE_AAAA))
                qpool->cache->insert(qpool->cache, msg);
            send_to_local(&query->addr, query->msg);
        }
        qpool->delete(qpool, query->id);
    }
    free(index);
}

static void qpool_delete(Query_Pool * qpool, uint16_t id)
{
    if (!qpool_query(qpool, id))
    {
        log_error("查询池中不存在此序号")
        return;
    }
    log_debug("删除查询 ID: 0x%04x", id)
    Dns_Query * query = qpool->pool[id % QUERY_POOL_MAX_SIZE];
    qpool->queue->push(qpool->queue, id + QUERY_POOL_MAX_SIZE);
    qpool->pool[id % QUERY_POOL_MAX_SIZE] = NULL;
    qpool->count--;
    uv_timer_stop(&query->timer);
    destroy_dnsmsg(query->msg);
    free(query->timer.data);
    free(query);
}

Query_Pool * new_qpool(uv_loop_t * loop, Cache * cache)
{
    log_info("初始化查询池")
    Query_Pool * qpool = (Query_Pool *) calloc(1, sizeof(Query_Pool));
    if (!qpool)
        log_fatal("内存分配错误")
    qpool->count = 0;
    qpool->queue = new_queue();
    for (uint16_t i = 0; i < QUERY_POOL_MAX_SIZE; ++i)
        qpool->queue->push(qpool->queue, i);
    qpool->ipool = new_ipool();
    qpool->loop = loop;
    qpool->cache = cache;
    
    qpool->full = &qpool_full;
    qpool->insert = &qpool_insert;
    qpool->delete = &qpool_delete;
    qpool->finish = &qpool_finish;
    return qpool;
}
