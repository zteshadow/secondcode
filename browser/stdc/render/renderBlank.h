/*
 * renderBlank.h
 *
 *  Created on: 2011-5-2
 *      Author: migr
 */

#ifndef RENDERBLANK_H_
#define RENDERBLANK_H_

#include "../inc/config.h"
#include "../css/cssSelector.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderBlank {
    
    NRenderNode d;
    
    uint8   overflow : 3;
    
} NRenderBlank;

#ifdef NBK_MEM_TEST
int renderBlank_memUsed(const NRenderBlank* rb);
#endif

NRenderBlank* renderBlank_create(void* node, nid type);
void renderBlank_delete(NRenderBlank** render);

void renderBlank_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderBlank_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderBlank_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderBlank_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);

coord renderBlank_getContentWidth(NRenderNode* rn);

#ifdef __cplusplus
}
#endif

#endif /* RENDERBLANK_H_ */
