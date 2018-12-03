/*
 * renderNode.h
 *
 *  Created on: 2010-12-28
 *      Author: wuyulun
 */

#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../dom/node.h"
#include "../css/cssSelector.h"

#ifdef __cplusplus
extern "C" {
#endif

enum NERenderNodeType {
    
    RNT_NONE,
    RNT_BLANK,
    RNT_BLOCK,
    RNT_INLINE,
    RNT_TEXT,
    RNT_IMAGE,
    RNT_INPUT,
    RNT_BR,
    RNT_HR,
    RNT_SELECT,
    RNT_TEXTAREA,
    RNT_OBJECT,
    RNT_A,
    RNT_TABLE,
    RNT_TR,
    RNT_TD,
    RNT_LAST
};

typedef NRect NRenderRect;

typedef struct _NStyle {
    
    int8    font_size;
    
    uint8   flo : 2;
    uint8   clr : 2;
    
    uint8   bold : 1;
    uint8   italic : 1;
    uint8   link : 1;
    uint8   text_align : 2;
    uint8   vert_align : 2;
    uint8   highlight : 1;
    uint8   underline : 1;
    uint8   display : 2;

    uint8   bg_repeat : 2;
    uint8   bg_x_t : 2;
    uint8   bg_y_t : 2;
    uint8   lh_t : 2;
    
    uint8   belongA : 1; // for draw contents selected by A element
    
    // for NBK extend
    uint8   ne_fold : 1;
    
    uint8   overflow;
    
    uint8   hasBgcolor : 1;
    uint8   hasBrcolorL : 1;
    uint8   hasBrcolorT : 1;
    uint8   hasBrcolorR : 1;
    uint8   hasBrcolorB : 1;

    NColor  color;
    NColor  background_color;

    NColor  border_color_l;
    NColor  border_color_t;
    NColor  border_color_r;
    NColor  border_color_b;
    
    // control
    NColor  ctlTextColor;
    NColor  ctlFgColor;
    NColor  ctlBgColor;
    NColor  ctlFillColor;
    NColor  ctlFocusColor;
    NColor  ctlEditColor;
    
    NCssBox margin;
    NCssBox border;
    NCssBox padding;
    
    char*   bgImage;
    coord   bg_x;
    coord   bg_y;
    
    int32   z_index;
    
    NCssValue   width;
    NCssValue   height;
    NCssValue   max_width;
    
    NCssValue   text_indent;
    NCssValue   line_height;
    
    NFloat  opacity;
    
    // other
    void*   view;
    NRect   dv;
    NFloat  zoom;
    
    // for clip calc
    NRect*  clip;
    
//    // just test
//    int     test;
    
} NStyle;

void style_init(NStyle* style);

struct _NLayoutStat;

typedef struct _NRenderNode {
    
    uint8   type;
    
    uint8   display : 2;
    uint8   zindex : 1;
    uint8   needLayout : 1;
    uint8   init : 1;
    uint8   flo : 2;
    uint8   clr : 2;
    uint8   vert_align : 2;
    
    NRect   r; // box
    
    // for align
    coord   ml;
    coord   mt;
    coord   mr;
    
    // for color
    uint8   hasBgcolor : 1;
    uint8   hasBrcolorL : 1;
    uint8   hasBrcolorT : 1;
    uint8   hasBrcolorR : 1;
    uint8   hasBrcolorB : 1;

    NColor  color;
    NColor  background_color;

    NColor  border_color_l;
    NColor  border_color_t;
    NColor  border_color_r;
    NColor  border_color_b;
    
    // for background image
    int16   bgiid;

    uint8   bg_repeat : 2;
    uint8   bg_x_t : 2;
    uint8   bg_y_t : 2;
    
    uint8   isAd : 1; // it's an AD
    
    coord   bg_x;
    coord   bg_y;

    void*   node; // not owned, type is Node, may be NULL.
    
    struct _NRenderNode* parent;
    struct _NRenderNode* child;
    struct _NRenderNode* prev;
    struct _NRenderNode* next;
    
    void (*GetStartPoint)(struct _NRenderNode* rn, coord* x, coord* y, bd_bool byChild);
    void (*Layout)(struct _NLayoutStat* stat, struct _NRenderNode* rn, NStyle* style, bd_bool force);
    void (*Paint)(struct _NLayoutStat* stat, struct _NRenderNode* rn, NStyle* style, NRect* rect);
    
    coord (*GetContentWidth)(struct _NRenderNode* rn);
    void (*GetAbsXY)(struct _NRenderNode* rn, coord* x, coord* y);
    
    // test only
//    int no;
//    int nidx;
//    void* pp;
    
} NRenderNode;

typedef struct _NLayoutStat {
    
    NRenderNode**   dat; // save dom path
    NRenderNode**   lay; // for layout path
    NRenderNode**   aux; // for float mode
    int16           top;
    int16           ld;
    NSheet*         sheet; // not onwed
    
} NLayoutStat;

#ifdef NBK_MEM_TEST
int renderNode_memUsed(const NRenderNode* rn);
#endif

NRenderNode* renderNode_create(void* view, NNode* node);
void renderNode_delete(NRenderNode** rn);
void renderNode_init(NRenderNode* rn);
void renderNode_calcStyle(NRenderNode* rn, NStyle* style);
bd_bool renderNode_layoutable(NRenderNode* rn, coord maxw);
bd_bool renderNode_isChildsNeedLayout(NRenderNode* rn);

bd_bool renderNode_has(NNode* node, uint8 type);

coord renderNode_getWidth(const NRenderNode* rn);
coord renderNode_getHeight(const NRenderNode* rn);
coord renderNode_getMaxBottomByLine(const NRenderNode* rn, uint8 clear);
coord renderNode_getFollowLineHeight(const NRenderNode* rn);

void renderNode_adjustPos(NRenderNode* rn, coord dy);
coord renderNode_getContentWidth(NRenderNode* rn);
void renderNode_getAbsXY(NRenderNode* rn, coord* x, coord* y);

void renderNode_getAvailArea(NRenderNode* rn, NRenderNode* cur, NRect* area);
bd_bool renderNode_getAvailAreaByChild(NLayoutStat* stat, NRenderNode* rn, NRect* area);
bd_bool renderNode_isNearestNodeFloat(NRenderNode* rn);

void renderNode_absRect(NRenderNode* rn, NRect* rect);

void renderNode_getRootAbsXYForFFFull(NRenderNode* rn, coord* x, coord* y);

int renderNode_getEditLength(NRenderNode* rn);
int renderNode_getEditMaxLength(NRenderNode* rn);
bd_bool renderNode_getRectOfEditing(NRenderNode* rn, NRect* rect, NFontId* fontId);
wchr* renderNode_getEditText16(NRenderNode* rn, int* len);
void renderNode_setEditText16(NRenderNode* rn, wchr* text, int len);

void renderNode_getSelPos(NRenderNode* rn, int* start, int* end);
void renderNode_setSelPos(NRenderNode* rn, int start, int end);

bd_bool renderNode_getAbsRect(NRenderNode* rn, NRect* rect);

void renderNode_drawBorder(NRenderNode* rn, NCssBox* border, NRect* rect, void* pfd);
void renderNode_drawBgImage(NRenderNode* rn, NRect* pr, NRect* vr, NStyle* style);
void renderNode_drawBgImageFF(NRenderNode* rn, NRect* pr, NRect* vr, NStyle* style);
void renderNode_drawFocusRing(NRect pr, NColor color, void* pfd);

// render tree traverse util
typedef int (*RTREE_TRAVERSE_START_CB)(NRenderNode* render, void* user, bd_bool* ignore);
typedef int (*RTREE_TRAVERSE_CB)(NRenderNode* render, void* user);
void rtree_traverse_depth(NRenderNode* root, RTREE_TRAVERSE_START_CB startCb, RTREE_TRAVERSE_CB closeCb, void* user);

NRenderNode* renderNode_getParent(NRenderNode* rn, uint8 type);
void renderNode_getAbsPos(NRenderNode* rn, coord* x, coord* y);

void renderNode_checkAdFF(NRenderNode* rn);
void renderNode_drawAdPlaceholderFF(NRenderNode* rn, NStyle* style, NRect* rect);

#ifdef __cplusplus
}
#endif

#endif /* RENDERNODE_H_ */
