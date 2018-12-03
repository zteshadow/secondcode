/*
 * renderText.c
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../dom/node.h"
#include "../dom/xml_tags.h"
#include "../dom/document.h"
#include "../dom/page.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "../css/css_helper.h"
#include "renderBlock.h"
#include "renderText.h"
#include "renderInline.h"

#ifdef NBK_MEM_TEST
int renderText_memUsed(const NRenderText* rt)
{
    int size = 0;
    if (rt) {
        size = sizeof(NRenderText);
        if (rt->lines)
            size += sizeof(NRTLine) * rt->max;
    }
    return size;
}
#endif

static wchr* rt_skip_space_prefix(wchr* text)
{
    wchr* p = text;
    while (*p && *p == 0x20)
        p++;
    return ((*p) ? p : text);
}

#define IS_LETTER(c) ((c >= 97 && c <= 122) || (c >= 65 && c <= 90))
#define IS_NUMBER(c) (c >= 48 && c <= 57)

enum NCharType {
    NECT_NONE,
    NECT_LETTER,
    NECT_NUMBER
};

static wchr* rt_break_line(
    void* pfd, const wchr* text, NFontId fontId,
                           coord maxw, coord* width, bd_bool zoom)
{
    coord w, tw = 0, bw;
    wchr* p = (wchr*)text;
    wchr* b = N_NULL;
    nid ct, lct = NECT_NONE;
    
    while (*p) {
        if (IS_LETTER(*p))
            ct = NECT_LETTER;
        else if (IS_NUMBER(*p))
            ct = NECT_NUMBER;
        else
            ct = NECT_NONE;
        
        if (ct) {
            if (b == N_NULL || ct != lct) {
                b = p;
                bw = tw;
                lct = ct;
            }
        }
        else {
            b = N_NULL;
            lct = NECT_NONE;
        }
        
        w = (zoom) ? NBK_gdi_getCharWidthByEditor(pfd, fontId, *p) \
                   : NBK_gdi_getCharWidth(pfd, fontId, *p);
        if (tw + w > maxw)
            break;
        
        tw += w;
        p++;
    }
    
    *width = tw;
    if (*p && b && b > text) {
        p = b;
        *width = bw;
    }
    return p;
}

static int16 rt_predict_line_num(const wchr* text, coord maxw, coord fw)
{
    int len = NBK_wcslen(text);
    int num = fw * len;
    num = (num / maxw) + 3; // even 2 letters will be broke to 2 lines
    return (int16)num;
}

NRenderText* renderText_create(void* node, nid type)
{
    NRenderText* rt = (NRenderText*)NBK_malloc0(sizeof(NRenderText));
    NNode* n = (NNode*)node;
    
    N_ASSERT(rt);
    
    renderNode_init(&rt->d);
    
    rt->d.type = RNT_TEXT;
    rt->d.node = node;
    
    rt->d.GetStartPoint = renderText_getStartPoint;
    rt->d.Layout = (type >= NEDOC_FULL) ? renderText_layoutSimple : renderText_layout;
    rt->d.Paint = (type >= NEDOC_FULL) ? renderText_paintSimple : renderText_paint;
    
    rt->docType = type;
    rt->space = n->space;
    
    return rt;
}

void renderText_delete(NRenderText** rt)
{
    NRenderText* t = *rt;
    if (t->lines) {
        NBK_free(t->lines);
        t->lines = N_NULL;
    }
    NBK_free(t);
    *rt = N_NULL;
}

void renderText_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    NRenderText* rt = (NRenderText*)rn;
    N_ASSERT(byChild == N_FALSE);
    
    *x = rt->sx;
    *y = rt->sy;
}

void renderText_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderText* rt = (NRenderText*)rn;
    NNode* node = (NNode*)rn->node;
    coord maxw = rn->parent->GetContentWidth(rn->parent);
    coord x, y, w, mw, h, fh;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    wchr *p, *q;
    NRect area;
    bd_bool hasAA = N_FALSE;
    int guard;
    coord mflb, mflh;
    NFontId fid;
    
    if (!rn->needLayout && !force)
        return;
    
    if (!renderNode_layoutable(rn, maxw))
        return;
        
    renderNode_calcStyle(rn, style);
    
    rt->underline = style->underline;
    rt->clip = *style->clip;
    
    fid = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    if (fid != rt->fontId && rt->lines) {
        NBK_free(rt->lines);
        rt->lines = N_NULL;
    }
    rt->fontId = fid;
    fh = NBK_gdi_getFontHeight(page->platform, rt->fontId);
    
    rt->line_spacing = DEFAULT_LINE_SPACE;
    if (style->line_height.type)
        rt->line_spacing = css_calcValue(style->line_height, fh, style, fh) - fh;
    h = fh + rt->line_spacing;
    
    rt->indent = css_calcValue(style->text_indent, style->font_size, style, 0);

    if (stat->aux[stat->ld]) {
        renderNode_getAvailArea(stat->aux[stat->ld], rn, &area);
        hasAA = N_TRUE;
    }
    else if (renderNode_getAvailAreaByChild(stat, rn->parent, &area)) {
        hasAA = N_TRUE;
    }
    
    if (hasAA) {
            x = area.l;
            y = area.t;
    }
    else {
        if (byChild) {
            rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
        }
        else {
            // follow previous object
            rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
        }
        area.l = 0;
        area.r = maxw;
        area.t = y;
        area.b = area.t + 1;
    }
    
    mflb = (byChild) ? 0 : renderNode_getMaxBottomByLine(rn->prev, N_TRUE);
    mflh = (byChild) ? renderNode_getFollowLineHeight(rn->parent) : 0;
    
    if (y >= area.b) hasAA = N_FALSE;
    
    if (rt->space) {
        bd_bool ignore = N_FALSE;
        
        if (x == 0)
            ignore = N_TRUE;
        else {
            NRenderNode* r = rn->prev;
            while (r) {
                if (r->display && r->flo == CSS_FLOAT_NONE) {
                    if (r->type == RNT_TEXT && ((NRenderText*)r)->space) {
                        x = r->r.r;
                        y = r->r.t;
                        ignore = N_TRUE;
                    }
                    break;
                }
                r = r->prev;
            }
        }
        
        rn->ml = x;
        rn->mt = y;
        rn->mr = area.r;
        rt->sy = y;
        
        if (ignore) {
            rn->r.l = rn->r.r = x;
            rn->r.t = rn->r.b = y;
            rt->sx = x;
            rn->display = CSS_DISPLAY_NONE;
            rn->needLayout = 0;
            rn->parent->needLayout = 1;
            return;
        }
        
        rn->r.l = x;
        rn->r.r = x + NBK_gdi_getTextWidth(page->platform, rt->fontId, node->d.text, node->len);
        rn->r.t = y;
        rn->r.b = y + h;
        rt->sx = rn->r.r;
        rn->needLayout = 0;
        rn->parent->needLayout = 1;
        return;
    }
    
    if (rt->lines == N_NULL) {
        const wchr hz = 0x3000;
        coord fw = NBK_gdi_getCharWidth(page->platform, rt->fontId, hz);
        rt->max = rt_predict_line_num(node->d.text, maxw, fw);
        rt->lines = (NRTLine*)NBK_malloc(sizeof(NRTLine) * rt->max);
        N_ASSERT(rt->lines);
    }
    rt->use = 0;
    
    rn->r.l = x;
    rn->r.r = rn->r.l;
    rn->r.t = y;
    rn->r.b = rn->r.t;
    rn->mr = 0;
    
    p = node->d.text;
    if (x == 0)
        p = rt_skip_space_prefix(p);
    rt->lines[rt->use].text = p;
    rt->lines[rt->use].r.l = x;
    rt->lines[rt->use].r.t = y;
    guard = (hasAA && !byChild) ? 2 : 1; // protect for infinite loop
    while (*p && rt->use < rt->max - 1) {
        
        mw = (y < area.b) ? area.r - x : maxw - x;
        q = rt_break_line(page->platform, p, rt->fontId, mw, &w, N_FALSE);
        
        if (q == p) {
            if (--guard < 0)
                break;
            
            // not enough space
            if (hasAA)
                x = (mflb < area.b) ? area.l : 0;
            else
                x = 0;
            y = mflb;
            mflb = mflh = 0;
            
            if (x == 0)
                p = rt_skip_space_prefix(p);
            
            rt->lines[rt->use].r.l = x;
            rt->lines[rt->use].r.t = y;
            rn->r.l = x;
            rn->r.r = rn->r.l;
            rn->r.t = y;
            rn->r.b = rn->r.t;
            continue;
        }
        
        rt->lines[rt->use].r.b = rt->lines[rt->use].r.t + h;
        rt->lines[rt->use].r.r = rt->lines[rt->use].r.l + w;
        rt->lines[rt->use].ml = x;
        rt->lines[rt->use].mr = x + mw;
        
        rn->r.r = N_MAX(rn->r.r, rt->lines[rt->use].r.r);
        rn->mr = N_MAX(rn->mr, x + mw);
        rt->use++;

        y += h;
        y = N_MAX(y, mflb);
        y = N_MAX(y, mflh);
        if (hasAA)
            x = (y < area.b) ? area.l : 0;
        else
            x = 0;
        mflb = mflh = 0;

        rt->lines[rt->use].text = q;
        rt->lines[rt->use].r.l = x;
        rt->lines[rt->use].r.t = y;
        rt->lines[rt->use].r.r = rt->lines[rt->use].r.l;
        rt->lines[rt->use].r.b = rt->lines[rt->use].r.t;
        
        p = q;
        if (x == 0)
            p = rt_skip_space_prefix(p);
        
    } // the rt->lines[rt->use].text is always point to end of string.
    
    rn->r.b = rt->lines[rt->use - 1].r.b;
    rn->r.l = rt->lines[rt->use - 1].r.l;
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    
    // keep start point for next sibling in align left mode
    rt->sx = rt->lines[rt->use - 1].r.r;
    rt->sy = rt->lines[rt->use - 1].r.t;
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderText_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderText* rt = (NRenderText*)rn;
    coord x, y, fa, fh, da, py;
    NPoint pos;
    int16 i;
    int len;
    NRect pr, cl;
//    NNode* node = (NNode*)rn->node;
    
    if (rn->display == CSS_DISPLAY_NONE)
        return;
    
    if (rt->space) // not paint white-space
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
    pr = rt->clip;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    cl.r = N_MIN(cl.r, pr.r);
    cl.b = N_MIN(cl.b, pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    fa = NBK_gdi_getFontAscent(page->platform, rt->fontId);
    fh = NBK_gdi_getFontHeight(page->platform, rt->fontId);
    da = rt->line_spacing >> 1;
    
    fa = (coord)nfloat_imul(fa, style->zoom);
    fh = (coord)nfloat_imul(fh, style->zoom);
    da = (coord)nfloat_imul(da, style->zoom);
    
    NBK_gdi_useFont(page->platform, rt->fontId);
    
//    rn->hasBgcolor = 1;
//    rn->background_color = colorYellow;
    
    if (style->highlight) {
        NBK_gdi_setPenColor(page->platform, &style->color);
        NBK_gdi_setBrushColor(page->platform, &style->background_color);
    }
    else {
        NBK_gdi_setPenColor(page->platform, &rn->color);
        if (rn->hasBgcolor)
            NBK_gdi_setBrushColor(page->platform, &rn->background_color);
    }
    
    for (i=0; i < rt->use; i++) {
        py = (coord)nfloat_imul(y + rt->lines[i].r.t, style->zoom);
        if (py + fh <= rect->t)
            continue;
        if (py >= rect->b)
            break;
        
        if ((rn->hasBgcolor || style->highlight) && !style->belongA) {
            pr = rt->lines[i].r;
            rect_move(&pr, pr.l + x, pr.t + y);
            rect_toView(&pr, style->zoom);
            rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
            NBK_gdi_clearRect(page->platform, &pr);
        }

        pos.x = (coord)nfloat_imul(x + rt->lines[i].r.l, style->zoom) - rect->l;
        pos.y = py - rect->t + fa + da;
        
        len = (int)(rt->lines[i+1].text - rt->lines[i].text);
        NBK_gdi_drawText(page->platform, rt->lines[i].text, len, &pos, rt->underline);
    }
    
    NBK_gdi_releaseFont(page->platform);

    NBK_gdi_cancelClippingRect(page->platform);
}

void renderText_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderText* rt = (NRenderText*)rn;
    NNode* node = (NNode*)rn->node;
    coord /*x, y,*/ tw;
    
    if (!rn->needLayout && !force)
        return;
    
    if (node->parent->id != TAGID_SPAN) { // text must under SPAN
        rn->needLayout = 0;
        return;
    }

    renderNode_calcStyle(rn, style);
    
