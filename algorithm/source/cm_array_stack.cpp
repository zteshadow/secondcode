/*
 * cm_list_stack.cpp
 *
 *  Created on: 1 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_stack.h"
#include "cm_debug.h"
#include "cm_lib.h"


#define CM_EMPTY_POS        (-1)
#define CM_CHECK_FULL(p)    CM_ASSERT((p)->top + 1 < (p)->capacity)
#define CM_CHECK_EMPTY(p)   CM_ASSERT((p)->top > CM_EMPTY_POS)


typedef void* STACK_ITEM;

typedef struct
{
    S32 capacity;
    S32 top;
    STACK_ITEM *data;

} CM_ARRAY_STACK;


//param is the maximum of item
void *cm_stack_create(void *param)
{
    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)CM_MALLOC(sizeof(CM_ARRAY_STACK));
    if (p)
    {
        S32 max = (S32)param;
        STACK_ITEM *data = (STACK_ITEM *)CM_MALLOC(sizeof(STACK_ITEM) * max);

        if (data != 0)
        {
            p->capacity = max;
            p->top = CM_EMPTY_POS;
            p->data = data;
        }
        else
        {
            CM_FREE(p);
            p = 0;
        }
    }

    return p;
}

S32 cm_stack_is_empty(void *stack)
{
    S32 value = 1;

    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        if (p->top > CM_EMPTY_POS)
        {
            value = 0;
        }
    }
    
    return value;
}

void cm_stack_make_empty(void *stack)
{
    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        p->top = CM_EMPTY_POS;
    }
}

void cm_stack_push(void *stack, void *item)
{
    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        CM_CHECK_FULL(p);

        p->data[++p->top] = item;
    }
}

void *cm_stack_pop(void *stack)
{
    void *data = 0;
    
    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        CM_CHECK_EMPTY(p);

        data = p->data[p->top--];
    }

    return data;
}

void *cm_stack_top(void *stack)
{
    void *data = 0;

    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        CM_CHECK_FULL(p);
        data = p->data[p->top];
    }

    return data;
}

void cm_stack_destory(void *stack)
{
    CM_ASSERT(stack != 0);

    CM_ARRAY_STACK *p = (CM_ARRAY_STACK *)stack;
    if (p)
    {
        if (p->data != 0)
        {
            CM_FREE(p->data);
        }

        CM_FREE(p);
    }
}

