/*
 * renderTextarea.h
 *
 *  Created on: 2011-4-9
 *      Author: migr
 */

#ifndef RENDERTEXTAREA_H_
#define RENDERTEXTAREA_H_

#include "../inc/config.h"
#include "renderNode.h"
#include "../inc/nbk_gdi.h"
#include "../dom/event.h"
#include "../editor/textEditor.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NRenderTextarea {
    
    NRenderNode     d;
    
    NFontId         fontId;
    bd_bool            inEdit : 1;
    bd_bool            absCoord : 1;
    NTextEditor*    editor;
    void*           pfd;
    
    NSize           editorSize; // in screen coordinate
    coord           trick; // track zoom change
    
} NRenderTextarea;

#ifdef NBK_MEM_TEST
int renderTextarea_memUsed(const NRenderTextarea* rt);
#endif

NRenderTextarea* renderTextarea_create(void* node, nid type);
void renderTextarea_delete(NRenderTextarea** area);

void renderTextarea_getStartPoint(NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
void renderTextarea_layout(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderTextarea_paint(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);
void renderTextarea_layoutSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, bd_bool force);
void renderTextarea_paintSimple(NLayoutStat* stat, NRenderNode* rn, NStyle*, NRenderRect* rect);

int renderTextarea_getEditTextLen(NRenderTextarea* rt);
char* renderTextarea_getEditText(NRenderTextarea* rt);
int renderTextarea_getEditLength(NRenderTextarea* rt);
int renderTextarea_getEditMaxLength(NRenderTextarea* rt);
void renderTextarea_getRectOfEditing(NRenderTextarea* rt, NRect* rect, NFontId* fontId);

void renderTextarea_edit(NRenderTextarea* rt, void* doc);
bd_bool renderTextarea_processKey(NRenderNode* rn, NEvent* event);

bd_bool renderTextarea_isEditing(NRenderTextarea* rt);

wchr* renderTextarea_getText16(NRenderTextarea* rt, int* len);
void renderTextarea_setText16(NRenderTextarea* rt, wchr* text, int len);

void renderTextarea_getSelPos(NRenderTextarea* rt, int* start, int* end);
void renderTextarea_setSelPos(NRenderTextarea* rt, int start, int end);

int16 renderTextarea_getDataSize(NRenderNode* rn, NStyle* style);

#ifdef __cplusplus
}
#endif

#endif /* RENDERTEXTAREA_H_ */
