/*
 * renderSelect.h
 *
 *  Created on: 2011-4-6
 *      Author: wuyulun
 */

#ifndef RENDERSELECT_H_
#define RENDERSELECT_H_

#include "../inc/config.h"
#include "renderNode.h"
#include "../inc/nbk_gdi.h"
#include "../tools/slist.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderSelect {
    
    NRenderNode d;
    
    NFontId     fontId;
    
    bd_bool        multiple : 1;
    bd_bool        expand : 1;
    bd_bool        empty : 1;
    
    bd_bool        modal : 1; // specific for paint
    bd_bool        move : 1;
    
    NSList*     lst;
    
    NRect       vr; // rect of popup
    coord       w; // width of popup
    coord       h; // height of popup
    coord       lastY;
    
    int16       visi;
    int16       items;
    int16       top;
    int16       cur;
    
} NRenderSelect;

#ifdef NBK_MEM_TEST
int renderSelect_memUsed(const NRenderSelect* rs);
#endif

NRenderSelect* renderSelect_create(void* node, nid type);
void renderSelect_delete(NRenderSelect** select);

void renderSelect_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderSelect_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderSelect_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRect* rect);
void renderSelect_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderSelect_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRect* rect);

void renderSelect_popup(NRenderSelect* select);
void renderSelect_dismiss(NRenderSelect* select);
bd_bool renderSelect_processKey(NRenderNode* rn, NEvent* event);

int16 renderSelect_getDataSize(NRenderNode* rn, NStyle* style);

#ifdef __cplusplus
}
#endif

#endif /* RENDERSELECT_H_ */
