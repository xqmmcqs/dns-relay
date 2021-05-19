/**
 * @file      rbtree.c
 * @brief     红黑树的实现
 * @author    xqmmcqs
 * @date      2021/4/14
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是红黑树的实现。
 *
 * 文件中实现了对红黑树节点的操作，包括初始化、删除、释放内存；
 *
 * 文件中还定义了对红黑树节点的链表的操作，包括初始化、插入、删除；
 *
 * 文件中实现了数个功能函数，包括求祖父节点、求叔父节点、求兄弟节点、求前驱、左旋、右旋、在插入和删除后维护红黑树的平衡、查找特定键的节点；
 *
 * 最后，借助上述函数，程序实现了头文件中声明的初始化、插入和查询操作。
*/

#include "rbtree.h"

#include <stdlib.h>

#include "dns_conversion.h"
#include "util.h"

static Rbtree_Node * NIL; ///< 叶节点

/**
 * @brief 求节点的祖父节点
 */
static inline Rbtree_Node * grandparent(Rbtree_Node * node)
{
    if (node->parent == NULL)
        return NULL;
    return node->parent->parent;
}

/**
 * @brief 求节点的叔父节点
 */
static inline Rbtree_Node * uncle(Rbtree_Node * node)
{
    if (grandparent(node) == NULL)
        return NULL;
    if (node->parent == grandparent(node)->right)
        return grandparent(node)->left;
    return grandparent(node)->right;
}

/**
 * @brief 求节点的兄弟节点
 */
static inline Rbtree_Node * sibling(Rbtree_Node * node)
{
    if (node->parent == NULL)
        return NULL;
    if (node->parent->left == node)
        return node->parent->right;
    else
        return node->parent->left;
}

/**
 * @brief 求节点的前驱节点
 * @note 假设前驱存在
 */
static Rbtree_Node * smallest_child(Rbtree_Node * node)
{
    if (node->left == NIL)
        return node;
    return smallest_child(node->left);
}

Rbtree * rbtree_init()
{
    log_debug("初始化红黑树");
    Rbtree * tree = (Rbtree *) calloc(1, sizeof(Rbtree));
    tree->root = NULL;
    NIL = (Rbtree_Node *) calloc(1, sizeof(Rbtree_Node));
    NIL->color = BLACK;
    return tree;
}

/**
 * @brief 将节点右旋
 */
static void rotate_right(Rbtree * tree, Rbtree_Node * node)
{
    Rbtree_Node * gp = grandparent(node);
    Rbtree_Node * fa = node->parent;
    Rbtree_Node * y = node->right;
    fa->left = y;
    if (y != NIL)
        y->parent = fa;
    node->right = fa;
    fa->parent = node;
    if (tree->root == fa)
        tree->root = node;
    node->parent = gp;
    if (gp != NULL)
    {
        if (gp->left == fa)
            gp->left = node;
        else
            gp->right = node;
    }
}

/**
 * @brief 将节点左旋
 */
static void rotate_left(Rbtree * tree, Rbtree_Node * node)
{
    if (node->parent == NULL)
    {
        tree->root = node;
        return;
    }
    Rbtree_Node * gp = grandparent(node);
    Rbtree_Node * fa = node->parent;
    Rbtree_Node * y = node->left;
    fa->right = y;
    if (y != NIL)
        y->parent = fa;
    node->left = fa;
    fa->parent = node;
    if (tree->root == fa)
        tree->root = node;
    node->parent = gp;
    if (gp != NULL)
    {
        if (gp->left == fa)
            gp->left = node;
        else
            gp->right = node;
    }
}

/**
 * @brief 初始化一个链表，分配内存
 * @return 指向新链表的指针
 */
static Dns_RR_LinkList * linklist_init()
{
    Dns_RR_LinkList * list = (Dns_RR_LinkList *) calloc(1, sizeof(Dns_RR_LinkList));
    return list;
}

/**
 * @brief 向链表中给定节点后插入一个新节点，分配内存
 */
static void linklist_insert(Dns_RR_LinkList * list, Rbtree_Value * value, time_t ttl)
{
    Dns_RR_LinkList * new_list_node = (Dns_RR_LinkList *) calloc(1, sizeof(Dns_RR_LinkList));
    new_list_node->expire_time = ttl;
    new_list_node->value = value;
    new_list_node->next = list->next;
    list->next = new_list_node;
}

