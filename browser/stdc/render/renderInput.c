/*
 * renderInput.c
 *
 *  Created on: 2011-3-24
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_ctlPainter.h"
#include "renderInput.h"
#include "imagePlayer.h"
#include "../dom/node.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../dom/xml_tags.h"
#include "../dom/page.h"
#include "../dom/view.h"
#include "../dom/document.h"
#include "../dom/history.h"
#include "../css/color.h"
#include "../css/css_helper.h"
#include "../css/css_val.h"
#include "../tools/str.h"
#include "../tools/unicode.h"
#include "../tools/dump.h"
#include "../loader/url.h"

#define DEFAULT_TEXT_SIZE       20
#define DEFAULT_HORI_SPACING    8
#define DEFAULT_VERT_SPACING    8
#define DEFAULT_SPACING         4
#define DEFAULT_BUTTON_WIDTH    60

static uint8 l_label_submit[7] = {0xe6, 0x8f, 0x90, 0xe4, 0xba, 0xa4, 0x0};
static uint8 l_label_reset[7] = {0xe9, 0x87, 0x8d, 0xe7, 0xbd, 0xae, 0x0};
static char l_label_go[3] = {'G', 'O', 0};

//static int l_input_no = 0;

#ifdef NBK_MEM_TEST
int renderInput_memUsed(const NRenderInput* ri)
{
    int size = 0;
    if (ri) {
        size = sizeof(NRenderInput);
        size += editBox_memUsed(ri->editor);
        if (ri->text)
            size += NBK_strlen(ri->text) + 1;
        if (ri->textU)
            size += (NBK_wcslen(ri->textU) + 1) * 2;
    }
    return size;
}
#endif

NRenderInput* renderInput_create(void* node, nid type)
{
    NRenderInput* ri = (NRenderInput*)NBK_malloc0(sizeof(NRenderInput));
    N_ASSERT(ri);
    
    renderNode_init(&ri->d);
    
    ri->d.type = RNT_INPUT;
    ri->d.node = node;
    ri->iid = -1;
    
    ri->d.GetStartPoint = renderInput_getStartPoint;
    ri->d.Layout = (type >= NEDOC_FULL) ? renderInput_layoutSimple : renderInput_layout;
    ri->d.Paint = (type >= NEDOC_FULL) ? renderInput_paintSimple : renderInput_paint;
    
    ri->absCoord = (type >= NEDOC_FULL) ? 1 : 0;
    
    ri->type = NEINPUT_TEXT;
    {
        char* p = attr_getValueStr(((NNode*)node)->atts, ATTID_TYPE);
        if (p) {
            /*if (NBK_strcmp(p, "text") == 0)
                ri->type = NEINPUT_TEXT;
            else*/ if (NBK_strcmp(p, "password") == 0)
                ri->type = NEINPUT_PASSWORD;
            else if (NBK_strcmp(p, "submit") == 0)
                ri->type = NEINPUT_SUBMIT;
            else if (NBK_strcmp(p, "reset") == 0)
                ri->type = NEINPUT_RESET;
            else if (NBK_strcmp(p, "button") == 0)
                ri->type = NEINPUT_BUTTON;
            else if (NBK_strcmp(p, "image") == 0)
                ri->type = NEINPUT_IMAGE;
            else if (NBK_strcmp(p, "hidden") == 0)
                ri->type = NEINPUT_HIDDEN;
            else if (NBK_strcmp(p, "checkbox") == 0)
                ri->type = NEINPUT_CHECKBOX;
            else if (NBK_strcmp(p, "radio") == 0)
                ri->type = NEINPUT_RADIO;
            else if (NBK_strcmp(p, "file") == 0)
                ri->type = NEINPUT_FILE;
            else if (NBK_strcmp(p, "go") == 0)
                ri->type = NEINPUT_GO;
        }
    }

    switch (ri->type) {
    case NEINPUT_TEXT:
    case NEINPUT_PASSWORD:
    {
        int16 l = attr_getCoord(((NNode*)node)->atts, ATTID_MAXLENGTH);
        char* p = attr_getValueStr(((NNode*)node)->atts, ATTID_VALUE);
        ri->editor = editBox_create((ri->type == NEINPUT_PASSWORD) ? 1 : 0);
        if (l != -1)
            editBox_setMaxLength(ri->editor, l);
        if (p)
            editBox_setText(ri->editor, (uint8*)p, -1);
        break;
    }
    case NEINPUT_CHECKBOX:
    case NEINPUT_RADIO:
        ri->checked = attr_getValueBool(((NNode*)node)->atts, ATTID_CHECKED);
        break;
    }
    
    return ri;
}

