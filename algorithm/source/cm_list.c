/*
 * cm_list.cpp
 *
 *  Created on: 25 Nov 2010
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_list.h"
#include "cm_debug.h"
#include "cm_lib.h"


typedef struct _CM_LIST_NODE
{
    void *data;
    struct _CM_LIST_NODE *next;

} CM_LIST_NODE;

typedef struct
{
    CM_LIST_NODE *head;
    CM_LIST_NODE *current;
    CM_LIST_NODE *tail;

} CM_LIST_STRUCT;


static CM_LIST_NODE *cm_list_make_node(void *data);


static CM_LIST_NODE *cm_list_make_node(void *data)
{
    CM_LIST_NODE *node = (CM_LIST_NODE *)CM_MALLOC(sizeof(CM_LIST_NODE));
    CM_ASSERT(node != 0);

    if (node)
    {
        node->data = data;
        node->next = 0;
    }

    return node;
}

void *cm_list_create(void)
{
    CM_LIST_STRUCT *list = (CM_LIST_STRUCT *)CM_MALLOC(sizeof(CM_LIST_STRUCT));

    if (list)
    {
        CM_LIST_NODE *node = cm_list_make_node(0);

        list->head = node;
        list->current = node;
        list->tail = node;
    }
    
    return list;
}

S32 cm_list_is_empty(void *list)
{
    S32 value = 1;
    
    CM_ASSERT(list != 0);
    if (list != 0)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        if (p->head != p->tail)
        {
            value = 0;
        }
    }

    return value;
}

void cm_list_make_empty(void *list)
{
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        CM_LIST_NODE *node = p->head->next;

        while (node != 0)
        {
            CM_LIST_NODE *q = node->next;
            CM_FREE(node);
            node = q;
        }
        
        p->tail = p->head;
    }
}

S32 cm_list_append_item(void *list, void *data)
{
	S32 value = -1;
    CM_ASSERT(list != 0 && data != 0);

    if (list != 0 && data != 0)
    {
        CM_LIST_NODE *node = cm_list_make_node(data);
        CM_ASSERT(node != 0);

        if (node)
        {
            CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
            CM_LIST_NODE *tail = p->tail;

            tail->next = node;
            p->tail = node;

            value = 0;
        }
    }
    
    return value;
}

S32 cm_list_ahead_item(void *list, void *data)
{
	S32 value = -1;
    CM_ASSERT(list != 0 && data != 0);

    if (list != 0 && data != 0)
    {
        CM_LIST_NODE *node = cm_list_make_node(data);
        CM_ASSERT(node != 0);

        if (node)
        {
            CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
            CM_LIST_NODE *head = p->head;

            node->next = head->next;
            head->next = node;

            if (p->tail == head)    //test if empty list
            {
                p->tail = node;
            }

            value = 0;
        }
    }
    
    return value;
}

S32 cm_list_find_item(void *list, void *data)
{    
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        CM_LIST_NODE *node = p->head->next;

        while (node != 0)
        {
            if (data == node->data)
            {
                return 1;
            }

            node = node->next;
        }
    }

    return 0;
}

void *cm_list_first_item(void *list)
{
    void *data = 0;

    CM_ASSERT(list != 0);
    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        if (p->head->next != 0)
        {
            data = p->head->next->data;
        }
    }

    return data;
}

void cm_list_delete_item(void *list, void *data)
{    
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        CM_LIST_NODE *q = p->head;
        CM_LIST_NODE *r = q->next;

        while (r != 0)
        {
            if (data == r->data)
            {
                q->next = r->next;
                CM_FREE(r);

                if (p->tail == r)
                {
                    p->tail = q;
                }
                return;
            }

            q = r;
            r = q->next;
        }
    }

    return;
}

void* cm_list_delete_first(void *list)
{
    void *data = 0;

    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;

        CM_LIST_NODE *node = p->head->next;
        if (node != 0)
        {
            if (p->tail == node)
            {
                p->tail = p->head;
            }

            p->head->next = node->next;

            data = node->data;
            CM_FREE(node);
        }
    }

    return data;
}

void cm_list_start_get_item(void *list)
{
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        p->current = p->head;
    }
}

void *cm_list_get_next_item(void *list)
{
	void *data = 0;
    CM_ASSERT(list != 0);

    if (list)
    {
		CM_LIST_NODE *node;
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;

        CM_ASSERT(p->current != 0);
        node = p->current->next;

        if (node)
        {
            data = node->data;
            p->current = node;
        }
    }

    return data;
}

void cm_list_destory(void *list)
{
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        CM_LIST_NODE *node = p->head;

        while (node != 0)
        {
            CM_LIST_NODE *q = node->next;
            CM_FREE(node);
            node = q;
        }
        
        CM_FREE(p);
    }
}

void cm_list_print(void* list)
{    
    CM_ASSERT(list != 0);

    if (list)
    {
        CM_LIST_STRUCT *p = (CM_LIST_STRUCT *)list;
        CM_LIST_NODE *node = p->head->next;

        while (node != 0)
        {
            CM_TRACE_5("%d ", node->data);
            node = node->next;
        }
    }

    return;
}
