/*
 * renderInline.h
 *
 *  Created on: 2011-10-31
 *      Author: wuyulun
 */

#ifndef RENDERINLINE_H_
#define RENDERINLINE_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderInline {
    
    NRenderNode d;
    
    bd_bool    beForce : 1;
    bd_bool    br : 1;
    bd_bool    hasWidth : 1;
    bd_bool    hasHeight : 1;
    
    coord   box_l;
    coord   box_r;
    
    NRenderNode*    head;
    NRenderNode*    body;
    NRenderNode*    tail;
    
} NRenderInline;

NRenderInline* renderInline_create(void* node, nid type);
void renderInline_delete(NRenderInline** ri);

void renderInline_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderInline_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderInline_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
coord renderInline_getContentWidth(NRenderNode* rn);
void renderInline_getAbsXY(NRenderNode* rn, coord* x, coord* y);

bd_bool renderInline_isBreak(NRenderInline* ri);
NRect renderInline_getHeadAlignRect(NRenderInline* ri, coord* width);
NRect renderInline_getTailAlignRect(NRenderInline* ri);
void renderInline_alignHead(NRenderInline* ri, coord dx, coord maxb);
void renderInline_alignBody(NRenderInline* ri, nid ha);
coord renderInline_alignTail(NRenderInline* ri, nid ha);

void renderInline_setMaxRight(NRenderNode* rn, coord mr);

#ifdef __cplusplus
}
#endif

#endif /* RENDERINLINE_H_ */
