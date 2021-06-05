/**
 * @file      rbtree.c
 * @brief     红黑树
 * @author    Ziheng Mao
 * @date      2021/4/14
 * @copyright GNU General Public License, version 3 (GPL-3.0)
 *
 * 本文件的内容是红黑树及节点链表的实现。
*/

#include "../include/rbtree.h"

#include <stdlib.h>
#include <string.h>

#include "../include/dns_conversion.h"
#include "../include/util.h"

static Rbtree_Node * NIL; ///< 叶节点

static void linklist_insert(Dns_RR_LinkList * list, Dns_RR_LinkList * new_list_node)
{
    log_debug("链表中插入新元素")
    new_list_node->next = list->next;
    list->next = new_list_node;
}

static void linklist_delete_next(Dns_RR_LinkList * list)
{
    log_debug("删除链表中的元素")
    Dns_RR_LinkList * temp = list->next;
    list->next = list->next->next;
    destroy_dnsrr(temp->value->rr);
    free(temp->value);
    free(temp);
}

static Dns_RR_LinkList * linklist_query_next(Dns_RR_LinkList * list, const uint8_t * qname, uint16_t qtype)
{
    log_debug("在链表中查找元素")
    time_t now_time = time(NULL);
    while (list->next != NULL)
    {
        if (list->next->expire_time != -1 && list->next->expire_time <= now_time)
            list = list->next;
        else if (strcmp(list->next->value->rr->name, qname) == 0 &&
                 (list->next->value->type == 255 || list->next->value->type == qtype))
            return list;
        else
            list = list->next;
    }
    return NULL;
}

Dns_RR_LinkList * new_linklist()
{
    Dns_RR_LinkList * list = (Dns_RR_LinkList *) calloc(1, sizeof(Dns_RR_LinkList));
    if (!list)
        log_fatal("内存分配错误")
    list->next = NULL;
    
    list->insert = &linklist_insert;
    list->delete_next = &linklist_delete_next;
    list->query_next = &linklist_query_next;
    return list;
}

/**
 * @brief 求节点的祖父节点
 *
 * @param node 当前节点
 * @return 如果存在，则返回祖父节点，否则返回NULL
 */
static inline Rbtree_Node * grandparent(Rbtree_Node * node)
{
    if (node->parent == NULL)
        return NULL;
    return node->parent->parent;
}

/**
 * @brief 求节点的叔父节点
 *
 * @param node 当前节点
 * @return 如果存在，则返回叔父节点，否则返回NULL
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
 *
 * @param node 当前节点
 * @return 如果存在，则返回兄弟节点，否则返回NULL
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
 * @brief 求子树的最小节点
 *
 * @param node 子树的根
 * @return 子树的最小节点
 */
static Rbtree_Node * smallest_child(Rbtree_Node * node)
{
    if (node->left == NIL)
        return node;
    return smallest_child(node->left);
}

/**
 * @brief 将节点右旋
 *
 * @param tree 节点所在的树
 * @param node 当前节点
 */
static void rotate_right(Rbtree * tree, Rbtree_Node * node)
{
    if (node->parent == NULL)
    {
        tree->root = node;
        return;
    }
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
 *
 * @param tree 节点所在的树
 * @param node 当前节点
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
 * @brief 依照不同情况调整红黑树的形态，使其平衡
 *
 * @param tree 节点所在的树
 * @param node 当前节点
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
 * @brief 初始化一个节点，分配内存
 *
 * @param key 节点的键
 * @param list 节点的值
 * @param fa 节点的父亲节点
 * @return 指向新节点的指针
 */
static Rbtree_Node * node_init(unsigned int key, Dns_RR_LinkList * list, Rbtree_Node * fa)
{
    Rbtree_Node * node = (Rbtree_Node *) calloc(1, sizeof(Rbtree_Node));
    if (!node)
        log_fatal("内存分配错误")
    node->key = key;
    node->rr_list = new_linklist();
    node->rr_list->insert(node->rr_list, list);
    node->color = RED;
    node->left = node->right = NIL;
    node->parent = fa;
    return node;
}

void rbtree_insert(Rbtree * tree, unsigned int key, Dns_RR_LinkList * list)
{
    log_debug("插入红黑树")
    Rbtree_Node * node = tree->root;
    if (node == NULL)
    {
        node = node_init(key, list, NULL);
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
                Rbtree_Node * new_node = node_init(key, list, node);
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
                Rbtree_Node * new_node = node_init(key, list, node);
                node->right = new_node;
                insert_case(tree, new_node);
                return;
            }
        }
        else
        {
            node->rr_list->insert(node->rr_list, list);
            return;
        }
    }
}

/**
 * @brief 从给定节点开始递归查找键为给定值的节点
 *
 * @param node 当前节点
 * @param key 键
 * @return 如果找到了这样的节点，返回指向该节点的指针，否则返回NULL
 */
static Rbtree_Node * rbtree_find(Rbtree_Node * node, unsigned int key)
{
    if (node->key > key)
    {
        if (node->left == NIL)return NULL;
        return rbtree_find(node->left, key);
    }
    else if (node->key < key)
    {
        if (node->right == NIL)return NULL;
        return rbtree_find(node->right, key);
    }
    else return node;
}

/**
 * @brief 销毁红黑树中的节点
 *
 * @param node 待销毁的节点
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
 *
 * @param tree 节点所在的树
 * @param node 当前节点
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
            rotate_left(tree, sibling(node));
        else
            rotate_right(tree, sibling(node));
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
 *
 * @param tree 节点所在的树
 * @param node 待删除的节点
 */
static void rbtree_delete(Rbtree * tree, Rbtree_Node * node)
{
    log_debug("删除红黑树中的节点")
    if (node->right != NIL)
    {
        Rbtree_Node * smallest = smallest_child(node->right);
        Dns_RR_LinkList * temp = node->rr_list;
        node->rr_list = smallest->rr_list;
        smallest->rr_list = temp;
        unsigned int temp1 = node->key;
        node->key = smallest->key;
        smallest->key = temp1;
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
    if (child != NIL)
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

Dns_RR_LinkList * rbtree_query(Rbtree * tree, unsigned int key)
{
    log_debug("查询红黑树")
    Rbtree_Node * node = rbtree_find(tree->root, key);
    if (node == NULL)return NULL;
    time_t now_time = time(NULL);
    Dns_RR_LinkList * list = node->rr_list;
    while (list->next != NULL)
    {
        if (list->next->expire_time != -1 && list->next->expire_time <= now_time)
            list->delete_next(list);
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

Rbtree * new_rbtree()
{
    log_debug("初始化红黑树")
    Rbtree * tree = (Rbtree *) calloc(1, sizeof(Rbtree));
    if (!tree)
        log_fatal("内存分配错误")
    tree->root = NULL;
    if (!NIL)
    {
        NIL = (Rbtree_Node *) calloc(1, sizeof(Rbtree_Node));
        if (!NIL)
            log_fatal("内存分配错误")
        NIL->color = BLACK;
        NIL->left = NIL->right = NIL;
    }
    
    tree->insert = &rbtree_insert;
    tree->query = &rbtree_query;
    return tree;
}
