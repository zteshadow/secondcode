/*
 * cm_bin_queue.h
 *
 *  Created on: 24 Feb 2012
 *      Author: samuel.song.bc@gmail.com
 *
 *  二项式堆是一个森林
 */

#ifndef _CM_BIN_QUEUE_H
#define _CM_BIN_QUEUE_H


#include "cm_data_type.h"
#include "cm_gdi.h"


struct CMBinQueueStruct;
typedef struct CMBinQueueStruct *CMBinQueue;


#ifdef __cplusplus
extern "C"
{
#endif

//can process 2^max items
CMBinQueue cm_bin_queue_create(S32 max);
void cm_bin_queue_destory(CMBinQueue q);

//h1 return value
CMBinQueue cm_bin_queue_merge(CMBinQueue h1, CMBinQueue h2);
CMBinQueue cm_bin_queue_insert(CMBinQueue h, S32 x);
CMBinQueue cm_bin_queue_delete_min(CMBinQueue h, S32 *min);

CM_POINT cm_bin_queue_print(CMBinQueue q, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc);


#ifdef __cplusplus
}
#endif


#endif //_CM_BIN_QUEUE_H