void renderInput_delete(NRenderInput** ri)
{
    NRenderInput* i = *ri;
    
    if (i->editor)
        editBox_delete(&i->editor);
    if (i->text)
        NBK_free(i->text);
    if (i->textU)
        NBK_free(i->textU);
    
    NBK_free(i);
    *ri = N_NULL;
}

void renderInput_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    if (byChild) N_KILL_ME()
    
    *x = rn->r.r;
    *y = rn->r.t;
}

void renderInput_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NView* view = (NView*)style->view;
    NPage* page = (NPage*)view->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NNode* node = (NNode*)rn->node;
    coord maxw = rn->parent->GetContentWidth(rn->parent);
    coord x, y, w, h;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    int16 sz;
    bd_bool hasAA = N_FALSE;
    char* txt;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    if (ri->type == NEINPUT_HIDDEN)
        rn->display = CSS_DISPLAY_NONE;
    
    if (style->hasBgcolor) {
        rn->hasBgcolor = 1;
        rn->background_color = style->background_color;
        rn->color = style->color;
    }
    
    ri->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
    h = NBK_gdi_getFontHeight(page->platform, ri->fontId);
    h += DEFAULT_VERT_SPACING;
    
    w = 0;
    sz = attr_getCoord(node->atts, ATTID_SIZE);
    if (ri->type == NEINPUT_TEXT || ri->type == NEINPUT_PASSWORD) {
        if (sz == -1) sz = DEFAULT_TEXT_SIZE;
        sz += 4;
        w = NBK_gdi_getCharWidth(page->platform, ri->fontId, 0x3000) / 2 * sz;
    }
    else if (ri->type == NEINPUT_CHECKBOX || ri->type == NEINPUT_RADIO) {
        w = h;
    }
    else if (ri->type == NEINPUT_FILE) {
        w = maxw;
    }
    else if (ri->type != NEINPUT_HIDDEN && ri->type != NEINPUT_UNKNOWN) {
        if (sz == -1) {
            ri->picGot = 0;
            
            if (ri->type == NEINPUT_IMAGE && ri->iid != -1 && !ri->picFail) {
                NSize size;
                bd_bool fail;
                ri->picGot = imagePlayer_getSize(view->imgPlayer, ri->iid, &size, &fail);
                if (ri->picGot) {
                    w = size.w;
                    h = size.h;
                    ri->picFail = N_FALSE;
                }
                else if (fail)
                    ri->picFail = N_TRUE;
            }
            
            if (!ri->picGot) {
                txt = attr_getValueStr(node->atts, ATTID_VALUE);
                if (txt == N_NULL)
                    txt = attr_getValueStr(node->atts, ATTID_ALT);
                if (txt == N_NULL) {
                    if (ri->type == NEINPUT_SUBMIT || ri->type == NEINPUT_IMAGE)
                        txt = (char*)l_label_submit;
                    else if (ri->type == NEINPUT_RESET)
                        txt = (char*)l_label_reset;
                    else if (ri->type == NEINPUT_GO)
                        txt = l_label_go;
                }
                
                if (txt) {
                    char* txtEsc = (char*)NBK_malloc(NBK_strlen(txt) + 1);
                    uint8* p = (uint8*)txtEsc;
                    int wcLen;
                    nbk_unescape(txtEsc, txt);
                    while (*p) {
                        if (*p < 0x20) *p = 0x20;
                        p++;
                    }
                    if (ri->textU)
                        NBK_free(ri->textU);
                    ri->textU = uni_utf8_to_utf16_str((uint8*)txtEsc, -1, &wcLen);
                    NBK_free(txtEsc);
                    w = NBK_gdi_getTextWidth(page->platform, ri->fontId, ri->textU, wcLen);
                }
                if (w == 0)
                    w = DEFAULT_BUTTON_WIDTH;
            }
        }
        else {
            w = sz;
        }
    }
    
    if (   ri->type != NEINPUT_HIDDEN
        && ri->type != NEINPUT_CHECKBOX
        && ri->type != NEINPUT_RADIO
        && ri->type != NEINPUT_FILE
        && ri->type != NEINPUT_UNKNOWN) {
        
        if (ri->type == NEINPUT_IMAGE && ri->picGot && !ri->picFail)
            {}
        else
            w += DEFAULT_HORI_SPACING;
    }
    
    ri->cssWH = 0;
    if (style->width.type) {
        w = css_calcValue(style->width, maxw, style, w);
        ri->cssWH = 1;
    }
    if (style->height.type && ri->type != NEINPUT_TEXT) {
        h = css_calcValue(style->height, h, style, h);
        ri->cssWH = 1;
    }
    
    if (w > maxw) w = maxw;
    
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
            coord mb = renderNode_getMaxBottomByLine(rn->prev, 0);
            rn->r.l = 0;
            rn->r.r = w;
            rn->r.t = mb;
            rn->r.b = mb + h;
        }
        rn->mr = maxw;
    }
    
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

