/*
 * renderInline.c
 *
 *  Created on: 2011-10-31
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
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
#include "renderInline.h"
#include "renderText.h"

NRenderInline* renderInline_create(void* node, nid type)
{
    NRenderInline* r = (NRenderInline*)NBK_malloc0(sizeof(NRenderInline));
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_INLINE;
    r->d.node = node;
    
    r->d.GetStartPoint = renderInline_getStartPoint;
    r->d.Layout = renderInline_layout;
    r->d.Paint = renderInline_paint;
    r->d.GetContentWidth = renderInline_getContentWidth;
    r->d.GetAbsXY = renderInline_getAbsXY;
    
    return r;
}

void renderInline_delete(NRenderInline** ri)
{
    NRenderInline* r = *ri;
    NBK_free(r);
    *ri = N_NULL;
}

void renderInline_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    NRenderNode *r;
    
    if (byChild) {
        NRenderNode* p = rn;
        r = p->prev;
        while (!r && p->type == RNT_INLINE) {
            if (p->parent->type == RNT_BLOCK && p->parent->child == rn) {
                r = N_NULL;
                break;
            }
            p = p->parent;
            r = p->prev;
        }
        *x = 0;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE) {
                r->GetStartPoint(r, x, y, N_FALSE);
                break;
            }
            r = r->prev;
        }
        *y = 0;
    }
    else {
        NRenderNode* last;
        r = rn->child;
        last = N_NULL;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE)
                last = r;
            r = r->next;
        }
        if (last) {
            last->GetStartPoint(last, x, y, N_FALSE);
            *y += rn->r.t;
        }
        else {
            *x = rn->r.r;
            *y = rn->r.t;
        }
    }
}

void renderInline_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderNode* r;
    NRenderInline* ri = (NRenderInline*)rn;
    coord ml, mr, tw, th;
    coord maxw;
    NRect area;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    bd_bool hasAA = N_FALSE, parAA = N_FALSE;
    
    if (!rn->needLayout && !force)
        return;
    
    if (force) {
        if (ri->beForce)
            ri->beForce = 0;
        else {
            ri->beForce = 1;
            rn->init = 0;
        }
    }
    
    ri->head = ri->body = ri->tail = N_NULL;
    ri->br = N_FALSE;
    
    if (!rn->init) {
        rn->init = 1;
        
        renderNode_calcStyle(rn, style);
        
        ri->box_l = style->margin.l + style->border.l + style->padding.l;
        ri->box_r = style->margin.r + style->border.r + style->padding.r;

        maxw = rn->parent->GetContentWidth(rn->parent);
        
        tw = maxw;
        if (style->width.type) {
            ri->hasWidth = 1;
            tw = css_calcValue(style->width, tw, style, tw);
            tw += ri->box_l + ri->box_r;
        }
        
        th = 0;
        if (style->height.type) {
            ri->hasHeight = 1;
            th = css_calcValue(style->height, th, style, th);
        }

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
    
    if ((rn->child && ri->beForce) || renderNode_isChildsNeedLayout(rn->child))
        return;
    
    if (rn->flo > CSS_FLOAT_NONE && stat->aux[stat->ld] == N_NULL)
        stat->aux[stat->ld] = rn;
    if (rn->clr)
        stat->aux[stat->ld] = N_NULL;
    
    r = rn->child;
    ml = N_MAX_COORD;
    mr = 0;
    tw = th = 0;
    while (r) {
        if (r->display) {
            ml = N_MIN(ml, r->r.l);
            mr = N_MAX(mr, r->r.r);
            th = N_MAX(th, r->r.b);
        }
        r = r->next;
    }
    if (ml == N_MAX_COORD) ml = 0;
    
    if (ri->hasWidth)
        tw = rect_getWidth(&rn->r);
    else
        tw = (mr - ml) + ri->box_l + ri->box_r;
    
    if (ri->hasHeight)
        th = rect_getHeight(&rn->r);
    
    if (!rn->clr) {
        if (stat->ld >= 0 && stat->aux[stat->ld]) {
            renderNode_getAvailArea(stat->aux[stat->ld], rn, &area);
            hasAA = N_TRUE;
        }
        else if (renderNode_getAvailAreaByChild(stat, rn->parent, &area)) {
            hasAA = N_TRUE;
            parAA = N_TRUE;
        }
    }
    
    if (hasAA) { // float
        
        bd_bool turn = N_TRUE;
        
        if (parAA) {
            coord y = area.b;
            if (rn->prev) {
                y = renderNode_getMaxBottomByLine(rn->prev, 0);
            }
            rn->r.l = 0;
            rn->r.r = maxw;
            rn->r.t = N_MAX(y, area.b);
            rn->r.b = rn->r.t + th;
            rn->mr = maxw;
        }
        else {
            turn = !renderNode_isNearestNodeFloat(rn->prev);
            
            if (rn->flo == CSS_FLOAT_NONE) {
                rn->r.l = 0;
                rn->r.r = maxw;
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
    else { // normal
        bd_bool noTurn = N_FALSE;
        
        if (byChild)
            rn->parent->GetStartPoint(rn->parent, &area.l, &area.t, N_TRUE);
        else
            rn->prev->GetStartPoint(rn->prev, &area.l, &area.t, N_FALSE);
        
        if (rn->child) {
            r = rn->child;
            while (r) {
                if (r->display && r->flo == CSS_FLOAT_NONE) {
                    if (r->r.l > 0) {
                        noTurn = N_TRUE;
                    }
                    else if (r->type == RNT_TEXT) {
                        NRenderText* rt = (NRenderText*)r;
                        if (rt->lines && rt->lines[0].r.l > 0)
                            noTurn = N_TRUE;
                    }
                    break;
                }
                r = r->next;
            }
            
            if (noTurn)
                area.l = ml;
            }
        
        if (noTurn) {
            rn->r.l = area.l;
            rn->r.r = rn->r.l + tw;
            rn->r.t = area.t;
            rn->r.b = rn->r.t + th;
        }
        else {
            coord mb = renderNode_getMaxBottomByLine(rn->prev, 0);
            rn->r.l = 0;
            rn->r.r = tw;
            rn->r.t = mb;
            rn->r.b = mb + th;
        }
        
        rn->mr = maxw;
    }
    
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    ri->beForce = 0;
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderInline_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
}

coord renderInline_getContentWidth(NRenderNode* rn)
{
    NRenderInline* ri = (NRenderInline*)rn;
    return (rect_getWidth(&rn->r) - ri->box_l - ri->box_r);
}

void renderInline_getAbsXY(NRenderNode* rn, coord* x, coord* y)
{
    *x = (rn->flo == CSS_FLOAT_NONE) ? 0 : rn->r.l;
    *y = rn->r.t;
}

bd_bool renderInline_isBreak(NRenderInline* ri)
{
    NRenderNode* r = ((NRenderNode*)ri)->child;
    coord y = -1;
    
    if (r == N_NULL)
        return N_FALSE;

    if (ri->head == N_NULL) {
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE) {
                if (y == -1)
                    y = r->mt;
                if (ri->head == N_NULL)
                    ri->head = r;
                
                if (r->mt != y) {
                    ri->br = N_TRUE;
                    if (ri->body == N_NULL)
                        ri->body = r;
                    ri->tail = r;
                    y = r->mt;
                }
                else if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
                    ri->br = N_TRUE;
                    ri->tail = r;
                }
            }
            r = r->next;
        }
    }

    return ri->br;
}

static void get_head_info(NRenderInline* ri, coord* width, NRect* box)
{
    NRenderNode* r = ri->head;
    coord hw;
    NRect bx;
    
    *width = 0;
    box->l = box->t = N_MAX_COORD;
    box->r = box->b = 0;
    
    while (r && r != ri->body) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
                bx = renderText_getHeadAlignRect((NRenderText*)r, &hw);
                *width += hw;
                box->l = N_MIN(box->l, bx.l);
                box->r = N_MAX(box->r, bx.r);
                box->b = N_MAX(box->b, bx.b);
                break;
            }
            else {
                    *width += rect_getWidth(&r->r);
                box->l = N_MIN(box->l, r->ml);
                box->r = N_MAX(box->r, r->mr);
                box->b = N_MAX(box->b, r->r.b);
            }
        }
        r = r->next;
    }
}

static NRenderNode* get_tail(NRenderInline* ri)
{
    NRenderNode *r, *t;
    
    if (!ri->br)
        return N_NULL;
    
    if (ri->tail == N_NULL) {
        r = ri->head;
        t = N_NULL;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE)
                t = r;
            r = r->next;
        }
        return t;
    }
    else if (ri->tail->type == RNT_TEXT && renderText_isBreak((NRenderText*)ri->tail)) {
        return ri->tail;
    }
    else {
        r = ri->tail->prev;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE) {
                if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r))
                    ;
                else
                    r = N_NULL;
                break;
            }
            r = r->prev;
        }
        return (r) ? r : ri->tail;
    }
}

static void get_tail_info(NRenderInline* ri, NRect* box)
{
    NRenderNode* tail;
    NRenderNode* r;
    NRect bx;
    
    box->l = box->t = N_MAX_COORD;
    box->r = box->b = 0;
    
    tail = get_tail(ri);
    if (tail == N_NULL)
        return;
    
    r = tail;
    if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
        bx = renderText_getTailAlignRect((NRenderText*)r, N_NULL);
        box->l = N_MIN(box->l, bx.l);
        box->t = N_MIN(box->t, bx.t);
        box->r = N_MAX(box->r, bx.r);
        box->b = N_MAX(box->b, bx.b);
        r = r->next;
    }
    while (r) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            box->l = N_MIN(box->l, r->ml);
            box->t = N_MIN(box->t, r->r.t);
            box->r = N_MAX(box->r, r->mr);
            box->b = N_MAX(box->b, r->r.b);
        }
        r = r->next;
    }
}

NRect renderInline_getHeadAlignRect(NRenderInline* ri, coord* width)
{
    coord hw;
    NRect box;
    get_head_info(ri, &hw, &box);
    box.b += ri->d.r.t;
    if (width)
        *width = hw;
    return box;
}

NRect renderInline_getTailAlignRect(NRenderInline* ri)
{
    NRect box;
    get_tail_info(ri, &box);
    box.t += ri->d.r.t;
    box.b += ri->d.r.t;
    return box;
}

void renderInline_alignHead(NRenderInline* ri, coord dx, coord maxb)
{
    NRenderNode* r = ri->head;
    coord w, dy;
    
    maxb -= ri->d.r.t;
    
    while (r && r != ri->body) {
        if (r->display && r->flo == CSS_FLOAT_NONE && r->type != RNT_BR) {
            if (r->type == RNT_TEXT) {
                renderText_alignHead((NRenderText*)r, dx, maxb);
                if (renderText_isBreak((NRenderText*)r))
                    break;
            }
            
                w = rect_getWidth(&r->r);
                r->r.l = r->ml + dx;
                r->r.r = r->r.l + w;
                
                dy = calcVertAlignDelta(maxb, r->r.b, r->vert_align);
                r->r.t += dy;
                r->r.b += dy;
            }
        r = r->next;
    }
}

static NRenderNode* get_body(NRenderInline* ri)
{
    if (!ri->br)
        return N_NULL;
    
    if (ri->body == N_NULL) {
        NRenderNode* r = ri->head;
        NRenderNode* t = N_NULL;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE)
                t = r;
            r = r->next;
        }
        return t;
    }
    else {
        NRenderNode* r = ri->body->prev;
        while (r) {
            if (r->display && r->flo == CSS_FLOAT_NONE) {
                if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r))
                    ;
                else
                    r = N_NULL;
                break;
            }
            r = r->prev;
        }
        return (r) ? r : ri->body;
    }
}

static void inline_align_line(NRenderNode* begin, NRenderNode* end, coord dx, coord maxb, bd_bool br)
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
        }
        r = r->next;
    }
    
    if (r && br) {
        if (r->type == RNT_TEXT) // adjust first line of a multi-lines text
            renderText_alignHead((NRenderText*)r, dx, maxb);
    }
}

void renderInline_alignBody(NRenderInline* ri, nid ha)
{
    NRenderNode* body = get_body(ri);
    NRenderNode* tail = get_tail(ri);
    NRenderNode *b, *r;
    NRenderText* rt;
    coord w, ml, mr, mb, y, hw;
    bd_bool br, found;
    NRect box;
    
    if (body == N_NULL || tail == N_NULL)
        return;
    
    b = N_NULL;
    r = body;
    w = mr = mb = 0;
    ml = N_MAX_COORD;
    y = -1;
    found = N_FALSE;
    if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
        rt = (NRenderText*)r;
        renderText_alignBody(rt, ha);
        w = renderText_alignTail(rt, ha);
        box = renderText_getTailAlignRect(rt, N_NULL);
        mb = box.b;
        r = r->next;
    }
    while (r && r != tail) {
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
                else {
                    w += rect_getWidth(&r->r);
                    mr = N_MAX(mr, r->mr);
                    mb = N_MAX(mb, r->r.b);
                    ml = N_MIN(ml, r->ml);
                    if (r->type == RNT_BR)
                        found = N_TRUE;
                }
            }
            else
                found = N_TRUE;
            
            if (found) {
                found = N_FALSE;
                
                inline_align_line(b, r, calcHoriAlignDelta(mr - ml, w, ha), mb, br);

                b = N_NULL;
                w = mr = mb = 0;
                ml = N_MAX_COORD;
                y = -1;
                
                if (br) {
                    if (r->type == RNT_TEXT) {
                    rt = (NRenderText*)r;
                        y = rt->sy;
                    renderText_alignBody(rt, ha);
                    w = renderText_alignTail(rt, ha);
                        box = renderText_getTailAlignRect(rt, N_NULL);
                        ml = box.l;
                        mr = box.r;
                        mb = box.b;
                    }
                }
                else if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
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
    
    if (r == tail && r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
        rt = (NRenderText*)r;
        box = renderText_getHeadAlignRect(rt, N_NULL);
        mb = N_MAX(mb, box.b);
        renderText_alignHead(rt, calcHoriAlignDelta(mr - ml, w, ha), mb);
        renderText_alignBody(rt, ha);
    }
}

coord renderInline_alignTail(NRenderInline* ri, nid ha)
{
    NRenderNode* tail = get_tail(ri);
    NRenderNode* r;
    coord w, ml, mr, mb, y, dx, dy, tw, hw;
    bd_bool br;
    NRect box;
    
    if (tail == N_NULL)
        return 0;
    
    // find y and total width of tail
    w = mr = mb = 0;
    ml = N_MAX_COORD;
    y = -1;
    r = tail;
    while (r) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            if (r->type == RNT_TEXT) {
                NRenderText* rt = (NRenderText*)r;
                y = rt->sy;
                box = renderText_getTailAlignRect(rt, &hw);
                w += hw;
                ml = N_MIN(ml, box.l);
                mr = N_MAX(mr, box.r);
                mb = N_MAX(mb, box.b);
            }
            else {
                y = r->mt;
                w += rect_getWidth(&r->r);
                ml = N_MIN(ml, r->ml);
                mr = N_MAX(mr, r->mr);
                mb = N_MAX(mb, r->r.b);
            }
        }
        r = r->next;
    }
    y += ri->d.r.t;
    mb += ri->d.r.t;
    
    // find nodes follow inline element in same line
    br = N_FALSE;
    r = ((NRenderNode*)ri)->next;
    while (r && y == r->mt && !br) {
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
                br = N_TRUE;
                box = renderText_getHeadAlignRect((NRenderText*)r, &hw);
                w += hw;
                mb = N_MAX(mb, box.b);
            }
            else if (r->type == RNT_INLINE && renderInline_isBreak((NRenderInline*)r)) {
                br = N_TRUE;
                box = renderInline_getHeadAlignRect((NRenderInline*)r, &hw);
                w += hw;
                mb = N_MAX(mb, box.b);
            }
            else {
                if (r->type == RNT_BR)
                    br = N_TRUE;
                w += rect_getWidth(&r->r);
                mb = N_MAX(mb, r->r.b);
            }
        }
        r = r->next;
    }
    mb -= ri->d.r.t;
    
    // align nodes in tail
    dx = calcHoriAlignDelta(mr - ml, w, ha);
    r = tail;
    tw = 0;
    while (r) {
        if (r->display && r->flo == CSS_FLOAT_NONE && r->type != RNT_BR) {
            if (r->type == RNT_TEXT) {
                NRenderText* rt = (NRenderText*)r;
                renderText_alignTailByInline(rt, dx, mb);
                renderText_getTailAlignRect(rt, &hw);
                tw += hw;
            }
            else {
                w = rect_getWidth(&r->r);
                r->r.l = r->ml + dx;
                r->r.r = r->r.l + w;
                dy = calcVertAlignDelta(mb, r->r.b, r->vert_align);
                r->r.t += dy;
                r->r.b += dy;
                tw += w;
            }
        }
        r = r->next;
    }
    
    return tw;
}

void renderInline_setMaxRight(NRenderNode* rn, coord mr)
{
    NRenderNode* r = rn->child;
    
    rn->mr = mr;
    
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
