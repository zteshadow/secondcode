/*
 * renderTable.h
 *
 *  Created on: 2011-9-9
 *      Author: wuyulun
 */

#ifndef RENDERTABLE_H_
#define RENDERTABLE_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderTable {
    
    NRenderNode d;
    
    bd_bool    beForce : 1;
    bd_bool    hasWidth : 1;
    
    int16   border;
    int16   cellspacing;
    int16   cellpadding;
    
    int16   cols_num;
    int16*  cols_width;
    
} NRenderTable;

NRenderTable* renderTable_create(void* node, nid type);
void renderTable_delete(NRenderTable** rt);

void renderTable_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderTable_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTable_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
void renderTable_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderTable_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
coord renderTable_getContentWidth(NRenderNode* rn);
void renderTable_getAbsXY(NRenderNode* rn, coord* x, coord* y);

coord renderTable_getColWidth(NRenderTable* table, int16 col);

#ifdef __cplusplus
}
#endif

#endif /* RENDERTABLE_H_ */
