/*
 * cm_child_tree.c
 *
 *  Created on: 20 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 * imply tree by only child list.
 */

#include "cm_tree.h"
#include "cm_list.h"
#include "cm_debug.h"
#include "cm_lib.h"
#include "cm_string.h"


typedef struct
{
    void *data;
    void *child;    //cm_list_create
    void *parent;

} CM_CHILD_TREE_STRUCT;


void *cm_tree_create_node(void *data)
{
    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)
                                   CM_MALLOC(sizeof(CM_CHILD_TREE_STRUCT));

    if (p != 0)
    {
        cm_memset(p, 0, sizeof(CM_CHILD_TREE_STRUCT));
        p->data = data;
        p->child = cm_list_create();
    }

    return (void *)p;
}

S32 cm_tree_append(void *root, void *leaf)
{
    S32 value = -1;
    CM_CHILD_TREE_STRUCT *parent, *p;

    CM_ASSERT(root != 0 && leaf != 0);

    parent = (CM_CHILD_TREE_STRUCT *)root;
    p = (CM_CHILD_TREE_STRUCT *)leaf;
    
    if (parent != 0 && p != 0)
    {
        p->parent = root;
        value = cm_list_append_item(parent->child, leaf);
    }

    return value;
}

void *cm_tree_get_child_by_value(void *root, void *value, CM_TREE_NODE_COMPARE f)
{
    void *node;

    CM_CHILD_TREE_STRUCT *parent;

    CM_ASSERT(root != 0);

    parent = (CM_CHILD_TREE_STRUCT *)root;
    if (parent != 0)
    {
        CM_CHILD_TREE_STRUCT *p;

        cm_list_start_get_item(parent->child);
        while ((node = cm_list_get_next_item(parent->child)) != 0)
        {
            p = (CM_CHILD_TREE_STRUCT *)node;

            if (f)
            {
                if (f(p->data, value))
                {
                    return node;
                }
            }
        }
    }

    CM_ASSERT(0);
    return 0;
}

void *cm_tree_get_parent(void *node)
{
    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);

    return p->parent;
}

S32 cm_tree_node_is_leaf(void *node)
{
    S32 value = 1;

    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);

    if (!cm_list_is_empty(p->child))
    {
        value = 0;
    }
    
    return value;
}

void *cm_tree_get_node_value(void *node)
{
    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    
    return p->data;
}

//work with cm_tree_get_next_child
void cm_tree_start_get_child(void *node)
{
    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    if (p != 0)
    {
        cm_list_start_get_item(p->child);
    }
}

void *cm_tree_get_next_child(void *node)
{
    void *data = 0;
    CM_CHILD_TREE_STRUCT *p = (CM_CHILD_TREE_STRUCT *)node;
    
    CM_ASSERT(node != 0);
    if (p != 0)
    {
        data = cm_list_get_next_item(p->child);
    }

    return data;
}

void cm_tree_destory(void *p, CM_TREE_NODE_FREE f)
{
    void *node;
    CM_CHILD_TREE_STRUCT *root = (CM_CHILD_TREE_STRUCT *)p;

    CM_ASSERT(p != 0);
    if (root != 0)
    {
        if (root->child != 0)
        {
            cm_list_start_get_item(root->child);
            while ((node = cm_list_get_next_item(root->child)) != 0)
            {
                cm_tree_destory(node, f);
            }

            cm_list_destory(root->child);
        }

        if (root->data && f)
        {
            f(root->data);
        }
        
        CM_FREE(p);
    }
}

