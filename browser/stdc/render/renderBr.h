/*
 * renderBr.h
 *
 *  Created on: 2011-1-1
 *      Author: wuyulun
 */

#ifndef RENDERBR_H_
#define RENDERBR_H_

#include "../inc/config.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderBr {
    
    NRenderNode d;
    
} NRenderBr;

#ifdef NBK_MEM_TEST
int renderBr_memUsed(const NRenderBr* rb);
#endif

NRenderBr* renderBr_create(void* node, nid type);
void renderBr_delete(NRenderBr** rb);

void renderBr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderBr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderBr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderBr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);

#ifdef __cplusplus
}
#endif

#endif /* RENDERBR_H_ */
