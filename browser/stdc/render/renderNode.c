/*
 * renderNode.c
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_limit.h"
#include "../inc/nbk_gdi.h"
#include "../dom/xml_tags.h"
#include "../dom/xml_atts.h"
#include "../dom/node.h"
#include "../dom/view.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/attr.h"
#include "../css/css_val.h"
#include "../css/color.h"
#include "../css/css_helper.h"
#include "../tools/str.h"
#include "renderNode.h"
#include "renderBlock.h"
#include "renderInline.h"
#include "renderText.h"
#include "renderImage.h"
#include "renderBr.h"
#include "renderInput.h"
#include "renderHr.h"
#include "renderSelect.h"
#include "renderTextarea.h"
#include "renderObject.h"
#include "renderBlank.h"
#include "renderA.h"
#include "renderTable.h"
#include "renderTr.h"
#include "renderTd.h"
#include "imagePlayer.h"

#ifdef NBK_MEM_TEST
int renderNode_memUsed(const NRenderNode* rn)
{
    int size = 0;
    if (rn) {
        switch (rn->type) {
        case RNT_BLANK:
            size = renderBlank_memUsed((NRenderBlank*)rn);
            break;
        case RNT_BLOCK:
            size = renderBlock_memUsed((NRenderBlock*)rn);
            break;
        case RNT_TEXT:
            size = renderText_memUsed((NRenderText*)rn);
            break;
        case RNT_IMAGE:
            size = renderImage_memUsed((NRenderImage*)rn);
            break;
        case RNT_INPUT:
            size = renderInput_memUsed((NRenderInput*)rn);
            break;
        case RNT_BR:
            size = renderBr_memUsed((NRenderBr*)rn);
            break;
        case RNT_HR:
            size = renderHr_memUsed((NRenderHr*)rn);
            break;
        case RNT_SELECT:
            size = renderSelect_memUsed((NRenderSelect*)rn);
            break;
        case RNT_TEXTAREA:
            size = renderTextarea_memUsed((NRenderTextarea*)rn);
            break;
        case RNT_OBJECT:
            size = renderObject_memUsed((NRenderObject*)rn);
            break;
        }
    }
    return size;
}
#endif

bd_bool renderNode_has(NNode* node, uint8 type)
{
    uint16 id = node->id;
    if (id > TAGID_CLOSETAG)
        id -= TAGID_CLOSETAG;
    
    switch (id) {
    case TAGID_BODY:
    case TAGID_CARD:
    case TAGID_DIV:
    case TAGID_P:
    case TAGID_UL:
    case TAGID_OL:
    case TAGID_LI:
    case TAGID_IMG:
    case TAGID_TEXT:
    case TAGID_BR:
    case TAGID_BUTTON:
    case TAGID_INPUT:
    case TAGID_TEXTAREA:
    case TAGID_H1:
    case TAGID_H2:
    case TAGID_H3:
    case TAGID_H4:
    case TAGID_H5:
    case TAGID_H6:
    case TAGID_HR:
    case TAGID_SELECT:
    case TAGID_OBJECT:
    case TAGID_TABLE:
    case TAGID_TR:
    case TAGID_TD:
    case TAGID_FORM:
    case TAGID_SPAN:
        return N_TRUE;
    default:
        if (doc_isAbsLayout(type)) {
            switch (id) {
            case TAGID_FORM:
            case TAGID_A:
            case TAGID_SPAN:
            case TAGID_FRAME:
            case TAGID_TC_ATTACHMENT:
                return N_TRUE;
            }
        }
        else {
            switch (id) {
            case TAGID_STRONG:
                return N_TRUE;
            }
        }
        return N_FALSE;
    }
}

NRenderNode* renderNode_create(void* view, NNode* node)
{
    NView* v = (NView*)view;
    NPage* p = (NPage*)v->page;
    nid id = node->id;
    
    switch (id) {
    case TAGID_BODY:
    case TAGID_CARD:
    case TAGID_DIV:
    case TAGID_P:
    case TAGID_UL:
    case TAGID_OL:
    case TAGID_LI:
    case TAGID_H1:
    case TAGID_H2:
    case TAGID_H3:
    case TAGID_H4:
    case TAGID_H5:
    case TAGID_H6:
    case TAGID_FORM:
    {
        NRenderBlock* r = renderBlock_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_IMG:
    {
        NRenderImage* r = renderImage_create(node, p->doc->type);
        char* src = (node->nodeRef) ? \
            attr_getValueStr(node->nodeRef->atts, ATTID_SRC) : \
            attr_getValueStr(node->atts, ATTID_SRC);
        if (src)
            r->iid = imagePlayer_getId(v->imgPlayer, src, NEIT_IMAGE, N_NULL);
        return (NRenderNode*)r;
    }
    case TAGID_TEXT:
    {
        NRenderText* r = renderText_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_HR:
    {
        NRenderHr* r = renderHr_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_BR:
    {
        NRenderBr* r = renderBr_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_TEXTAREA:
    {
        NRenderTextarea* r = renderTextarea_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_BUTTON:
    case TAGID_INPUT:
    {
        NRenderInput* r = renderInput_create(node, p->doc->type);
        char* src = attr_getValueStr(node->atts, ATTID_SRC);
        if (src)
            r->iid = imagePlayer_getId(v->imgPlayer, src, NEIT_IMAGE, N_NULL);
        return (NRenderNode*)r;
    }
    case TAGID_SELECT:
    {
        NRenderSelect* r = renderSelect_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_OBJECT:
    {
        if (doc_isAbsLayout(p->doc->type)) {
        NRenderObject* r = renderObject_create(node, p->doc->type);
        char* src = attr_getValueStr(node->atts, ATTID_SRC);
        if (src)
            r->iid = imagePlayer_getId(v->imgPlayer, src, NEIT_FLASH_IMAGE, N_NULL);
        return (NRenderNode*)r;
    }
        else {
            NRenderBlock* r = renderBlock_create(node, p->doc->type);
            return (NRenderNode*)r;
        }
    }
    case TAGID_A:
    case TAGID_TC_ATTACHMENT:
    {
        if (doc_isAbsLayout(p->doc->type)) {
            NRenderA* r = renderA_create(node, p->doc->type);
            return (NRenderNode*)r;
        }
        else
            return N_NULL;
    }
    case TAGID_TABLE:
    {
        NRenderTable* r = renderTable_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_TR:
    {
        NRenderTr* r = renderTr_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_TD:
    {
        NRenderTd* r = renderTd_create(node, p->doc->type);
        return (NRenderNode*)r;
    }
    case TAGID_SPAN:
    {
        if (doc_isAbsLayout(p->doc->type))
            return (NRenderNode*)renderBlank_create(node, p->doc->type);
        else
            return (NRenderNode*)renderInline_create(node, p->doc->type);
    }
    default:
        if (doc_isAbsLayout(p->doc->type)) {
            switch (id) {
            case TAGID_FORM:
            case TAGID_FRAME:
            {
                NRenderBlank* r = renderBlank_create(node, p->doc->type);
                return (NRenderNode*)r;
            }
            }
        }
        else {
            switch (id) {
            case TAGID_STRONG:
            {
                NRenderInline* r = renderInline_create(node, p->doc->type);
                return (NRenderNode*)r;
            }
            }
        }
        return N_NULL;
    }
}

void renderNode_delete(NRenderNode** rn)
{
    NRenderNode* t = *rn;
    
    switch (t->type) {
    case RNT_BLOCK:
    {
        NRenderBlock* r = (NRenderBlock*)t;
        renderBlock_delete(&r);
        break;
    }
    case RNT_INLINE:
    {
        NRenderInline* r = (NRenderInline*)t;
        renderInline_delete(&r);
        break;
    }
    case RNT_IMAGE:
    {
        NRenderImage* r = (NRenderImage*)t;
        renderImage_delete(&r);
        break;
    }
    case RNT_TEXT:
    {
        NRenderText* r = (NRenderText*)t;
        renderText_delete(&r);
        break;
    }
    case RNT_HR:
    {
        NRenderHr* r = (NRenderHr*)t;
        renderHr_delete(&r);
        break;
    }
    case RNT_BR:
    {
        NRenderBr* r = (NRenderBr*)t;
        renderBr_delete(&r);
        break;
    }
    case RNT_INPUT:
    {
        NRenderInput* r = (NRenderInput*)t;
        renderInput_delete(&r);
        break;
    }
    case RNT_SELECT:
    {
        NRenderSelect* r = (NRenderSelect*)t;
        renderSelect_delete(&r);
        break;
    }
    case RNT_TEXTAREA:
    {
        NRenderTextarea* r = (NRenderTextarea*)t;
        renderTextarea_delete(&r);
        break;
    }
    case RNT_OBJECT:
    {
        NRenderObject* r = (NRenderObject*)t;
        renderObject_delete(&r);
        break;
    }
    case RNT_BLANK:
    {
        NRenderBlank* r = (NRenderBlank*)t;
        renderBlank_delete(&r);
        break;
    }
    case RNT_A:
    {
        NRenderA* r = (NRenderA*)t;
        renderA_delete(&r);
        break;
    }
    case RNT_TABLE:
    {
        NRenderTable* r = (NRenderTable*)t;
        renderTable_delete(&r);
        break;
    }
    case RNT_TR:
    {
        NRenderTr* r = (NRenderTr*)t;
        renderTr_delete(&r);
        break;
    }
    case RNT_TD:
    {
        NRenderTd* r = (NRenderTd*)t;
        renderTd_delete(&r);
        break;
    }
    default:
        N_KILL_ME();
        break;
    }
}

void renderNode_init(NRenderNode* rn)
{
    rn->type = RNT_NONE;
    rn->display = CSS_DISPLAY_BLOCK;
    rn->zindex = 0; // basic panel
    rn->needLayout = 1;
    rn->init = 0;
    rn->flo = CSS_FLOAT_NONE;
    rn->bgiid = -1;
    
    rn->GetContentWidth = renderNode_getContentWidth;
    rn->GetAbsXY = renderNode_getAbsXY;
}

void renderNode_calcStyle(NRenderNode* rn, NStyle* style)
{
    rn->flo = style->flo;
    rn->clr = style->clr;
    rn->display = style->display;
    rn->vert_align = style->vert_align;
    
    // foreground or text color
    rn->color = style->color;
    
    // background color
    if (style->hasBgcolor) {
        rn->hasBgcolor = 1;
        rn->background_color = style->background_color;
    }
    
    // border color
    if (style->hasBrcolorL) {
        rn->hasBrcolorL = 1;
        rn->border_color_l = style->border_color_l;
    }
    if (style->hasBrcolorT) {
        rn->hasBrcolorT = 1;
        rn->border_color_t = style->border_color_t;
    }
    if (style->hasBrcolorR) {
        rn->hasBrcolorR = 1;
        rn->border_color_r = style->border_color_r;
    }
    if (style->hasBrcolorB) {
        rn->hasBrcolorB = 1;
        rn->border_color_b = style->border_color_b;
    }
    
    // background image
    if (style->bgImage) {
        rn->bg_repeat = style->bg_repeat;
        rn->bg_x_t = style->bg_x_t; rn->bg_x = style->bg_x;
        rn->bg_y_t = style->bg_y_t; rn->bg_y = style->bg_y;
    }
}

bd_bool renderNode_layoutable(NRenderNode* rn, coord maxw)
{
    if (maxw <= 0) {
        rn->needLayout = 0;
        rn->display = CSS_DISPLAY_NONE;
        return N_FALSE;
    }
    else
        return N_TRUE;
}

bd_bool renderNode_isChildsNeedLayout(NRenderNode* rn)
{
    NRenderNode* r = rn;
    while (r) {
        if (r->needLayout)
            return N_TRUE;
        r = r->next;
    }
    return N_FALSE;
}

coord renderNode_getWidth(const NRenderNode* rn)
{
    return rn->r.r - rn->r.l;
}

coord renderNode_getHeight(const NRenderNode* rn)
{
    return rn->r.b - rn->r.t;
}

coord renderNode_getMaxBottomByLine(const NRenderNode* rn, uint8 clear)
{
    NRenderNode* r = (NRenderNode*)rn;
    coord mb = 0;
    coord mt = -1;
    
    while (r) {
        if (mt != -1 && mt != r->mt)
            break;
        
        if ((clear || r->flo == CSS_FLOAT_NONE) && r->display) {
            if (mt == -1)
                mt = r->mt;
            mb = N_MAX(mb, r->r.b);
        }
        
        r = r->prev;
    }
    
    if (r && r->type == RNT_TEXT) {
        NRenderText* rt = (NRenderText*)r;
        if (renderText_isBreak(rt))
            mb = N_MAX(mb, r->r.b);
    }
    
    return mb;
}

coord renderNode_getFollowLineHeight(const NRenderNode* rn)
{
    NRenderNode* r = (NRenderNode*)rn;
    NRenderNode* prev = r->prev;
    coord lh = 0, mt = -1;
    NRect box;
    
    while (r->type == RNT_INLINE && !prev) {
        r = r->parent;
        prev = r->prev;
    }
    if (r->type != RNT_INLINE)
        return 0;
    
    r = prev;
    while (r) {
        if (mt != -1 && mt != r->mt)
            break;
        
        if (r->display && r->flo == CSS_FLOAT_NONE) {
            if (mt == -1)
                mt = r->mt;
            
            if (r->type == RNT_TEXT && renderText_isBreak((NRenderText*)r)) {
                box = renderText_getTailAlignRect((NRenderText*)r, N_NULL);
                lh = N_MAX(lh, rect_getHeight(&box));
                break;
            }
            if (r->type == RNT_INLINE && renderInline_isBreak((NRenderInline*)r)) {
                box = renderInline_getTailAlignRect((NRenderInline*)r);
                lh = N_MAX(lh, rect_getHeight(&box));
                break;
            }
            
            lh = N_MAX(lh, rect_getHeight(&r->r));
        }
        
        r = r->prev;
    }
    
    return lh;
}

coord renderNode_getContentWidth(NRenderNode* rn)
{
    return rn->r.r - rn->r.l;
}

void renderNode_getAbsXY(NRenderNode* rn, coord* x, coord* y)
{
    *x += rn->r.l;
    *y += rn->r.t;
}

static void rn_calc_avail_area(NRect* dst, NRect* src)
{
    if (src->t >= dst->b) {
        *dst = *src;
        return;
    }

    dst->t = N_MIN(dst->t, src->t);
    dst->b = N_MAX(dst->b, src->b);
    dst->l = N_MAX(dst->l, src->l);
    dst->r = N_MIN(dst->r, src->r);
}

void renderNode_getAvailArea(NRenderNode* rn, NRenderNode* cur, NRect* area)
{
    coord mw, x, y;
    NRect v, a, r;
    NRenderNode* p = rn;
    
    mw = rn->parent->GetContentWidth(rn->parent);
    rn->parent->GetStartPoint(rn->parent, &v.l, &v.t, N_TRUE);
    v.r = v.l + mw;
    v.b = v.t;
    a = v;
    
    if (rn == cur) {
        area->t = (rn->prev) ? renderNode_getMaxBottomByLine(rn->prev, N_TRUE) : v.t;
        area->b = area->t;
        area->l = v.l;
        area->r = area->l + mw;
        return;
    }
    
    while (p != cur) {
        
        if (p->type == RNT_TEXT) {
            if (!((NRenderText*)p)->space) {
                p->GetStartPoint(p, &x, &y, N_FALSE);
                a.l = x;
                a.t = y;
                a.r = (y < a.b) ? a.r : v.r;
                a.b = p->r.b;
            }
        }
        else {
            r.t = p->r.t;
            r.b = p->r.b;
            if (p->flo == CSS_FLOAT_RIGHT) {
                r.l = v.l;
                r.r = p->r.l;
            }
            else {
                // CSS_FLOAT_NONE or CSS_FLOAT_LEFT
                r.l = p->r.r;
                r.r = v.r;
            }
            
            rn_calc_avail_area(&a, &r);
        }
        
        p = p->next;
    }
    
    *area = a;
}

// MUST be called by first child!
bd_bool renderNode_getAvailAreaByChild(NLayoutStat* stat, NRenderNode* rn, NRect* area)
{
    int idx = stat->ld - 1;
    coord de/*, d, w*/;