//    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
//    rn->r = *attr_getRect(node->parent->atts, ATTID_TC_RECT);
//    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    rn->r = rn->parent->r;
    rn->mt = rn->r.t;
    
    rt->clip = *style->clip;
    
    // ignore space
    if (rt->space) {
        rn->needLayout = 0;
        return;
    }
    
    rt->fontId = NBK_gdi_getFont(page->platform, style->font_size,
        ((rt->docType == NEDOC_NARROW) ? style->bold : 0), style->italic);
    
    if (rt->lines == N_NULL) {
        rt->max = 2;
        rt->lines = (NRTLine*)NBK_malloc(sizeof(NRTLine) * rt->max);
        N_ASSERT(rt->lines);
    }
    rt->use = 1;
    
    tw = NBK_gdi_getTextWidth(page->platform, rt->fontId, node->d.text, node->len);
    tw += rn->r.l;
    rt->lines[0].text = node->d.text;
    rt->lines[0].r = rn->r;
    if (rt->docType == NEDOC_FULL)
        rt->lines[0].r.r = N_MIN(rn->r.r, tw);
    else {
        rt->lines[0].r.r = tw;
        rn->r.r = tw;
    }
    rt->lines[1].text = node->d.text + node->len;
    
    rt->beBreak = 0;
    rn->needLayout = 0;
}

