/*
 * renderBr.c
 *
 *  Created on: 2011-1-1
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../css/css_val.h"
#include "../dom/document.h"
#include "renderBr.h"

#ifdef NBK_MEM_TEST
int renderBr_memUsed(const NRenderBr* rb)
{
    int size = 0;
    if (rb) {
        size = sizeof(NRenderBr);
    }
    return size;
}
#endif

NRenderBr* renderBr_create(void* node, nid type)
{
    NRenderBr* rb = (NRenderBr*)NBK_malloc0(sizeof(NRenderBr));
    N_ASSERT(rb);
    
    renderNode_init(&rb->d);
    
    rb->d.type = RNT_BR;
    rb->d.node = node;
    
    rb->d.GetStartPoint = renderBr_getStartPoint;
    rb->d.Layout = (type >= NEDOC_FULL) ? renderBr_layoutSimple : renderBr_layout;
    rb->d.Paint = renderBr_paint;
    
    return rb;
}

void renderBr_delete(NRenderBr** rb)
{
    NRenderBr* r = *rb;
    NBK_free(r);
    *rb = N_NULL;
}

void renderBr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    N_ASSERT(byChild == 0);
    
    *x = 0;
    *y = rn->r.b;
}

void renderBr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    coord x, y, lh = style->font_size, mb, h;
    
    if (!rn->needLayout && !force)
        return;
    
    if (rn->prev) {
        rn->prev->GetStartPoint(rn->prev, &x, &y, N_FALSE);
        if (rn->prev->type != RNT_BR) {
            mb = renderNode_getMaxBottomByLine(rn->prev, N_FALSE);
            h = mb - y;
            lh = N_MAX(lh, h);
        }
    }
    else {
        rn->parent->GetStartPoint(rn->parent, &x, &y, N_TRUE);
        h = renderNode_getFollowLineHeight(rn->parent);
        lh = N_MAX(lh, h);
    }
    
    rn->r.l = x;
    rn->r.t = y;
    rn->r.r = rn->r.l;
    rn->r.b = rn->r.t + lh;
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    rn->mr = rn->r.r;
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderBr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
}

void renderBr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    rn->needLayout = 0;
}
