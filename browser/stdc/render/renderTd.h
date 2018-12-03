/*
 * renderTd.h
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#ifndef RENDERTD_H_
#define RENDERTD_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderTd {
    
    NRenderNode d;
    
    bd_bool    beForce : 1;
    bd_bool    text_align : 2;
    
} NRenderTd;

NRenderTd* renderTd_create(void* node, nid type);
void renderTd_delete(NRenderTd** rt);

void renderTd_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderTd_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTd_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
void renderTd_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTd_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
void renderTd_getAbsXY(NRenderNode* rn, coord* x, coord* y);
coord renderTd_getContentWidth(NRenderNode* rn);

int16 renderTd_getDataSize(NRenderNode* rn, NStyle* style);

void renderTd_setMaxRightOfChilds(NRenderNode* rn, coord mr);

#ifdef __cplusplus
}
#endif

#endif /* RENDERTD_H_ */
