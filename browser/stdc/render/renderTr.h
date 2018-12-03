/*
 * renderTr.h
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#ifndef RENDERTR_H_
#define RENDERTR_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderTr {
    
    NRenderNode d;
    
    uint8   beForce : 1;
    
} NRenderTr;

NRenderTr* renderTr_create(void* node, nid type);
void renderTr_delete(NRenderTr** rt);

void renderTr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderTr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
void renderTr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTr_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);

int renderTr_getColumnNum(NRenderNode* rn);
void renderTr_predictColsWidth(NRenderNode* rn, int16* colsWidth, int maxCols, NStyle* style);
coord renderTr_getContentWidthByTd(NRenderNode* tr, NRenderNode* td);

#ifdef __cplusplus
}
#endif

#endif /* RENDERTR_H_ */
