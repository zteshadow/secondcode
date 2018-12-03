/*
 * renderHr.h
 *
 *  Created on: 2011-4-4
 *      Author: migr
 */

#ifndef RENDERHR_H_
#define RENDERHR_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderHr {
    
    NRenderNode d;
    
} NRenderHr;

#ifdef NBK_MEM_TEST
int renderHr_memUsed(const NRenderHr* rh);
#endif

NRenderHr* renderHr_create(void* node, nid type);
void renderHr_delete(NRenderHr** rh);

void renderHr_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderHr_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderHr_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderHr_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderHr_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);

#ifdef __cplusplus
}
#endif

#endif /* RENDERHR_H_ */