/**
 * @brief 依照不同情况调整红黑树的形态，使其平衡
 */
static void insert_case(Rbtree * tree, Rbtree_Node * node)
{
    if (node->parent == NULL)
    {
        tree->root = node;
        node->color = BLACK;
        return;
    }
    if (node->parent->color == RED)
    {
        if (uncle(node)->color == RED)
        {
            node->parent->color = uncle(node)->color = BLACK;
            grandparent(node)->color = RED;
            insert_case(tree, grandparent(node));
        }
        else
        {
            if (node->parent->right == node && grandparent(node)->left == node->parent)
            {
                rotate_left(tree, node);
                node->color = BLACK;
                node->parent->color = RED;
                rotate_right(tree, node);
            }
            else if (node->parent->left == node && grandparent(node)->right == node->parent)
            {
                rotate_right(tree, node);
                node->color = BLACK;
                node->parent->color = RED;
                rotate_left(tree, node);
            }
            else if (node->parent->left == node && grandparent(node)->left == node->parent)
            {
                node->parent->color = BLACK;
                grandparent(node)->color = RED;
                rotate_right(tree, node->parent);
            }
            else if (node->parent->right == node && grandparent(node)->right == node->parent)
            {
                node->parent->color = BLACK;
                grandparent(node)->color = RED;
                rotate_left(tree, node->parent);
            }
        }
    }
}

/**
 * @brief 初始化一个节点，分配内存，并向节点的链表中插入值
 * @param fa 新节点的父亲节点
 * @return 指向新节点的指针
 */
static Rbtree_Node * node_init(unsigned int key, Rbtree_Value * value, time_t ttl, Rbtree_Node * fa)
{
    Rbtree_Node * node = (Rbtree_Node *) calloc(1, sizeof(Rbtree_Node));
    node->key = key;
    node->rr_list = linklist_init();
    linklist_insert(node->rr_list, value, ttl);
    node->color = RED;
    node->left = node->right = NIL;
    node->parent = fa;
    return node;
}

/**
 * @details 此函数从根节点开始迭代查找插入位置，如果该键对应的节点不存在，则创建一个新节点，并且维护树的平衡；否则在原有节点的链表上插入新元素。
 */
void rbtree_insert(Rbtree * tree, unsigned int key, Rbtree_Value * value, time_t ttl)
{
    log_debug("向红黑树中插入值");
    Rbtree_Node * node = tree->root;
    if (node == NULL)
    {
        node = node_init(key, value, ttl, NULL);
        insert_case(tree, node);
        return;
    }
    while (1)
    {
        if (key < node->key)
        {
            if (node->left != NIL)node = node->left;
            else
            {
                Rbtree_Node * new_node = node_init(key, value, ttl, node);
                node->left = new_node;
                insert_case(tree, new_node);
                return;
            }
        }
        else if (key > node->key)
        {
            if (node->right != NIL)node = node->right;
            else
            {
                Rbtree_Node * new_node = node_init(key, value, ttl, node);
                node->right = new_node;
                insert_case(tree, new_node);
                return;
            }
        }
        else
        {
            linklist_insert(node->rr_list, value, ttl);
            return;
        }
    }
}

/**
 * @brief 从给定节点开始递归查找键为给定值的节点
 * @return 如果找到了这样的节点，返回指向该节点的指针，否则返回NULL
 */
static Rbtree_Node * rbtree_find(Rbtree_Node * node, unsigned int data)
{
    if (node->key > data)
    {
        if (node->left == NIL)return NULL;
        return rbtree_find(node->left, data);
    }
    else if (node->key < data)
    {
        if (node->right == NIL)return NULL;
        return rbtree_find(node->right, data);
    }
    else return node;
}

/**
 * @brief 删除链表中给定节点的下一个节点，释放内存
 */
static void linklist_delete_next(Dns_RR_LinkList * list)
{
    Dns_RR_LinkList * temp = list->next;
    list->next = list->next->next;
    destroy_dnsrr(temp->value->rr);
    free(temp->value);
    free(temp);
}

