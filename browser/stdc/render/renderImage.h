/*
 * renderImage.h
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#ifndef RENDERIMAGE_H_
#define RENDERIMAGE_H_

#include "../inc/config.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderImage {

    NRenderNode d;
    
    bd_bool        fail;
    int16       iid;
    
    coord       sx; // x before align
    
    NRect       clip;
    
} NRenderImage;

#ifdef NBK_MEM_TEST
int renderImage_memUsed(const NRenderImage* ri);
#endif

NRenderImage* renderImage_create(void* node, nid type);
void renderImage_delete(NRenderImage** ri);

void renderImage_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderImage_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderImage_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderImage_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderImage_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);

int16 renderImage_getDataSize(NRenderNode* rn, NStyle* style);

#ifdef __cplusplus
}
#endif

#endif /* RENDERIMAGE_H_ */
