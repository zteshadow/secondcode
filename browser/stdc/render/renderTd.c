/*
 * renderTd.c
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "renderTable.h"
#include "renderTr.h"
#include "renderTd.h"
#include "renderText.h"
#include "renderImage.h"
#include "renderInput.h"
#include "renderSelect.h"
#include "renderTextarea.h"
#include "renderInline.h"

NRenderTd* renderTd_create(void* node, nid type)
{
    NRenderTd* r = (NRenderTd*)NBK_malloc0(sizeof(NRenderTd));
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_TD;
    r->d.node = node;
    
    r->d.GetStartPoint = renderTd_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderTd_layoutSimple : renderTd_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderTd_paintSimple : renderTd_paint;
    r->d.GetContentWidth = renderTd_getContentWidth;
    r->d.GetAbsXY = renderTd_getAbsXY;
    
    return r;
}

void renderTd_delete(NRenderTd** rt)
{
    NRenderTd* r = *rt;
    NBK_free(r);
    *rt = N_NULL;
}

void renderTd_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    NRenderTable* table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    
    if (byChild) {
        *x = 0;
        *y = 0;
    }
    else {
        *x = rn->r.r;
        if (table->cellspacing > 0)
            *x += table->cellspacing;
        else
            *x -= table->border;
        *y = rn->r.t;
    }
}

void renderTd_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderTd* rt = (NRenderTd*)rn;
    NRenderNode* r;
    NRenderTable* table;
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
        
        rt->text_align = style->text_align;
        
        maxw = renderTr_getContentWidthByTd(rn->parent, rn);
        rn->r.l = 0;
        rn->r.r = maxw;
    }
    else {
        maxw = rect_getWidth(&rn->r);
    }
    
    if (!renderNode_layoutable(rn, maxw))
        return;
    
    if (rn->child && (rn->child->needLayout || rt->beForce))
        return;
    
    r = rn->child;
    tw = th = 0;
    while (r) {
        if (r->display) {
            tw = N_MAX(tw, r->r.r);
            th = N_MAX(th, r->r.b);
        }
        r = r->next;
    }
    
    table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    tw += table->border * 2 + table->cellpadding * 2;
    th += table->border * 2 + table->cellpadding * 2;
    
    if (byChild)
        rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
    else
        rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
    
    rn->r.l = x;
    rn->r.r = x + tw;
    rn->r.t = y;
    rn->r.b = y + th;
    
    rn->ml = rn->r.l;
    rn->mr = rn->ml + maxw;
    rn->mt = rn->r.t;

    rt->beForce = 0;
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderTd_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl;
    NRenderTable* table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    
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

    if (table->border > 0) {
        NCssBox border;
        border.l = border.t = border.r = border.b = table->border;
        rn->border_color_l = (rn->hasBrcolorL) ? rn->border_color_l : colorBlack;
        rn->hasBrcolorL = 1;
        rn->border_color_t = (rn->hasBrcolorT) ? rn->border_color_t : colorBlack;
        rn->hasBrcolorT = 1;
        rn->border_color_r = (rn->hasBrcolorR) ? rn->border_color_r : colorBlack;
        rn->hasBrcolorR = 1;
        rn->border_color_b = (rn->hasBrcolorB) ? rn->border_color_b : colorBlack;
        rn->hasBrcolorB = 1;
        rect_toView(&border, style->zoom);
        renderNode_drawBorder(rn, &border, &pr, page->platform);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}

void renderTd_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    
}

void renderTd_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    
}

typedef struct _NTdGetSizeTask {
    NStyle* style;
    int     size;
    int     text;
    bd_bool    hasText;
} NTdGetSizeTask;

static int td_get_size_cb(NRenderNode* render, void* user, bd_bool* ignore)
{
    NTdGetSizeTask* task = (NTdGetSizeTask*)user;
    
    switch (render->type) {
    case RNT_TEXT:
        task->text += renderText_getDataSize(render, task->style);
        task->hasText = N_TRUE;
        break;
    case RNT_IMAGE:
        task->size += renderImage_getDataSize(render, task->style);
        break;
    case RNT_INPUT:
        task->size += renderInput_getDataSize(render, task->style);
        break;
    case RNT_SELECT:
        task->size += renderSelect_getDataSize(render, task->style);
        break;
    case RNT_TEXTAREA:
        task->size += renderTextarea_getDataSize(render, task->style);
        break;
    }
    
    return 0;
}

int16 renderTd_getDataSize(NRenderNode* rn, NStyle* style)
{
    NTdGetSizeTask task;
    task.style = style;
    task.size = 0;
    task.text = 0;
    task.hasText = N_FALSE;
    rtree_traverse_depth(rn, td_get_size_cb, N_NULL, &task);
    
    if (task.hasText) {
        if (task.text < 10)
            task.size += task.text;
        else
            task.size += 10;
    }
    
    return (int16)task.size;
}

void renderTd_getAbsXY(NRenderNode* rn, coord* x, coord* y)
{
    NRenderTable* table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    *x += rn->r.l + table->border + table->cellpadding;
    *y += rn->r.t + table->border + table->cellpadding;
}

coord renderTd_getContentWidth(NRenderNode* rn)
{
    NRenderTable* table = (NRenderTable*)renderNode_getParent(rn->parent, RNT_TABLE);
    return rect_getWidth(&rn->r) - table->border * 2 - table->cellpadding * 2;
}

void renderTd_setMaxRightOfChilds(NRenderNode* rn, coord mr)
{
    NRenderNode* r = rn->child;
    while (r) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            if (r->type == RNT_TEXT)
                renderText_setMaxRight(r, mr);
            else if (r->type == RNT_INLINE)
                renderInline_setMaxRight(r, mr);
            else
                r->mr = mr;
        }
        r = r->next;
    }
}
