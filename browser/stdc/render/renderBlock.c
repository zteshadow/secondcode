/*
 * renderBlock.c
 *
 *  Created on: 2010-12-29
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../inc/nbk_ctlPainter.h"
#include "../css/css_val.h"
#include "../css/color.h"
#include "../css/css_helper.h"
#include "../dom/xml_atts.h"
#include "../dom/attr.h"
#include "../dom/node.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../dom/page.h"
#include "../tools/dump.h"
#include "renderBlock.h"
#include "renderInline.h"
#include "renderText.h"
#include "imagePlayer.h"

#ifdef NBK_MEM_TEST
int renderBlock_memUsed(const NRenderBlock* rb)
{
    int size = 0;
    if (rb) {
        size = sizeof(NRenderBlock);
    }
    return size;
}
#endif

NRenderBlock* renderBlock_create(void* node, nid type)
{
    NRenderBlock* rb = (NRenderBlock*)NBK_malloc0(sizeof(NRenderBlock));
    N_ASSERT(rb);
    
    renderNode_init(&rb->d);
    
    rb->d.type = RNT_BLOCK;
    rb->d.node = node;

    rb->d.GetStartPoint = renderBlock_getStartPoint;
    rb->d.Layout = (type >= NEDOC_FULL) ? renderBlock_layoutSimple : renderBlock_layout;
    rb->d.Paint = (type >= NEDOC_FULL) ? renderBlock_paintSimple : renderBlock_paint;
    rb->d.GetContentWidth = renderBlock_getContentWidth;
    rb->d.GetAbsXY = renderBlock_getAbsXY;
    
    return rb;
}

void renderBlock_delete(NRenderBlock** rb)
{
    NRenderBlock* r = *rb;
    NBK_free(r);
    *rb = N_NULL;
}

void renderBlock_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    if (byChild) {
        // start point inside block
        *x = 0;
        *y = 0;
    }
    else if (rn->display == CSS_DISPLAY_NONE) {
        *x = 0;
        *y = 0;
        if (rn->prev)
            rn->prev->GetStartPoint(rn->prev, x, y, N_FALSE);
    }
    else {
        // start point follow block
        if (rn->display == CSS_DISPLAY_INLINE_BLOCK) {
            *x = rn->r.r;
            *y = rn->r.t;
        }
        else {
            *x = 0;
            *y = rn->r.b;
        }
    }
}

static void block_align_line(NRenderNode* begin, NRenderNode* end, coord dx, coord maxb, bd_bool br)
{
    NRenderNode* r = begin;
    coord dy, w;
    
    while (r && r != end) {
        if (r->display && r->flo == CSS_FLOAT_NONE && r->type != RNT_BR) {
            w = rect_getWidth(&r->r);
            r->r.l = r->ml + dx;
            r->r.r = r->r.l + w;
            
            dy = calcVertAlignDelta(maxb, r->r.b, r->vert_align);
            r->r.t += dy;
            r->r.b += dy;
            
            if (r->type == RNT_TEXT)
                renderText_alignHead((NRenderText*)r, dx, maxb);
            else if (r->type == RNT_INLINE)
                renderInline_alignHead((NRenderInline*)r, dx, maxb);
        }
        r = r->next;
    }
    
    if (r && br) {
        if (r->type == RNT_TEXT) // adjust first line of a multi-lines text
            renderText_alignHead((NRenderText*)r, dx, maxb);
        else if (r->type == RNT_INLINE)
            renderInline_alignHead((NRenderInline*)r, dx, maxb);
    }
}

void block_align(NRenderNode* rn, nid ha)
{
    NRenderNode *b, *r;
    coord w, ml, mr, mb, x, y, hw;
    bd_bool br, found;
    NRect box;
    
    b = N_NULL;
    r = rn->child;
    w = mr = mb = 0;
    ml = N_MAX_COORD;
    y = -1;
    found = N_FALSE;
    while (r) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            br = N_FALSE;
            if (b == N_NULL)
                b = r;
            if (y == -1)
                y = r->mt;
            
            if (y == r->mt) {
                // at same line
                if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
                    box = renderText_getHeadAlignRect((NRenderText*)r, &hw);
                    w += hw;
                    ml = N_MIN(ml, box.l);
                    mr = N_MAX(mr, box.r);
                    mb = N_MAX(mb, box.b);
                    br = N_TRUE;
                    found = N_TRUE;
                }
                else if (r->type == RNT_INLINE && renderInline_isBreak((NRenderInline*)r)) {
                    box = renderInline_getHeadAlignRect((NRenderInline*)r, &hw);
                    w += hw;
                    ml = N_MIN(ml, box.l);
                    mr = N_MAX(mr, box.r);
                    mb = N_MAX(mb, box.b);
                    br = N_TRUE;
                    found = N_TRUE;
                }
                else {
                    w += rect_getWidth(&r->r);
                    ml = N_MIN(ml, r->ml);
                    mr = N_MAX(mr, r->mr);
                    mb = N_MAX(mb, r->r.b);
                    if (r->type == RNT_BR)
                        found = N_TRUE;
                }
            }
            else
                found = N_TRUE;
            
            if (found) {
                found = N_FALSE;
                
                block_align_line(b, r, calcHoriAlignDelta(mr - ml, w, ha), mb, br);

                b = N_NULL;
                w = mr = mb = 0;
                ml = N_MAX_COORD;
                y = -1;
                
                if (br) {
                    if (r->type == RNT_TEXT) {
                        NRenderText* rt = (NRenderText*)r;
                        renderText_alignBody(rt, ha);
                        w = renderText_alignTail(rt, ha);
                        box = renderText_getTailAlignRect(rt, N_NULL);
                        ml = box.l;
                        mr = box.r;
                        mb = box.b;
                        y = rt->sy;
                    }
                    else if (r->type == RNT_INLINE) {
                        NRenderInline* ri = (NRenderInline*)r;
                        renderInline_alignBody(ri, ha);
                        w = renderInline_alignTail(ri, ha);
                        box = renderInline_getTailAlignRect(ri);
                        ml = box.l;
                        mr = box.r;
                        mb = box.b;
                        r->GetStartPoint(r, &x, &y, N_FALSE);
                    }
                }
                else if (   (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r))
                         || (r->type == RNT_INLINE && renderInline_isBreak((NRenderInline*)r)) ) {
                    continue;
                }
                else {
                    b = r;
                    y = r->mt;
                    w = rect_getWidth(&r->r);
                    ml = N_MIN(ml, r->ml);
                    mr = N_MAX(mr, r->mr);
                    mb = N_MAX(mb, r->r.b);
                }
            }
        }
        
        r = r->next;
    }
    
            if (b)
                block_align_line(b, N_NULL, calcHoriAlignDelta(mr - ml, w, ha), mb, N_FALSE);
}

// margin overlay
static coord block_calc_y_with_overlay(NRenderNode* rn, coord y)
{
    coord yy = y;
    if (   rn->prev && rn->prev->type == RNT_BLOCK && rn->display
        && (rn->clr || rn->prev->flo == CSS_FLOAT_NONE)) {
        // margin overlay
        NRenderBlock* pr = (NRenderBlock*)rn->prev;
        coord delta = N_MIN(pr->margin.b, ((NRenderBlock*)rn)->margin.t);
        yy -= delta;
    }
    return yy;
}

void renderBlock_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderNode* r;
    NRenderBlock* rb = (NRenderBlock*)rn;
    coord tw, th;
    coord maxw;
    NRect area;
    bd_bool hasAA = N_FALSE, parAA = N_FALSE;
    
    if (!rn->needLayout && !force)
        return;
    
    /*
     * layout:
     * 1. initializate width
     * 2. layout child
     *    adjust pos & size
     */
    
    if (force) {
        if (rb->beForce)
            rb->beForce = 0;
        else {
            rb->beForce = 1;
            rn->init = 0;
        }
    }
    
    if (!rn->init) {
        NNode* node = (NNode*)rn->node;
        
        rn->init = 1;
        
        renderNode_calcStyle(rn, style);
        
        rb->text_align = style->text_align;
        rb->margin = style->margin;
        rb->border = style->border;
        rb->padding = style->padding;
        rb->hasWidth = rb->hasHeight = 0;
        rb->overflow = style->overflow;
        
        rb->ne_display = (attr_getValueStr(node->atts, ATTID_NE_DISPLAY)) ? 1 : 0;

        maxw = (rn->parent) ? rn->parent->GetContentWidth(rn->parent) : rect_getWidth(&rn->r);

        tw = maxw;
        if (style->width.type) {
            rb->hasWidth = 1;
            tw = css_calcValue(style->width, tw, style, tw);
        }

        th = 0;
        if (style->height.type) {
            rb->hasHeight = 1;
            th = css_calcValue(style->height, th, style, th);
        }
        
        if (rn->display == CSS_DISPLAY_INLINE_BLOCK && style->max_width.type)
            tw = N_MIN(tw, css_calcValue(style->max_width, maxw, style, maxw));
        
        if (rb->overflow == CSS_OVERFLOW_HIDDEN && tw && th)
            rect_set(style->clip, 0, 0, tw, th);
        
        if (rb->hasWidth) {
            tw += rb->margin.l + rb->margin.r \
                  + rb->border.l + rb->border.r \
                  + rb->padding.l + rb->padding.r;
            if (tw > maxw)
                tw = maxw;
        }

        if (rb->hasHeight)
            th += rb->margin.t + rb->margin.b \
                + rb->border.t + rb->border.b \
                + rb->padding.t + rb->padding.b;

        rn->r.l = 0;
        rn->r.r = tw;
        rn->r.t = 0;
        rn->r.b = th;
        rn->mr = maxw;
    }
    else {
        maxw = rn->mr;
    }
    
    if (!renderNode_layoutable(rn, maxw))
        return;
    
    if ((rn->child && rb->beForce) || renderNode_isChildsNeedLayout(rn->child))
        return;
    
    if (rn->flo > CSS_FLOAT_NONE && stat->aux[stat->ld] == N_NULL)
        stat->aux[stat->ld] = rn;
    if (rn->clr)
        stat->aux[stat->ld] = N_NULL;
    
    r = rn->child;
    tw = th = 0;
    while (r) {
        if (r->display) {
            tw = N_MAX(tw, r->r.r);
            th = N_MAX(th, r->r.b);
        }
        r = r->next;
    }
    
    if (rb->hasWidth)
        tw = rect_getWidth(&rn->r);
    else
        tw += rb->margin.l + rb->margin.r \
                + rb->border.l + rb->border.r \
                + rb->padding.l + rb->padding.r;
    
    if (rb->hasHeight)
        th = rect_getHeight(&rn->r);
    else
    th += rb->margin.t + rb->margin.b \
        + rb->border.t + rb->border.b \
        + rb->padding.t + rb->padding.b;
    
    if (!rn->clr) {
        if (stat->ld >= 0 && stat->aux[stat->ld]) {
            renderNode_getAvailArea(stat->aux[stat->ld], rn, &area);
            hasAA = N_TRUE;
        }
        else if (rn->parent && renderNode_getAvailAreaByChild(stat, rn->parent, &area)) {
            hasAA = N_TRUE;
            parAA = N_TRUE;
        }
    }
    
    if (hasAA) {
        
        bd_bool turn = N_TRUE;
        
        if (parAA) {
            coord y = area.b;
            if (rn->prev) {
                y = renderNode_getMaxBottomByLine(rn->prev, 0);
                y = block_calc_y_with_overlay(rn, y);
            }
            rn->r.l = 0;
            rn->r.r = tw;
            rn->r.t = N_MAX(y, area.b);
            rn->r.b = rn->r.t + th;
            rn->mr = maxw;
        }
        else {
            if (rn->prev)
                turn = (rn->prev->flo == CSS_FLOAT_NONE) ? N_TRUE : N_FALSE;
            
            if (rn->flo == CSS_FLOAT_NONE) {
                rn->r.l = 0;
                rn->r.r = tw;
                rn->r.t = (turn) ? area.b : area.t;
                rn->r.b = rn->r.t + th;
                rn->mr = maxw;
                
                stat->aux[stat->ld] = N_NULL;
            }
            else if (!turn && rect_getWidth(&area) >= tw) {
                if (rn->flo <= CSS_FLOAT_LEFT) {
                    rn->r.l = area.l;
                    rn->r.r = rn->r.l + tw;
                }
                else {
                    rn->r.l = area.r - tw;
                    rn->r.r = area.r;
                }
                rn->r.t = area.t;
                rn->r.b = rn->r.t + th;
                rn->mr = area.r;
            }
            else {
                if (rn->flo <= CSS_FLOAT_LEFT) {
                    rn->r.l = 0;
                    rn->r.r = tw;
                }
                else {
                    rn->r.l = maxw - tw;
                    rn->r.r = maxw;
                }
                rn->r.t = area.b;
                rn->r.b = rn->r.t + th;
                rn->mr = maxw;
            }
        }
    }
    else {
        
        area.l = area.t = 0;
        
        if (rn->prev) {
            if (rn->display == CSS_DISPLAY_INLINE_BLOCK) {
                if (maxw - rn->prev->r.r >= tw) {
                    area.l = rn->prev->r.r;
                    area.t = rn->prev->r.t;
                }
                else {
                    area.t = renderNode_getMaxBottomByLine(rn->prev, rn->clr);
                    area.t = block_calc_y_with_overlay(rn, area.t);
                }
            }
            else {
                area.t = renderNode_getMaxBottomByLine(rn->prev, rn->clr);
                area.t = block_calc_y_with_overlay(rn, area.t);
            }
        }
        else if (rn->parent) {
            rn->parent->GetStartPoint(rn->parent, &area.l, &area.t, N_TRUE);
        }
        else { // root
            if (((NView*)style->view)->optWidth > 0)
                maxw = tw;
        }
        
        rn->r.t = area.t;
        rn->r.b = rn->r.t + ((th) ? th : 1);
        rn->r.l = area.l;
        rn->r.r = rn->r.l + \
                  ((rn->display == CSS_DISPLAY_INLINE_BLOCK || rb->hasWidth) ? tw : maxw);
        rn->mr = maxw;
        
        rect_move(style->clip, style->clip->l + area.l, style->clip->t + area.t);
        rect_intersect(&rn->r, style->clip);
    }
    
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    
    if (rn->parent == N_NULL) {
        NBK_helper_getViewableRect(
            ((NPage*)((NView*)style->view)->page)->platform, &area);
        if (rn->r.b < rect_getHeight(&area))
            rn->r.b = rect_getHeight(&area);
    }
    
    block_align(rn, rb->text_align);
    
    rb->beForce = 0;
    rn->needLayout = 0;
    if (rn->parent)
        rn->parent->needLayout = 1;
}