//    NRect a;
//    coord box_t, box_l, box_r;
    
    if (rn->prev && rn->prev->flo == CSS_FLOAT_NONE)
        return N_FALSE;
    if (rn->flo != CSS_FLOAT_NONE)
        return N_FALSE;
    if (idx < 0 || stat->aux[idx] == N_NULL)
        return N_FALSE;
    
    renderNode_getAvailArea(stat->aux[idx], rn, area);
    
//    if (rn->type == RNT_BLOCK) {
//        NRenderBlock* rb = (NRenderBlock*)rn;
//        box_t = rb->margin.t + rb->border.t + rb->padding.t;
//        box_l = rb->margin.l + rb->border.l + rb->padding.l;
//        box_r = rb->margin.r + rb->border.r + rb->padding.r;
//    }
//    else
//        box_t = box_l = box_r = 0;
    
    // switch to relative pos
    de = area->t;
    area->t -= de;
    area->b -= de;
    
//    a = *area;
//    
//    d = rn->r.t + box_t;
//    if (d >= a.b)
//        return N_FALSE;
//    
//    de = a.b - d;
//    area->b = area->t + de;
//    
//    w = rect_getWidth(&a);
//    
//    d = rn->r.l + box_l;
//    de = a.l - d;
//    if (de < 0) {
//        w += de;
//        de = 0;
//    }
//    area->l = de;
//    area->r = area->l + w;
//    
//    d = rn->r.r - box_r;
//    de = a.r - d;
//    if (de > 0) {
//        w -= de;
//        area->r = area->l + w;
//    }
    
    return N_TRUE;
}

