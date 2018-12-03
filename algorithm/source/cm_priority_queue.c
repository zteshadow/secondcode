
#include "cm_priority_queue.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_debug.h"


#define CM_HORIZONTAL_PAD   (45)
#define CM_NODE_RADIUS      (30)
#define CM_MIN_VALUE        (CMPQueueData)(0)


struct CMHeapStruct
{
    S32 capacity;
    S32 size;
    CMPQueueData *data;

    CMPQueueCompareFunc compare_func;
};


static S32 cm_general_compare(CMPQueueData d1, CMPQueueData d2);
static void cm_general_print(CMPQueueData d, S8 *text);

static CM_POINT cm_p_queue_print_ext(CMPQueue q, CMPQueueDataToString func,
                                  S32 i, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc);

static CM_POINT cm_p_queue_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad);

static CM_POINT cm_p_queue_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad);

static CM_POINT cm_p_queue_draw_node(CMPQueue q, CMPQueueDataToString func,
                                     S32 i, const CM_POINT* p, void *hdc);


//从index开始下虑data
static void cm_p_queue_percolate_down(CMPQueue q, S32 index, CMPQueueData data);


static S32 cm_general_compare(CMPQueueData d1, CMPQueueData d2)
{
    if (d1 > d2)
    {
        return 1;
    }
    else if (d1 < d2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static void cm_general_print(CMPQueueData d, S8 *text)
{
    cm_sprintf(text, "%d", (S32)d);
}

static CM_POINT cm_p_queue_print_ext(CMPQueue q, CMPQueueDataToString func,
                                  S32 i, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc)
{
    S32 border;
    CM_POINT l_pos = {0}, pos, r_pos = {0};

    border = x;
    *r = x;

    pos.x = x;
    pos.y = y;

    if (q != 0 && i <= q->size)
    {
        if (2 * i <= q->size)       //with left
        {
            x = x - CM_HORIZONTAL_PAD > left ? x - CM_HORIZONTAL_PAD : left + CM_HORIZONTAL_PAD;

            l_pos = cm_p_queue_print_ext(q, func, 2 * i,
                                           x, y + CM_HORIZONTAL_PAD,
                                           left, &border,
                                           hdc);
        }

        if (2 * i + 1 <= q->size)
        {
            pos = cm_p_queue_right_node_pos(x, y, &l_pos, CM_HORIZONTAL_PAD);

            r_pos = cm_p_queue_print_ext(q, func, 2 * i + 1, 
                                         pos.x, pos.y, border + CM_HORIZONTAL_PAD,
                                         &border, hdc);

            *r = r_pos.x + CM_NODE_RADIUS;
        }

        pos = cm_p_queue_mid_node_pos(x, y, &l_pos, &r_pos, CM_HORIZONTAL_PAD);        
        
        pos = cm_p_queue_draw_node(q, func, i, &pos, hdc);
		*r = pos.x > border ? pos.x : border;

        if (2 * i <= q->size)
        {
            cm_gdi_draw_line(hdc, &l_pos, &pos);
        }

        if (2 * i + 1 <= q->size)
        {
            cm_gdi_draw_line(hdc, &pos, &r_pos);
        }
    }

    return pos;
}

static CM_POINT cm_p_queue_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad)
{
    CM_POINT pos;

    if (left->y == 0)
    {
        pos.x = x + pad;
        pos.y = y + pad;
    }
    else
    {
        pos.x = left->x + 2 * pad;
        pos.y = left->y;
    }

    return pos;
}

static CM_POINT cm_p_queue_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad)
{
    CM_POINT pos;

    if (left->y == 0 && right->y == 0)
    {
        pos.x = x;
        pos.y = y;
    }
    else if (left->y == 0)
    {
        pos.x = right->x - pad;
        pos.y = right->y - pad;
        
    }
    else if (right->y == 0)
    {
        pos.x = left->x + pad;
        pos.y = left->y - pad;
    }
    else
    {
        pos.x = (left->x + right->x) / 2;
        pos.y = left->y - pad;
    }

    return pos;
}