static void paint_text(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y, da;
    NRect pr, cl;
    NECtrlState st = NECS_NORMAL;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    da = (ri->cssWH) ? 0 : (coord)nfloat_imul(1, style->zoom);
    rect_grow(&pr, -da, -da);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;

    if (ri->inEdit)
        st = NECS_INEDIT;
    else if (style->highlight)
        st = NECS_HIGHLIGHT;
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintText(page->platform, &pr, st) == N_FALSE) {
        
        NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
        NBK_gdi_clearRect(page->platform, &pr);
        
        if (style->highlight || ri->inEdit) {
            renderNode_drawFocusRing(pr,
                (ri->inEdit) ? style->ctlEditColor : style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);

    if (ri->inEdit) {
        int len;
        wchr* text = renderInput_getEditText16(ri, &len);
        if (NBK_editInput(page->platform, ri->fontId, &pr, text, len))
            return;
    }

    if (ri->editor) {
        coord dx = DEFAULT_HORI_SPACING >> 1;
        coord dy = DEFAULT_VERT_SPACING >> 1;
        dx = (coord)nfloat_imul(dx, style->zoom);
        dy = (coord)nfloat_imul(dy, style->zoom);
        rect_grow(&pr, -dx, -dy);
        rect_grow(&pr, da, da);
        NBK_gdi_setClippingRect(page->platform, &pr);
        pr.r -= 2;
        NBK_gdi_useFont(page->platform, ri->fontId);
        editBox_draw(ri->editor, ri->fontId, &pr, ri->inEdit, style);
        NBK_gdi_releaseFont(page->platform);
        NBK_gdi_cancelClippingRect(page->platform);
    }
}

static void paint_button(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y, dx, dy, da;
    NRect pr, cl;
    NPoint pt;
    int len = 0;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    da = (ri->cssWH) ? 0 : (coord)nfloat_imul(1, style->zoom);
    rect_grow(&pr, -da, -da);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;

    if (ri->textU)
        len = NBK_wcslen(ri->textU);
    
    imagePlayer_setDraw(page->view->imgPlayer, rn->bgiid);
    if (rn->bgiid != -1) {
        NRect ir = rn->r;
        rect_move(&ir, ir.l + x, ir.t + y);
        rect_toView(&ir, style->zoom);
        rect_move(&ir, ir.l - rect->l, ir.t - rect->t);
        renderNode_drawBgImage(rn, &ir, rect, style);
    }
    else {
        NBK_gdi_setClippingRect(page->platform, &cl);
        
        if (NBK_paintButton(page->platform, ri->fontId, &pr, ri->textU, len,
                            ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL))) {
            NBK_gdi_cancelClippingRect(page->platform);
            return;
        }
    }

    if (rn->bgiid == -1) {
        NBK_gdi_setBrushColor(page->platform,
            (rn->hasBgcolor) ? &rn->background_color : &style->ctlBgColor);
        NBK_gdi_clearRect(page->platform, &pr);
        
        if (!style->highlight) {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
        
        NBK_gdi_cancelClippingRect(page->platform);
    }
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    cl = pr;

    if (ri->textU) {
        coord fw, fh, fa;
        
        fw = NBK_gdi_getTextWidth(page->platform, ri->fontId, ri->textU, len);
        fh = NBK_gdi_getFontHeight(page->platform, ri->fontId);
        fa = NBK_gdi_getFontAscent(page->platform, ri->fontId);
        
        fw = (coord)nfloat_imul(fw, style->zoom);
        fh = (coord)nfloat_imul(fh, style->zoom);
        fa = (coord)nfloat_imul(fa, style->zoom);
        
        fw = (fw > rect_getWidth(&pr)) ? rect_getWidth(&pr) : fw;
        dx = (rect_getWidth(&pr) - fw) >> 1;
        dy = (rect_getHeight(&pr) - fh) >> 1;
        
        NBK_gdi_useFont(page->platform, ri->fontId);
        
        pt.x = pr.l + dx;
        pt.y = pr.t + dy + fa;
        NBK_gdi_setPenColor(page->platform,
            (rn->hasBgcolor) ? &rn->color : &style->ctlFgColor);
        NBK_gdi_drawText(page->platform, ri->textU, len, &pt, 0);
        
        NBK_gdi_releaseFont(page->platform);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
}

static void paint_buttonImage(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NView* view = (NView*)style->view;
    NPage* page = (NPage*)view->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y;
    NRect pr, cl;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;

    NBK_gdi_setClippingRect(page->platform, &cl);
    
    imagePlayer_draw(view->imgPlayer, ri->iid, &pr);
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
    
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_checkbox(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y, da;
    NRect pr, cl;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    da = (coord)nfloat_imul(4, style->zoom);
    rect_grow(&pr, -da, -da);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);

    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintCheckbox(
        page->platform, &pr, ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL), ri->checked)) {
        NBK_gdi_cancelClippingRect(page->platform);
        return;
    }
    
    NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
    NBK_gdi_clearRect(page->platform, &pr);
    
    if (!style->highlight) {
        NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
        NBK_gdi_drawRect(page->platform, &pr);
    }
    
    if (ri->checked) {
        da = (coord)nfloat_imul(3, style->zoom);
        cl = pr;
        rect_grow(&cl, -da, -da);
        NBK_gdi_setBrushColor(page->platform, &style->ctlEditColor);
        NBK_gdi_clearRect(page->platform, &cl);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
}

static void paint_radio(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y, da;
    NRect pr, cl;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    da = (coord)nfloat_imul(4, style->zoom);
    rect_grow(&pr, -da, -da);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);

    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintRadio(
        page->platform, &pr, ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL), ri->checked)) {
        NBK_gdi_cancelClippingRect(page->platform);
        return;
    }
    
    if (!style->highlight) {
        NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
        NBK_gdi_drawCircle(page->platform, &pr);
    }
    
    if (ri->checked) {
        int i;
        da = (coord)nfloat_imul(4, style->zoom);
        cl = pr;
        rect_grow(&cl, -da, -da);
        NBK_gdi_setPenColor(page->platform, &style->ctlEditColor);
        for (i=0; i < 4; i++) {
            NBK_gdi_drawCircle(page->platform, &cl);
            rect_grow(&cl, -1, -1);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
}

static void paint_file(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    coord x, y, da;
    NRect pr, cl;
    int len = 0;
    
    renderNode_getAbsPos(rn->parent, &x, &y);

    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    da = (coord)nfloat_imul(1, style->zoom);
    rect_grow(&pr, -da, -da);

    if (ri->textU == N_NULL) {
        if (ri->text) {
            int wcLen;
            ri->textU = uni_utf8_to_utf16_str((uint8*)ri->text, -1, &wcLen);
            len = wcLen;
        }
    }
    else
        len = NBK_wcslen(ri->textU);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;

    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintBrowse(page->platform, ri->fontId, &pr, ri->textU, len,
                        ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL))) {
        NBK_gdi_cancelClippingRect(page->platform);
        return;
    }
    
    NBK_gdi_setBrushColor(page->platform, &style->ctlBgColor);
    NBK_gdi_clearRect(page->platform, &pr);
    NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
    NBK_gdi_drawRect(page->platform, &pr);
    
    if (ri->textU) {
        coord fa;
        NPoint pt;
        
        fa = NBK_gdi_getFontAscent(page->platform, ri->fontId);
        fa = (coord)nfloat_imul(fa, style->zoom);
        
        pt.x = pr.l + 2;
        pt.y = pr.t + fa;
        
        NBK_gdi_useFont(page->platform, ri->fontId);
        NBK_gdi_drawText(page->platform, ri->textU, len, &pt, 0);
        NBK_gdi_releaseFont(page->platform);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
}

