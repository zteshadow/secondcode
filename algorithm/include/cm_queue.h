/*
 * cm_queue.h
 *
 *  Created on: 18 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_QUEUE_H
#define _CM_QUEUE_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_queue_create(void *param);

S32 cm_queue_is_empty(void *queue);
void cm_queue_make_empty(void *queue);
S32 cm_queue_is_full(void *queue);

//special
S32 cm_queue_item_exist(void *queue, void *item);

void cm_queue_enqueue(void *queue, void *item);
void *cm_queue_dequeue(void *queue);
void *cm_queue_front(void *queue);

void cm_queue_destory(void *queue);


#ifdef __cplusplus
}
#endif


#endif //_CM_QUEUE_H

