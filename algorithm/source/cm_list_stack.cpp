/*
 * cm_list_stack.cpp
 *
 *  Created on: 1 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_stack.h"
#include "cm_list.h"
#include "cm_debug.h"
//#include "cm_lib.h"


void *cm_stack_create(void *param)
{
    void *list = cm_list_create();
    return list;
}

S32 cm_stack_is_empty(void *stack)
{
    CM_ASSERT(stack != 0);
    return cm_list_is_empty(stack);
}

void cm_stack_make_empty(void *stack)
{
    CM_ASSERT(stack != 0);
    cm_list_make_empty(stack);
}

void cm_stack_push(void *stack, void *item)
{
    CM_ASSERT(stack != 0);

    cm_list_ahead_item(stack, item);
}

void *cm_stack_pop(void *stack)
{
    CM_ASSERT(stack != 0);
    return cm_list_delete_first(stack);
}

void *cm_stack_top(void *stack)
{
    CM_ASSERT(stack != 0);
    return cm_list_first_item(stack);
}

void cm_stack_destory(void *stack)
{
    CM_ASSERT(stack != 0);
    
    cm_list_destory(stack);
}

