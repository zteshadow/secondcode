/*
 * cm_stack.h
 *
 *  Created on: 3 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_STACK_H
#define _CM_STACK_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_stack_create(void *param);

S32 cm_stack_is_empty(void *stack);
void cm_stack_make_empty(void *stack);
void cm_stack_push(void *stack, void *item);
void *cm_stack_pop(void *stack);
void *cm_stack_top(void *stack);

void cm_stack_destory(void *stack);


#ifdef __cplusplus
}
#endif


#endif //_CM_STACK_H