bd_bool renderNode_isNearestNodeFloat(NRenderNode* rn)
{
    NRenderNode* r = rn;
    
    while (r) {
        if (r->display) {
            if (r->type == RNT_TEXT && ((NRenderText*)r)->space) {
                // ignore space
            }
            else {
                return (r->flo == CSS_FLOAT_NONE) ? N_FALSE : N_TRUE;
            }
        }
        r = r->prev;
    }
    
    return N_FALSE;
}

void renderNode_adjustPos(NRenderNode* rn, coord dy)
{
    rn->r.t += dy;
    rn->r.b += dy;
    rn->mt += dy;
}

void renderNode_absRect(NRenderNode* rn, NRect* rect)
{
    if (rn->parent) {
        renderNode_getAbsPos(rn->parent, &rect->l, &rect->t);
    }
    else {
        rect->l = rect->t = 0;
    }
    rect->l += rn->r.l;
    rect->r = rect->l + rect_getWidth(&rn->r);
    rect->t += rn->r.t;
    rect->b = rect->t + rect_getHeight(&rn->r);
}

void style_init(NStyle* style)
{
    NColor editColor = {0xff, 0x8c, 0x0, 0xff};
    NColor focusColor = {0x03, 0x5c, 0xfe, 0xff};
    
    NBK_memset(style, 0, sizeof(NStyle));
    
    style->ctlTextColor = colorBlack;
    style->ctlFgColor = colorGray;
    style->ctlBgColor = colorLightGray;
    style->ctlFillColor = colorWhite;
    style->ctlFocusColor = focusColor;
    style->ctlEditColor = editColor;
    
    style->opacity.i = 1;
    style->opacity.f = 0;
}

