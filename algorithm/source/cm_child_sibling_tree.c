/*
 * cm_child_sibling_tree.c
 *
 *  Created on: 21 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 * imply tree by child and sibling.
 */

#include "cm_tree.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


typedef struct
{
    void *data;
    
    void *child;
    void *sibling;

    void *current;  //for parse

    void *parent;

} CM_CS_TREE_STRUCT; //CS(Child and Sibling)


void *cm_tree_create_node(void *data)
{
    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)
                                   CM_MALLOC(sizeof(CM_CS_TREE_STRUCT));

    if (p != 0)
    {
        cm_memset(p, 0, sizeof(CM_CS_TREE_STRUCT));
        p->data = data;
    }

    return (void *)p;
}

//almost the same with child list tree
S32 cm_tree_append(void *root, void *leaf)
{
    S32 value = -1;
    CM_CS_TREE_STRUCT *parent = (CM_CS_TREE_STRUCT *)root;
    CM_CS_TREE_STRUCT *child = (CM_CS_TREE_STRUCT *)leaf;

    CM_ASSERT(parent != 0 && child != 0);
    
    if (parent != 0 && child != 0)
    {
        value = 1;

        child->sibling = parent->child;
        child->parent = parent;

        parent->child = child;
    }

    return value;
}

void *cm_tree_get_child_by_value(void *root, void *value, CM_TREE_NODE_COMPARE f)
{
    CM_CS_TREE_STRUCT *parent = (CM_CS_TREE_STRUCT *)root;

    CM_ASSERT(parent != 0);
    if (parent != 0)
    {
        CM_CS_TREE_STRUCT *child = (CM_CS_TREE_STRUCT *)parent->child;

        while (child)
        {
            if (f(child->data, value))
            {
                return child;
            }

            child = child->sibling;
        }
    }

    //CM_ASSERT(0);
    return 0;
}

void *cm_tree_get_parent(void *node)
{
    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);

    return p->parent;
}

S32 cm_tree_node_is_leaf(void *node)
{
    S32 value = 1;

    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);

    if (p->child)
    {
        value = 0;
    }
    
    return value;
}

void *cm_tree_get_node_value(void *node)
{
    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    
    return p->data;
}

//work with cm_tree_get_next_child
void cm_tree_start_get_child(void *node)
{
    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    if (p != 0)
    {
        p->current = p->child;
    }
}

void *cm_tree_get_next_child(void *node)
{
    CM_CS_TREE_STRUCT *data = 0;
    CM_CS_TREE_STRUCT *p = (CM_CS_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    if (p != 0)
    {
        data = (CM_CS_TREE_STRUCT *)p->current;
        if (data)
        {
            p->current = data->sibling;
        }
    }

    return (void *)data;
}

void cm_tree_destory(void *p, CM_TREE_NODE_FREE f)
{
    CM_CS_TREE_STRUCT *root = (CM_CS_TREE_STRUCT *)p;

    CM_ASSERT(p != 0);
    if (root != 0)
    {
        //free child
        if (root->child)
        {
            cm_tree_destory(root->child, f);
        }

        //free sibling
        if (root->sibling)
        {
            cm_tree_destory(root->sibling, f);
        }

        if (root->data && f)
        {
            f(root->data);
        }

        CM_FREE(p);
    }
}

