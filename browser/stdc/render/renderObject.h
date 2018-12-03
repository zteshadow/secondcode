/*
 * renderObject.h
 *
 *  Created on: 2011-4-26
 *      Author: wuyulun
 */

#ifndef RENDEROBJECT_H_
#define RENDEROBJECT_H_

#include "../inc/config.h"
#include "renderNode.h"
#include "../css/cssSelector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRenderObject {
    
    NRenderNode     d;
    
    int16       iid;
    
} NRenderObject;

#ifdef NBK_MEM_TEST
int renderObject_memUsed(const NRenderObject* ro);
#endif

NRenderObject* renderObject_create(void* node, nid type);
void renderObject_delete(NRenderObject** object);

void renderObject_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderObject_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderObject_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderObject_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderObject_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);

#ifdef __cplusplus
}
#endif

#endif /* RENDEROBJECT_H_ */
