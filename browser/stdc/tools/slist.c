/*
 * 2010-12-25: wuyulun
 */

#include "../inc/config.h"
#include "slist.h"

NSList* sll_create(void)
{
    NSList* list = NBK_malloc0(sizeof(NSList));
    return list;
}

void sll_delete(NSList** list)
{
    sll_reset(*list);
    NBK_free(*list);
    *list = N_NULL;
}

void sll_reset(NSList* list)
{
    NSLNode* p = list->head;
    NSLNode* t;
    
    while (p) {
        t = p;
        p = p->next;
        NBK_free(t);
    }
    list->head = list->curr = N_NULL;
    list->headRemoved = 0;
}

void sll_append(NSList* list, void* item)
{
    NSLNode *n;
    
    n = (NSLNode*)NBK_malloc0(sizeof(NSLNode));
    n->item = item;
    
    if (list->head == N_NULL) {
        list->head = n;
        list->curr = list->head;
    }
    else {
        while (list->curr->next)
            list->curr = list->curr->next;
        
        list->curr->next = n;
        list->curr = n;
    }
}

void* sll_first(NSList* list)
{
    list->headRemoved = 0;
    
    if (list->head) {
        list->curr = list->head;
        return list->head->item;
    }
    else
        return N_NULL;
}

void* sll_next(NSList* list)
{
    if (list->headRemoved) {
        list->headRemoved = 0;
        if (list->curr)
            return list->curr->item;
        else
            return N_NULL;
    }
    
    if (list->curr && list->curr->next) {
        list->curr = list->curr->next;
        return list->curr->item;
    }
    return N_NULL;
}

void* sll_last(NSList* list)
{
    NSLNode* n = list->curr;
    list->headRemoved = 0;
    while (n && n->next)
        n = n->next;
    return n->item;
}

void* sll_getAt(NSList* list, int index)
{
    int i = 0;
    NSLNode* n = list->head;
    while (i < index && n) {
        i++;
        n = n->next;
    }
    if (n && i == index)
        return n->item;
    else
        return N_NULL;
}

void* sll_removeFirst(NSList* list)
{
    if (list->head) {
        NSLNode* n = list->head;
        void* item = n->item;
        list->head = list->head->next;
        list->curr = list->head;
        NBK_free(n);
        return item;
    }
    else
        return N_NULL;
}

void* sll_removeCurr(NSList* list)
{
    NSLNode* prev;
    NSLNode* t;
    void* data;
    
    if (list->curr == N_NULL)
        return N_NULL;
    
    prev = list->head;
    
    if (prev == list->curr) {
        list->head = list->curr->next;
        if (list->head)
            list->head->item = list->curr->next->item;
        list->curr = list->head;
        list->headRemoved = 1;
        data = prev->item;
        NBK_free(prev);
        return data;
    }
    
    while (prev->next != list->curr)
        prev = prev->next;
    
    t = list->curr;
    data = t->item;
    
    prev->next = list->curr->next;
    if (prev->next)
        prev->next->item = list->curr->next->item;
    list->curr = prev;
    list->headRemoved = 0;
    
    NBK_free(t);
    return data;
}

bd_bool sll_isEmpty(NSList* list)
{
    return ((list->head) ? N_FALSE : N_TRUE);
}
