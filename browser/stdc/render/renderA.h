/*
 * renderA.h
 *
 *  Created on: 2011-8-22
 *      Author: wuyulun
 */

#ifndef RENDERA_H_
#define RENDERA_H_

#include "../inc/config.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderA {
    
    NRenderNode     d;
    
    uint8   overflow : 3;

} NRenderA;

NRenderA* renderA_create(void* node, nid type);
void renderA_delete(NRenderA** ra);

void renderA_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderA_layout(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderA_paint(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect);
void renderA_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, bd_bool force);
void renderA_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle* style, NRect* rect);

#ifdef __cplusplus
}
#endif

#endif /* RENDERA_H_ */