/**
 * @brief 释放红黑树中的节点内存
 * @note 默认该节点的链表为空（即只有一个头节点）
 */
static void destroy_node(Rbtree_Node * node)
{
    free(node->rr_list);
    free(node);
    node = NULL;
}

/**
 * @brief 依照不同情况调整红黑树的形态，使其平衡
 */
static void delete_case(Rbtree * tree, Rbtree_Node * node)
{
    if (node->parent == NULL)
    {
        node->color = BLACK;
        return;
    }
    if (sibling(node)->color == RED)
    {
        node->parent->color = RED;
        sibling(node)->color = BLACK;
        if (node == node->parent->left)
            rotate_left(tree, node->parent);
        else
            rotate_right(tree, node->parent);
    }
    if (node->parent->color == BLACK && sibling(node)->color == BLACK
                                        && sibling(node)->left->color == BLACK && sibling(node)->right->color == BLACK)
    {
        sibling(node)->color = RED;
        delete_case(tree, node->parent);
    }
    else if (node->parent->color == RED && sibling(node)->color == BLACK
                                           && sibling(node)->left->color == BLACK && sibling(node)->right->color == BLACK)
    {
        sibling(node)->color = RED;
        node->parent->color = BLACK;
    }
    else
    {
        if (sibling(node)->color == BLACK)
        {
            if (node == node->parent->left && sibling(node)->left->color == RED
                                              && sibling(node)->right->color == BLACK)
            {
                sibling(node)->color = RED;
                sibling(node)->left->color = BLACK;
                rotate_right(tree, sibling(node)->left);
            }
            else if (node == node->parent->right && sibling(node)->left->color == BLACK
                                                    && sibling(node)->right->color == RED)
            {
                sibling(node)->color = RED;
                sibling(node)->right->color = BLACK;
                rotate_left(tree, sibling(node)->right);
            }
        }
        sibling(node)->color = node->parent->color;
        node->parent->color = BLACK;
        if (node == node->parent->left)
        {
            sibling(node)->right->color = BLACK;
            rotate_left(tree, sibling(node));
        }
        else
        {
            sibling(node)->left->color = BLACK;
            rotate_right(tree, sibling(node));
        }
    }
}

/**
 * @brief 删除红黑树中的节点
 */
static void rbtree_delete(Rbtree * tree, Rbtree_Node * node)
{
    log_debug("删除红黑树中的节点");
    if (node->right != NIL)
    {
        Rbtree_Node * smallest = smallest_child(node->right);
        Dns_RR_LinkList * temp = node->rr_list;
        node->rr_list = smallest->rr_list;
        smallest->rr_list = temp;
        node->key = smallest->key;
        node = smallest;
    }
    Rbtree_Node * child = node->left == NIL ? node->right : node->left;
    if (node->parent == NULL)
    {
        if (node->left == NIL && node->right == NIL)
            tree->root = NULL;
        else
        {
            child->parent = NULL;
            tree->root = child;
            tree->root->color = BLACK;
        }
        destroy_node(node);
        return;
    }
    if (node->parent->left == node)
        node->parent->left = child;
    else
        node->parent->right = child;
    child->parent = node->parent;
    if (node->color == BLACK)
    {
        if (child->color == RED)
            child->color = BLACK;
        else
            delete_case(tree, child);
    }
    destroy_node(node);
}

/**
 * @details 此函数查找给定键的节点，如果该节点存在，则删去节点链表中已经超时的部分，此时若链表不为空，则返回该链表；否则删除该节点并返回NULL。
 */
Dns_RR_LinkList * rbtree_query(Rbtree * tree, unsigned int data)
{
    log_debug("在红黑树中查询");
    Rbtree_Node * node = rbtree_find(tree->root, data);
    if (node == NULL)return NULL;
    time_t now_time = time(NULL);
    Dns_RR_LinkList * list = node->rr_list;
    while (list->next != NULL)
    {
        if (list->next->expire_time != -1 && list->next->expire_time <= now_time)
            linklist_delete_next(list);
        else
            list = list->next;
    }
    if (node->rr_list->next != NULL)
        return node->rr_list->next;
    else
    {
        rbtree_delete(tree, node);
        return NULL;
    }
}
