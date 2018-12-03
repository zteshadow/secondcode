/*
 * renderImage.c
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../dom/xml_atts.h"
#include "../dom/attr.h"
#include "../dom/xml_atts.h"
#include "../dom/node.h"
#include "../dom/view.h"
#include "../dom/document.h"
#include "../dom/page.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "../css/cssSelector.h"
#include "../css/css_helper.h"
#include "renderImage.h"
#include "renderBlock.h"
#include "imagePlayer.h"

#ifdef NBK_MEM_TEST
int renderImage_memUsed(const NRenderImage* ri)
{
    int size = 0;
    if (ri)
        size = sizeof(NRenderImage);
    return size;
}
#endif

NRenderImage* renderImage_create(void* node, nid type)
{
    NRenderImage* ri = (NRenderImage*)NBK_malloc0(sizeof(NRenderImage));
    N_ASSERT(ri);
    
    renderNode_init(&ri->d);
    
    ri->d.type = RNT_IMAGE;
    ri->d.node = node;
    
    ri->d.GetStartPoint = renderImage_getStartPoint;
    ri->d.Layout = (type >= NEDOC_FULL) ? renderImage_layoutSimple : renderImage_layout;
    ri->d.Paint = (type >= NEDOC_FULL) ? renderImage_paintSimple : renderImage_paint;
    
    ri->fail = N_FALSE;
    ri->iid = -1;
    
    return ri;
}

void renderImage_delete(NRenderImage** ri)
{
    NRenderImage* temp = *ri;
    NBK_free(temp);
    *ri = N_NULL;
}

void renderImage_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    NRenderImage* ri = (NRenderImage*)rn;
    
    N_ASSERT(byChild == 0);
    
    *x = ri->sx;
    *y = rn->mt;
}

void renderImage_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderImage* ri = (NRenderImage*)rn;
    NNode* node = (NNode*)rn->node;
    coord maxw = rn->parent->GetContentWidth(rn->parent);
    coord x, y;
    bd_bool byChild = (rn->prev) ? N_FALSE : N_TRUE;
    coord iw, ih;
    NRect area;
    bd_bool hasAA = N_FALSE, parAA = N_FALSE;
    NCssValue* cv;
    
    if (!rn->needLayout && !force)
        return;
    
    if (!renderNode_layoutable(rn, maxw))
        return;
    
    renderNode_calcStyle(rn, style);
    
    ri->clip = *style->clip;
    
    iw = ih = -1;
    
    cv = attr_getCssValue(node->atts, ATTID_WIDTH);
    if (cv && cv->type)
        iw = css_calcValue(*cv, maxw, style, -1);
    if (iw == -1 && style->width.type)
        iw = css_calcValue(style->width, maxw, style, -1);
    
    cv = attr_getCssValue(node->atts, ATTID_HEIGHT);
    if (cv && cv->type)
        ih = css_calcValue(*cv, 0, style, -1);
    if (ih == -1 && style->height.type)
        ih = css_calcValue(style->height, 0, style, -1);
    
    if ((iw == -1 || ih == -1) && ri->iid != -1 && !ri->fail) {
        NView* view = (NView*)style->view;
        NSize size;
        bd_bool fail;
        bd_bool got = imagePlayer_getSize(view->imgPlayer, ri->iid, &size, &fail);
        if (got) {
            iw = size.w;
            ih = size.h;
            ri->fail = N_FALSE;
        }
        else if (fail)
            ri->fail = N_TRUE;
    }
    if (iw == -1)
        iw = UNKNOWN_IMAGE_WIDTH;
    if (ih == -1)
        ih = UNKNOWN_IMAGE_HEIGHT;
    if (iw > maxw) { // keep ratio
        ih = ih * maxw / iw;
        iw = maxw;
    }
    
    if (rn->flo > CSS_FLOAT_NONE && stat->aux[stat->ld] == N_NULL)
        stat->aux[stat->ld] = rn;
    if (rn->clr)
        stat->aux[stat->ld] = N_NULL;
    
    if (stat->aux[stat->ld]) {
        renderNode_getAvailArea(stat->aux[stat->ld], rn, &area);
        hasAA = N_TRUE;
    }
    else if (renderNode_getAvailAreaByChild(stat, rn->parent, &area)) {
        hasAA = N_TRUE;
        parAA = N_TRUE;
    }

    if (hasAA && !byChild) {
        rn->prev->GetStartPoint(rn->prev, &x, &y, N_FALSE);
        if (y >= area.b)
            hasAA = N_FALSE;
    }
    
    if (hasAA) { // float
        
        bd_bool turn = N_FALSE;
        
        if (parAA && !byChild) {
            
            coord maxR;
            bd_bool ig = N_FALSE;
            
            rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
            maxR = area.r;
            
            while (x && maxR - x < iw) {
                if (!ig) {
                    y = renderNode_getMaxBottomByLine(rn->prev, 0);
                    ig = N_TRUE;
                }
                else
                    y = area.b;
                
                if (y < area.b) {
                    x = area.l;
                }
                else {
                    x = 0;
                    maxR = maxw;
                }
                turn = N_TRUE;
            }
            
            rn->r.l = x;
            rn->r.r = x + iw;
            rn->r.t = y;
            rn->r.b = y + ih;
            rn->mr = maxR;
        }
        else {
            
            if (rn->prev) {
                turn = (rn->prev->flo == CSS_FLOAT_NONE && rn->flo != CSS_FLOAT_NONE) ? N_TRUE : N_FALSE;
            }
            
            if (!turn && rect_getWidth(&area) >= iw) { // enough
                if (rn->flo <= CSS_FLOAT_LEFT) {
                    rn->r.l = area.l;
                    rn->r.r = rn->r.l + iw;
                }
                else {
                    rn->r.l = area.r - iw;
                    rn->r.r = area.r;
                }
                rn->r.t = area.t;
                rn->r.b = area.t + ih;
                rn->mr = area.r;
            }
            else {
                if (rn->flo <= CSS_FLOAT_LEFT) {
                    rn->r.l = 0;
                    rn->r.r = iw;
                }
                else {
                    rn->r.l = maxw - iw;
                    rn->r.r = maxw;
                }
                rn->r.t = area.b;
                rn->r.b = rn->r.t + ih;
                rn->mr = maxw;
            }
        }
    }
    else { // normal
        
        if (byChild) {
            // first object of block
            rn->parent->GetStartPoint(rn->parent, &x, &y, byChild);
        }
        else {
            rn->prev->GetStartPoint(rn->prev, &x, &y, byChild);
        }
        
        if (maxw - x >= iw) {
            // follow previous object
            rn->r.l = x;
            rn->r.r = x + iw;
            rn->r.t = y;
            rn->r.b = y + ih;
        }
        else {
            // turn to new line
            coord mb = renderNode_getMaxBottomByLine(rn->prev, 0);
            rn->r.l = 0;
            rn->r.r = iw;
            rn->r.t = mb;
            rn->r.b = mb + ih;
        }
        rn->mr = maxw;
    }
    
    ri->sx = rn->r.r;
    rn->ml = rn->r.l;
    rn->mt = rn->r.t;
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderImage_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderImage* ri = (NRenderImage*)rn;
    coord x, y;
    NRect c, pr, cl;
    
    if (rn->display == CSS_DISPLAY_NONE)
        return;
    
    renderNode_getAbsPos(rn->parent, &x, &y);
    
    pr = rn->r;
    rect_move(&pr, pr.l + x, pr.t + y);
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    if (rn->flo > CSS_FLOAT_NONE && stat->aux[stat->ld] == N_NULL)
        stat->aux[stat->ld] = rn;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    cl.l = cl.t = 0;
    cl.r = N_MIN(rect_getWidth(rect), pr.r);
    cl.b = N_MIN(rect_getHeight(rect), pr.b);
    c = ri->clip;
    rect_move(&c, c.l + x, c.t + y);
    rect_toView(&c, style->zoom);
    rect_move(&c, c.l - rect->l, c.t - rect->t);
    cl.r = N_MIN(cl.r, c.r);
    cl.b = N_MIN(cl.b, c.b);
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    if (ri->iid == -1 || ri->fail) {
        NBK_gdi_setBrushColor(page->platform, &colorWhite);
        NBK_gdi_clearRect(page->platform, &pr);
        if (!style->highlight) {
            NBK_gdi_setPenColor(page->platform, &colorGray);
            NBK_gdi_drawRect(page->platform, &pr);
        }
    }
    else {
        NView* view = (NView*)style->view;
        imagePlayer_draw(view->imgPlayer, ri->iid, &pr);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
//    if (style->highlight && !style->belongA)
//        renderNode_drawFocusRing(pr, style->ctlFgColor, page->platform);
}

void renderImage_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    NRenderImage* ri = (NRenderImage*)rn;
    NNode* n = (NNode*)rn->node;
    coord x, y;
    NRect* c;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_checkAdFF(rn);
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    c = attr_getRect(n->atts, ATTID_TC_RECT);
    if (c) rn->r = *c;
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    
    ri->clip = *style->clip;
    
    rn->needLayout = 0;
}

void renderImage_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NView* view = (NView*)style->view;
    NPage* page = (NPage*)view->page;
    NRenderImage* ri = (NRenderImage*)rn;
    NRect c, pr, cl;
   
    pr = rn->r;
    if (!rect_isIntersect(&style->dv, &pr))
        return;
    if (!rect_isIntersect(&ri->clip, &pr))
        return;
    rect_toView(&pr, style->zoom);
    if (!rect_isIntersect(rect, &pr))
        return;
    
    rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

    c = ri->clip;
    rect_toView(&c, style->zoom);
    rect_move(&c, c.l - rect->l, c.t - rect->t);

    cl.l = c.l;
    cl.t = c.t;
    cl.r = N_MIN(rect_getWidth(rect), c.r);
    cl.b = N_MIN(rect_getHeight(rect), c.b);
    cl.r = N_MIN(cl.r, pr.r);
    cl.b = N_MIN(cl.b, pr.b);
    NBK_gdi_setClippingRect(page->platform, &cl);

    if (ri->iid == -1 && ri->d.bgiid == -1) {
        NBK_gdi_setBrushColor(page->platform, &colorWhite);
        NBK_gdi_clearRect(page->platform, &pr);
        if (!style->highlight) {
            NBK_gdi_setPenColor(page->platform, &colorGray);
            NBK_gdi_drawRect(page->platform, &pr);
        }
    }
    else {
        if (rn->bgiid != -1) {
            // draw background image
            imagePlayer_setDraw(view->imgPlayer, rn->bgiid);
            renderNode_drawBgImageFF(rn, &pr, rect, style);
        }
        if (ri->iid != -1)
            imagePlayer_draw(view->imgPlayer, ri->iid, &pr);
    }

    NBK_gdi_cancelClippingRect(page->platform);

    if (style->highlight && !style->belongA)
        renderNode_drawFocusRing(pr, style->ctlEditColor, page->platform);
}

int16 renderImage_getDataSize(NRenderNode* rn, NStyle* style)
{
    NRenderImage* ri = (NRenderImage*)rn;
    NNode* node = (NNode*)rn->node;
    coord iw = -1;
    int16 size;
    NCssValue* cv;
    
    cv = attr_getCssValue(node->atts, ATTID_WIDTH);
    if (cv && cv->type)
        iw = css_calcValue(*cv, 0, style, -1);
    if (iw == -1 && ri->iid != -1) {
        NView* view = (NView*)style->view;
        NSize size;
        bd_bool fail;
        if (imagePlayer_getSize(view->imgPlayer, ri->iid, &size, &fail))
            iw = size.w;
    }
    if (iw == -1)
        iw = UNKNOWN_IMAGE_WIDTH;
    
    size = iw / style->font_size;
    if (iw % style->font_size)
        size++;
    
    return size;
}