void renderBlock_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl, border;
    NRenderBlock* rb = (NRenderBlock*)rn;
    NRect area;
    bd_bool drawAA = N_FALSE;
    
    if (rn->display == CSS_DISPLAY_NONE)
        return;
    
    renderNode_getAbsPos(rn->parent, &x, &y);
    
    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    NBK_memset(&area, 0, sizeof(NRect));
    if (stat->aux[stat->ld]) {
        renderNode_getAvailArea(stat->aux[stat->ld], rn, &area);
        drawAA = (area.t >= rn->r.t) ? N_TRUE : N_FALSE;
    }
    
    if (style->ne_fold)
        NBK_gdi_setBrushColor(page->platform, &style->background_color);
    else if (rn->hasBgcolor)
        NBK_gdi_setBrushColor(page->platform, &rn->background_color);
    
    if (drawAA && !style->highlight) {
        pr.l = x + N_MAX(area.l, rn->r.l);
        pr.t = y + area.t;
        pr.r = pr.l + N_MIN(rect_getWidth(&area), rect_getWidth(&rn->r));
        pr.b = N_MIN(pr.t + rect_getHeight(&area), rn->r.b - rb->margin.b);
        rect_toView(&pr, style->zoom);
        rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
        if (rn->hasBgcolor) {
            cl.l = cl.t = 0;
            cl.r = N_MIN(rect_getWidth(rect), pr.r);
            cl.b = N_MIN(rect_getHeight(rect), pr.b);
            NBK_gdi_setClippingRect(page->platform, &cl);
            NBK_gdi_clearRect(page->platform, &pr);
            NBK_gdi_cancelClippingRect(page->platform);
        }
    }
    
    pr.l = x + rn->r.l + rb->margin.l;
    pr.t = y + rn->r.t + rb->margin.t;
    if (drawAA) pr.t += rect_getHeight(&area);
    pr.r = pr.l + rect_getWidth(&rn->r) - rb->margin.l - rb->margin.r;
    pr.b = pr.t + rect_getHeight(&rn->r) - rb->margin.t - rb->margin.b;
    rect_toView(&pr, style->zoom);
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (rn->hasBgcolor && !style->highlight) {
        if (rn->parent)
            NBK_gdi_clearRect(page->platform, &pr);
        else {
            pr = rn->r;
            rect_toView(&pr, style->zoom);
            NBK_gdi_clearRect(page->platform, &pr);
        }
    }
    
    if (drawAA) pr.t -= rect_getHeight(&area);

    border = rb->border;
    rect_toView(&border, style->zoom);
    renderNode_drawBorder(rn, &border, &pr, page->platform);
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (rn->bgiid != -1 && !style->highlight) {
        pr = rn->r;
        pr.l += rb->margin.l + rb->border.l;
        pr.t += rb->margin.t + rb->border.t;
        pr.r -= rb->margin.r + rb->border.r;
        pr.b -= rb->margin.b + rb->border.b;
        rect_move(&pr, pr.l + x, pr.t + y);
        rect_toView(&pr, style->zoom);
        rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
        renderNode_drawBgImage(rn, &pr, rect, style);
    }
    
