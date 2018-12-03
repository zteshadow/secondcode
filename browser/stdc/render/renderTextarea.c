/*
 * renderTextarea.c
 *
 *  Created on: 2011-4-9
 *      Author: migr
 */

#include "../inc/config.h"
#include "../inc/nbk_ctlPainter.h"
#include "renderTextarea.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/history.h"
#include "../tools/unicode.h"
#include "../tools/str.h"

#define DEFAULT_HORI_SPACING    4
#define DEFAULT_VERT_SPACING    4
#define DEFAULT_SPACING         4
#define DEFAULT_COLS            40
#define DEFAULT_ROWS            3

#ifdef NBK_MEM_TEST
int renderTextarea_memUsed(const NRenderTextarea* rt)
{
    int size = 0;
    if (rt) {
        size = sizeof(NRenderTextarea);
        size += textEditor_memUsed(rt->editor);
    }
    return size;
}
#endif

NRenderTextarea* renderTextarea_create(void* node, nid type)
{
    NRenderTextarea* r = (NRenderTextarea*)NBK_malloc0(sizeof(NRenderTextarea));
    N_ASSERT(r);
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_TEXTAREA;
    r->d.node = node;
    
    r->d.GetStartPoint = renderTextarea_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderTextarea_layoutSimple : renderTextarea_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderTextarea_paintSimple : renderTextarea_paint;
    
    r->absCoord = (type >= NEDOC_FULL) ? 1 : 0;
    
    r->editor = textEditor_create();
    
    return r;
}

void renderTextarea_delete(NRenderTextarea** area)
{
    NRenderTextarea* r = *area;
    textEditor_delete(&r->editor);
    NBK_free(r);
    *area = N_NULL;
}

void renderTextarea_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    if (byChild) {
        N_KILL_ME()
    }
    
    *x = rn->r.r;
    *y = rn->r.t;
}

void renderTextarea_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderTextarea* rt = (NRenderTextarea*)rn;
    NNode* node = (NNode*)rn->node;
    coord maxw = rn->parent->GetContentWidth(rn->parent);
    coord x, y, w, h;
    int /*cols,*/ rows;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    bd_bool hasAA = N_FALSE;
    
    if (!rn->needLayout && !force)
        return;
    
    rt->pfd = page->platform;
    
    rt->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
//    cols = attr_getCoord(node->atts, ATTID_COLS);
//    if (cols == -1)
//        cols = DEFAULT_COLS;
    rows = attr_getCoord(node->atts, ATTID_ROWS);
    if (rows == -1)
        rows = DEFAULT_ROWS;
    else if (rows > 5)
        rows = 5;
    
//    w = NBK_gdi_getCharWidth(page->platform, rt->fontId, 0x3000) / 2 * cols;
//    w += DEFAULT_HORI_SPACING;
//    if (w > maxw) w = maxw;
    w = maxw - DEFAULT_HORI_SPACING;
    
    h = (NBK_gdi_getFontHeight(page->platform, rt->fontId) + 2) * rows;
    h += DEFAULT_VERT_SPACING;
    
    if (hasAA) {
        
    }
    else {
        if (byChild) {
            rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
        }
        else {
            rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
        }
        
        if (maxw - x >= w) {
            rn->r.l = x;
            rn->r.r = x + w;
            rn->r.t = y;
            rn->r.b = y + h;
        }
        else {
            coord maxb = renderNode_getMaxBottomByLine(rn->prev, 0);
            rn->r.l = 0;
            rn->r.r = w;
            rn->r.t = maxb;
            rn->r.b = maxb + h;
        }
        rn->mr = maxw;
    }
    
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;

    rt->editor->fontId = rt->fontId;
    rt->editor->vis = rows;
    rt->editor->width = rect_getWidth(&rn->r) - DEFAULT_HORI_SPACING - 6;
    
    if (node->child && node->child->id == TAGID_TEXT)
        textEditor_setText(rt->editor, node->child->d.text, node->child->len, page->platform);

    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderTextarea_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderTextarea* rt = (NRenderTextarea*)rn;
    coord x, y, dx, dy;
    NRect pr, cl;
    NECtrlState st = NECS_NORMAL;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    rect_grow(&pr, -1, -1);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    
    if (rt->inEdit)
        st = NECS_INEDIT;
    else if (style->highlight)
        st = NECS_HIGHLIGHT;
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintTextarea(page->platform, &pr, st) == N_FALSE) {
        NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
        NBK_gdi_clearRect(page->platform, &pr);
        
        if (style->highlight) {
            renderNode_drawFocusRing(pr,
                (rt->inEdit) ? style->ctlEditColor : style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (rt->inEdit) {
        int len;
        wchr* text = renderTextarea_getText16(rt, &len);
        if (NBK_editTextarea(page->platform, rt->fontId, &pr, text, len))
            return;
    }

    dx = DEFAULT_HORI_SPACING >> 1;
    dy = DEFAULT_VERT_SPACING >> 1;
    dx = (coord)nfloat_imul(dx, style->zoom);
    dy = (coord)nfloat_imul(dy, style->zoom);
    rect_grow(&pr, -dx, -dy);
    NBK_gdi_setClippingRect(page->platform, &pr);
    textEditor_draw(rt->editor, &pr, rt->inEdit, style);
    NBK_gdi_cancelClippingRect(page->platform);
}

void renderTextarea_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderTextarea* rt = (NRenderTextarea*)rn;
    NNode* node = (NNode*)rn->node;
    coord x, y;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    
    rt->pfd = page->platform;
    
    rt->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    rn->r = *attr_getRect(node->atts, ATTID_TC_RECT);
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);

    {
        NNode* child = ((NNode*)rn->node)->child;
        while (child && child->id != TAGID_TEXT)
            child = child->child;
        if (child) {
            textEditor_setText(rt->editor, child->d.text, child->len, page->platform);
        }
    }
    
    rn->needLayout = 0;
}

