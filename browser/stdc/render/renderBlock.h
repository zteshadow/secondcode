/*
 * renderBlock.h
 *
 *  Created on: 2010-12-29
 *      Author: wuyulun
 */

#ifndef RENDERBLOCK_H_
#define RENDERBLOCK_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderBlock {
    
    NRenderNode d;
    
    bd_bool    text_align : 2;
    bd_bool    overflow : 3;
    bd_bool    beForce : 1;
    bd_bool    hasWidth : 1;
    bd_bool    hasHeight : 1;
    
    bd_bool    ne_display : 1;
    
    NCssBox margin;
    NCssBox border;
    NCssBox padding;
    
} NRenderBlock;

#ifdef NBK_MEM_TEST
int renderBlock_memUsed(const NRenderBlock* rb);
#endif

void block_align(NRenderNode* rn, nid ha);

NRenderBlock* renderBlock_create(void* node, nid type);
void renderBlock_delete(NRenderBlock** rb);

void renderBlock_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderBlock_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderBlock_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
void renderBlock_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderBlock_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRenderRect* rect);
coord renderBlock_getContentWidth(NRenderNode* rn);
void renderBlock_getAbsXY(NRenderNode* rn, coord* x, coord* y);

//BOOL renderBlock_getAvailAreaByChild(NLayoutStat* stat, NRenderBlock* rb, NRect* area);

#ifdef __cplusplus
}
#endif

#endif /* RENDERBLOCK_H_ */