//    if (style->highlight)
//        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
    
    if (rn->flo > CSS_FLOAT_NONE && stat->aux[stat->ld] == N_NULL)
        stat->aux[stat->ld] = rn;
    if (rn->flo == CSS_FLOAT_NONE)
        stat->aux[stat->ld] = N_NULL;
}

void renderBlock_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderBlock* rb = (NRenderBlock*)rn;
    NNode* node = (NNode*)rn->node;
    coord x, y;
    NRect* c;
    
    if (!rn->needLayout && !force)
        return;

    if (force) {
        if (rb->beForce)
            rb->beForce = 0;
        else {
            rb->beForce = 1;
            rn->init = 0;
        }
    }
    
    if (rn->init)
        return;
    
    rn->init = 1;
    renderNode_checkAdFF(rn);
    renderNode_calcStyle(rn, style);
    if (rn->hasBgcolor && style->opacity.f != 0)
        rn->background_color.a = 255 / 10 * style->opacity.f;
    
    rb->text_align = style->text_align;
    rb->margin = style->margin;
    rb->border = style->border;
    rb->padding = style->padding;
    rb->overflow = style->overflow;
    
    rb->ne_display = (attr_getValueStr(node->atts, ATTID_NE_DISPLAY)) ? 1 : 0;

    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    
    c = attr_getRect(node->atts, ATTID_TC_RECT);
    if (c) rn->r = *c;
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    rect_intersect(&rn->r, style->clip);
    
    c = attr_getRect(node->atts, ATTID_TC_PCLIP);
    if (c) {
        NRect rt = *c;
        rect_move(&rt, x + rt.l, y + rt.t);
        rect_intersect(&rn->r, &rt);
    }
    
    if (rb->overflow == CSS_OVERFLOW_HIDDEN || c)
        rect_intersect(style->clip, &rn->r);
    
    rn->needLayout = 0;
}