void renderTextarea_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderTextarea* rt = (NRenderTextarea*)rn;
    NRect pr, cl;
    coord dx, dy, lh;
    NECtrlState st = NECS_NORMAL;
    coord w, h;
    NRect va;
    bd_bool zoomChange = N_FALSE;
    
    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    
    if (rt->inEdit)
        st = NECS_INEDIT;
    else if (style->highlight)
        st = NECS_HIGHLIGHT;
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintTextarea(page->platform, &pr, st) == N_FALSE) {
        NBK_gdi_setBrushColor(page->platform, (rn->hasBgcolor) ? &rn->background_color : &style->ctlFillColor);
        NBK_gdi_clearRect(page->platform, &pr);
        
        if (style->highlight || rt->inEdit) {
            renderNode_drawFocusRing(pr,
                (rt->inEdit) ? style->ctlEditColor : style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (rt->inEdit) {
        int len;
        wchr* text = renderTextarea_getText16(rt, &len);
        if (NBK_editTextarea(page->platform, rt->fontId, &pr, text, len))
            return;
    }
    
    lh = NBK_gdi_getFontHeightByEditor(page->platform, rt->fontId);
    NBK_helper_getViewableRect(page->platform, &va);

    w = rect_getWidth(&pr);
    h = rect_getHeight(&pr);
    
    if (rt->trick != w) {
        rt->trick = w;
        zoomChange = N_TRUE;
    }
    
    if (w > rect_getWidth(&va))
        w = rect_getWidth(&va);
    w = w - DEFAULT_HORI_SPACING;
    if (h > rect_getHeight(&va))
        h = rect_getHeight(&va);
    h = h - DEFAULT_VERT_SPACING;
    
    rt->editorSize.w = w;
    rt->editorSize.h = h;

    if (zoomChange)
        textEditor_prepare(rt->editor, rt->fontId, w, h, lh, page->platform);
    
    dx = DEFAULT_HORI_SPACING >> 1;
    dy = DEFAULT_VERT_SPACING >> 1;
    rect_setWidth(&pr, w);
    rect_setHeight(&pr, h);
    rect_grow(&pr, -dx, -dy);
    NBK_gdi_setClippingRect(page->platform, &cl);
    textEditor_draw(rt->editor, &pr, rt->inEdit, style);
    NBK_gdi_cancelClippingRect(page->platform);
}

int renderTextarea_getEditTextLen(NRenderTextarea* rt)
{
    return textEditor_getTextLen(rt->editor);
}

// need release
char* renderTextarea_getEditText(NRenderTextarea* rt)
{
    return textEditor_getText(rt->editor);
}

void renderTextarea_edit(NRenderTextarea* rt, void* doc)
{
    NPage* page = (NPage*)((NDocument*)doc)->page;
    NNode* node = (NNode*) rt->d.node;
    uint8* defaulTxt = N_NULL;
    bd_bool lastInEdit = N_FALSE;    
    defaulTxt = (uint8*) attr_getValueStr(node->atts, ATTID_ALT);// rt->editor->defaultTxt;
    lastInEdit = rt->inEdit;

    if(!rt->inEdit)
    rt->inEdit = !rt->inEdit;
    
    if (rt->inEdit) {
        bd_bool clearDefault = N_FALSE;
        int len = NBK_strlen((char*) defaulTxt);
        int i = 0;
        int8 offset;
        uint8* tooFar = defaulTxt + len;
        
        NBK_fep_enable(page->platform);
       
        while (defaulTxt && defaulTxt < tooFar && i < rt->editor->use) {
            wchr tmpWChar = uni_utf8_to_utf16((uint8*) defaulTxt, &offset);
            if (tmpWChar != rt->editor->text[i]) {
                break;
            }
            defaulTxt += offset;
            i++;
        }
        if (defaulTxt && defaulTxt >= tooFar && i >= rt->editor->use)
            clearDefault = N_TRUE;
        
        if (clearDefault) {
            textEditor_clearAllText(rt->editor,page->platform);
        }
        else if (!lastInEdit) {
            textEditor_SeleAllText(rt->editor,page->platform);
            NBK_fep_updateCursor(page->platform);
        }
        else if (lastInEdit) {
            textEditor_clearSelText(rt->editor);
            NBK_fep_updateCursor(page->platform);
        }
    }
    else {
        if (rt->editor->use <= 0) {
            if (defaulTxt)
            {
                wchr* tmp = uni_utf8_to_utf16_str((uint8*) defaulTxt, NBK_strlen((char*)defaulTxt), N_NULL);
                textEditor_setText(rt->editor, tmp, NBK_wcslen(tmp), page->platform);
                NBK_free(tmp);
            }
        }
        
        NBK_fep_disable(page->platform);
    }
}

bd_bool renderTextarea_processKey(NRenderNode* rn, NEvent* event)
{
    NRenderTextarea* rt = (NRenderTextarea*)rn;
    NPage* page = (NPage*)event->page;
    
    if (rt->inEdit) {
        bd_bool handle = N_TRUE;
        NRect vr = rn->r;

        if (NEEVENT_PEN == event->type) {
            if (!doc_isAbsLayout(page->doc->type))
                renderNode_getAbsRect(rn, &vr);
            
            rect_toView(&vr, history_getZoom((NHistory*)page->history));
            if (!rect_hasPt(&vr, event->d.penEvent.pos.x, event->d.penEvent.pos.y))
                handle = N_FALSE;
        }

        if (handle)
            return textEditor_handleKey(rt->editor, event, &vr, rt->fontId);
    }

        return N_FALSE;
}

int renderTextarea_getEditLength(NRenderTextarea* rt)
{
    if (rt->editor)
        return rt->editor->use;
    else
        return 0;
}

int renderTextarea_getEditMaxLength(NRenderTextarea* rt)
{
    if (rt->editor)
        return rt->editor->max;
    else
        return 0;
}

void renderTextarea_getRectOfEditing(NRenderTextarea* rt, NRect* rect, NFontId* fontId)
{
    NRect r;
    coord x, y;
    
    if (rt->absCoord) {
        r = rt->d.r;
        r.t += textEditor_getYOfEditing(rt->editor, rt->pfd);
        r.b = r.t + NBK_gdi_getFontHeight(rt->pfd, rt->fontId);
    }
    else {
        renderNode_getAbsPos(rt->d.parent, &x, &y);

        r.l = x + rt->d.r.l;
        r.r = r.l + rect_getWidth(&rt->d.r);

        r.t = y + rt->d.r.t;
        r.t += textEditor_getYOfEditing(rt->editor, rt->pfd);
        r.b = r.t + NBK_gdi_getFontHeight(rt->pfd, rt->fontId);
    }
    
    *rect = r;
    *fontId = rt->fontId;
}

bd_bool renderTextarea_isEditing(NRenderTextarea* rt)
{
    return rt->inEdit;
}

wchr* renderTextarea_getText16(NRenderTextarea* rt, int* len)
{
    if (rt->editor) {
        *len = rt->editor->use;
        return rt->editor->text;
    }
    else {
        *len = 0;
        return N_NULL;
    }
}

void renderTextarea_setText16(NRenderTextarea* rt, wchr* text, int len)
{
    if (rt->editor) {
        textEditor_setText(rt->editor, text, len, N_NULL);
    }
}

void renderTextarea_getSelPos(NRenderTextarea* rt, int* start, int* end)
{
    if (rt->editor) {
        *start = rt->editor->anchorPos;
        *end = rt->editor->cur;
    }
}

void renderTextarea_setSelPos(NRenderTextarea* rt, int start, int end)
{
    if (rt->editor) {
        rt->editor->anchorPos = start;
        rt->editor->cur = end;
    }
}

int16 renderTextarea_getDataSize(NRenderNode* rn, NStyle* style)
{
    NNode* node = (NNode*)rn->node;
    int16 cols, size;
    
    cols = attr_getCoord(node->atts, ATTID_COLS);
    if (cols == -1)
        cols = DEFAULT_COLS;
    
    size = cols / 2;
    if (cols % 2)
        size++;
    
    return size;
}
