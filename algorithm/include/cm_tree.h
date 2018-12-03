/*
 * cm_tree.h
 *
 *  Created on: 20 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_TREE_H
#define _CM_TREE_H


#include "cm_data_type.h"


typedef void (*CM_TREE_NODE_FREE)(void *data);

//return 1 if equal
typedef S32 (*CM_TREE_NODE_COMPARE)(void *data1, void *data2);


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_tree_create_node(void *data);
S32 cm_tree_append(void *root, void *leaf);
void *cm_tree_get_child_by_value(void *root, void *value, CM_TREE_NODE_COMPARE f);
void *cm_tree_get_parent(void *node);

S32 cm_tree_node_is_leaf(void *node);
void *cm_tree_get_node_value(void *node);

//work with cm_tree_get_next_child
void cm_tree_start_get_child(void *root);
void *cm_tree_get_next_child(void *root);

void cm_tree_destory(void *root, CM_TREE_NODE_FREE f);


#ifdef __cplusplus
}
#endif


#endif //_CM_TREE_H