void renderBlock_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderBlock* rb = (NRenderBlock*)rn;
    NRect pr, cl, border;
  
    pr = rn->r;
    if (!rect_isIntersect(&style->dv, &pr))
        return;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    pr = rn->r;
    rect_toView(&pr, style->zoom);
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    
    NBK_gdi_setClippingRect(page->platform, &cl);

    if (   rb->ne_display
        && NBK_paintFoldBackground(page->platform, &pr,
                                   (style->ne_fold) ? NECS_HIGHLIGHT : NECS_NORMAL)) {
        NBK_gdi_cancelClippingRect(page->platform);
        return;
    }

    // draw background color
    if (style->ne_fold) {
        NBK_gdi_setBrushColor(page->platform, &style->background_color);
        NBK_gdi_clearRect(page->platform, &pr);
    }
    else if (rn->hasBgcolor && !style->highlight) {
        NBK_gdi_setBrushColor(page->platform, &rn->background_color);
        NBK_gdi_clearRect(page->platform, &pr);
    }
    
    border = rb->border;
    rect_toView(&border, style->zoom);
    renderNode_drawBorder(rn, &border, &pr, page->platform);

    NBK_gdi_cancelClippingRect(page->platform);
    
    // draw background image
    imagePlayer_setDraw(page->view->imgPlayer, rn->bgiid);
    if (!style->highlight)
        renderNode_drawBgImageFF(rn, &pr, rect, style);

    // draw focus ring
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
}