void renderInput_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NRenderInput* ri = (NRenderInput*)rn;
    
    switch (ri->type) {
    case NEINPUT_TEXT:
    case NEINPUT_PASSWORD:
        paint_text(rn, style, rect);
        break;
        
    case NEINPUT_SUBMIT:
    case NEINPUT_RESET:
    case NEINPUT_BUTTON:
    case NEINPUT_GO:
        paint_button(rn, style, rect);
        break;
        
    case NEINPUT_IMAGE:
        if (ri->iid == -1 || ri->picFail || !ri->picGot)
            paint_button(rn, style, rect);
        else
            paint_buttonImage(rn, style, rect);
        break;
        
    case NEINPUT_CHECKBOX:
        paint_checkbox(rn, style, rect);
        break;
        
    case NEINPUT_RADIO:
        paint_radio(rn, style, rect);
        break;
        
    case NEINPUT_FILE:
        paint_file(rn, style, rect);
        break;
    }
}

bd_bool renderInput_isVisible(NRenderInput* ri)
{
    return ((ri->type == NEINPUT_HIDDEN) ? N_FALSE : N_TRUE);
}

bd_bool renderInput_isEditing(NRenderInput* ri)
{
    return ri->inEdit;
}

void renderInput_edit(NRenderInput* ri, void* doc)
{
    NPage* page = (NPage*)((NDocument*)doc)->page;
    NNode* node = (NNode*) ri->d.node;
    uint8* alt;
    
    if (!renderInput_isVisible(ri))
        return;
    
    alt = (uint8*)attr_getValueStr(node->atts, ATTID_ALT);
    
        ri->inEdit = !ri->inEdit;
    
    if (ri->inEdit) {
        int len = NBK_strlen((char*)alt);
            int i = 0;
            int8 offset;
        uint8* tooFar = alt + len;
            bd_bool clearDefault = N_FALSE;
        
        while (alt && alt < tooFar && i < ri->editor->use) {
            wchr tmpWChar = uni_utf8_to_utf16(alt, &offset);
            if (tmpWChar != ri->editor->text[i])
                    break;
            alt += offset;
                i++;
            }
        if (alt && alt >= tooFar && i >= ri->editor->use)
                clearDefault = N_TRUE;

            if (clearDefault) {
                editBox_clearAllText(ri->editor);
            }
        else if (ri->inEdit) {
                ri->editor->anchorPos = 0;
                ri->editor->cur = ri->editor->use;
                NBK_fep_updateCursor(page->platform);
            }
        else if (!ri->inEdit) {
                editBox_clearSelText(ri->editor);
                NBK_fep_updateCursor(page->platform);
            }
        NBK_fep_enable(page->platform);
    }
    else {
        if (ri->editor->use <= 0) {
            if (alt)
                editBox_setText(ri->editor, alt, -1);
        }
        NBK_fep_disable(page->platform);
        editBox_maskAll(ri->editor);
    }
}

