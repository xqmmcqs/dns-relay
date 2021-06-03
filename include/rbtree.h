/**
 * @file      rbtree.h
 * @brief     红黑树
 * @author    Ziheng Mao
 * @date      2021/4/14
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件定义了红黑树的结构和接口。
 *
 * 红黑树节点的键为一个无符号整型。由于存在键冲突的可能性，采用拉链法，因此红黑树节点的值为一个链表，链表中的每个节点对应一个特定查询的答案。
 *
 * 红黑树有三个接口：初始化、插入新的键-值，查询键对应的值。
 *
 * 链表中的每个节点定义有过期时间，在查询过程中会检查节点是否过期，如果过期，程序会自动删除链表中的节点；
 *
 * 如果一个红黑树节点的值链表为空，则会删除这个节点。
*/

#ifndef DNSR_RBTREE_H
#define DNSR_RBTREE_H

#include <time.h>

#include "dns_structure.h"

/// 红黑树节点的颜色
typedef enum
{
    BLACK, RED
} Color;

/// 红黑树节点链表的节点的值，对应一个特定查询的答案
typedef struct rbtree_value
{
    Dns_RR * rr; ///< 指向一个Dns_RR的链表
    uint16_t ancount; ///< RR链表中Answer Section的数目
    uint16_t nscount; ///< RR链表中Authority Section的数目
    uint16_t arcount; ///< RR链表中Addition Section的数目
    uint8_t type; ///< RR对应的Question的类型
} Rbtree_Value;

/// 红黑树节点链表
typedef struct dns_rr_linklist
{
    Rbtree_Value * value; ///< 指向当前链表节点的值
    time_t expire_time; ///< 过期的时刻
    struct dns_rr_linklist * next; ///< 链表的下一个节点
    
    /**
     * @brief 向链表中插入结点
     * @param list 当前节点
     * @param new_list_node 新节点
     */
    void (* insert)(struct dns_rr_linklist * list, struct dns_rr_linklist * new_list_node);
    
    /**
     * @brief 删除链表中当前节点的下一个结点
     *
     * @param list 当前节点
     * @note list不能是链表的尾节点
     */
    void (* delete_next)(struct dns_rr_linklist * list);
    
    /**
     * @brief 在链表中查找特定的值
     *
     * @param list 链表起始节点
     * @param qname NAME字段
     * @param qtype type字段
     * @return 如果查找到节点，返回该节点在链表中的前驱，否则返回NULL
     */
    struct dns_rr_linklist * (* query_next)(struct dns_rr_linklist * list, const uint8_t * qname, const uint16_t qtype);
} Dns_RR_LinkList;

/// 红黑树的节点
typedef struct rbtree_node
{
    unsigned int key; ///< 红黑树节点的键
    Dns_RR_LinkList * rr_list; ///< 指向当前节点对应的链表
    Color color; ///< 当前节点的颜色
    struct rbtree_node * left; ///< 指向当前节点的左子节点
    struct rbtree_node * right; ///< 指向当前节点的右子节点
    struct rbtree_node * parent; ///< 指向当前节点的父亲节点
} Rbtree_Node;

/// 红黑树
typedef struct rbtree
{
    Rbtree_Node * root; ///< 指向红黑树的根节点
    
    /**
     * @brief 向红黑树中插入键-值对
     *
     * @param tree 红黑树
     * @param key 键
     * @param list 值
     *
     * 此函数从根节点开始迭代查找插入位置，如果该键对应的节点不存在，则创建一个新节点，并且维护树的平衡；否则在原有节点的链表上插入新元素。
     */
    void (* insert)(struct rbtree * tree, unsigned int key, Dns_RR_LinkList * list);
    
    /**
     * @brief 在红黑树中查找键对应的值
     *
     * @param tree 红黑树
     * @param data 键
     * @return 如果找到了对应的值，返回一个没有头节点的链表；否则返回NULL
     *
     * 此函数查找给定键的节点，如果该节点存在，则删去节点链表中已经超时的部分，此时若链表不为空，则返回该链表；否则删除该节点并返回NULL。
     */
    Dns_RR_LinkList * (* query)(struct rbtree * tree, unsigned int data);
} Rbtree;

/**
 * @brief 创建链表节点
 *
 * @return 新的链表节点
 */
Dns_RR_LinkList * new_linklist();

/**
 * @brief 创建红黑树
 *
 * @return 新的红黑树
 */
Rbtree * new_rbtree();

#endif //DNSR_RBTREE_H

