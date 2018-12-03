/*
 * renderTable.c
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../css/color.h"
#include "../css/cssSelector.h"
#include "../css/css_helper.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../tools/dump.h"
#include "renderBlock.h"
#include "renderTable.h"
#include "renderTr.h"
#include "renderTd.h"

#define DUMP_FUNCTIONS      0
#define NBK_TABLE_MAX_COLS  8

NRenderTable* renderTable_create(void* node, nid type)
{
    NRenderTable* r = (NRenderTable*)NBK_malloc0(sizeof(NRenderTable));
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_TABLE;
    r->d.node = node;
    
    r->d.GetStartPoint = renderTable_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderTable_layoutSimple : renderTable_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderTable_paintSimple : renderTable_paint;
    r->d.GetContentWidth = renderTable_getContentWidth;
    r->d.GetAbsXY = renderTable_getAbsXY;
    
    return r;
}

void renderTable_delete(NRenderTable** rt)
{
    NRenderTable* r = *rt;
    if (r->cols_width)
        NBK_free(r->cols_width);
    NBK_free(r);
    *rt = N_NULL;
}

#if DUMP_FUNCTIONS
static void table_dump_cols_width(NRenderTable* table, NStyle* style)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    int i;
    
    dump_char(page, "table column width", -1);
    dump_return(page);
    for (i=0; table->cols_width[i] != -1; i++) {
        dump_int(page, i);
        dump_int(page, table->cols_width[i]);
        dump_return(page);
    }
}
#endif

static int table_max_column(NRenderNode* rn)
{
    NRenderNode* r = rn->child;
    int max = 0;
    
    while (r) {
        if (r->type == RNT_TR)
            max = N_MAX(max, renderTr_getColumnNum(r));
        r = r->next;
    }
    return max;
}

static bd_bool table_has_design_width(NRenderTable* table, coord maxw, NStyle* style)
{
    NCssValue *cvs, *cv;
    NRenderNode* r;
    bd_bool set;
    int i, j, n;
    coord w, used;
    
    r = ((NRenderNode*)table)->child;
    while (r) {
        if (r->type == RNT_TR)
            break;
        r = r->next;
    }
    if (r == N_NULL)
        return N_FALSE;
    
    r = r->child;
    while (r) {
        if (r->type == RNT_TD)
            break;
        r = r->next;
    }
    if (r == N_NULL)
        return N_FALSE;
    
    cvs = (NCssValue*)NBK_malloc0(sizeof(NCssValue) * table->cols_num);
    set = N_FALSE;
    i = -1;
    while (r) {
        if (r->type == RNT_TD) {
            i++;
            cv = attr_getCssValue(((NNode*)r->node)->atts, ATTID_WIDTH);
            if (cv && cv->type) {
                cvs[i] = *cv;
                set = N_TRUE;
            }
        }
        r = r->next;
    }
    n = i;
    
    if (set) {
        maxw -= table->cellspacing * i;
        used = 0;
        for (i=j=0; i <= n; i++) {
            if (cvs[i].type) {
                w = css_calcValue(cvs[i], maxw, style, 0);
                table->cols_width[i] = w;
                used += w;
            }
            else
                j++;
        }
        // average allocate rest space for columns
        if (j) {
            w = (maxw - used) / j;
            if (w < 0) w = 0;
            for (i=0; i <= n; i++) {
                if (cvs[i].type == NECVT_NONE)
                    table->cols_width[i] = w;
            }
        }
    }
    
    NBK_free(cvs);
    return set;
}

static void table_predict_column_width(NRenderTable* table, coord maxw, NStyle* style)
{
    NRenderNode* r;
    int i, num;
    
    num = table_max_column((NRenderNode*)table);
    if (num < NBK_TABLE_MAX_COLS)
        num = NBK_TABLE_MAX_COLS;
    
    if (table->cols_num < num) {
        if (table->cols_width)
            NBK_free(table->cols_width);
        table->cols_width = (int16*)NBK_malloc(sizeof(int16) * (num + 1));
        table->cols_num = num;
    }
    NBK_memset(table->cols_width, -1, sizeof(int16) * (table->cols_num + 1));
    
    if (table_has_design_width(table, maxw, style))
        return;
    
    r = ((NRenderNode*)table)->child;
    while (r) {
        if (r->type == RNT_TR) {
            renderTr_predictColsWidth(r, table->cols_width, table->cols_num, style);
        }
        r = r->next;
    }
    
    for (i=num=0; table->cols_width[i] != -1; i++)
        num += table->cols_width[i];
    
    if (num == 0)
        return;
    
    maxw -= table->cellspacing * (i - 1);
    
//    table_dump_cols_width(table, style);
    
    for (i=0; table->cols_width[i] != -1; i++)
        table->cols_width[i] = table->cols_width[i] * maxw / num;
    
//    table_dump_cols_width(table, style);
}

static void table_adjust_column(NRenderTable* table, NStyle* style, bd_bool shrink)
{
    NRenderNode *tr, *td;
    int i;
    coord x;
    
    if (shrink) {
        NBK_memset(table->cols_width, -1, sizeof(int16) * (table->cols_num + 1));
        tr = ((NRenderNode*)table)->child;
        while (tr) {
            if (tr->type == RNT_TR) {
                td = tr->child;
                i = 0;
                while (td && i < table->cols_num) {
                    if (td->type == RNT_TD) {
                        table->cols_width[i] = N_MAX(table->cols_width[i], rect_getWidth(&td->r));
                        i++;
                    }
                    td = td->next;
                }
            }
            tr = tr->next;
        }
    }
    
//    table_dump_cols_width(table, style);
    
    tr = ((NRenderNode*)table)->child;
    while (tr) {
        if (tr->type == RNT_TR) {
            td = tr->child;
            i = x = 0;
            while (td && i < table->cols_num) {
                if (td->type == RNT_TD) {
                    td->r.l = x;
                    td->r.r = x + table->cols_width[i];
                    td->ml = td->r.l;
                    td->mr = td->r.r;
                    renderTd_setMaxRightOfChilds(td,
                        table->cols_width[i] - table->border * 2 - table->cellpadding * 2);
                    block_align(td, ((NRenderTd*)td)->text_align);
                    i++;
                    x = td->r.r;
                    if (table->cellspacing > 0)
                        x += table->cellspacing;
                    else
                        x -= table->border;
                    
                    tr->r.r = N_MAX(tr->r.r, td->r.r);
                }
                td = td->next;
            }
        }
        tr = tr->next;
    }
}

static int table_is_changed_worker(NRenderNode* rn, void* user, bd_bool* ignore)
{
    bd_bool* change = (bd_bool*)user;
    
    switch(rn->type) {
    case RNT_TABLE:
    case RNT_TR:
    case RNT_TD:
        if (rn->needLayout) {
            *change = N_TRUE;
            return 1;
        }
        break;
    default:
        break;
    }
    
    return 0;
}

static bd_bool table_is_changed(NRenderNode* rn)
{
    bd_bool change = N_FALSE;
    rtree_traverse_depth(rn, table_is_changed_worker, N_NULL, &change);
    return change;
}

static int table_relayout_worker(NRenderNode* rn, void* user, bd_bool* ignore)
{
    rn->init = 0;
    rn->needLayout = 1;
    return 0;
}

static void table_relayout(NRenderNode* rn)
{
    rtree_traverse_depth(rn, table_relayout_worker, N_NULL, N_NULL);
}

void renderTable_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderTable* rt = (NRenderTable*)rn;
    NNode* node = (NNode*)rn->node;
    NRenderNode* r;
    coord maxw, tw, th, x, y;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    NCssValue* cv;
    
    if (!rn->needLayout && !force) {
        if (table_is_changed(rn))
            table_relayout(rn);
    }
    
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
        rt->hasWidth = 0;
        
        renderNode_calcStyle(rn, style);
        
        rt->border = attr_getCoord(node->atts, ATTID_BORDER);
        if (rt->border == -1) rt->border = 0;
        rt->cellspacing = attr_getCoord(node->atts, ATTID_CELLSPACING);
        if (rt->cellspacing == -1) rt->cellspacing = 0;
        rt->cellpadding = attr_getCoord(node->atts, ATTID_CELLPADDING);
        if (rt->cellpadding == -1) rt->cellpadding = 1;
        
        maxw = rn->parent->GetContentWidth(rn->parent);
        cv = attr_getCssValue(node->atts, ATTID_WIDTH);
        if (cv && cv->type) {
            maxw = css_calcValue(*cv, maxw, style, maxw);
            rt->hasWidth = 1;
        }
        else if (style->width.type) {
            maxw = css_calcValue(style->width, maxw, style, maxw);
            rt->hasWidth = 1;
        }
        
        rn->r.l = 0;
        rn->r.r = maxw;
        
        maxw -= rt->border * 2 + rt->cellspacing * 2;
        if (rt->cellspacing == 0)
            maxw += rt->border * 2;
        table_predict_column_width(rt, maxw, style);
    }
    else {
        maxw = rect_getWidth(&rn->r);
    }
    
    if (rn->child && (rn->child->needLayout || rt->beForce))
        return;

    table_adjust_column(rt, style, (rt->hasWidth) ? N_FALSE : N_TRUE);
    
    r = rn->child;
    tw = th = 0;
    while (r) {
        if (r->display && r->type == RNT_TR) {
            tw = N_MAX(tw, r->r.r);
            th = N_MAX(th, r->r.b);
        }
        r = r->next;
    }
    
    tw += rt->border * 2 + rt->cellspacing * 2;
    th += rt->border * 2 + rt->cellspacing * 2;
    if (rt->cellspacing == 0) {
        tw -= rt->border * 2;
        th -= rt->border * 2;
    }
    
    if (byChild)
        rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
    else {
        x = 0;
        y = renderNode_getMaxBottomByLine(rn->prev, N_TRUE);
    }
    
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

void renderTable_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl;
    NRenderTable* table = (NRenderTable*)rn;
    
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

void renderTable_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    
}

void renderTable_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    
}

coord renderTable_getColWidth(NRenderTable* table, int16 col)
{
    if (col >= 0 && col < table->cols_num)
        return table->cols_width[col];
    else
        return 0;
}

void renderTable_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    if (byChild) {
        *x = 0;
        *y = 0;
    }
    else {
        *x = 0;
        *y = rn->r.b;
    }
}

coord renderTable_getContentWidth(NRenderNode* rn)
{
    NRenderTable* rt = (NRenderTable*)rn;
    coord w = rect_getWidth(&rn->r);
    if (rt->cellspacing)
        w -= rt->border * 2 + rt->cellspacing * 2;
    return w;
}

void renderTable_getAbsXY(NRenderNode* rn, coord* x, coord* y)
{
    NRenderTable* table = (NRenderTable*)rn;
    *x += rn->r.l;
    *y += rn->r.t;
    if (table->cellspacing) {
        *x += table->border + table->cellspacing;
        *y += table->border + table->cellspacing;
    }
}
