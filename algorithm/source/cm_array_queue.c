/*
 * cm_array_queue.c
 *
 *  Created on: 18 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_queue.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


#define CM_CHECK_FULL(p)    CM_ASSERT((((p)->rear + 1) % (p)->capacity) != (p)->front)
#define CM_CHECK_EMPTY(p)   CM_ASSERT((p)->front != (p)->rear)


typedef void *QueueDataType;

typedef struct
{
    S32 capacity;
    S32 front;
    S32 rear;
    QueueDataType *data;
    
} CM_QUEUE_STRUCT;


void *cm_queue_create(void *param)
{
    CM_QUEUE_STRUCT *p = 0;

    CM_ASSERT(param != 0);

    if (param != 0)
    {
        p = CM_MALLOC(sizeof(CM_QUEUE_STRUCT));
        p->data = (QueueDataType *)CM_MALLOC(sizeof(QueueDataType) * (S32)param);
        if (p->data)
        {
            p->capacity = (S32)param;
            p->front = 0;
            p->rear = 0;
        }
        else
        {
            CM_FREE(p);
            p = 0;
        }
        
    }

    return p;
}

S32 cm_queue_is_empty(void *queue)
{
    S32 value = 1;
    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;

    CM_ASSERT(queue != 0);
    if (p)
    {
        value = (p->front == p->rear) ? 1 : 0;
    }

    return value;
}

void cm_queue_make_empty(void *queue)
{
    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;

    CM_ASSERT(p != 0);
    if (p)
    {
        p->front = 0;
        p->rear = 0;
    }
}

S32 cm_queue_is_full(void *queue)
{
    S32 value = 0;
    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;

    CM_ASSERT(p != 0);

    if (p)
    {
        value = (((p->rear + 1) % p->capacity) == p->front) ? 1 : 0;
    }

    return value;
}

S32 cm_queue_item_exist(void *queue, void *item)
{
    S32 i;

    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;
    if (p != 0)
    {
        for (i = p->front; i != p->rear; i = ((i + 1) % p->capacity))
        {
            if (item == p->data[i])
            {
                return 1;
            }
        }
    }

    return 0;
}

void cm_queue_enqueue(void *queue, void *item)
{
    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;

    CM_ASSERT(p != 0);

    if (p != 0)
    {
        CM_CHECK_FULL(p);
        p->data[p->rear] = item;
        p->rear = (p->rear + 1) % p->capacity;
    }
}

void *cm_queue_dequeue(void *queue)
{
    void *item = 0;

    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;
    if (p != 0)
    {
        CM_CHECK_EMPTY(p);
        item = p->data[p->front];
        p->front = (p->front + 1) % p->capacity;
    }

    return item;
}

void *cm_queue_front(void *queue)
{
    void *item = 0;

    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;
    if (p != 0)
    {
        CM_CHECK_EMPTY(p);
        item = p->data[p->front];
    }

    return item;
}

void cm_queue_destory(void *queue)
{
    CM_QUEUE_STRUCT *p = (CM_QUEUE_STRUCT *)queue;
    if (p != 0)
    {
        if (p->data)
        {
            CM_FREE(p->data);
        }

        CM_FREE(p);
    }
}