void renderNode_getRootAbsXYForFFFull(NRenderNode* rn, coord* x, coord* y)
{
    NRenderNode* r = rn;
    NNode* body;
    
    if (r->parent == N_NULL) {
        *x = *y = 0;
        return;
    }
    
    while (r->parent)
        r = r->parent;
    
    body = (NNode*)r->node;
    if (body && body->id == TAGID_BODY) {
        *x = r->r.l;
        *y = r->r.t;
    }
    else {
        *x = *y = 0;
    }
}

int renderNode_getEditLength(NRenderNode* rn)
{
    if (rn == N_NULL)
        return 0;
    
    if (rn->type == RNT_INPUT)
        return renderInput_getEditLength((NRenderInput*)rn);
    else if (rn->type == RNT_TEXTAREA)
        return renderTextarea_getEditLength((NRenderTextarea*)rn);
    else
        return 0;
}

int renderNode_getEditMaxLength(NRenderNode* rn)
{
    if (rn == N_NULL)
        return 0;
    
    if (rn->type == RNT_INPUT)
        return renderInput_getEditMaxLength((NRenderInput*)rn);
    else if (rn->type == RNT_TEXTAREA)
        return renderTextarea_getEditMaxLength((NRenderTextarea*)rn);
    else
        return 0;
}

