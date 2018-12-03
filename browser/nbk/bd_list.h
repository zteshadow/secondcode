
#ifndef _BD_DLIST_H
#define _BD_DLIST_H


#include "cm_data_type.h"


typedef struct _bd_list bd_list;
struct _bd_list
{
	void *data;
	bd_list *next;
	bd_list *prev;
};


#ifdef __cplusplus
extern "C"
{
#endif


bd_list* bd_list_alloc(void);
void bd_list_free(bd_list *list);
void bd_list_free_1(bd_list *list);

bd_list* bd_list_append(bd_list *list, void *data);
bd_list* bd_list_insert(bd_list *list, void *data, S32 position);
bd_list* bd_list_prepend(bd_list *list, void *data);

bd_list* bd_list_remove(bd_list *list, const void *data);
bd_list* bd_list_remove_all(bd_list *list, const void *data);
bd_list* bd_list_remove_link(bd_list *list, bd_list *link);
bd_list* bd_list_delete_link(bd_list *list, bd_list *link);

bd_list* bd_list_nth(bd_list *list, S32 n);
bd_list* bd_list_nth_prev(bd_list *list, S32 n);

bd_list* bd_list_find(bd_list *list, const void *data);

S32 bd_list_position(bd_list *list, bd_list *link);
S32 bd_list_index(bd_list *list, const void *data);

bd_list* bd_list_last(bd_list *list);
bd_list* bd_list_first(bd_list *list);
S32 bd_list_length(bd_list *list);
//void bd_list_foreach(bd_list *list, bd_func func, void *data);

#define bd_list_foreach(node, list)								\
	for (node = (list); node; node = (node)->next)

#define bd_list_previous(list)	((list)?(((bd_list*)list)->prev):NULL)
#define bd_list_next(list)	((list)?(((bd_list*)list)->next):NULL)


#ifdef __cplusplus
}
#endif


#endif	//_BD_DLIST_H

