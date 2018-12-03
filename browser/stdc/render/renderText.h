/*
 * renderText.h
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#ifndef RENDERTEXT_H_
#define RENDERTEXT_H_

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRTLine {
    
    NRect   r;
    wchr*   text;
    coord   ml;
    coord   mr;
    
} NRTLine;

typedef struct _NRenderText {
    
    NRenderNode d;
    
    NRTLine*    lines;
    int16       max;
    int16       use;
    
    NFontId     fontId;
    
    bd_bool    underline : 1;
    bd_bool    beBreak : 1;
    bd_bool    space : 1; // pure spaces
    
    int8    line_spacing;
    int8    indent;
    
    coord   sx; // x before align
    coord   sy;
    
    NRect   clip;
    nid     docType;
    
} NRenderText;

#ifdef NBK_MEM_TEST
int renderText_memUsed(const NRenderText* rt);
#endif

NRenderText* renderText_create(void* node, nid type);
void renderText_delete(NRenderText** rt);

void renderText_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderText_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderText_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderText_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderText_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderText_adjustPos(NRenderNode* rn, coord dy);

bd_bool renderText_isBreak(NRenderText* rt);
NRect renderText_getHeadAlignRect(NRenderText* rt, coord* width);
NRect renderText_getTailAlignRect(NRenderText* rt, coord* width);
void renderText_alignHead(NRenderText* rt, coord dx, coord maxb);
void renderText_alignBody(NRenderText* rt, nid ha);
coord renderText_alignTail(NRenderText* rt, nid ha);
void renderText_alignTailByInline(NRenderText* rt, coord dx, coord maxb);

void renderText_setMaxRight(NRenderNode* rn, coord mr);

bd_bool renderText_hasPt(NRenderText* rt, NPoint pt);

int16 renderText_getDataSize(NRenderNode* rn, NStyle* style);

//void renderText_breakLineByWidthFF(NRenderNode* rn, NStyle* style, coord width);

#ifdef __cplusplus
}
#endif

#endif /* RENDERTEXT_H_ */
