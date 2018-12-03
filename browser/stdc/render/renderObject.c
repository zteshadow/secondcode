/*
 * renderObject.c
 *
 *  Created on: 2011-4-26
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "renderObject.h"
#include "imagePlayer.h"
#include "../dom/attr.h"
#include "../dom/page.h"
#include "../dom/view.h"
#include "../dom/xml_atts.h"
#include "../css/color.h"
#include "../tools/unicode.h"

#ifdef NBK_MEM_TEST
int renderObject_memUsed(const NRenderObject* ro)
{
    int size = 0;
    if (ro)
        size = sizeof(NRenderObject);
    return size;
}
#endif

NRenderObject* renderObject_create(void* node, nid type)
{
    NRenderObject* ro = (NRenderObject*)NBK_malloc0(sizeof(NRenderObject));
    N_ASSERT(ro);
    
    renderNode_init(&ro->d);
    
    ro->d.type = RNT_OBJECT;
    ro->d.node = node;
    
    ro->d.GetStartPoint = renderObject_getStartPoint;
    ro->d.Layout = (type >= NEDOC_FULL) ? renderObject_layoutSimple : renderObject_layout;
    ro->d.Paint = (type >= NEDOC_FULL) ? renderObject_paintSimple : renderObject_paint;
    
    ro->iid = -1;
    
    return ro;
}

void renderObject_delete(NRenderObject** object)
{
    NRenderObject* ob = *object;
    NBK_free(ob);
    *object = N_NULL;
}

void renderObject_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild)
{
    N_ASSERT(byChild == 0);
    
    *x = rn->r.r;
    *y = rn->r.t;
}

void renderObject_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    coord x, y;
    
    if (!rn->needLayout && !force)
        return;
    
    rn->parent->GetStartPoint(rn->parent, &x, &y, N_TRUE);
    rn->r.l = x;
    rn->r.t = y;
    rn->r.r = x + 1;
    rn->r.b = y + 1;
    
    rn->needLayout = 0;
    rn->parent->needLayout = 1;
}

void renderObject_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
}

void renderObject_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force)
{
    coord x, y;
    NNode* node = (NNode*)rn->node;
    
    if (!rn->needLayout && !force)
        return;
    
    renderNode_getRootAbsXYForFFFull(rn, &x, &y);
    rn->r = *attr_getRect(node->atts, ATTID_TC_RECT);
    rect_move(&rn->r, x + rn->r.l, y + rn->r.t);
    
    rn->needLayout = 0;
}

void renderObject_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect)
{
    NPage* page = (NPage*)((NView*)style->view)->page;
    NRenderObject* ro = (NRenderObject*)rn;
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
    
    if (ro->iid == -1) {
        renderNode_drawFocusRing(pr, colorLightGray, page->platform);
    }
    else {
        NView* view = (NView*)style->view;
        imagePlayer_draw(view->imgPlayer, ro->iid, &pr);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}
