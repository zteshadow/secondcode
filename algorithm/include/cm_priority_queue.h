/*
 * cm_priority_queue.h
 *
 *  Created on: 15 Feb 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_PRIORITY_QUEUE_H
#define _CM_PRIORITY_QUEUE_H


#include "cm_data_type.h"
#include "cm_gdi.h"


typedef void *CMPQueueData;

typedef S32 (*CMPQueueCompareFunc)(CMPQueueData d1, CMPQueueData d2);
typedef void (*CMPQueueDataToString)(CMPQueueData d, S8 *buffer);

struct CMHeapStruct;
typedef struct CMHeapStruct *CMPQueue;


#ifdef __cplusplus
extern "C"
{
#endif

//set f=0 for fundamental data type:int, float, char etc.
CMPQueue cm_p_queue_create(S32 max, CMPQueueCompareFunc f);
void cm_p_queue_destory(CMPQueue q);

S32 cm_p_queue_is_empty(CMPQueue q);
void cm_p_queue_make_empty(CMPQueue q);
S32 cm_p_queue_is_full(CMPQueue q);

void cm_p_queue_enqueue(CMPQueue q, CMPQueueData data);
CMPQueueData cm_p_queue_delete_min(CMPQueue q);

CM_POINT cm_p_queue_print(CMPQueue q, CMPQueueDataToString func,
                          S32 x, S32 y, 
                          S32 left, S32 *r,
                          void *hdc);

//build a PQueue from array
CMPQueue cm_p_queue_build(CMPQueueData *data, S32 size, CMPQueueCompareFunc f);


#ifdef __cplusplus
}
#endif


#endif //_CM_PRIORITY_QUEUE_H

