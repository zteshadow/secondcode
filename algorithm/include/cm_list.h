/*
 * cm_list.h
 *
 *  Created on: 25 Nov 2010
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_LIST_H
#define _CM_LIST_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_list_create(void);

S32 cm_list_is_empty(void *list);
void cm_list_make_empty(void *list);

//insert
S32 cm_list_append_item(void *list, void *data);
S32 cm_list_ahead_item(void *list, void *data);

//find 1 if found, 0 not
S32 cm_list_find_item(void *list, void *data);
void *cm_list_first_item(void *list);

//delete
void cm_list_delete_item(void *list, void *data);
void *cm_list_delete_first(void *list);

//work together with next
void cm_list_start_get_item(void *list);
void *cm_list_get_next_item(void *list);

void cm_list_destory(void *list);

//for test
void cm_list_print(void* list);


#ifdef __cplusplus
}
#endif


#endif //_CM_LIST_H
