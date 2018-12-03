/*
 * cm_binary_tree.h
 *
 *  Created on: 24 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_BINARY_TREE_H
#define _CM_BINARY_TREE_H


#include "cm_data_type.h"
#include "cm_gdi.h"


struct BTreeNode;
typedef struct BTreeNode *SearchTree;
typedef struct BTreeNode *Position;


#ifdef __cplusplus
extern "C"
{
#endif


void cm_binary_tree_free(SearchTree tree);

SearchTree cm_binary_tree_insert(SearchTree t, S32 value);
SearchTree cm_binary_tree_delete(SearchTree t, S32 value);
S32 cm_binary_tree_retrieve_data(Position node);

Position cm_binary_tree_find(SearchTree tree, S32 value);
Position cm_binary_tree_find_min(SearchTree tree);
Position cm_binary_tree_find_max(SearchTree tree);

S32 cm_binary_tree_max_depth(SearchTree tree);

void cm_binary_tree_print(SearchTree tree);

//left: 最左边的边界
//r   : 返回该树的最右边界
CM_POINT cm_binary_tree_print_ext(SearchTree t, S32 x, S32 y, S32 left, S32 *r, void *hdc);


#ifdef __cplusplus
}
#endif


#endif //_CM_BINARY_TREE_H

