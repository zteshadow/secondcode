/*
 * cm_leftist_heap.h
 *
 *  Created on: 20 Feb 2012
 *      Author: samuel.song.bc@gmail.com
 *
 *  左式堆：左边>=右边，二叉且根最小
 */

#ifndef _CM_LEFTIST_HEAP_H
#define _CM_LEFTIST_HEAP_H


#include "cm_data_type.h"
#include "cm_gdi.h"


struct CMLHeapStruct;
typedef struct CMLHeapStruct *CMLHeap;


#ifdef __cplusplus
extern "C"
{
#endif


void cm_l_heap_free(CMLHeap q);

CMLHeap cm_l_heap_merge(CMLHeap h1, CMLHeap h2);
CMLHeap cm_l_heap_insert(CMLHeap h, S32 x);
CMLHeap cm_l_heap_delete_min(CMLHeap h, S32 *min);

CM_POINT cm_l_heap_print(CMLHeap q, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc);


#ifdef __cplusplus
}
#endif


#endif //_CM_LEFTIST_HEAP_H