coord renderBlock_getContentWidth(NRenderNode* rn)
{
    NRenderBlock* rb = (NRenderBlock*)rn;
    return (rect_getWidth(&rn->r) \
            - rb->margin.l - rb->margin.r \
            - rb->border.l - rb->border.r \
            - rb->padding.l - rb->padding.r);
}

void renderBlock_getAbsXY(NRenderNode* rn, coord* x, coord* y)
{
    NRenderBlock* rb = (NRenderBlock*)rn;
    *x += rn->r.l + rb->margin.l + rb->border.l + rb->padding.l;
    *y += rn->r.t + rb->margin.t + rb->border.t + rb->padding.t;
}

//// MUST be called by first child!
//BOOL renderBlock_getAvailAreaByChild(NLayoutStat* stat, NRenderBlock* rb, NRect* area)
//{
//    NRenderNode* rn = (NRenderNode*)rb;
//    int idx = stat->ld - 1;
//    coord de, d, w;
//    NRect a;
//    
//    if (rn->prev && rn->prev->flo == CSS_FLOAT_NONE)
//        return N_FALSE;
//    if (rn->flo != CSS_FLOAT_NONE)
//        return N_FALSE;
//    if (idx < 0 || stat->aux[idx] == N_NULL)
//        return N_FALSE;
//    
//    renderNode_getAvailArea(stat->aux[idx], rn, area);
//    
//    // switch to relative pos
//    de = area->t;
//    area->t -= de;
//    area->b -= de;
//    
//    a = *area;
//    
//    d = rb->d.r.t + rb->margin.t + rb->border.t + rb->padding.t;
//    if (d >= a.b)
//        return N_FALSE;
//    de = a.b - d;
//    area->b = area->t + de;
//    
//    w = rect_getWidth(&a);
//    
//    d = rb->d.r.l + rb->margin.l + rb->border.l + rb->padding.l;
//    de = a.l - d;
//    if (de < 0) {
//        w += de;
//        de = 0;
//    }
//    area->l = de;
//    area->r = area->l + w;
//    
//    d = rb->d.r.r - rb->margin.r - rb->border.r - rb->padding.r;
//    de = a.r - d;
//    if (de > 0) {
//        w -= de;
//        area->r = area->l + w;
//    }
//    
//    return N_TRUE;
//}
