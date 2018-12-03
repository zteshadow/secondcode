/*
 * renderBlank.c
 *
 *  Created on: 2011-5-2
 *      Author: migr
 */

#include "renderBlank.h"
#include "../css/css_val.h"
#include "../dom/document.h"
#include "../dom/xml_atts.h"
#include "../dom/attr.h"

#ifdef NBK_MEM_TEST
int renderBlank_memUsed(const NRenderBlank* rb)
{
    int size = 0;
    if (rb) {
        size = sizeof(NRenderBlank);
    }
    return size;
}
#endif

NRenderBlank* renderBlank_create(void* node, nid type)
{
    NRenderBlank* r = (NRenderBlank*)NBK_malloc0(sizeof(NRenderBlank));
    N_ASSERT(r);
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_BLANK;
    r->d.node = node;
    
    r->d.Layout = (type >= NEDOC_FULL) ? renderBlank_layoutSimple : renderBlank_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderBlank_paintSimple : renderBlank_paint;
    r->d.GetContentWidth = renderBlank_getContentWidth;
    
    return r;
}

void renderBlank_delete(NRenderBlank** render)
{
    NRenderBlank* r = *render;
    NBK_free(r);
    *render = N_NULL;
}

void renderBlank_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    rn->needLayout = 0;
}

void renderBlank_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
}

void renderBlank_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderBlank* rb = (NRenderBlank*)rn;
    NNode* n = (NNode*)rn->node;
    coord x, y;
    NRect* c;
    
    if (!rn->needLayout && !force)
        return;
    
    rb->overflow = style->overflow;
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);

    c = attr_getRect(n->atts, ATTID_TC_RECT);
    if (c) rn->r = *c;
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    rect_intersect(&rn->r, style->clip);
    
    if (rb->overflow == CSS_OVERFLOW_HIDDEN)
        rect_intersect(style->clip, &rn->r);

    rn->needLayout = 0;
}

void renderBlank_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NRect pr;
    
    if (rn->bgiid == -1)
        return;
    
    pr = rn->r;
    if (!rect_isIntersect(&style->dv, &pr))
        return;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    // draw background image
    imagePlayer_setDraw(((NView*)style->view)->imgPlayer, rn->bgiid);
    renderNode_drawBgImageFF(rn, &pr, rect, style);
}

coord renderBlank_getContentWidth(NRenderNode* rn)
{
    return rn->parent->GetContentWidth(rn->parent);
}
