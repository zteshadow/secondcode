/*
 * bd_list.c
 *
 *  Created on: 2010-12-10
 *      Author: HouYuqi
 */

#include "bd_list.h"
#include "cm_lib.h"


bd_list* bd_list_alloc(void)
{
	return CM_MALLOC(sizeof(bd_list));
}

void bd_list_free(bd_list *list)
{
	while (list)
	{
		bd_list *temp = list;
		list = bd_list_delete_link(list, temp);
	}
}

void bd_list_free_1(bd_list *list)
{
	CM_FREE(list);
}

bd_list* bd_list_append(bd_list *list, void *data)
{
	bd_list *last;
	bd_list *new_list = bd_list_alloc();
	if (!new_list) return list;
	
	new_list->data = data;
	new_list->next = NULL;
	
	if (list)
	{
		last = bd_list_last(list);
		last->next = new_list;
		new_list->prev = last;
		return list;
	}
	else
	{
		new_list->prev = NULL;
		return new_list;
	}
}

bd_list* bd_list_insert(bd_list *list, void *data, S32 position)
{
	bd_list *new_list, *temp_list;
	
	if (position < 0)
		return bd_list_append(list, data);
	else if (position == 0)
		return bd_list_prepend(list, data);
	
	temp_list = bd_list_nth(list, position);
	if (!temp_list)
		return bd_list_append(list, data);
	
	new_list = bd_list_alloc();
	if (!new_list)
		return list;
	
	new_list->data = data;
	new_list->prev = temp_list->prev;
	if (temp_list->prev)
		temp_list->prev->next = new_list;
	new_list->next = temp_list;
	temp_list->prev = new_list;

	if (temp_list == list)
		return new_list;
	
	return list;
}

bd_list* bd_list_prepend(bd_list *list, void *data)
{
	bd_list *new_list;
	
	new_list = bd_list_alloc();
	if (!new_list) return list;
	
	new_list->data = data;
	new_list->next = list;
	
	if (list)
	{
		new_list->prev = list->prev;
		if (list->prev)
			list->prev->next = new_list;
		list->prev = new_list;
	}
	else
		new_list->prev = NULL;
	
	return new_list;
}

bd_list* bd_list_remove(bd_list *list, const void *data)
{
	bd_list *temp;
	
	temp = list;
	while (temp)
	{
		if (temp->data != data)
			temp = temp->next;
		else
		{
			if (temp->prev)
				temp->prev->next = temp->next;
			if (temp->next)
				temp->next->prev = temp->prev;
			
			if (list == temp)
				list = list->next;
			
			bd_list_free_1(temp);
			break;
		}
	}
	
	return list;
}

bd_list* bd_list_remove_all(bd_list *list, const void *data)
{
	 bd_list *tmp = list;

	while (tmp)
	{
		if (tmp->data != data)
			tmp = tmp->next;
		else
		{
			bd_list *next = tmp->next;

			if (tmp->prev)
				tmp->prev->next = next;
			else
				list = next;
			if (next)
				next->prev = tmp->prev;

			bd_list_free_1(tmp);
			tmp = next;
		}
	}
	return list;
}

bd_list* bd_list_remove_link(bd_list *list, bd_list *link)
{
	if (link)
	{
		if (link->prev)
			link->prev->next = link->next;
		if (link->next)
			link->next->prev = link->prev;
		
		if (link == list)
			list = list->next;
		
		link->next = link->prev = NULL;
	}
	
	return list;
}

bd_list* bd_list_delete_link(bd_list *list, bd_list *link)
{
	list = bd_list_remove_link(list, link);
	bd_list_free_1(link);
	return list;
}

bd_list* bd_list_nth(bd_list *list, S32 n)
{
	while ((n-- > 0) && list)
		list = list->next;
	
	return list;
}

bd_list* bd_list_nth_prev(bd_list *list, S32 n)
{
	while ((n-- > 0) && list)
		list = list->prev;
	
	return list;
}


bd_list* bd_list_find(bd_list *list, const void *data)
{
	while (list)
	{
		if (list->data == data)
			break;
		list = list->next;
	}
	
	return list;
}

S32 bd_list_position(bd_list *list, bd_list *link)
{
	S32 i = 0;
	while (list)
	{
		if (list == link)
			return i;
		i++;
		list = list->next;
	}
	
	return -1;
}

S32 bd_list_index(bd_list *list, const void *data)
{
	S32 i = 0;
	while (list)
	{
		if (list->data == data)
			return i;
		i++;
		list = list->next;
	}
	
	return -1;
}

bd_list* bd_list_last(bd_list *list)
{
	if (list)
	{
		while (list->next)
			list = list->next;		
	}
	
	return list;
}

bd_list* bd_list_first(bd_list *list)
{

	if (list)
	{
		while (list->prev)
			list = list->prev;
	}
	
	return list;
}

S32 bd_list_length(bd_list *list)
{
	S32 n = 0;
	while (list)
	{
		n++;
		list = list->next;
	}
	
	return n;
}

