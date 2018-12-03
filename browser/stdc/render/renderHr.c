/*
 * renderHr.c
 *
 *  Created on: 2011-4-4
 *      Author: migr
 */

#include "../inc/config.h"
#include "../css/css_val.h"
#include "../css/color.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "renderHr.h"

#ifdef NBK_MEM_TEST
int renderHr_memUsed(const NRenderHr* rh)
{
    int size = 0;
    if (rh) {
        size = sizeof(NRenderHr);
    }
    return size;
}
#endif

NRenderHr* renderHr_create(void* node, nid type)
{
    NRenderHr* r = (NRenderHr*)NBK_malloc0(sizeof(NRenderHr));
    N_ASSERT(r);
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_HR;
    r->d.node = node;

    r->d.GetStartPoint = renderHr_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderHr_layoutSimple : renderHr_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderHr_paintSimple : renderHr_paint;
    
    return r;
}

void renderHr_delete(NRenderHr** rh)
{
    NRenderHr* r = *rh;
    NBK_free(r);
    *rh = N_NULL;
}

void renderHr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    N_ASSERT(byChild == 0);
    
    *x = rn->r.l;
    *y = rn->r.b;
}

void renderHr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    coord y;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    
    if (rn->hasBrcolorT)
        rn->color = rn->border_color_t;
    else
        rn->color = style->color;
    
    y = (rn->prev) ? renderNode_getMaxBottomByLine(rn->prev, 0) : 0;
    
    rn->r.t = y;
    rn->r.b = rn->r.t + 2 + DEFAULT_LINE_SPACE;
    rn->r.l = 0;
    rn->r.r = rn->parent->GetContentWidth(rn->parent);
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    rn->mr = rn->r.r;
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderHr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    pr.t += DEFAULT_LINE_SPACE >> 1;
    pr.b = pr.t + 2;
    rect_toView(&pr, style->zoom);
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    NBK_gdi_setBrushColor(page->platform, &rn->color);
    NBK_gdi_clearRect(page->platform, &pr);
    
    NBK_gdi_cancelClippingRect(page->platform);
}

void renderHr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    rn->needLayout = 0;
}

void renderHr_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    
}