void renderText_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderText* rt = (NRenderText*)rn;
    NRect r, pr, cl;
    NPoint pos;
    coord fa;
    wchr* text;
    int len, i;
    
    if (rt->space)
        return;
    
    if (((NNode*)rn->node)->parent->id != TAGID_SPAN)
        return;
    
    r = rn->r;
    if (!rect_isIntersect(&style->dv, &r))
        return;
    if (!rect_isIntersect(&rt->clip, &r))
        return;
    rect_toView(&r, style->zoom);
    if (!rect_isIntersect(rect, &r))
        return;

    rect_move(&r, r.l - rect->l, r.t - rect->t);
    
    pr = rt->clip;
    rect_toView(&pr, style->zoom);
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), r.r);
    cl.b = N_MIN(rect_getHeight(rect), r.b);
    cl.r = N_MIN(cl.r, pr.r);
    cl.b = N_MIN(cl.b, pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    fa = NBK_gdi_getFontAscent(page->platform, rt->fontId);
    fa = (coord)nfloat_imul(fa, style->zoom);
    
    NBK_gdi_useFont(page->platform, rt->fontId);
    
    if (style->highlight) {
        NBK_gdi_setPenColor(page->platform, &style->color);
        NBK_gdi_setBrushColor(page->platform, &style->background_color);
    }
    else {
        NBK_gdi_setPenColor(page->platform, &rn->color);
        if (rn->hasBgcolor)
            NBK_gdi_setBrushColor(page->platform, &rn->background_color);
    }
    
    for (i=0; i < rt->use; i++) {
        if ((rn->hasBgcolor || style->highlight) && !style->belongA) {
            pr = rt->lines[i].r;
            rect_toView(&pr, style->zoom);
            rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
            NBK_gdi_clearRect(page->platform, &pr);
        }
        text = rt->lines[i].text;
        len = rt->lines[i + 1].text - rt->lines[i].text;
        pos.x = (coord)nfloat_imul(rt->lines[i].r.l, style->zoom) - rect->l;
        pos.y = (coord)nfloat_imul(rt->lines[i].r.t, style->zoom) - rect->t + fa;
        NBK_gdi_drawText(page->platform, text, len, &pos, rt->underline);
    }
    
    NBK_gdi_releaseFont(page->platform);
    
    NBK_gdi_cancelClippingRect(page->platform);
}