bd_bool renderNode_getRectOfEditing(NRenderNode* rn, NRect* rect, NFontId* fontId)
{
    if (rn == N_NULL)
        return N_FALSE;
    
    if (rn->type == RNT_INPUT) {
        renderInput_getRectOfEditing((NRenderInput*)rn, rect, fontId);
        return N_TRUE;
    }
    else if (rn->type == RNT_TEXTAREA) {
        renderTextarea_getRectOfEditing((NRenderTextarea*)rn, rect, fontId);
        return N_TRUE;
    }
    else
        return N_FALSE;
}

wchr* renderNode_getEditText16(NRenderNode* rn, int* len)
{
    *len = 0;

    if (rn) {
        if (rn->type == RNT_INPUT)
            return renderInput_getEditText16((NRenderInput*)rn, len);
        else if (rn->type == RNT_TEXTAREA)
            return renderTextarea_getText16((NRenderTextarea*)rn, len);
    }
    
    return N_NULL;
}

void renderNode_setEditText16(NRenderNode* rn, wchr* text, int len)
{
    if (rn) {
        int l = (len == -1) ? NBK_wcslen(text) : len;
        if (rn->type == RNT_INPUT)
            renderInput_setEditText16((NRenderInput*)rn, text, l);
        else if (rn->type == RNT_TEXTAREA)
            renderTextarea_setText16((NRenderTextarea*)rn, text, l);
    }
}

