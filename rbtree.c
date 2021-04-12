//
// Created by xqmmcqs on 2021/4/11.
//

#include <stdlib.h>
#include "rbtree.h"

static inline Rbtree_Node * grandparent(Rbtree_Node * node)
{
    if (node->parent == NULL)
        return NULL;
    return node->parent->parent;
}

static inline Rbtree_Node * uncle(Rbtree_Node * node)
{
    if (grandparent(node) == NULL)
        return NULL;
    if (node->parent == grandparent(node)->right)
        return grandparent(node)->left;
    return grandparent(node)->right;
}

static inline Rbtree_Node * sibling(Rbtree_Node * node)
{
    if (node->parent == NULL)
        return NULL;
    if (node->parent->left == node)
        return node->parent->right;
    else
        return node->parent->left;
}

static Rbtree_Node * smallest_child(Rbtree_Node * node)
{
    if (node->left == NULL)
        return node;
    return smallest_child(node->left);
}

void rbtree_init(Rbtree * tree)
{
    tree->root = NULL;
}

static void rotate_right(Rbtree * tree, Rbtree_Node * node)
{
    Rbtree_Node * gp = grandparent(node);
    Rbtree_Node * fa = node->parent;
    Rbtree_Node * y = node->right;
    fa->left = y;
    if (y != NULL)
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

static void rotate_left(Rbtree * tree, Rbtree_Node * node)
{
    Rbtree_Node * gp = grandparent(node);
    Rbtree_Node * fa = node->parent;
    Rbtree_Node * y = node->right;
    fa->right = y;
    if (y != NULL)
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

static Rbtree_Node * rbtree_find(Rbtree_Node * node, unsigned int data)
{
    if (node->key > data)
    {
        if (node->left == NULL)return NULL;
        return rbtree_find(node->left, data);
    }
    else if (node->key < data)
    {
        if (node->right == NULL)return NULL;
        return rbtree_find(node->right, data);
    }
    else return node;
}

static void linklist_delete_next(Dns_RR_LinkList * list)
{
    Dns_RR_LinkList * temp = list->next;
    list->next = list->next->next;
    free(temp);
}

static void destroy_node(Rbtree_Node * node)
{
    free(node->value);
    free(node);
}

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

static void rbtree_delete(Rbtree * tree, Rbtree_Node * node)
{
    if (node->right != NULL)
    {
        Rbtree_Node * smallest = smallest_child(node);
        Dns_RR_LinkList * temp = node->value;
        node->value = smallest->value;
        smallest->value = temp;
        node->key = smallest->key;
        node = smallest;
    }
    Rbtree_Node * child = node->left == NULL ? node->right : node->left;
    if (node->parent == NULL)
    {
        destroy_node(node);
        if (node->left == NULL && node->right == NULL)
            tree->root = NULL;
        else
        {
            child->parent = NULL;
            tree->root = child;
            tree->root->color = BLACK;
        }
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

Dns_RR_LinkList * rbtree_query(Rbtree * tree, unsigned int data)
{
    Rbtree_Node * node = rbtree_find(tree->root, data);
    if (node == NULL)return NULL;
    time_t now_time = time(NULL);
    Dns_RR_LinkList * list = node->value;
    while (list->next != NULL)
    {
        if (list->next->ttl != -1 && list->next->ttl < now_time)
            linklist_delete_next(list);
        list = list->next;
    }
    if (node->value->next != NULL)
        return node->value->next;
    else
    {
        rbtree_delete(tree, node);
        return NULL;
    }
}

static Dns_RR_LinkList * linklist_init()
{
    Dns_RR_LinkList * list = (Dns_RR_LinkList *) calloc(1, sizeof(Dns_RR_LinkList));
    list->rr = NULL;
    list->next = NULL;
    return list;
}

static void linklist_insert(Dns_RR_LinkList * list, Dns_RR * value, time_t ttl)
{
    Dns_RR_LinkList * new_list_node = (Dns_RR_LinkList *) calloc(1, sizeof(Dns_RR_LinkList));
    new_list_node->ttl = ttl;
    new_list_node->rr = value;
    new_list_node->next = list->next;
    list->next = new_list_node;
}

static Rbtree_Node * node_init(unsigned int key, Dns_RR * value, time_t ttl, Rbtree_Node * fa)
{
    Rbtree_Node * node = (Rbtree_Node *) calloc(1, sizeof(Rbtree_Node));
    node->key = key;
    node->value = linklist_init();
    linklist_insert(node->value, value, ttl);
    node->color = RED;
    node->left = node->right = NULL;
    node->parent = fa;
    return node;
}

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

void
rbtree_insert(Rbtree * tree, Rbtree_Node * node, unsigned int key, Dns_RR * value, time_t ttl, Rbtree_Node * fa)
{
    if (node == NULL)
    {
        node = node_init(key, value, ttl, fa);
        insert_case(tree, node);
        return;
    }
    if (key > node->key)
        rbtree_insert(tree, node->left, key, value, ttl, node);
    else if (key < node->key)
        rbtree_insert(tree, node->right, key, value, ttl, node);
    else
        linklist_insert(node->value, value, ttl);
}