bd_bool renderText_isBreak(NRenderText* rt)
{
    return ((rt->use > 1) ? N_TRUE : N_FALSE);
}

NRect renderText_getHeadAlignRect(NRenderText* rt, coord* width)
{
    coord hw = rect_getWidth(&rt->lines[0].r);
    NRect box;
    box.l = rt->lines[0].ml;
    box.t = rt->lines[0].r.t;
    box.r = rt->lines[0].mr;
    box.b = rt->lines[0].r.b;
    if (width)
        *width = hw;
    return box;
}

NRect renderText_getTailAlignRect(NRenderText* rt, coord* width)
{
    NRect box;
    int i = rt->use - 1;

    if (width) {
        *width = (i < 0) ? 0 : rect_getWidth(&rt->lines[i].r);
}

    if (rt->space) {
        box.l = rt->d.ml;
        box.t = rt->d.r.t;
        box.r = rt->d.mr;
        box.b = rt->d.r.b;
}
    else {
        box.l = rt->lines[i].ml;
        box.t = rt->lines[i].r.t;
        box.r = rt->lines[i].mr;
        box.b = rt->lines[i].r.b;
}

    return box;
}

void renderText_alignHead(NRenderText* rt, coord dx, coord maxb)
{
    coord w, dy;
    
    if (rt->lines) {
        w = rect_getWidth(&rt->lines[0].r);
        rt->lines[0].r.l = rt->lines[0].ml + dx;
        rt->lines[0].r.r = rt->lines[0].r.l + w;
        dy = calcVertAlignDelta(maxb, rt->lines[0].r.b, rt->d.vert_align);
        rt->lines[0].r.t += dy;
        rt->lines[0].r.b += dy;
        rt->d.r.r = rt->lines[0].r.r;
    }
}

