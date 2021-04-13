//
// Created by xqmmcqs on 2021/4/13.
//

#include <stdlib.h>
#include "dns_conversion.h"
#include "cache.h"
#include "query_pool.h"
#include "util.h"
#include "dns_client.h"
#include "dns_server.h"

extern Rbtree * tree;

Query_Pool * qpool_init()
{
    Query_Pool * qpool = (Query_Pool *) calloc(1, sizeof(Query_Pool));
    qpool->count = 0;
    qpool->index_que = queue_init();
    for (uint16_t i = 0; i < QUERY_POOL_SIZE; ++i)
        queue_push(qpool->index_que, i);
    qpool->upool = upool_init();
    return qpool;
}

bool qpool_full(Query_Pool * qpool)
{
    return qpool->count == QUERY_POOL_SIZE;
}

void qpool_insert(Query_Pool * qpool, const struct sockaddr * addr, Dns_Msg * msg)
{
    Dns_Query * query = (Dns_Query *) calloc(1, sizeof(Dns_Query));
    uint16_t id = queue_pop(qpool->index_que);
    qpool->p[id % QUERY_POOL_SIZE] = query;
    qpool->count++;
    
    query->id = id;
    query->prev_id = msg->header->id;
    query->addr = *addr;
    query->msg = copy_dnsmsg(msg);
    
    Rbtree_Value * value = query_cache(tree, msg->que);
    if (value != NULL)
    {
        query->msg->header->qr = DNS_QR_ANSWER;
        if (query->msg->header->rd == 1)query->msg->header->ra = 1;
        query->msg->header->ancount = value->ancount;
        query->msg->header->nscount = value->nscount;
        query->msg->header->arcount = value->arcount;
        query->msg->rr = value->rr;
        
        // 污染
        if (value->rr->type == DNS_TYPE_A && value->rr->rdata != NULL && (*(int *) value->rr->rdata) == 0)
        {
            query->msg->header->rcode = DNS_RCODE_NXDOMAIN;
            destroy_dnsrr(query->msg->rr);
            query->msg->header->ancount = 0;
        }
    }
    else
    {
        if (upool_full(qpool->upool))
        {
            print_log(FATAL, "UDP pool full");
            qpool_delete(qpool, id);
            return;
        }
        Udp_Req * ureq = (Udp_Req *) calloc(1, sizeof(Udp_Req));
        ureq->id = upool_insert(qpool->upool, ureq);
        ureq->prev_id = id;
        query->msg->header->id = ureq->id;
        //TODO:timer
        send_to_remote(query->msg);
    }
    
    destroy_dnsmsg(msg);
    if (query->msg->rr != NULL)
        send_to_local(addr, query->msg);
}

static bool qpool_query(Query_Pool * qpool, uint16_t id)
{
    return qpool->p[id % QUERY_POOL_SIZE] != NULL && qpool->p[id % QUERY_POOL_SIZE]->id == id;
}

void qpool_finish(Query_Pool * qpool, Dns_Msg * msg)
{
    uint16_t uid = msg->header->id;
    if (!upool_query(qpool->upool, uid))
    {
        return;
    }
    Udp_Req * ureq = upool_delete(qpool->upool, uid);
    if (!qpool_query(qpool, ureq->prev_id))
    {
        return;
    }
    Dns_Query * query = qpool->p[ureq->prev_id % QUERY_POOL_SIZE];
    
    if (strcmp(msg->que->qname, query->msg->que->qname) == 0 && msg->que->qtype == query->msg->que->qtype)
    {
        query->msg = copy_dnsmsg(msg);
        query->msg->header->id = query->prev_id;
        
        if (msg->header->rcode == DNS_RCODE_OK && (msg->que->qtype == DNS_TYPE_A || msg->que->qtype == DNS_TYPE_CNAME))
            insert_cache(tree, msg);
        
        send_to_local(&query->addr, query->msg);
        qpool_delete(qpool, query->id);
    }
    
    destroy_dnsmsg(msg);
    free(ureq);
}

void qpool_delete(Query_Pool * qpool, uint16_t id)
{
    if (!qpool_query(qpool, id))
    {
        return;
    }
    Dns_Query * query = qpool->p[id % QUERY_POOL_SIZE];
    queue_push(qpool->index_que, id + QUERY_POOL_SIZE);
    qpool->p[id % UDP_POOL_SIZE] = NULL;
    qpool->count--;
    destroy_dnsmsg(query->msg);
    free(query);
}
