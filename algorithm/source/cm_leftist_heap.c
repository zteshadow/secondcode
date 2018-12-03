
#include "cm_leftist_heap.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_debug.h"


#define CM_HORIZONTAL_PAD   (45)
#define CM_NODE_RADIUS      (30)


struct CMLHeapStruct
{
    S32 data;

    S32 npl; //null path length
    struct CMLHeapStruct *left;
    struct CMLHeapStruct *right;
};


static S32 cm_l_head_get_npl(CMLHeap h);
static CMLHeap cm_l_heap_create(S32 data);
static CM_POINT cm_l_heap_right_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        S32 pad);
static CM_POINT cm_l_heap_mid_node_pos(S32 x, S32 y,
                                        const CM_POINT* left,
                                        const CM_POINT* right,
                                        S32 pad);

static CM_POINT cm_l_heap_draw_node(CMLHeap node, 
                                         const CM_POINT* p, void *hdc);


static S32 cm_l_head_get_npl(CMLHeap h)
{
    if (h == 0)
    {
        return -1;
    }
    else
    {
        return h->npl;
    }
}

static CMLHeap cm_l_heap_create(S32 data)
{
    CMLHeap p = (CMLHeap)CM_MALLOC(sizeof(*p));
    
    if (p != 0)
    {
        cm_memset(p, 0, sizeof(*p));
        p->data = data;
    }

    return p;
}

static CM_POINT cm_l_heap_right_node_pos(S32 x, S32 y,
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

static CM_POINT cm_l_heap_mid_node_pos(S32 x, S32 y,
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

static CM_POINT cm_l_heap_draw_node(CMLHeap node, 
                                         const CM_POINT* p, void *hdc)
{
    CM_POINT pos = {0};

    if (node)
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

        cm_sprintf(text, "%d", node->data);
        text_pos = pos;
        text_pos.x -= 4;
        text_pos.y -= 8;
        cm_gdi_draw_text(hdc, text, cm_strlen(text), &text_pos);
    }

    return pos;
}


void cm_l_heap_free(CMLHeap q)
{
    CM_ASSERT(q != 0);

    if (q != 0)
    {
        cm_l_heap_free(q->left);
        cm_l_heap_free(q->right);
        CM_FREE(q);
    }
    
    return;
}

CMLHeap cm_l_heap_merge(CMLHeap h1, CMLHeap h2)
{
    if (h1 == 0)
    {
        return h2;
    }
    else if (h2 == 0)
    {
        return h1;
    }
    else
    {
        if (h1->data > h2->data)
        {
            h2->right = cm_l_heap_merge(h1, h2->right);

            if (cm_l_head_get_npl(h2->right) > cm_l_head_get_npl(h2->left))
            {
                CMLHeap p = h2->left;

                h2->left = h2->right;
                h2->right = p;
            }
            
            h2->npl = cm_l_head_get_npl(h2->right) + 1;
            return h2;
        }
        else
        {
            return cm_l_heap_merge(h2, h1);
        }
    }
}

CMLHeap cm_l_heap_insert(CMLHeap h, S32 x)
{
    CMLHeap p = cm_l_heap_create(x);
    return cm_l_heap_merge(h, p);
}

CMLHeap cm_l_heap_delete_min(CMLHeap h, S32 *min)
{
    CMLHeap p = 0;

    CM_ASSERT(h != 0);

    if (h != 0)
    {
        if (min != 0)
        {
            *min = h->data;
        }

        p = cm_l_heap_merge(h->left, h->right);
        CM_FREE(h);
    }

    return p;
}

CM_POINT cm_l_heap_print(CMLHeap tree, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc)

{
    S32 border;
    CM_POINT l_pos = {0}, pos, r_pos = {0};

    border = x;
    *r = x;

    pos.x = x;
    pos.y = y;

    if (tree != 0)
    {
        if (tree->left != 0)
        {
            x = x - CM_HORIZONTAL_PAD > left ? x - CM_HORIZONTAL_PAD : left + CM_HORIZONTAL_PAD;

            l_pos = cm_l_heap_print(tree->left, 
                                           x,
                                           y + CM_HORIZONTAL_PAD,
                                           left,
                                           &border,
                                           hdc);
        }

        if (tree->right != 0)
        {
            pos = cm_l_heap_right_node_pos(x, y, &l_pos, CM_HORIZONTAL_PAD);

            r_pos = cm_l_heap_print(tree->right, pos.x, pos.y, border + CM_HORIZONTAL_PAD, &border, hdc);

            *r = r_pos.x + CM_NODE_RADIUS;
        }

        pos = cm_l_heap_mid_node_pos(x, y, &l_pos, &r_pos, CM_HORIZONTAL_PAD);        
        
        pos = cm_l_heap_draw_node(tree, &pos, hdc);
		*r = pos.x > border ? pos.x : border;

        if (tree->left != 0)
        {
            cm_gdi_draw_line(hdc, &l_pos, &pos);
        }

        if (tree->right != 0)
        {
            cm_gdi_draw_line(hdc, &pos, &r_pos);
        }
    }

    return pos;
}

