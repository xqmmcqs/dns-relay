//
// Created by xqmmcqs on 2021/4/13.
//

#include "query_pool.h"

#include <stdlib.h>

#include "dns_conversion.h"
#include "util.h"
#include "dns_client.h"
#include "dns_server.h"

static bool qpool_full(Query_Pool * this)
{
    return this->count == QUERY_POOL_SIZE;
}

/**
 * @brief 超时回调函数
 * @param timer 超时的计时器
 */
static void timeout_cb(uv_timer_t * timer)
{
    log_info("超时");
    uv_timer_stop(timer);
    Query_Pool * qpool = *(Query_Pool **) (timer->data + sizeof(uint16_t));
    qpool->delete(qpool, *(uint16_t *) timer->data);
}

static void qpool_insert(Query_Pool * qpool, const struct sockaddr * addr, const Dns_Msg * msg)
{
    log_debug("添加新查询请求");
    Dns_Query * query = (Dns_Query *) calloc(1, sizeof(Dns_Query));
    uint16_t id = qpool->queue->pop(qpool->queue);
    qpool->pool[id % QUERY_POOL_SIZE] = query;
    qpool->count++;
    
    query->id = id;
    query->prev_id = msg->header->id;
    query->addr = *addr;
    query->msg = copy_dnsmsg(msg);
    
    Rbtree_Value * value = query_cache(qpool->tree, query->msg->que);
    if (value != NULL)
    {
        log_debug("命中");
        query->msg->header->qr = DNS_QR_ANSWER;
        if (query->msg->header->rd == 1)query->msg->header->ra = 1;
        query->msg->header->ancount = value->ancount;
        query->msg->header->nscount = value->nscount;
        query->msg->header->arcount = value->arcount;
        query->msg->rr = value->rr;
        
        // 污染
        if ((value->rr->type == DNS_TYPE_A || value->rr->type == DNS_TYPE_AAAA) && value->rr->rdata != NULL &&
            (*(int *) value->rr->rdata) == 0)
        {
            query->msg->header->rcode = DNS_RCODE_NXDOMAIN;
            destroy_dnsrr(query->msg->rr);
            query->msg->header->ancount = 0;
        }
    }
    else
    {
        log_debug("未命中");
        if (qpool->upool->full(qpool->upool))
        {
            log_error("UDP pool full");
            qpool->delete(qpool, id);
            return;
        }
        Udp_Req * ureq = (Udp_Req *) calloc(1, sizeof(Udp_Req));
        ureq->id = qpool->upool->insert(qpool->upool, ureq);
        ureq->prev_id = id;
        query->msg->header->id = ureq->id;
        uv_timer_init(qpool->loop, &query->timer);
        query->timer.data = malloc(sizeof(uint16_t) + sizeof(Query_Pool *));
        *(uint16_t *) query->timer.data = query->id;
        *(Query_Pool **) (query->timer.data + sizeof(uint16_t)) = qpool;
        uv_timer_start(&query->timer, timeout_cb, 5000, 5000);
        send_to_remote(query->msg);
    }
    if (query->msg->rr != NULL)
        send_to_local(addr, query->msg);
}

static bool qpool_query(Query_Pool * this, uint16_t id)
{
    return this->pool[id % QUERY_POOL_SIZE] != NULL && this->pool[id % QUERY_POOL_SIZE]->id == id;
}

static void qpool_finish(Query_Pool * this, const Dns_Msg * msg)
{
    uint16_t uid = msg->header->id;
    if (!this->upool->query(this->upool, uid))
    {
        return;
    }
    Udp_Req * ureq = this->upool->delete(this->upool, uid);
    if (qpool_query(this, ureq->prev_id))
    {
        Dns_Query * query = this->pool[ureq->prev_id % QUERY_POOL_SIZE];
        log_debug("结束查询 ID: 0x%x", query->id);
        
        if (strcmp(msg->que->qname, query->msg->que->qname) == 0)
        {
            query->msg = copy_dnsmsg(msg);
            query->msg->header->id = query->prev_id;
            if (msg->header->rcode == DNS_RCODE_OK &&
                (msg->que->qtype == DNS_TYPE_A || msg->que->qtype == DNS_TYPE_CNAME ||
                 msg->que->qtype == DNS_TYPE_AAAA))
                insert_cache(this->tree, msg);
            send_to_local(&query->addr, query->msg);
        }
        this->delete(this, query->id);
    }
    free(ureq);
}

static void qpool_delete(Query_Pool * this, uint16_t id)
{
    if (!qpool_query(this, id))
    {
        return;
    }
    log_debug("删除 DNS query ID: 0x%x", id);
    Dns_Query * query = this->pool[id % QUERY_POOL_SIZE];
    this->queue->push(this->queue, id + QUERY_POOL_SIZE);
    this->pool[id % QUERY_POOL_SIZE] = NULL;
    this->count--;
    uv_timer_stop(&query->timer);
    destroy_dnsmsg(query->msg);
    free(query->timer.data);
    free(query);
}

Query_Pool * qpool_init(uv_loop_t * loop, Rbtree * tree)
{
    log_debug("初始化query pool");
    Query_Pool * qpool = (Query_Pool *) calloc(1, sizeof(Query_Pool));
    qpool->count = 0;
    qpool->queue = queue_init();
    for (uint16_t i = 0; i < QUERY_POOL_SIZE; ++i)
        qpool->queue->push(qpool->queue, i);
    qpool->upool = upool_init();
    qpool->loop = loop;
    qpool->tree = tree;
    
    qpool->full = &qpool_full;
    qpool->insert = &qpool_insert;
    qpool->delete = &qpool_delete;
    qpool->finish = &qpool_finish;
    return qpool;
}
