/*
 * cm_tree_utility.h
 *
 *  Created on: 10 May 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_TREE_UTILITY_H
#define _CM_TREE_UTILITY_H


#include "cm_data_type.h"
#include "cm_gdi.h"


typedef void *CMTree;
typedef void *CMTreeNode;

typedef S32 (*CMTreeCompareFunc)(CMTreeNode d1, CMTreeNode d2);
typedef void (*CMTreeNodeToString)(CMTreeNode d, S8 *buffer);
typedef CMTreeNode (*CMTreeGetLeftChild)(CMTree tree);
typedef CMTreeNode (*CMTreeGetRightChild)(CMTree tree);

typedef struct
{
    CMTreeCompareFunc compare;
    CMTreeNodeToString print;
    CMTreeGetLeftChild getLeft;
    CMTreeGetRightChild getRight;
    
} CMTreeFunction;


#ifdef __cplusplus
extern "C"
{
#endif

CM_POINT cm_binary_tree_print(CMTree tree, CMTreeFunction *func,
                          S32 x, S32 y, void *hdc);


#ifdef __cplusplus
}
#endif


#endif //_CM_TREE_UTILITY_H