bd_bool renderNode_getAbsRect(NRenderNode* rn, NRect* rect)
{
    if (rn->parent == N_NULL)
        return N_FALSE;
    
    switch (rn->type) {
    case RNT_BLOCK:
    case RNT_TEXT:
    case RNT_IMAGE:
    case RNT_INPUT:
    case RNT_TEXTAREA:
    case RNT_SELECT:
    {
        coord x, y;
        renderNode_getAbsPos(rn->parent, &x, &y);
        rect->l = x + rn->r.l;
        rect->t = y + rn->r.t;
        rect->r = rect->l + rect_getWidth(&rn->r);
        rect->b = rect->t + rect_getHeight(&rn->r);
        return N_TRUE;
    }
    }

    return N_FALSE;
}

void renderNode_drawBorder(NRenderNode* rn, NCssBox* border, NRect* rect, void* pfd)
{
    NRect l;
    
    NBK_gdi_setPenColor(pfd, &rn->border_color_l);
    NBK_gdi_drawRect(pfd, rect);

#if 0
    if (rn->hasBrcolorL && border->l) {
        l = *rect;
        l.r = l.l + border->l;
        NBK_gdi_setBrushColor(pfd, &rn->border_color_l);
        NBK_gdi_clearRect(pfd, &l);
    }

    if (rn->hasBrcolorT && border->t) {
        l = *rect;
        l.b = l.t + border->t;
        NBK_gdi_setBrushColor(pfd, &rn->border_color_t);
        NBK_gdi_clearRect(pfd, &l);
    }

    if (rn->hasBrcolorR && border->r) {
        l = *rect;
        l.l = l.r - border->r;
        NBK_gdi_setBrushColor(pfd, &rn->border_color_r);
        NBK_gdi_clearRect(pfd, &l);
    }

    if (rn->hasBrcolorB && border->b) {
        l = *rect;
        l.t = l.b - border->b;
        NBK_gdi_setBrushColor(pfd, &rn->border_color_b);
        NBK_gdi_clearRect(pfd, &l);
    }
#endif
}