bd_bool renderInput_processKey(NRenderNode* rn, NEvent* event)
{
    NRenderInput* ri = (NRenderInput*)rn;
    NPage* page = (NPage*)event->page;
    
    if (ri->inEdit) {
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
            return editBox_handleKey(ri->editor, event, &vr, ri->fontId);
        }
        
        return N_FALSE;
    }

// need release
char* renderInput_getEditText(NRenderInput* ri)
{
    if ((ri->type == NEINPUT_TEXT || ri->type == NEINPUT_PASSWORD) && ri->editor)
        return editBox_getText(ri->editor);
    else
        return N_NULL;
}

int renderInput_getEditTextLen(NRenderInput* ri)
{
    if ((ri->type == NEINPUT_TEXT || ri->type == NEINPUT_PASSWORD) && ri->editor)
        return editBox_getTextLen(ri->editor);
    else
        return 0;
}

void renderInput_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NNode* node = (NNode*)rn->node;
    coord x, y;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    
    ri->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    rn->r = *attr_getRect(node->atts, ATTID_TC_RECT);
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    
    ri->border = style->border;
    
    rn->needLayout = 0;
}

static void paint_text_simple(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NRect pr, cl;
    NECtrlState st = NECS_NORMAL;
    
    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    if (ri->inEdit)
        st = NECS_INEDIT;
    else if (style->highlight)
        st = NECS_HIGHLIGHT;
    
    cl = pr;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintText(page->platform, &pr, st) == N_FALSE) {
        
        NBK_gdi_setBrushColor(page->platform, (rn->hasBgcolor) ? &rn->background_color : &colorWhite);
        NBK_gdi_clearRect(page->platform, &pr);
        
        if (style->highlight || ri->inEdit) {
            renderNode_drawFocusRing(pr,
                (ri->inEdit) ? style->ctlEditColor : style->ctlFocusColor, page->platform);
        }
        else {
            NRect border = ri->border;
            if (border.l || border.t || border.r || border.b) {
                renderNode_drawBorder(rn, &border, &pr, page->platform);
            }
            else {
                NBK_gdi_setPenColor(page->platform, &colorGray);
                NBK_gdi_drawRect(page->platform, &pr);
            }
        }
    }
    NBK_gdi_cancelClippingRect(page->platform);
    
    if (ri->inEdit) {
        int len;
        wchr* text = renderInput_getEditText16(ri, &len);
        if (NBK_editInput(page->platform, ri->fontId, &pr, text, len))
            return;
    }

    if (ri->editor) {
        coord dx = DEFAULT_HORI_SPACING >> 1;
        coord dy = (rect_getHeight(&rn->r) - NBK_gdi_getFontHeight(page->platform, ri->fontId)) >> 1;
        NRect va;
        coord w, h;
        
        dx = (coord)nfloat_imul(dx, style->zoom);
        dy = (coord)nfloat_imul(dy, style->zoom);

        NBK_gdi_setClippingRect(page->platform, &cl);
        
        NBK_helper_getViewableRect(page->platform, &va);

        w = rect_getWidth(&pr);
        h = rect_getHeight(&pr);
        if (w > rect_getWidth(&va))
            w = rect_getWidth(&va);
        
        ri->editorSize.w = w;
        ri->editorSize.h = h;
        
        rect_setWidth(&pr, w);
        rect_grow(&pr, -dx, -dy);

        NBK_gdi_useFont(page->platform, ri->fontId);
        editBox_draw(ri->editor, ri->fontId, &pr, ri->inEdit, style);
        NBK_gdi_releaseFont(page->platform);

        NBK_gdi_cancelClippingRect(page->platform);
    }
}

