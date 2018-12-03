/*
 * renderSelect.c
 *
 *  Created on: 2011-4-6
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_callback.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_ctlPainter.h"
#include "renderSelect.h"
#include "../css/color.h"
#include "../dom/history.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../dom/node.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../dom/xml_tags.h"
#include "../dom/event.h"
#include "../tools/slist.h"

#define DEFAULT_HORI_SPACING    8
#define DEFAULT_VERT_SPACING    8
#define DEFAULT_ARROW_WIDTH     15
#define DEFAULT_SPACING         4
#define DEFAULT_BUTTON_WIDTH    60

#ifdef NBK_MEM_TEST
int renderSelect_memUsed(const NRenderSelect* rs)
{
    int size = 0;
    if (rs)
        size = sizeof(NRenderSelect);
    return size;
}
#endif

NRenderSelect* renderSelect_create(void* node, nid type)
{
    NRenderSelect* r = (NRenderSelect*)NBK_malloc0(sizeof(NRenderSelect));
    NNode* n = (NNode*)node;
    N_ASSERT(r);
    
    renderNode_init(&r->d);
    
    r->d.type = RNT_SELECT;
    r->d.node = node;

    r->d.GetStartPoint = renderSelect_getStartPoint;
    r->d.Layout = (type >= NEDOC_FULL) ? renderSelect_layoutSimple : renderSelect_layout;
    r->d.Paint = (type >= NEDOC_FULL) ? renderSelect_paintSimple : renderSelect_paint;

    r->multiple = attr_getValueBool(n->atts, ATTID_MULTIPLE);
    return r;
}

void renderSelect_delete(NRenderSelect** select)
{
    NRenderSelect* r = *select;
    if (r->lst)
        sll_delete(&r->lst);
    NBK_free(r);
    *select = N_NULL;
}

void renderSelect_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    if (byChild) {
        N_KILL_ME()
    }
    
    *x = rn->r.r;
    *y = rn->r.t;
}

void renderSelect_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderSelect* rs = (NRenderSelect*)rn;
    NNode* node = (NNode*)rn->node;
    coord maxw = rn->parent->GetContentWidth(rn->parent);
    coord x, y, w, h;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    bd_bool hasAA = N_FALSE;
    
    if (!rn->needLayout && !force)
        return;
    
    rs->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
    h = NBK_gdi_getFontHeight(page->platform, rs->fontId);
    h += DEFAULT_VERT_SPACING;
    
    w = 0;
    {
        nid tags[] = {TAGID_TEXT, 0};
        NSList* lst = node_getListByTag(node, tags, page->view->path);
        NNode* n;
        
        n = sll_first(lst);
        while (n) {
            w = N_MAX(w, NBK_gdi_getTextWidth(page->platform, rs->fontId, n->d.text, n->len));
            n = sll_next(lst);
        }
        sll_delete(&lst);
        w += DEFAULT_HORI_SPACING + DEFAULT_ARROW_WIDTH + DEFAULT_SPACING;
        if (w > maxw) w = maxw;
    }
    
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
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

static void draw_list_normal(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderSelect* rs = (NRenderSelect*)rn;
    NNode* node = (NNode*)rn->node;
    coord c, dx, dy;
    NRect pr = *rect;
    NPoint pt;
    NNode* label = N_NULL;
    
    {
        nid tags[] = {TAGID_OPTION, 0};
        NSList* lst = node_getListByTag(node, tags, page->view->path);
        NNode* n;
        
        if (sll_isEmpty(lst)) {
            sll_delete(&lst);
            rs->empty = 1;
            return;
        }
        
        // find item current selected, else select the first
        n = sll_first(lst);
        if (n) {
            while (n) {
                if (attr_getValueBool(n->atts, ATTID_SELECTED_P))
                    break;
                n = sll_next(lst);
            }
            if (n == N_NULL) {
                n = sll_first(lst);
                attr_setValueBool(n->atts, ATTID_SELECTED_P, N_TRUE);
            }
            
            if (n->child && n->child->id == TAGID_TEXT)
                label = n->child;
        }
        sll_delete(&lst);
    }
    
    if (NBK_paintSelectNormal(page->platform, rs->fontId, rect,
                            ((label) ? label->d.text : N_NULL),
                            ((label) ? label->len : 0),
                            (style->highlight) ? NECS_HIGHLIGHT : NECS_NORMAL) )
        return;
    
    NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
    NBK_gdi_clearRect(page->platform, &pr);
    
    // draw popup sign
    c = pr.l;
    pr.l = pr.r - nfloat_imul(DEFAULT_ARROW_WIDTH, style->zoom);
    NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
    NBK_gdi_drawRect(page->platform, &pr);
    pr.l = c;
    
    if (style->highlight) {
        renderNode_drawFocusRing(pr, style->ctlFocusColor, page->platform);
    }
    else {
        NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
        NBK_gdi_drawRect(page->platform, &pr);
    }
    
    if (label) {
        pr.r -= nfloat_imul(DEFAULT_ARROW_WIDTH, style->zoom);
        dx = DEFAULT_HORI_SPACING >> 1;
        dy = DEFAULT_VERT_SPACING >> 1;
        dx = nfloat_imul(dx, style->zoom);
        dy = nfloat_imul(dy, style->zoom);
        rect_grow(&pr, -dx, -dy);
        
        c = rect_getHeight(&pr);
        c -= nfloat_imul(NBK_gdi_getFontHeight(page->platform, rs->fontId), style->zoom);
        c /= 2;
        c += nfloat_imul(NBK_gdi_getFontAscent(page->platform, rs->fontId), style->zoom);
        
        NBK_gdi_useFont(page->platform, rs->fontId);
        pt.x = pr.l;
        pt.y = pr.t + c;
        NBK_gdi_setPenColor(page->platform, &style->ctlTextColor);
        NBK_gdi_drawText(page->platform, label->d.text, label->len, &pt, 0);
        NBK_gdi_releaseFont(page->platform);
    }
}

static void list_building(NRenderSelect* rs, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NNode* node = (NNode*)rs->d.node;
    NNode *opt, *txt;
    coord lh;
    int i;
    
    if (rs->empty)
        return;
    
    if (rs->lst == N_NULL) {
        nid tags[] = {TAGID_OPTION, 0};
        
        rs->lst = node_getListByTag(node, tags, page->view->path);
        
        // calc max width of popup
        rs->w = rs->h = 0;
        i = 0;
        opt = sll_first(rs->lst);
        while (opt) {
            txt = opt->child;
            if (txt)
                rs->w = N_MAX(rs->w, NBK_gdi_getTextWidth(page->platform, rs->fontId,
                    txt->d.text, txt->len) + DEFAULT_SPACING);
            if (attr_getValueBool(opt->atts, ATTID_SELECTED_P))
                rs->cur = i;
            i++;
            opt = sll_next(rs->lst);
        }
        rs->visi = rs->items = i;
        rs->top = 0;
        
        rs->w += DEFAULT_ARROW_WIDTH;
        rs->w = nfloat_imul(rs->w, style->zoom);
        if (rs->w > rect_getWidth(rect))
            rs->w = rect_getWidth(rect);
        
        // calc max height of popup
        lh = NBK_gdi_getFontHeight(page->platform, rs->fontId) + DEFAULT_SPACING;
        lh = nfloat_imul(lh, style->zoom);
        rs->h = lh * rs->items;
        if (rs->h > rect_getHeight(rect)) {
            rs->visi = rect_getHeight(rect) / lh;
            rs->h = lh * rs->visi;
        }
        
        if (rs->cur >= rs->top + rs->visi) {
            rs->top = rs->cur;
            i = rs->top + rs->visi - rs->items;
            if (i > 0)
                rs->top -= i;
        }
    }
}

static int build_items(NSList* lst, wchr** items, int num)
{
    NNode* n = sll_first(lst);
    wchr* p = (num > 0) ? (wchr*)NBK_malloc(sizeof(wchr*) * num) : N_NULL;
    int i = 0;
    wchr* txt;
    
    while (n) {
        txt = (n->child) ? n->child->d.text : N_NULL;
        if (p)
            ((wchr**)p)[i] = txt;
        i++;
        n = sll_next(lst);
    }
    
    if (items)
        *items = p;
    return i;
}

static bd_bool process_by_shell(NRenderSelect* rs, NPage* page)
{
    bd_bool ret = N_FALSE;
    
    if (rs->lst) {
        int sel = rs->cur;
        wchr* items = N_NULL;
        int num = build_items(rs->lst, N_NULL, 0);
        build_items(rs->lst, &items, num);
        
        if (NBK_paintSelectExpand(page->platform, items, num, &sel)) {
            
            NEvent evt;
            evt.type = NEEVENT_KEY;
            evt.page = page;

            if (sel != -1 && sel != rs->cur) {
                evt.d.keyEvent.key = NEKEY_ENTER;
                rs->cur = sel;
            }
            else
                evt.d.keyEvent.key = NEKEY_BACKSPACE;
            
            renderSelect_processKey((NRenderNode*)rs, &evt);
            ret = N_TRUE;
        }
        
        if (items)
            NBK_free(items);
        
        if (ret)
            nbk_cb_call(NBK_EVENT_PAINT_CTRL, &page->cbEventNotify, N_NULL);
    }
    
    return ret;
}

static void draw_list_expand(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderSelect* rs = (NRenderSelect*)rn;
    NNode *opt, *txt;
    coord lh, fa;
    int i = 0;
    NRect pr = *rect, ir;
    NPoint pt;
    
    lh = NBK_gdi_getFontHeight(page->platform, rs->fontId) + DEFAULT_SPACING;
    lh = nfloat_imul(lh, style->zoom);
    fa = NBK_gdi_getFontAscent(page->platform, rs->fontId) + DEFAULT_SPACING / 2;
    fa = nfloat_imul(fa, style->zoom);

    rect_grow(&pr, 1, 1);
    NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
    NBK_gdi_clearRect(page->platform, &pr);
    NBK_gdi_setPenColor(page->platform, &style->ctlFgColor);
    NBK_gdi_drawRect(page->platform, &pr);
    rect_grow(&pr, -1, -1);
         
    ir.l = pr.l;
    ir.r = pr.r;
    ir.t = pr.t;
    ir.b = ir.t + lh;
    NBK_gdi_setClippingRect(page->platform, &pr);
    NBK_gdi_useFont(page->platform, rs->fontId);
    for (i = rs->top; i < rs->top + rs->visi; i++) {
        opt = (NNode*)sll_getAt(rs->lst, i);
        txt = opt->child;
        if (txt) {
            if (i == rs->cur) {
                NBK_gdi_setPenColor(page->platform, &style->ctlFillColor);
                NBK_gdi_setBrushColor(page->platform, &style->ctlFocusColor);
            }
            else {
                NBK_gdi_setPenColor(page->platform, &style->ctlTextColor);
                NBK_gdi_setBrushColor(page->platform, &style->ctlFillColor);
            }
            NBK_gdi_clearRect(page->platform, &ir);
            pt.x = ir.l;
            pt.y = ir.t + fa;
            NBK_gdi_drawText(page->platform, txt->d.text, txt->len, &pt, 0);
        }
        ir.t = ir.b;
        ir.b = ir.t + lh;
    }
    NBK_gdi_releaseFont(page->platform);
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_list_normal(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    coord x, y;
    NRect pr, cl;
    
    renderNode_getAbsPos(rn->parent, &x, &y);
    
    pr = rn->r;
    pr.r -= DEFAULT_SPACING;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;

    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    draw_list_normal(rn, style, &pr);
    
    NBK_gdi_cancelClippingRect(page->platform);
}

static void calc_popup_pos(NRenderSelect* rs, NRect* pr,
    coord x, coord y, coord w, coord h, NRect* rect, NFloat zoom)
{
    pr->l = x;
    pr->t = y;
    if (pr->l == 0) pr->l = 1;
    if (pr->t == 0) pr->t = 1;
    
    pr->l = nfloat_imul(pr->l, zoom);
    pr->t = nfloat_imul(pr->t, zoom);
    if (pr->l + w > rect->r) pr->l = rect->r - w - 1;
    if (pr->t + h > rect->b) pr->t = rect->b - h - 1;
    
    pr->r = pr->l + w;
    pr->b = pr->t + h;
    
    rs->vr = *pr;
    
    rect_move(pr, pr->l - rect->l, pr->t - rect->t);
}

static void paint_list_expand(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NRenderSelect* rs = (NRenderSelect*)rn;
    coord x, y;
    NRect pr;

    list_building(rs, style, rect);
    
    if (process_by_shell(rs, (NPage*)((NView*)style->view)->page))
        return;
        
    renderNode_getAbsPos(rn->parent, &x, &y);
    
    calc_popup_pos(rs, &pr, rn->r.l + x, rn->r.b + y, rs->w, rs->h, rect, style->zoom);
    
    draw_list_expand(rn, style, &pr);    
}

void renderSelect_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect)
{
    NRenderSelect* rs = (NRenderSelect*) rn;
    
    paint_list_normal(rn, style, rect);
    
    if (rs->expand && rs->modal)
            paint_list_expand(rn, style, rect);
    
//    if (rs->expand) {
//        if (rs->modal)
//            paint_list_expand(rn, style, rect);
//        else
//            view_setModalRender(((NView*)style->view)->page, rn);
//    }
//    else {
//        view_clearModalRender(((NView*)style->view)->page);
//    }
}

void renderSelect_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderSelect* rs = (NRenderSelect*)rn;
    NNode* node = (NNode*)rn->node;
    coord x, y;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_calcStyle(rn, style);
    
    rs->fontId = NBK_gdi_getFont(page->platform, style->font_size, style->bold, style->italic);
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    rn->r = *attr_getRect(node->atts, ATTID_TC_RECT);
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    
    rn->needLayout = 0;
}

static void paint_list_normal_simple(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRect pr, cl;
    
    pr = rn->r;
    rect_intersect(&pr, &style->dv);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);
    
    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r) + 1;
    cl.b = N_MIN(rect_getHeight(rect), pr.b) + 1;
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    draw_list_normal(rn, style, &pr);
    
    NBK_gdi_cancelClippingRect(page->platform);
}

static void paint_list_expand_simple(NRenderNode* rn, NStyle* style, NRect* rect)
{
    NRenderSelect* rs = (NRenderSelect*)rn;
    NRect pr;

    list_building(rs, style, rect);
    
    if (process_by_shell(rs, (NPage*)((NView*)style->view)->page))
        return;
    
    calc_popup_pos(rs, &pr, rn->r.l, rn->r.b, rs->w, rs->h, rect, style->zoom);
    
    draw_list_expand(rn, style, &pr);
}

void renderSelect_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect)
{
    NRenderSelect* rs = (NRenderSelect*)rn;
    
    paint_list_normal_simple(rn, style, rect);
    
    if (rs->expand && rs->modal)
            paint_list_expand_simple(rn, style, rect);
    
//    if (rs->expand) {
//        if (rs->modal)
//            paint_list_expand_simple(rn, style, rect);
//        else
//            view_setModalRender(((NView*)style->view)->page, rn);
//    }
//    else {
//        view_clearModalRender(((NView*)style->view)->page);
//    }
}

void renderSelect_popup(NRenderSelect* select)
{
    if (select->empty)
        return;
    
    if (select->expand)
        return;
    
    select->expand = 1;
}

void renderSelect_dismiss(NRenderSelect* select)
{
    if (!select->expand)
        return;
    
    select->expand = 0;
    if (select->lst)
        sll_delete(&select->lst);
}

static void list_select(NRenderSelect* select, NPage* page)
{
    NNode* n = sll_first(select->lst);
    int i = 0;
    
    while (n) {
        attr_setValueBool(n->atts, ATTID_SELECTED_P, (i == select->cur) ? N_TRUE : N_FALSE);
        if (i == select->cur) {
            char* onpick = attr_getValueStr(n->atts, ATTID_ONPICK);
            if (onpick) {
                doc_doGet(page->doc, onpick, N_FALSE, N_FALSE);
                break;
            }
            else if (attr_getValueInt32(n->parent->atts, ATTID_TC_IA) > 0) {
                NEvent evt;
                NBK_memset(&evt, 0, sizeof(NEvent));
                evt.type = NEEVENT_DOM;
                evt.d.domEvent.type = NEDOM_SELECT_CHANGE;
                evt.d.domEvent.node = n->parent;
                node_active_with_ia(page->doc, &evt, N_NULL);
                break;
            }
        }
        i++;
        n = sll_next(select->lst);
    }

    renderSelect_dismiss(select);
}

static void list_move_up(NRenderSelect* select)
{
    if (select->cur == 0)
        return;
    
    select->cur--;
    if (select->cur < select->top)
        select->top--;
}

static void list_move_down(NRenderSelect* select)
{
    if (select->cur == select->items - 1)
        return;
    
    select->cur++;
    if (select->cur >= select->top + select->visi)
        select->top++;
}

static int16 get_selected(NRenderSelect* select, NPoint pos)
{
    if (rect_hasPt(&select->vr, pos.x, pos.y)) {
        coord y = select->vr.t;
        coord lh = rect_getHeight(&select->vr) / select->visi;
        int i;
        
        for (i=0; i < select->visi; i++, y += lh) {
            if (pos.y >= y && pos.y < y + lh)
                return select->top + i;
        }
    }
    
    return -1;
}

static void list_pen_down(NRenderSelect* select, NEvent* event)
{
    int16 sel = get_selected(select, event->d.penEvent.pos);
    if (sel != -1)
        select->cur = sel;
    
    select->lastY = event->d.penEvent.pos.y;
    select->move = 0;
}

static void list_pen_move(NRenderSelect* select, NEvent* event)
{
    coord lh = select->h / select->visi;
    coord dy = event->d.penEvent.pos.y - select->lastY;
    
    if (N_ABS(dy) < lh)
        return;
    
    select->lastY = event->d.penEvent.pos.y;
    select->move = 1;
    
    if (dy > 0) { // drag down
        if (select->top > 0)
            select->top--;
    }
    else { // drag up
        if (select->top + select->visi < select->items)
            select->top++;
    }
}

static void list_pen_up(NRenderSelect* select, NEvent* event)
{
    int16 sel = get_selected(select, event->d.penEvent.pos);
    if (sel == select->cur)
        list_select(select, (NPage*)event->page);
    else if (!select->move)
        renderSelect_dismiss(select);
}

bd_bool renderSelect_processKey(NRenderNode* rn, NEvent* event)
{
    NRenderSelect* rs = (NRenderSelect*)rn;
    bd_bool handled = N_TRUE;
    
    if (!rs->expand)
        return N_FALSE;
    
    switch (event->type) {
    case NEEVENT_KEY:
    {
        switch (event->d.keyEvent.key) {
        case NEKEY_LEFT:
        case NEKEY_RIGHT:
        case NEKEY_BACKSPACE:
            renderSelect_dismiss(rs);
            break;
        case NEKEY_UP:
            list_move_up(rs);
            break;
        case NEKEY_DOWN:
            list_move_down(rs);
            break;
        case NEKEY_ENTER:
            list_select(rs, (NPage*)event->page);
            break;
        default:
            handled = N_FALSE;
            break;
        }
        break;
    }
    case NEEVENT_PEN:
    {
        switch (event->d.penEvent.type) {
        case NEPEN_DOWN:
            list_pen_down(rs, event);
            break;
        case NEPEN_MOVE:
            list_pen_move(rs, event);
            break;
        case NEPEN_UP:
            list_pen_up(rs, event);
            break;
        }
        break;
    }
    default:
        handled = N_FALSE;
        break;
    }
    
    return handled;
}

int16 renderSelect_getDataSize(NRenderNode* rn, NStyle* style)
{
    return 10;
}