void renderNode_drawBgImage(NRenderNode* rn, NRect* pr, NRect* vr, NStyle* style)
{
    NView* view = (NView*)style->view;
    NSize is;
    bd_bool fail;
    
    if (rn->bgiid == -1)
        return;

    if (imagePlayer_getSize(view->imgPlayer, rn->bgiid, &is, &fail)) {
        coord x, y;
        NImageDrawInfo di;
        di.id = rn->bgiid;
        di.r = rn->r;
        if (rn->type == RNT_BLOCK) {
            NRenderBlock* rb = (NRenderBlock*)rn;
            di.r.l += rb->margin.l + rb->border.l;
            di.r.t += rb->margin.t + rb->border.t;
            di.r.r -= rb->margin.r + rb->border.r;
            di.r.b -= rb->margin.b + rb->border.b;
        }
        renderNode_getAbsPos(rn->parent, &x, &y);
        rect_move(&di.r, di.r.l + x, di.r.t + y);
        di.pr = pr;
        di.vr = vr;
        di.zoom = &style->zoom;
        di.repeat = rn->bg_repeat;
        di.ox = css_calcBgPos(rect_getWidth(&di.r), is.w, rn->bg_x_t, rn->bg_x);
        di.oy = css_calcBgPos(rect_getHeight(&di.r), is.h, rn->bg_y_t, rn->bg_y);
        imagePlayer_drawBg(view->imgPlayer, di);
    }
}

static void get_clipped_background_if_exist(NRenderNode* rn, NStyle* style)
{
    NView* view = (NView*)style->view;
    
    if (imagePlayer_isClipped(view->imgPlayer, rn->bgiid)) {

        NNode* node = (NNode*)rn->node;
        NRect* tc_rect;
        int16 id = -1;
        NImageDrawInfo info;
        coord tc_originw;
        
        tc_rect = attr_getRect(node->atts, ATTID_TC_RECT);
        if (tc_rect == N_NULL)
            return;
        
        NBK_memset(&info, 0, sizeof(NImageDrawInfo));
        info.id = rn->bgiid;
        info.r = *tc_rect;
        tc_originw = attr_getCoord(node->atts, ATTID_TC_CUTEORIGINW);
        if (tc_originw != -1)
            info.r.r = info.r.l + tc_originw;
        info.ox_t = rn->bg_x_t;
        info.oy_t = rn->bg_y_t;
        info.ox = rn->bg_x;
        info.oy = rn->bg_y;
        info.repeat = rn->bg_repeat;
        id = imagePlayer_getIdClipped(view->imgPlayer, &info);
        
        if (id != -1) {
            NBK_memset(&info, 0, sizeof(NImageDrawInfo));
            if (imagePlayer_getDrawInfo(view->imgPlayer, id, &info)) {
                rn->bgiid = id;
                if (rn->bg_x_t == CSS_POS_PIXEL) {
                    rn->bg_x = info.ox;
                    rn->bg_y = info.oy;
                }
            }
        }    
    }
}

void renderNode_drawBgImageFF(NRenderNode* rn, NRect* pr, NRect* vr, NStyle* style)
{
    NView* view = (NView*)style->view;
    NNode* node = (NNode*)rn->node;
    NSize is;
    bd_bool fail;
    int id = -1;
    coord tc_originw = -1;
    
    if (rn->bgiid == -1)
        return;
    
    if (rn->bgiid == IMAGE_CUTE_ID) {
        NRenderNode* first = rn->prev;
        while (first) {
            if (   first->bgiid != -1
                && first->bgiid != IMAGE_CUTE_ID )
                break;
            first = first->prev;
        }
        if (first == N_NULL)
            return;
        
        id = first->bgiid; // found image to use
        tc_originw = attr_getCoord(((NNode*)first->node)->atts, ATTID_TC_CUTEORIGINW);
    }
    
    if (id == -1) {
        get_clipped_background_if_exist(rn, style);
        id = rn->bgiid;
    }

    if (imagePlayer_getSize(view->imgPlayer, id, &is, &fail)) {
        NImageDrawInfo di;
        coord tc_offsetx;
        coord w;
        NRect* tc_rect = attr_getRect(node->atts, ATTID_TC_RECT);
        
        tc_offsetx = attr_getCoord(node->atts, ATTID_TC_CUTEOFFSETX);
        if (tc_originw == -1)
            tc_originw = attr_getCoord(node->atts, ATTID_TC_CUTEORIGINW);
        
        if (tc_offsetx != -1 || tc_originw != -1)
            w = tc_originw;
        else
            w = rect_getWidth(tc_rect);
        
        di.id = id;
        di.r = *tc_rect;
        di.pr = pr;
        di.vr = vr;
        di.zoom = &style->zoom;
        di.repeat = rn->bg_repeat;
        di.ox = css_calcBgPos(w, is.w, rn->bg_x_t, rn->bg_x);
        if (tc_offsetx != -1) {
            di.ox -= tc_offsetx;
            if (di.ox < 0 && (rn->bg_repeat == CSS_REPEAT || rn->bg_repeat == CSS_REPEAT_X)) { // adjust x offset
                int n = -di.ox / is.w;
                di.ox += is.w * n;
            }
        }
        di.oy = css_calcBgPos(rect_getHeight(tc_rect), is.h, rn->bg_y_t, rn->bg_y);
        
        imagePlayer_drawBg(view->imgPlayer, di);
    }
}