void renderText_alignBody(NRenderText* rt, nid ha)
{
    int i;
    coord w, mw, da;
    
    for (i=1; i < rt->use - 1; i++) {
        mw = rt->lines[i].mr - rt->lines[i].ml;
        w = rect_getWidth(&rt->lines[i].r);
        da = calcHoriAlignDelta(mw, w, ha);
        rt->lines[i].r.l = rt->lines[i].ml + da;
        rt->lines[i].r.r = rt->lines[i].r.l + w;
        rt->d.r.r = N_MAX(rt->d.r.r, rt->lines[i].r.r);
    }
}

coord renderText_alignTail(NRenderText* rt, nid ha)
{
    int i;
    coord w, mw, da, y, mb, hw;
    NRenderNode* r;
    bd_bool br = N_FALSE;
    NRect box;
    
    i = rt->use - 1;
    w = rect_getWidth(&rt->lines[i].r);
    y = rt->sy;
    mb = rt->lines[i].r.b;
    r = ((NRenderNode*)rt)->next;
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
    
    mw = rt->lines[i].mr - rt->lines[i].ml;
    da = calcHoriAlignDelta(mw, w, ha);
    w = rect_getWidth(&rt->lines[i].r);
    rt->lines[i].r.l = rt->lines[i].ml + da;
    rt->lines[i].r.r = rt->lines[i].r.l + w;
    
    da = calcVertAlignDelta(mb, rt->lines[i].r.b, rt->d.vert_align);
    rt->lines[i].r.t += da;
    rt->lines[i].r.b += da;
    
    // adjust the whole block of text
    rt->d.r.l = rt->lines[i].r.l;
    rt->d.r.t = rt->lines[0].r.t;
    rt->d.r.r = N_MAX(rt->d.r.r, rt->lines[i].r.r);
    rt->d.r.b = rt->lines[i].r.b;
    
    return w;
}

void renderText_alignTailByInline(NRenderText* rt, coord dx, coord maxb)
{
    int i = rt->use - 1;
    coord w, da;
    
    if (i < 0)
        return;
    
    w = rect_getWidth(&rt->lines[i].r);
    rt->lines[i].r.l = rt->lines[i].ml + dx;
    rt->lines[i].r.r = rt->lines[i].r.l + w;
    
    da = calcVertAlignDelta(maxb, rt->lines[i].r.b, rt->d.vert_align);
    rt->lines[i].r.t += da;
    rt->lines[i].r.b += da;

    rt->d.r.l = rt->lines[i].r.l;
    rt->d.r.t = rt->lines[0].r.t;
    rt->d.r.r = N_MAX(rt->d.r.r, rt->lines[i].r.r);
    rt->d.r.b = rt->lines[i].r.b;
}

void renderText_adjustPos(NRenderNode* rn, coord dy)
{
    NRenderText* rt = (NRenderText*)rn;
    int i;
    
    renderNode_adjustPos(rn, dy);
    
    if (rt->lines == N_NULL)
        return;
    
    for (i=0; i < rt->use; i++) {
        rt->lines[i].r.t += dy;
        rt->lines[i].r.b += dy;
    }
    i--;
    rt->sx = rt->lines[i].r.l;
    rt->sy = rt->lines[i].r.t;
    
    rt->clip.t += dy;
    rt->clip.b += dy;
}

bd_bool renderText_hasPt(NRenderText* rt, NPoint pt)
{
    coord x, y;
    int i;
    NRect r;
    
    renderNode_getAbsPos(rt->d.parent, &x, &y);
    
    for (i=0; i < rt->use; i++) {
        r = rt->lines[i].r;
        rect_move(&r, x + r.l, y + r.t);
        if (rect_hasPt(&r, pt.x, pt.y))
            return N_TRUE;
    }
    
    return N_FALSE;
}

