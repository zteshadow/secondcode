/*
 * renderTr.c
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../css/css_val.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../tools/dump.h"
#include "renderBlock.h"
#include "renderTable.h"
#include "renderTr.h"
#include "renderTd.h"

NRenderTr* renderTr_create(void* node, nid type)
{
    NRenderTr* r = (NRenderTr*)NBK_malloc0(sizeof(NRenderTr));
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_TR;
    r->d.node = node;

    r->d.GetStartPoint = renderTr_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderTr_layoutSimple : renderTr_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderTr_paintSimple : renderTr_paint;
    
    return r;
}

void renderTr_delete(NRenderTr** rt)
{
    NRenderTr* r = *rt;
    NBK_free(r);
    *rt = N_NULL;
}

void renderTr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderTr* rt = (NRenderTr*)rn;
    NRenderNode* r;
    coord maxw, tw, th, x, y;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    
    if (!rn->needLayout && !force)
        return;

    if (force) {
        if (rt->beForce)
            rt->beForce = 0;
        else {
            rt->beForce = 1;
            rn->init = 0;
        }
    }
    
    if (!rn->init) {
        rn->init = 1;
        
        renderNode_calcStyle(rn, style);
        
        maxw = rn->parent->GetContentWidth(rn->parent);
        rn->r.l = 0;
        rn->r.r = maxw;
    }
    else {
        maxw = rect_getWidth(&rn->r);
    }
    
    if (rn->child && (rn->child->needLayout || rt->beForce))
        return;

    r = rn->child;
    tw = th = 0;
    while (r) {
        if (r->display && r->type == RNT_TD) {
            tw = N_MAX(tw, r->r.r);
            th = N_MAX(th, r->r.b);
        }
        r = r->next;
    }
    
    // adjust cell height
    r = rn->child;
    while (r) {
        if (r->display && r->type == RNT_TD) {
            r->r.b = r->r.t + th;
        }
        r = r->next;
    }
    
    if (byChild)
        rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
    else
        rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
    
    rn->r.l = x;
    rn->r.r = x + tw;
    rn->r.t = y;
    rn->r.b = y + th;
    
    rn->ml = rn->r.l;
    rn->mr = maxw;
    rn->mt = rn->r.t;

    rt->beForce = 0;
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderTr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl;
    
    if (rn->display == CSS_DISPLAY_NONE)
        return;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (rn->hasBgcolor) {
        NBK_gdi_setBrushColor(page->platform, &rn->background_color);
        NBK_gdi_clearRect(page->platform, &pr);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (rn->bgiid != -1 && !style->highlight) {
        renderNode_drawBgImage(rn, &pr, rect, style);
    }
}

void renderTr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    
}

void renderTr_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    
}

int renderTr_getColumnNum(NRenderNode* rn)
{
    NRenderNode* r = rn->child;
    int n = 0;
    
    while (r) {
        if (r->type == RNT_TD)
            n++;
        r = r->next;
    }
    return n;
}

void renderTr_predictColsWidth(NRenderNode* rn, int16* colsWidth, int maxCols, NStyle* style)
{
    NRenderNode* r = rn->child;
    int i = 0;
//    NPage* page = (NPage*)((NView*)style->view)->page;
    
    while (r) {
        if (r->type == RNT_TD) {
            int16 size = renderTd_getDataSize(r, style);
            if (i == maxCols - 1)
                break;
//            dump_int(page, i);
//            dump_int(page, size);
//            dump_return(page);
            colsWidth[i] = N_MAX(size, colsWidth[i]);
            i++;
        }
        r = r->next;
    }
}

coord renderTr_getContentWidthByTd(NRenderNode* tr, NRenderNode* td)
{
    NRenderTable* table = (NRenderTable*)renderNode_getParent(tr->parent, RNT_TABLE);
    NRenderNode* r;
    int16 i;
    
    r = tr->child;
    i = 0;
    while (r) {
        if (r->type == RNT_TD) {
            if (r == td) {
                return renderTable_getColWidth(table, i);
            }
            i++;
        }
        r = r->next;
    }
    
    return 0;
}

void renderTr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    NRenderTable* table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    
    if (byChild) {
        *x = 0;
        *y = 0;
    }
    else {
        *x = 0;
        *y = rn->r.b;
        if (table->cellspacing > 0)
            *y += table->cellspacing;
        else
            *y -= table->border;
    }
}