void renderNode_getSelPos(NRenderNode* rn, int* start, int* end)
{
    if (rn) {
        if (rn->type == RNT_INPUT)
            renderInput_getSelPos((NRenderInput*)rn, start, end);
        else if (rn->type == RNT_TEXTAREA)
            renderTextarea_getSelPos((NRenderTextarea*)rn, start, end);
    }
}

void renderNode_setSelPos(NRenderNode* rn, int start, int end)
{
    if (rn) {
        if (rn->type == RNT_INPUT)
            renderInput_setSelPos((NRenderInput*)rn, start, end);
        else if (rn->type == RNT_TEXTAREA)
            renderTextarea_setSelPos((NRenderTextarea*)rn, start, end);
    }
}

void rtree_traverse_depth(NRenderNode* root,
                          RTREE_TRAVERSE_START_CB startCb,
                          RTREE_TRAVERSE_CB closeCb,
                          void* user)
{
    NRenderNode* n = root;
    int l = -1;
    bd_bool ignore = N_FALSE;
    NRenderNode** sta = (NRenderNode**)NBK_malloc(sizeof(NRenderNode*) * MAX_TREE_DEPTH);
    
    while (n) {
        
        if (!ignore) {
            if (startCb && startCb(n, user, &ignore))
                break;
        }
        
        if (!ignore && n->child) {
            l++;
            if (l == MAX_TREE_DEPTH)
                break;
            sta[l] = n;
            n = n->child;
        }
        else {
            if (closeCb && !ignore && n->child == N_NULL) {
                if (closeCb(n, user))
                    break;
            }
            
            ignore = N_FALSE;
            
            if (n == root)
                break;
            else if (n->next)
                n = n->next;
            else {
                n = sta[l--];
                if (closeCb && closeCb(n, user))
                    break;
                ignore = N_TRUE;
            }
        }
    }
    
    NBK_free(sta);
}

NRenderNode* renderNode_getParent(NRenderNode* rn, uint8 type)
{
    NRenderNode* r = rn;
    while (r) {
        if (r->type == type)
            break;
        r = r->parent;
    }
    return r;
}

void renderNode_getAbsPos(NRenderNode* rn, coord* x, coord* y)
{
    NRenderNode* r = rn;
    *x = *y = 0;
    while (r) {
        r->GetAbsXY(r, x, y);
        r = r->parent;
    }
}

void renderNode_checkAdFF(NRenderNode* rn)
{
    NNode* node = (NNode*)rn->node;
    int ad = attr_getValueInt32(node->atts, ATTID_TC_ISADS);
    if (ad > 0)
        rn->isAd = 1;
}

void renderNode_drawAdPlaceholderFF(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRect pr, cl;
    
    pr = rn->r;
    if (!rect_isIntersect(&style->dv, &pr))
        return;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    NBK_drawAdPlaceHolder(page->platform, &pr);
    NBK_gdi_cancelClippingRect(page->platform);
}

void renderNode_drawFocusRing(NRect pr, NColor color, void* pfd)
{
    NRect l;
    coord bold = FOCUS_RING_BOLD;
    coord da = bold >> 1;
    
    NBK_gdi_setBrushColor(pfd, &color);
    
    l = pr;
    l.l -= da;
    l.r = l.l + bold;
    NBK_gdi_clearRect(pfd, &l);
    
    l = pr;
    l.t -= da;
    l.b = l.t + bold;
    NBK_gdi_clearRect(pfd, &l);
    
    l = pr;
    l.r += da;
    l.l = l.r - bold;
    NBK_gdi_clearRect(pfd, &l);
    
    l = pr;
    l.b += da;
    l.t = l.b - bold;
    NBK_gdi_clearRect(pfd, &l);
}