static void paint_button_simple(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NNode* node = (NNode*)rn->node;
    NRect pr, cl;
    int len = 0;
    wchr* label = N_NULL;
    
    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    if (node->child && node->child->id == TAGID_TEXT && node->child->len > 0) {
        label = node->child->d.text;
    }
    else {
        if (ri->textU == N_NULL) {
            char* txt = attr_getValueStr(node->atts, ATTID_VALUE);
            if (txt == N_NULL && rn->bgiid == -1) {
                if (ri->type == NEINPUT_SUBMIT)
                    txt = (char*)l_label_submit;
                else if (ri->type == NEINPUT_RESET)
                    txt = (char*)l_label_reset;
            }
            if (txt)
                ri->textU = uni_utf8_to_utf16_str((uint8*)txt, -1, N_NULL);
        }
        label = ri->textU;
    }
    
    if (label)
        len = NBK_wcslen(label);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;

    imagePlayer_setDraw(page->view->imgPlayer, rn->bgiid);
    if (rn->bgiid != -1) {
        renderNode_drawBgImageFF(rn, &pr, rect, style);
    }
    else if (ri->iid != -1)
    {
        NBK_gdi_setClippingRect(page->platform, &cl);
        imagePlayer_draw(page->view->imgPlayer, ri->iid, &pr);
        NBK_gdi_cancelClippingRect(page->platform);
    }
    else {
        NBK_gdi_setClippingRect(page->platform, &cl);
        
        if (NBK_paintButton(page->platform, ri->fontId, &pr, label, len,
                            ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL))) {
            NBK_gdi_cancelClippingRect(page->platform);
            return;
        }
        
        NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
        NBK_gdi_drawRect(page->platform, &pr);
        NBK_gdi_cancelClippingRect(page->platform);
    }
    
    NBK_gdi_setClippingRect(page->platform, &cl);

    if (style->highlight)
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
    
    NBK_gdi_useFont(page->platform, ri->fontId);

    if (label) {
        NPoint pt;
        coord w = NBK_gdi_getTextWidth(page->platform, ri->fontId, label, len);
        pt.x = pr.l + (coord)nfloat_imul((rect_getWidth(&rn->r) - w) / 2, style->zoom);
        pt.y = pr.t + (coord)nfloat_imul((rect_getHeight(&rn->r) - NBK_gdi_getFontHeight(page->platform, ri->fontId)) / 2 \
                    + NBK_gdi_getFontAscent(page->platform, ri->fontId), style->zoom);
        NBK_gdi_setPenColor(page->platform, &rn->color);
        NBK_gdi_drawText(page->platform, label, len, &pt, 0);
    }
    
    NBK_gdi_releaseFont(page->platform);
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_radio_simple(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NRect pr, cl;
    
    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);

    if (NBK_paintRadio(
            page->platform, &pr, ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL), ri->checked)) {
    }
    else {
        if (style->highlight) {
            renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawCircle(page->platform, &pr);
        }
        
        if (ri->checked) {
            rect_grow(&pr, -2, -2);
            NBK_gdi_setBrushColor(page->platform, &style->ctlEditColor);
            NBK_gdi_clearRect(page->platform, &pr);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_checkbox_simple(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NRect pr, cl;
    
    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (NBK_paintCheckbox(page->platform, &pr,
                          ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL), ri->checked)) {
        // nothing to do
    }
    else {
        if (style->highlight) {
            renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
        
        if (ri->checked) {
            rect_grow(&pr, -2, -2);
            NBK_gdi_setBrushColor(page->platform, &style->ctlEditColor);
            NBK_gdi_clearRect(page->platform, &pr);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_file_simple(NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderInput* ri = (NRenderInput*)rn;
    NRect pr, cl;
    int len = 0;

    pr = rn->r;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (ri->text && ri->textU == N_NULL)
        ri->textU = uni_utf8_to_utf16_str((uint8*)ri->text, -1, N_NULL);
    if (ri->textU)
        len = NBK_wcslen(ri->textU);
    
    if (NBK_paintBrowse(page->platform, ri->fontId, &pr, ri->textU, len,
                        ((style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL))) {
        // nothing to do
    }
    else {
        if (style->highlight) {
            renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
        }
        else {
            NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
            NBK_gdi_drawRect(page->platform, &pr);
        }
        
        if (ri->textU) {
            NPoint pt;
            pt.x = pr.l + (coord)nfloat_imul(DEFAULT_SPACING, style->zoom);
            pt.y = pr.t + (coord)nfloat_imul((rect_getHeight(&rn->r) - NBK_gdi_getFontHeight(page->platform, ri->fontId)) / 2 \
                        + NBK_gdi_getFontAscent(page->platform, ri->fontId), style->zoom);
            NBK_gdi_setPenColor(page->platform, &rn->color);
            
            NBK_gdi_useFont(page->platform, ri->fontId);
            NBK_gdi_drawText(page->platform, ri->textU, len, &pt, 0);
            NBK_gdi_releaseFont(page->platform);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}

void renderInput_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NRenderInput* ri = (NRenderInput*)rn;
    
    switch (ri->type) {
    case NEINPUT_TEXT:
    case NEINPUT_PASSWORD:
        paint_text_simple(rn, style, rect);
        break;
        
    case NEINPUT_SUBMIT:
    case NEINPUT_RESET:
    case NEINPUT_BUTTON:
    case NEINPUT_IMAGE:
        paint_button_simple(rn, style, rect);
        break;
        
    case NEINPUT_RADIO:
        paint_radio_simple(rn, style, rect);
        break;
        
    case NEINPUT_CHECKBOX:
        paint_checkbox_simple(rn, style, rect);
        break;
        
    case NEINPUT_FILE:
        paint_file_simple(rn, style, rect);
        break;
    }
}

int renderInput_getEditLength(NRenderInput* ri)
{
    if (ri->editor)
        return ri->editor->use;
    else
        return 0;
}

int renderInput_getEditMaxLength(NRenderInput* ri)
{
    if (ri->editor)
        return ri->editor->max;
    else
        return 0;
}

void renderInput_getRectOfEditing(NRenderInput* ri, NRect* rect, NFontId* fontId)
{
    NRect r;
    coord x, y;
    
    if (ri->absCoord) {
        r = ri->d.r;
    }
    else {
        renderNode_getAbsPos(ri->d.parent, &x, &y);

        r.l = x + ri->d.r.l;
        r.t = y + ri->d.r.t;
        r.r = r.l + rect_getWidth(&ri->d.r);
        r.b = r.t + rect_getHeight(&ri->d.r);
    }
    
    *rect = r;
    *fontId = ri->fontId;
}

void renderInput_setText(NRenderInput* ri, char* text)
{
    if (ri->text)
        NBK_free(ri->text);
    ri->text = text;
    
    if (ri->textU) {
        NBK_free(ri->textU);
        ri->textU = N_NULL;
    }
}

void renderInput_setEditText16(NRenderInput* ri, wchr* text, int len)
{
    if (ri->editor) {
        editBox_setText16(ri->editor, text, len);
    }
}

wchr* renderInput_getEditText16(NRenderInput* ri, int* len)
{
    if (ri->editor) {
        *len = ri->editor->use;
        return ri->editor->text;
    }
    else {
        *len = 0;
        return N_NULL;
    }
}

void renderInput_getSelPos(NRenderInput* ri, int* start, int* end)
{
    if (ri->editor) {
        *start = ri->editor->anchorPos;
        *end = ri->editor->cur;
    }
}

void renderInput_setSelPos(NRenderInput* ri, int start, int end)
{
    bd_bool left = N_TRUE;
    
    if (ri->editor) {
        ri->editor->anchorPos = start;
        if (ri->editor->cur < end)
            left = N_FALSE;
        
        //
        if(!ri->editor->cur&&end == ri->editor->use)
            left = N_TRUE;
        if (!end && ri->editor->cur == ri->editor->use)
            left= N_FALSE;
        
        ri->editor->cur = end;
       
        editBox_cursorChanged(ri->editor, left);
    }
}

int16 renderInput_getDataSize(NRenderNode* rn, NStyle* style)
{
    NRenderInput* ri = (NRenderInput*)rn;
    NNode* node = (NNode*)rn->node;
    int16 sz, size;
    
    sz = attr_getCoord(node->atts, ATTID_SIZE);
    if (sz == -1) {
        if (ri->type == NEINPUT_TEXT || ri->type == NEINPUT_PASSWORD)
            size = DEFAULT_TEXT_SIZE / 2;
        else if (ri->type == NEINPUT_CHECKBOX || ri->type == NEINPUT_RADIO)
            size = 2;
        else if (ri->type == NEINPUT_HIDDEN)
            size = 0;
        else
            size = DEFAULT_BUTTON_WIDTH / style->font_size;
    }
    else {
        size = sz / 2;
        if (sz % 2)
            size++;
    }
    
    return size;
}