int16 renderText_getDataSize(NRenderNode* rn, NStyle* style)
{
    NNode* node = (NNode*)rn->node;
    return node->len;
}

#if 0
void renderText_breakLineByWidthFF(NRenderNode* rn, NStyle* style, coord width)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NNode* node = (NNode*)rn->node;
    NRenderText* rt = (NRenderText*)rn;
    wchr *p, *q;
    coord fh, lh, w, x, y;
    BOOL gotXy = N_FALSE;
    coord maxw, ox;
    
    if (rt->fontId == 0) // ignore space
        return;
    if (node->parent->id != TAGID_SPAN)
        return;
    
    if (node->len < 10) {
        if (   node->parent->prev == N_NULL
            || node->parent->prev->id != TAGID_SPAN
            || node->parent->prev->child == N_NULL
            || node->parent->prev->child->id != TAGID_TEXT
            || node->parent->prev->child->render == N_NULL
            || ((NRenderText*)node->parent->prev->child->render)->beBreak == 0)
            return;
    }
    
    if (rt->lines)
        NBK_free(rt->lines);
    {
        const wchr hz = 0x3000;
        coord fw = NBK_gdi_getCharWidthByEditor(page->platform, rt->fontId, hz);
        rt->max = rt_predict_line_num(node->d.text, width, fw);
        rt->lines = (NRTLine*)NBK_malloc(sizeof(NRTLine) * rt->max);
    }
    
    // get x,y from preivous span/text if it exists
    if (   node->parent->prev
        && node->parent->prev->id == TAGID_SPAN
        && node->parent->prev->child
        && node->parent->prev->child->id == TAGID_TEXT) {
        NRenderText* rt2 = (NRenderText*)node->parent->prev->child->render;
        if (rt2 && rt2->lines && rt2->use > 0) {
            x = rt2->lines[rt2->use - 1].r.l;
            y = rt2->lines[rt2->use - 1].r.t;
            ox = rt2->lines[rt2->use - 1].r.r - x;
            gotXy = N_TRUE;
        }
    }
    if (!gotXy) {
        x = rn->r.l;
        y = rn->r.t;
        ox = 0;
    }
    
    fh = NBK_gdi_getFontHeightByEditor(page->platform, rt->fontId);
    lh = (fh < 20) ? 2 : 0;
    rt->use = 0;
    p = node->d.text;
    rt->lines[rt->use].text = p;
    rt->lines[rt->use].r.l = x + ox;
    rt->lines[rt->use].r.t = y;
    rn->ml = rn->mr = 0;
    while (*p && rt->use < rt->max - 1) {
        maxw = width - ox;
        if (ox) ox = 0;
        q = rt_break_line(page->platform, p, rt->fontId, maxw, &w, N_TRUE);
        if (q == p) {
            y += fh + lh;
            rt->lines[rt->use].r.l = x;
            rt->lines[rt->use].r.t = y;
            continue;
        }
        
        rt->lines[rt->use].r.r = rt->lines[rt->use].r.l + w;
        rt->lines[rt->use].r.b = y + fh;
        rn->ml = N_MIN(rn->ml, rt->lines[rt->use].r.l);
        rn->mr = N_MAX(rn->mr, rt->lines[rt->use].r.r);

        y += fh + lh;
        rt->use++;
        rt->lines[rt->use].text = q;
        rt->lines[rt->use].r.l = x;
        rt->lines[rt->use].r.t = y;
        
        p = q;
    }

    if (rt->use > 0) {
        rn->r.l = rn->ml;
        rn->r.r = rn->mr;
        rn->r.t = rt->lines[0].r.t;
        rn->r.b = rt->lines[rt->use - 1].r.b;
        rn->mt = rn->r.t;
        rn->parent->r = rn->r; // make rect of span same as text
        rt->clip = rn->r;
    }
    
    rt->beBreak = 1;
}
#endif

void renderText_setMaxRight(NRenderNode* rn, coord mr)
{
    NRenderText* rt = (NRenderText*)rn;
    
    rn->mr = mr;
    
    if (rt->lines) {
        int i;
        for (i=0; i < rt->use; i++) {
            rt->lines[i].mr = mr;
        }
    }
}