static CM_POINT cm_p_queue_draw_node(CMPQueue q, CMPQueueDataToString func,
                                     S32 i, const CM_POINT* p, void *hdc)
{
    CM_POINT pos = {0};

    if (q)
    {
        S32 offset;
        S8 text[11];
        CM_RECT rect;
        CM_POINT text_pos;

        offset = CM_NODE_RADIUS / 2;
        rect.x = p->x - offset > 0 ? p->x - offset : 1;
        rect.y = p->y - offset > 0 ? p->y - offset : 1;

        rect.w = CM_NODE_RADIUS;
        rect.h = CM_NODE_RADIUS;

        cm_gdi_draw_circle(hdc, &rect);

        pos.x = rect.x + offset;
        pos.y = rect.y + offset;

        //cm_sprintf(text, "%d", q->data[i]);
        func(q->data[i], text);
        
        text_pos = pos;
        text_pos.x -= 4;
        text_pos.y -= 8;
        cm_gdi_draw_text(hdc, text, cm_strlen(text), &text_pos);
    }

    return pos;
}

static void cm_p_queue_percolate_down(CMPQueue q, S32 index, CMPQueueData value)
{
    S32 i, child;

    for (i = index; i * 2 <= q->size; i = child)
    {
        child = i * 2;

        if (child < q->size && q->compare_func(q->data[child], q->data[child + 1]) > 0)
        {
            child++;
        }

        if (q->compare_func(value, q->data[child]) > 0)
        {
            q->data[i] = q->data[child];
        }
        else
        {
            break;
        }
    }

    q->data[i] = value;
}

CMPQueue cm_p_queue_create(S32 max, CMPQueueCompareFunc f)
{
    CMPQueue q;

    CM_ASSERT(max > 0);

    q = (CMPQueue)CM_MALLOC(sizeof(*q));
    
    if (q != 0)
    {
        q->data = (CMPQueueData *)CM_MALLOC(sizeof(S32) * (max + 1));
        if (q->data == 0)
        {
            CM_FREE(q);
            q = 0;
        }
        else
        {
            q->capacity = max;
            q->size = 0;
            q->data[0] = (CMPQueueData)CM_MIN_VALUE;

            q->compare_func = (f != 0) ? f : cm_general_compare;
        }
    }

    return q;
}

void cm_p_queue_destory(CMPQueue q)
{
    CM_ASSERT(q != 0);

    if (q)
    {
        CM_ASSERT(q->data != 0);
        if (q->data)
        {
            CM_FREE(q->data);
        }

        CM_FREE(q);
    }
}

S32 cm_p_queue_is_empty(CMPQueue q)
{
    CM_ASSERT(q != 0);

    if (q != 0)
    {
        return q->size == 0 ? 1 : 0;
    }
    else
    {
        return 1;
    }
}

void cm_p_queue_make_empty(CMPQueue q)
{
    CM_ASSERT(q != 0);

    if (q != 0)
    {
        q->size = 0;
    }
}

S32 cm_p_queue_is_full(CMPQueue q)
{
    CM_ASSERT(q != 0);

    if (q != 0)
    {
        return q->size == q->capacity ? 1 : 0;
    }
    else
    {
        return 0;
    }
}

void cm_p_queue_enqueue(CMPQueue q, CMPQueueData data)
{
    S32 i;

    CM_ASSERT(q != 0);

    if (q != 0)
    {
        CM_ASSERT(q->size < q->capacity);

        for (i = ++q->size; q->compare_func(q->data[i/2], data) > 0; i /= 2)
        {
            q->data[i] = q->data[i/2];
        }

        q->data[i] = data;
    }
}

CMPQueueData cm_p_queue_delete_min(CMPQueue q)
{
    CMPQueueData value = CM_MIN_VALUE, last;

    CM_ASSERT(q != 0);

    if (q != 0)
    {
        CM_ASSERT(q->size > 0);
        if (q->size > 0)
        {
            value = q->data[1];
            last = q->data[q->size--];
            cm_p_queue_percolate_down(q, 1, last);
        }
    }

    return value;
}

CM_POINT cm_p_queue_print(CMPQueue q, CMPQueueDataToString func,
                          S32 x, S32 y, 
                          S32 left, S32 *r,
                          void *hdc)
{
    if (func == 0)
    {
        func = cm_general_print;
    }
    
    return cm_p_queue_print_ext(q, func, 1, x, y, left, r, hdc);
}

CMPQueue cm_p_queue_build(CMPQueueData *data, S32 size, CMPQueueCompareFunc f)
{
    CMPQueue q = cm_p_queue_create(size, f);
    if (q != 0)
    {
        S32 i;

    #if 0
        for (i = 0; i < size; i++)
        {
            cm_p_queue_enqueue(q, data[i]);
        }
    #else
        cm_memcpy(q->data + 1, data, sizeof(CMPQueueData) * size);
        q->size = size;
        for (i = size / 2; i > 0; i--)
        {
            cm_p_queue_percolate_down(q, i, q->data[i]);
        }
    #endif
    }

    return q;
}

