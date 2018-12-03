/*
 * view.h
 *
 *  Created on: 2010-12-29
 *      Author: wuyulun
 */

#ifndef VIEW_H_
#define VIEW_H_

#include "../inc/config.h"
#include "../inc/nbk_limit.h"
#include "node.h"
#include "../render/renderNode.h"
#include "../render/imagePlayer.h"

#ifdef __cplusplus
extern "C" {
#endif

// CSS z-index info

typedef struct _NZidxCell {
    NRenderNode*    rn;
    NRect           rect;
} NZidxCell;

typedef struct _NZidxPanel {
    int32       z;
    int16       use;
    NZidxCell   cells[MAX_ZIDX_CELL_IN_PANEL];
} NZidxPanel;

typedef struct _NZidxMatrix {
    bd_bool        ready;
    NZidxPanel  panels[MAX_ZIDX_PANEL];
} NZidxMatrix;

// View / Render Tree

typedef struct _NView {
    
    void*           page; // not owned
    
    NRenderNode*    root;
    
    NLayoutStat     stack;
    NRenderNode*    lastChild;
    
    coord   scrWidth; // init layout width
    coord   optWidth; // mobile optimized width
    NRect   overflow; // contents overflow
    
    NNode** path; // stack
    
    NNode*  focus;
    
    NImagePlayer*   imgPlayer;
    
    bd_bool    ownPlayer : 1;
    
    NZidxMatrix*    zidx;
    
    NRect   frameRect; // ff only
    
//    NRenderNode* modalRender; // paint this render in last
    
} NView;

#ifdef NBK_MEM_TEST
int view_memUsed(const NView* view);
#endif

NView* view_create(void* page);
void view_delete(NView** view);

void view_begin(NView* view);
void view_clear(NView* view);

void view_processNode(NView* view, NNode* node);

void view_layout(NView* view, NRenderNode* root, bd_bool force);
bd_bool view_layoutTextFF(NView* view);

void view_paint(NView* view, NRect* rect);
void view_paintFocus(NView* view, NRect* rect);
void view_paintControl(NView* view, NRect* rect);
void view_paintMainBodyBorder(NView* view, NNode* node, NRect* rect,bd_bool boder);
void view_paintPartially(NView* view, NRenderNode* root, NRect* rect);
void view_paintWithZindex(NView* view, NRect* rect);
void view_paintFocusWithZindex(NView* view, NRect* rect);

void view_picChanged(NView* view, int16 imgId);
void view_picUpdate(NView* view, int16 imgId);

NRect view_getRectWithOverflow(NView* view);
coord view_width(NView* view);
coord view_height(NView* view);
NFloat view_getMinZoom(NView* view);

void view_setFocusNode(NView* view, NNode* node);
void* view_getFocusNode(NView* view);
NNode* view_findFocusWithZindex(NView* view, coord x, coord y);

void view_enablePic(NView* view, bd_bool enable);
void view_stop(NView* view);
void view_pause(NView* view);
void view_resume(NView* view);

void view_dump_render_tree_2(NView* view, NRenderNode* root);

void rtree_delete_node(NView* view, NRenderNode* node);
bd_bool rtree_replace_node(NView* view, NRenderNode* old, NRenderNode* novice);
bd_bool rtree_insert_node(NRenderNode* node, NRenderNode* novice, bd_bool before);
bd_bool rtree_insert_node_as_child(NRenderNode* parent, NRenderNode* novice);

void view_nodeDisplayChanged(NView* view, NRenderNode* rn, nid display);
void view_nodeDisplayChangedFF(NView* view, NRenderNode* rn, nid display);
void view_checkFocusVisible(NView* view);

void view_buildZindex(NView* view);
bd_bool view_isZindex(NView* view);
void view_clearZindex(NView* view);

#ifdef __cplusplus
}
#endif

#endif /* VIEW_H_ */
