/*
 * renderA.c
 *
 *  Created on: 2011-8-22
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../css/color.h"
#include "../css/css_helper.h"
#include "../css/css_val.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../dom/attr.h"
#include "../dom/node.h"
#include "../dom/xml_atts.h"
#include "renderA.h"
#include "imagePlayer.h"

NRenderA* renderA_create(void* node, nid type)
{
    NRenderA* r = (NRenderA*)NBK_malloc0(sizeof(NRenderA));
    N_ASSERT(r);
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_A;
    r->d.node = node;
    
    r->d.GetStartPoint = renderA_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderA_layoutSimple : renderA_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderA_paintSimple : renderA_paint;
    
    return r;
}

void renderA_delete(NRenderA** ra)
{
    NRenderA* r = *ra;
    NBK_free(r);
    *ra = N_NULL;
}

void renderA_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
}

void renderA_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
}

void renderA_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect)
{
}

void renderA_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderA* ra = (NRenderA*)rn;
    NNode* n = (NNode*)rn->node;
    coord x, y;
    NRect* c;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    
    ra->overflow = style->overflow;
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);

    c = attr_getRect(n->atts, ATTID_TC_RECT);
    if (c) rn->r = *c;
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    rect_intersect(&rn->r, style->clip);

    if (ra->overflow == CSS_OVERFLOW_HIDDEN)
        rect_intersect(style->clip, &rn->r);

    rn->needLayout = 0;
}

void renderA_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRect pr;
    
    pr = rn->r;
    if (!rect_isIntersect(&style->dv, &pr))
        return;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    if (rn->child == N_NULL && style->highlight) {
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
        return;
    }

    // draw background image
    if (rn->bgiid != -1) {
        imagePlayer_setDraw(page->view->imgPlayer, rn->bgiid);
        renderNode_drawBgImageFF(rn, &pr, rect, style);
    }
}
