/*
 * view.c
 *
 *  Created on: 2010-12-29
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_limit.h"
#include "../inc/nbk_callback.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_gdi.h"
#include "../inc/nbk_settings.h"
#include "view.h"
#include "node.h"
#include "attr.h"
#include "page.h"
#include "document.h"
#include "history.h"
#include "xml_tags.h"
#include "xml_atts.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "../css/cssSelector.h"
#include "../tools/dump.h"
#include "../tools/str.h"
#include "../render/renderNode.h"
#include "../render/renderBlock.h"
#include "../render/renderBlank.h"
#include "../render/renderImage.h"
#include "../render/renderInput.h"
#include "../render/renderText.h"
#include "../render/renderSelect.h"

#define DUMP_FUNCTIONS      0
#define DUMP_RENDER_TREE    0

#define TEST_PAINT_TIME     0

#ifdef NBK_MEM_TEST
int view_memUsed(const NView* view)
{
    int size = 0;
    if (view)
        size = sizeof(NView);
    return size;
}
#endif

NView* view_create(void* page)
{
    NPage* p = (NPage*)page;
    NView* v = (NView*)NBK_malloc0(sizeof(NView));
    N_ASSERT(v);
    
    v->page = page;
    
    if (p->main) {
        v->imgPlayer = imagePlayer_create(v);
        v->ownPlayer = 1;
    }
    else {
        while (p->parent)
            p = p->parent;
        v->imgPlayer = p->view->imgPlayer;
    }
    
    return v;
}

void view_delete(NView** view)
{
    NView* v = *view;
    NPage* p = (NPage*)v->page;

    if (p->main && v->ownPlayer) {
        imagePlayer_delete(&v->imgPlayer);
    }
    
    view_clear(v);
    
    if (v->stack.dat) {
        NBK_free(v->stack.dat);
        v->stack.dat = N_NULL;
        NBK_free(v->stack.lay);
        v->stack.lay = N_NULL;
        NBK_free(v->stack.aux);
        v->stack.aux = N_NULL;
    }
    if (v->path) {
        NBK_free(v->path);
        v->path = N_NULL;
    }
    
    if (v->zidx)
        NBK_free(v->zidx);
    
    NBK_free(v);
    *view = N_NULL;
}

void view_begin(NView* view)
{
    NPage* p = (NPage*)view->page;
    
    if (view->stack.dat == N_NULL) {
        view->stack.dat = (NRenderNode**)NBK_malloc(sizeof(NRenderNode*) * MAX_TREE_DEPTH);
        N_ASSERT(view->stack.dat);
        view->stack.lay = (NRenderNode**)NBK_malloc(sizeof(NRenderNode*) * MAX_TREE_DEPTH);
        N_ASSERT(view->stack.lay);
        view->stack.aux = (NRenderNode**)NBK_malloc(sizeof(NRenderNode*) * MAX_TREE_DEPTH);
        N_ASSERT(view->stack.aux);
    }
    if (view->path == N_NULL) {
        view->path = (NNode**)NBK_malloc(sizeof(NNode*) * MAX_TREE_DEPTH);
        N_ASSERT(view->path);
    }
    
    view_clear(view);
    
    view->stack.top = -1;
    view->stack.ld = -1;
    view->lastChild = N_NULL;
    view->optWidth = 0;

    if (p->main && view->ownPlayer) {
        imagePlayer_reset(view->imgPlayer);
        view->imgPlayer->cache = p->cache;
    }
    
    if (view->zidx)
        view->zidx->ready = N_FALSE;
}

static void rtree_delete_tree(NView* view, NRenderNode* root)
{
    int16 top = -1;
    NRenderNode* n = root;
    NRenderNode* t = N_NULL;
    
    while (n) {
        if (n->child) {
            view->stack.dat[++top] = n;
            n = n->child;
        }
        else {
            if (n == root)
                break;
            t = n;
            if (n->next)
                n = n->next;
                else {
                    n = view->stack.dat[top--];
                    n->child = N_NULL;
                }
            
            renderNode_delete(&t);
        }
    }
    if (n)
        renderNode_delete(&n);
}

//#define VIEW_TEST_RELEASE
void view_clear(NView* view)
{
#ifdef VIEW_TEST_RELEASE
    double t_consume;
#endif
    
    if (view->stack.dat == N_NULL)
        return;

#ifdef VIEW_TEST_RELEASE
    t_consume = NBK_currentMilliSeconds();
#endif
    
    NBK_memset(view->stack.dat, 0, sizeof(NRenderNode*) * MAX_TREE_DEPTH);
    NBK_memset(view->stack.lay, 0, sizeof(NRenderNode*) * MAX_TREE_DEPTH);
    NBK_memset(view->stack.aux, 0, sizeof(NRenderNode*) * MAX_TREE_DEPTH);
    
    rtree_delete_tree(view, view->root);
    
    view->root = N_NULL;
    view->focus = N_NULL;
    
#ifdef VIEW_TEST_RELEASE
    dump_char(view->page, "view clear", -1);
    dump_int(view->page, (int)(NBK_currentMilliSeconds() - t_consume));
    dump_return(view->page);
#endif
}

static void view_insert_as_child(NRenderNode** parent, NRenderNode* lastChild, NRenderNode* node)
{
    if (*parent == N_NULL) {
        *parent = node;
    }
    else {
        if (lastChild == N_NULL) {
            (*parent)->child = node;
            node->parent = *parent;
        }
        else {
            lastChild->next = node;
            node->parent = *parent;
            node->prev = lastChild;
        }
    }
}

static void view_push_node(NView* view, NRenderNode* node)
{
    N_ASSERT(view->stack.top < MAX_TREE_DEPTH);
    view->stack.dat[++view->stack.top] = node;
    view->lastChild = node->child;
}

static void view_pop_node(NView* view)
{
    N_ASSERT(view->stack.top >= 0);
    view->lastChild = view->stack.dat[view->stack.top--];
}

// building render tree
void view_processNode(NView* view, NNode* node)
{
    NPage* page = (NPage*)view->page;
    NRenderNode* rn = N_NULL;
    
    if (node->id > TAGID_CLOSETAG) {
        if (renderNode_has(node, page->doc->type) && !node_is_single(node->id)) {
            view_pop_node(view);
        }
        return;
    }
    
    if (node->id == TAGID_TEXT && view->root == N_NULL) {
        // ignore text not belong to any block.
        return;
    }
    
    rn = renderNode_create(view, node);
    if (rn == N_NULL) // no need render, or mem insufficent?
        return;
    node->render = rn;
    
    if (node->id == TAGID_TEXT || node_is_single(node->id)) {
        
        if (view->stack.top < 0) {
            node->render = N_NULL;
            renderNode_delete(&rn);
            return;
        }
        
        // don't push text node, because it has no close status.
        view_insert_as_child(&view->stack.dat[view->stack.top], view->lastChild, rn);
        view->lastChild = rn;
        return;
    }
    
    if (view->root == N_NULL) {
        view_insert_as_child(&view->root, N_NULL, rn);
        
        // initialize width
        rect_set(&view->root->r, 0, 0, ((view->optWidth > 0) ? view->optWidth : view->scrWidth), 0);
        view->root->needLayout = 1;
    }
    else {
        view_insert_as_child(&view->stack.dat[view->stack.top], view->lastChild, rn);
    }
    view_push_node(view, rn);
}

#if DUMP_FUNCTIONS
static void view_dump_render_node_header(const NView* view)
{
    NPage* page = (NPage*)view->page;
    dump_char(page, "level", -1);
    dump_char(page, "type", -1);
    dump_char(page, "tag", -1);
    dump_char(page, "rect", -1);
    dump_char(page, "mt", -1);
    dump_char(page, "mr", -1);
    dump_char(page, "width", -1);
    dump_char(page, "height", -1);
    dump_char(page, "text", -1);
    dump_return(page);
}

static void view_dump_render_node(const NView* view, const NRenderNode* r)
{
    NPage* page = (NPage*)view->page;
    char buf[128];

    dump_int(page, view->stack.ld);
    
    switch (r->type) {
    case RNT_BLOCK:
        dump_char(page, "<block>", -1);
        break;
    case RNT_TEXT:
        dump_char(page, "<text>", -1);
        break;
    case RNT_IMAGE:
        dump_char(page, "<image>", -1);
        break;
    case RNT_BR:
        dump_char(page, "<br>", -1);
        break;
    }
    
    dump_char(page, (char*)xml_getTagNames()[((NNode*)r->node)->id], -1);
    
    dump_NRect(page, &r->r);
    
    NBK_sprintf(buf, "ml=%d mt=%d mr=%d %d x %d",
        r->ml, r->mt, r->mr, rect_getWidth(&r->r), rect_getHeight(&r->r));
    dump_char(page, buf, -1);
    
    if (r->type == RNT_TEXT) {
        NNode* n = (NNode*)r->node;
        dump_wchr(page, n->d.text, n->len);
    }
    
    dump_return(page);
}

void view_dump_render_tree_2(NView* view, NRenderNode* root)
{
    NPage* page = (NPage*)view->page;
    NRenderNode** sta = (NRenderNode**)NBK_malloc(sizeof(NRenderNode*) * MAX_TREE_DEPTH);
    char* level = (char*)NBK_malloc(512);
    int16 lv = -1;
    NRenderNode* r = root;
    NNode* n;
    bd_bool ignore = N_FALSE;
    int i, j, k;
    
    dump_return(page);
    dump_char(page, "===== render tree =====", -1);
    dump_return(page);
    
    while (r) {
        if (!ignore) {
            sta[lv+1] = r;
            for (i=1, j=0; i <= lv+1; i++) {
                
                if (sta[i]->next)
                    level[j++] = '|';
                else {
                    if (sta[i] == r)
                        level[j++] = '`';
                    else
                        level[j++] = ' ';
                }
                
                if (sta[i] == r) {
                    level[j++] = '-';
                    level[j++] = '-';
                }
                else {
                    level[j++] = ' ';
                    level[j++] = ' ';
                }
                
                level[j++] = ' ';
            }
            k = j;

            switch (r->type) {
            case RNT_BLOCK:
                NBK_strcpy(&level[j], "block");
                j += 5;
                break;
            case RNT_INLINE:
                NBK_strcpy(&level[j], "inline");
                j += 6;
                break;
            case RNT_TEXT:
                NBK_strcpy(&level[j], "text");
                j += 4;
                break;
            case RNT_IMAGE:
                NBK_strcpy(&level[j], "image");
                j += 5;
                break;
            case RNT_BR:
                NBK_strcpy(&level[j], "br");
                j += 2;
                break;
            case RNT_INPUT:
                NBK_strcpy(&level[j], "input");
                j += 5;
                break;
            case RNT_HR:
                NBK_strcpy(&level[j], "hr");
                j += 2;
                break;
            case RNT_SELECT:
                NBK_strcpy(&level[j], "select");
                j += 6;
                break;
            case RNT_BLANK:
                NBK_strcpy(&level[j], "blank");
                j += 5;
                break;
            case RNT_A:
                NBK_strcpy(&level[j], "a");
                j += 1;
                break;
            case RNT_TABLE:
                NBK_strcpy(&level[j], "table");
                j += 5;
                break;
            case RNT_TR:
                NBK_strcpy(&level[j], "tr");
                j += 2;
                break;
            case RNT_TD:
                NBK_strcpy(&level[j], "td");
                j += 2;
                break;
            }
            
            n = (NNode*)r->node;
            if (n) {
                level[j++] = '/';
                NBK_strcpy(&level[j], xml_getTagNames()[n->id]);
                j += NBK_strlen(xml_getTagNames()[n->id]);
            }
            
            if (r->type == RNT_BLOCK && ((NRenderBlock*)r)->overflow == CSS_OVERFLOW_HIDDEN) {
                NBK_strcpy(&level[j], " hidden");
                j += 7;
            }
            
            level[j++] = '\t';
            NBK_strcpy(&level[j], (r->needLayout) ? " L" : " n");
            j += 2;
            NBK_strcpy(&level[j], (r->display) ? " D" : " h");
            j += 2;
            
            dump_char(page, level, j);
            dump_NRect(page, &r->r);
            dump_int(page, r->ml);
            dump_int(page, r->mt);
            dump_int(page, r->mr);
            
//            if (attr_getRect(n->atts, ATTID_TC_RECT))
//                dump_NRect(page, attr_getRect(n->atts, ATTID_TC_RECT));
            
            if (n && n->id == TAGID_TEXT) {
                NRenderText* rt = (NRenderText*)r;
#if 0
                dump_wchr(page, n->d.text, n->len);
#else
                if (rt->lines) {
                    dump_return(page);
                    for (j=0; j < 4; j++)
                        level[k++] = '\t';
                    for (i=0; i < rt->use; i++) {
                        dump_char(page, level, k);
                        dump_int(page, i);
                        dump_NRect(page, &rt->lines[i].r);
                        dump_int(page, rt->lines[i].ml);
                        dump_int(page, rt->lines[i].mr);
                        dump_wchr(page, rt->lines[i].text, (int)(rt->lines[i+1].text - rt->lines[i].text));
                        if (i < rt->use - 1)
                            dump_return(page);
                    }
                }
#endif
                }
            
            dump_return(page);
        }
        
        if (!ignore && r->child) {
            sta[++lv] = r;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == root)
                    break;
            if (r->next)
                r = r->next;
                else {
                r = sta[lv--];
                    ignore = N_TRUE;
                }
            }
        }
    
    NBK_free(level);
    NBK_free(sta);
    
    dump_flush(page);
}

static void view_dump_dom_node(const NView* view, const NNode* node)
{
    char level[64];
    char ch;
    int16 i, lv;
    lv = view->stack.ld;
    if (node->id == TAGID_TEXT)
        lv++;
    for (i=0; i <= lv; i++) {
        ch = (i == lv) ? '+' : ' ';
        level[i] = ch;
    }
    level[i++] = '-';
    level[i++] = ' ';
    NBK_strcpy(&level[i], xml_getTagNames()[node->id]);
    dump_char(view->page, level, -1);
    if (node->id == TAGID_TEXT)
        dump_wchr(view->page, node->d.text, node->len);
    dump_return(view->page);
}
#endif


void view_layout(NView* view, NRenderNode* root, bd_bool force)
{
    NRenderNode* r = root;
    bd_bool ignore = N_FALSE;
    NStyle style;
    NPage* page = (NPage*)view->page;
    NRect* clips;
    bd_bool useClip = N_FALSE;
    
    if (page->workState != NEWORK_IDLE)
	{
        return;
	}
    
    if (root == N_NULL)
        return;
    
    // request cliped background images (ff only)
    if (  (page->doc->type == NEDOC_FULL || page->doc->type == NEDOC_NARROW)
        && view->imgPlayer->multipics)
        useClip = N_TRUE;
    
    page->workState = NEWORK_LAYOUT;
    
        clips = (NRect*)NBK_malloc(sizeof(NRect) * MAX_TREE_DEPTH);
    rect_set(&clips[0], 0, 0, N_MAX_COORD, N_MAX_COORD);
    
    rect_set(&view->overflow, 0, 0, 0, 0);
    if (force || view->root->needLayout) {
        rect_set(&view->root->r, 0, 0, ((view->optWidth > 0) ? view->optWidth : view->scrWidth), 0);
        if (page->settings && page->settings->selfAdaptionLayout)
            view->root->r.r = view->scrWidth;
    }
    
    view->stack.ld = -1;
    view->stack.sheet = page->sheet;
    while (r) {
        
        if (!ignore) {
            NNode* node = (NNode*)r->node;
            
            style_init(&style);
            if (node)
                node_calcStyle(view, node, &style);
            
            // request background image
            if (r->bgiid == -1 && style.bgImage) {
                NImageClipInfo* ci = N_NULL;
                NImageClipInfo clip;
                coord tc_offsetx = attr_getCoord(node->atts, ATTID_TC_CUTEOFFSETX);
                if (useClip && tc_offsetx == -1) {
                    NRect* tr = attr_getRect(node->atts, ATTID_TC_RECT);
                    if (tr) {
                        coord tc_originw = attr_getCoord(node->atts, ATTID_TC_CUTEORIGINW);
                        clip.repeat = style.bg_repeat;
                        clip.x_t = (style.bg_x_t == CSS_POS_PERCENT) ? 1 : 0;
                        clip.y_t = (style.bg_y_t == CSS_POS_PERCENT) ? 1 : 0;
                        clip.x = style.bg_x;
                        clip.y = style.bg_y;
                        clip.w = (tc_originw == -1) ? rect_getWidth(tr) : tc_originw;
                        clip.h = rect_getHeight(tr);
                        ci = &clip;
                    }
                }
                if (tc_offsetx == -1)
                r->bgiid = imagePlayer_getId(view->imgPlayer, style.bgImage, NEIT_CSS_IMAGE, ci);
                else
                    r->bgiid = IMAGE_CUTE_ID;
            }
            
            if (style.z_index != 0 && r->type == RNT_BLOCK)
                r->zindex = 1;
        }

        style.view = view;
        style.clip = &clips[view->stack.ld + 1];
        r->Layout(&view->stack, r, &style, force);
//        view_dump_render_node(view, r);
        
        if (!ignore && r->child) {
            view->stack.lay[++view->stack.ld] = r;
            view->stack.aux[view->stack.ld] = N_NULL;
            r = r->child;
            clips[view->stack.ld + 1] = clips[view->stack.ld];
        }
        else {
            ignore = N_FALSE;
            if (r->child) {
                if (rect_getWidth(&r->r) && rect_getHeight(&r->r))
                rect_unite(&view->overflow, &r->r);
            }
            if (r == root)
                break;
            else if (r->next) {
                r = r->next;
                clips[view->stack.ld + 1] = clips[view->stack.ld];
            }
            else {
            // return to parent
                    r = view->stack.lay[view->stack.ld--];
                    ignore = N_TRUE;
                }
            }
        }
    
    // normalized
    if (view->overflow.l < 0) view->overflow.l = 0;
    if (view->overflow.t < 0) view->overflow.t = 0;
    
    if (page->main) {
//        dump_char(page, "document overflow", -1);
//        dump_NRect(page, &view->overflow);
//        dump_return(page);
    }
    else {
        NPage* mainPage = page_getMainPage(page);
        rect_unite(&mainPage->view->overflow, &view->overflow);
    }
    
    NBK_free(clips);
    
    page->workState = NEWORK_IDLE;
    
#if DUMP_RENDER_TREE
    view_dump_render_tree_2(view, view->root);
#endif
}

NRect view_getRectWithOverflow(NView* view)
{
    NRect r = {0, 0, 0, 0};
    if (view->root) {
        r = view->root->r;
        rect_unite(&r, &view->overflow);
    }
    return r;
}

coord view_width(NView* view)
{
    if (view->root == N_NULL) {
        return view->scrWidth;
    }
    else {
        NRect r = view_getRectWithOverflow(view);
        return rect_getWidth(&r);
    }
}

coord view_height(NView* view)
{
    if (view->root == N_NULL) {
        return 0;
    }
    else {
        NRect r = view_getRectWithOverflow(view);
        return rect_getHeight(&r);
    }
}

void view_paint(NView* view, NRenderRect* rect)
{
    NRenderNode* r;
    NPage* page = (NPage*)view->page;
    bd_bool ignore = N_FALSE;
    NStyle style;
    NFloat zoom;
    
#if TEST_PAINT_TIME
    double t_consume;
    int t_times = 0, t_total = 0;
#endif
        
    if (view->root == N_NULL)
        return;
    
    if (page->workState != NEWORK_IDLE)
	{
        return;
	}
    
    page->workState = NEWORK_PAINT;
    
#if TEST_PAINT_TIME
    t_consume = NBK_currentMilliSeconds();
#endif
    
    zoom = history_getZoom((NHistory*)page->history);
    r = view->root;
    view->stack.aux[0] = N_NULL;
    view->stack.ld = 0;
    while (r && !r->needLayout) {
        
        if (!ignore) {
            style_init(&style);
            style.view = view;
            style.dv = view_getRectWithOverflow(view);
            style.zoom = zoom;
            if (r->display)
                r->Paint(&view->stack, r, &style, rect);
            else if (r->type == RNT_BLOCK)
                ignore = N_TRUE;
            
#if TEST_PAINT_TIME
            t_total++;
            if (style.test)
                t_times++;
#endif
        }
        
        if (!ignore && r->child) {
            view->stack.lay[++view->stack.ld] = r;
            view->stack.aux[view->stack.ld] = N_NULL;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == view->root)
                break;
            else if (r->next)
                r = r->next;
                else {
                    r = view->stack.lay[view->stack.ld--];
                    ignore = N_TRUE;
                }
            }
        }
    
#if TEST_PAINT_TIME
    if (page->doc->finish) {
        dump_char(page, "view paint", -1);
        dump_int(page, (int)(NBK_currentMilliSeconds() - t_consume));
        dump_int(page, t_times);
        dump_int(page, t_total);
        dump_return(page);
    }
#endif
    
    page->workState = NEWORK_IDLE;
}

#define MAX_FOCUS_AREA  16

typedef struct _NTrPaintFocus {
    
    NView*  view;
    NRect*  rect;
    NStyle  style;
    NRect   area[MAX_FOCUS_AREA];
    int     areaNum;
    coord   top;
    coord   bottom;
    coord   center;
    bd_bool    absLayout;
    int     test;
    
} NTrPaintFocus;

static int vw_paint_focus(NNode* node, void* user, bd_bool* ignore)
{
    NRenderNode* r = (NRenderNode*)node->render;
    
    if (r) {
    NTrPaintFocus* tpf = (NTrPaintFocus*)user;
        if (tpf->style.belongA && (r->type == RNT_BLOCK || r->type == RNT_BLANK))
            return 0;
        tpf->style.highlight = (r->type == RNT_BLOCK || tpf->style.ne_fold) ? 0 : 1;
        r->Paint(&tpf->view->stack, r, &tpf->style, tpf->rect);
    }
    
    return 0;
}

static int calc_focus_area(NTrPaintFocus* task, NRect* rect)
{
    if (task->areaNum == -1) {
        task->areaNum = 0;
        task->area[0] = *rect;
        task->top = rect->t;
        task->bottom = rect->b;
        task->center = rect->t + ((rect->b - rect->t) >> 1);
    }
    else {
        if (   rect->t == task->top
            || rect->b == task->bottom
            || (rect->t <= task->center && rect->b >= task->center)) {
            rect_unite(&task->area[task->areaNum], rect);
        }
        else {
            if (task->areaNum > 0) {
                bd_bool l, r;
                coord dx, dy;
                l = r = N_FALSE;
                dx = task->area[task->areaNum].l - task->area[task->areaNum-1].l;
                dy = task->area[task->areaNum].r - task->area[task->areaNum-1].r;
                if (N_ABS(dx) < 10)
                    l = N_TRUE;
                if (N_ABS(dy) < 10)
                    r = N_TRUE;
                if (l && r) {
                    rect_unite(&task->area[task->areaNum-1], &task->area[task->areaNum]);
                    task->areaNum--;
                }
            }
            if (task->areaNum == MAX_FOCUS_AREA - 1)
                return 1;
            task->areaNum++;
            task->area[task->areaNum] = *rect;
            task->top = rect->t;
            task->bottom = rect->b;
            task->center = rect->t + ((rect->b - rect->t) >> 1);
        }
    }
    return 0;
}

static int vw_paint_focus_get_area(NNode* node, void* user, bd_bool* ignore)
{
    int ret = 0;
    NRenderNode* r = (NRenderNode*)node->render;
    NTrPaintFocus* task = (NTrPaintFocus*)user;
    
    if (r) {
        coord x, y;
        NRect pr;
        
        if (task->absLayout)
            x = y = 0;
        else
            renderNode_getAbsPos(r->parent, &x, &y);
        
        if (r->type == RNT_TEXT) {
            int i;
            NRect cl;
            NRenderText* rt = (NRenderText*)r;
            cl = rt->clip;
            rect_move(&cl, cl.l + x, cl.t + y);
            for (i=0; rt->lines && i < rt->use; i++) {
                pr = rt->lines[i].r;
                rect_move(&pr, pr.l + x, pr.t + y);
                rect_intersect(&pr, &cl);
                if (rect_getWidth(&pr) && rect_getHeight(&pr))
                ret = calc_focus_area(task, &pr);
            }
        }
        else if (   r->type != RNT_A
                 && r->type != RNT_BLANK
                 && r->type != RNT_BLOCK
                 && r->type != RNT_INLINE
                 && r->type != RNT_BR) {
            pr = r->r;
            if (task->absLayout) {
                if (r->type == RNT_IMAGE) {
                    NRenderImage* ri = (NRenderImage*)r;
                    rect_intersect(&pr, &ri->clip);
                }
            }
            rect_move(&pr, pr.l + x, pr.t + y);
            ret = calc_focus_area(task, &pr);
        }
    }
    
    return ret;
}

void view_paintFocus(NView* view, NRect* rect)
{
    NColor focusColor = {0x03, 0x5c, 0xfe, 0xff};
    
    NPage* page = (NPage*)view->page;
    NTrPaintFocus task;
    
    if (view->focus == N_NULL)
        return;
    
    switch (view->focus->id) {
    case TAGID_A:
    case TAGID_ANCHOR:
    case TAGID_SPAN:
    case TAGID_BUTTON:
    case TAGID_INPUT:
    case TAGID_TEXTAREA:
    case TAGID_SELECT:
    case TAGID_IMG:
    case TAGID_DIV:
        break;
    default:
        return;
    }
    
    if (page->workState != NEWORK_IDLE)
	{
        return;
	}
    
    page->workState = NEWORK_PAINT;

    NBK_memset(&task, 0, sizeof(NTrPaintFocus));
    task.view = view;
    task.rect = rect;
    style_init(&task.style);
    task.style.view = view;
    task.style.dv = view_getRectWithOverflow(view);
    task.style.highlight = 1;
    task.style.color = colorWhite;
    task.style.background_color = focusColor;
    task.style.zoom = history_getZoom((NHistory*)page->history);
    task.areaNum = -1;
    task.absLayout = doc_isAbsLayout(page->doc->type);
    
    task.test = 0;
    
    if (view->focus->id == TAGID_DIV) {
        NRenderNode* r = (NRenderNode*)view->focus->render;
        NNode* n = (NNode*)r->node;
        if (attr_getValueStr(n->atts, ATTID_NE_DISPLAY)) {
            NColor bg = {0x62, 0x92, 0xD2, 0xff};
            task.style.ne_fold = 1;
            task.style.highlight = 0;
            task.style.background_color = bg;
            node_traverse_depth(view->focus, vw_paint_focus, N_NULL, &task);
        }
        else
            r->Paint(&view->stack, r, &task.style, rect);
    }
    else if (view->focus->id == TAGID_A && view->focus->child == N_NULL) {
        NRenderNode* r = (NRenderNode*)view->focus->render;
        if (r)
            r->Paint(&view->stack, r, &task.style, rect);
    }
    else if (view->focus->id == TAGID_A || view->focus->id == TAGID_ANCHOR) {
        int i;
        NRect pr, cl;
        NColor fill = {0x60, 0xbe, 0xfd, 255};
        coord bold = FOCUS_RING_BOLD;
        coord da = bold >> 1;
        
        // calc focus area
        node_traverse_depth(view->focus, vw_paint_focus_get_area, N_NULL, &task);
        
        // adjust focus area
        for (i=0; i < task.areaNum; i++) {
            if (i + 1 == task.areaNum) {
                if (task.areaNum == 1 && task.area[i+1].r <= task.area[i].l)
                    break;
                task.area[i+1].t = task.area[i].b;
            }
            else
                task.area[i].b = task.area[i+1].t;
	}
    
        cl.l = cl.t = 0;
        cl.r = rect_getWidth(rect);
        cl.b = rect_getHeight(rect);
        NBK_gdi_setClippingRect(page->platform, &cl);
    
        // fill background
        NBK_gdi_setBrushColor(page->platform, &fill);
        for (i=0; i <= task.areaNum; i++) {
            rect_toView(&task.area[i], task.style.zoom);
            rect_move(&task.area[i], task.area[i].l - rect->l, task.area[i].t - rect->t);
            NBK_gdi_clearRect(page->platform, &task.area[i]);
        }

        // draw borders
        NBK_gdi_setBrushColor(page->platform, &task.style.background_color);
        for (i=0; i <= task.areaNum; i++) {
    
            // draw left side
            pr = task.area[i];
            pr.l -= bold;
            pr.r = pr.l + bold;
            if (i == 0 || task.area[i].l < task.area[i-1].l)
                pr.t -= da;
            if (i == task.areaNum || task.area[i].l < task.area[i+1].l)
                pr.b += da;
            NBK_gdi_clearRect(page->platform, &pr);
    
            // draw right side
            pr = task.area[i];
            pr.l = pr.r;
            pr.r = pr.l + bold;
            if (i == 0 || task.area[i].r > task.area[i-1].r)
                pr.t -= da;
            if (i == task.areaNum || task.area[i].r > task.area[i+1].r)
                pr.b += da;
            NBK_gdi_clearRect(page->platform, &pr);
            
            // draw up side
            pr = task.area[i];
            pr.t -= bold;
            pr.b = pr.t + bold;
            if (i == 0) {
                pr.l -= da;
                pr.r += da;
                NBK_gdi_clearRect(page->platform, &pr);
            }
            else {
                if (task.area[i-1].l > task.area[i].r) {
                    pr.l -= da;
                    pr.r += da;
                    NBK_gdi_clearRect(page->platform, &pr);
                }
                else {
                    if (task.area[i-1].l > task.area[i].l) {
                        pr.r = task.area[i-1].l;
                        pr.l -= da;
                        NBK_gdi_clearRect(page->platform, &pr);
                    }
                    if (task.area[i-1].r < task.area[i].r) {
                        pr.l = task.area[i-1].r;
                        pr.r = task.area[i].r;
                        pr.r += da;
                        NBK_gdi_clearRect(page->platform, &pr);
                    }
                }
            }
            
            // draw down side
            pr = task.area[i];
            pr.t = pr.b;
            pr.b = pr.t + bold;
            if (i == task.areaNum) {
                pr.l -= da;
                pr.r += da;
                NBK_gdi_clearRect(page->platform, &pr);
            }
            else {
                if (task.area[i+1].r < task.area[i].l) {
                    pr.l -= da;
                    pr.r += da;
                    NBK_gdi_clearRect(page->platform, &pr);
                }
                else {
                    if (task.area[i].r > task.area[i+1].r) {
                        pr.l = task.area[i+1].r;
                        pr.r += da;
                        NBK_gdi_clearRect(page->platform, &pr);
                    }
                    if (task.area[i].l < task.area[i+1].l) {
                        pr.l = task.area[i].l;
                        pr.r = task.area[i+1].l;
                        pr.l -= da;
                        NBK_gdi_clearRect(page->platform, &pr);
                    }
                }
            }
        }
    
        NBK_gdi_cancelClippingRect(page->platform);
    
        task.style.belongA = 1;
        node_traverse_depth(view->focus, vw_paint_focus, N_NULL, &task);
    }
    else {
        node_traverse_depth(view->focus, vw_paint_focus, N_NULL, &task);
    }
    
    page->workState = NEWORK_IDLE;
    
//    dump_char(page, "paintFocus", -1);
//    dump_int(page, tpf.test);
//    dump_return(page);
}

void view_paintControl(NView* view, NRect* rect)
{
    NRenderNode* r;
    NPage* page = (NPage*)view->page;
    NStyle style;
    
    if (page->workState != NEWORK_IDLE)
        return;
    
    if (view->focus == N_NULL /*&& view->modalRender == N_NULL*/)
        return;
    
    page->workState = NEWORK_PAINT;
    
    style_init(&style);
    style.view = view;
    style.dv = view_getRectWithOverflow(view);
    style.zoom = history_getZoom((NHistory*) page->history);

    if (view->focus) {
        r = (NRenderNode*)view->focus->render;
        style.highlight = 1;
    if (   view->focus->id == TAGID_INPUT
        || view->focus->id == TAGID_TEXTAREA) {
        r->Paint(&view->stack, r, &style, rect);
    }
        else if (view->focus->id == TAGID_SELECT) {
            NRenderSelect* rs = (NRenderSelect*)r;
        rs->modal = 1;
        r->Paint(&view->stack, r, &style, rect);
        rs->modal = 0;
    }
    }
    
//    if (view->modalRender && view->modalRender->type == RNT_SELECT) {
//        NRenderSelect* rs = (NRenderSelect*)view->modalRender;
//        r = view->modalRender;
//        rs->modal = 1;
//        r->Paint(&view->stack, r, &style, rect);
//        rs->modal = 0;
//    }
    
    page->workState = NEWORK_IDLE;
}

void view_paintMainBodyBorder(NView* view, NNode* node, NRect* rect, bd_bool focused)
{
    NPage* page = (NPage*)view->page;
    NRenderNode* rn = (NRenderNode*)node->render;
    NRect pr, cl;
    
    if (rn == N_NULL)
        return;
    
    if (page->workState != NEWORK_IDLE)
	{
        return;
	}
    
    page->workState = NEWORK_PAINT;
    
    cl.l = cl.t = 0;
    cl.r = rect_getWidth(rect);
    cl.b = rect_getHeight(rect);
    NBK_gdi_setClippingRect(page->platform, &cl);

    pr = rn->r;
    rect_toView(&pr, history_getZoom((NHistory*)page->history));
        
    if (rect_isIntersect(rect, &pr)) {
        NColor clr = { 0xff, 0x8c, 0x0, 0xff };
        int i;
        
        rect_move(&pr, pr.l - rect->l, pr.t - rect->t);

        if (focused) {
            NColor color = {0xff, 0x8c, 0x0, 0x7d};
            NBK_gdi_setBrushColor(page->platform, &color);
            NBK_gdi_clearRect(page->platform, &pr);
        }
        
        NBK_gdi_setPenColor(page->platform, &clr);
        for (i=0; i < 3; i++) {
            NBK_gdi_drawRect(page->platform, &pr);
            rect_grow(&pr, 1, 1);
        }
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
    
    page->workState = NEWORK_IDLE;
}

void view_setFocusNode(NView* view, NNode* node)
{
//    view->focus = node;
    page_setFocusedNode((NPage*)view->page, node);
}

void* view_getFocusNode(NView* view)
{
    return view->focus;
}

void view_picChanged(NView* view, int16 imgId)
{
    NPage* page = (NPage*)view->page;
    NRenderNode* r = view->root;
    bd_bool ignore = N_FALSE, layout;
    int lv = 0;
    bd_bool lay[MAX_TREE_DEPTH];
    NRenderNode* img = N_NULL;

    lay[0] = N_FALSE;
    while (r) { // fixme: make change minimized
        
        if (ignore) {
            if (lay[lv]) {
                r->init = 0;
                r->needLayout = 1;
            }
        }
        else {
            layout = N_FALSE;
            if (r->bgiid == imgId) {
                layout = N_TRUE;
            }
            else if (   (r->type == RNT_IMAGE && ((NRenderImage*)r)->iid == imgId)
                     || (r->type == RNT_INPUT && ((NRenderInput*)r)->iid == imgId)) {
                layout = N_TRUE;
                lay[lv] = N_TRUE;
                img = r;
            }
            
            if (layout || lay[lv]) {
                r->init = 0;
                r->needLayout = 1;
            }
        }
        
        if (!ignore && r->child) {
            view->stack.lay[++lv] = r;
            lay[lv] = lay[lv-1];
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == view->root)
                break;
            if (r->next)
                r = r->next;
                else {
                r = view->stack.lay[lv--];
                lay[lv] = lay[lv+1];
                    ignore = N_TRUE;
                }
            }
        }

//    dump_return(page);
//    dump_char(page, "pictures changed ---", -1);
//    view_dump_render_tree_2(view, view->root);
    
    doc_viewChange(page->doc);
}

// update pictures in current visible view
// imgId == -1, check all gifs
void view_picUpdate(NView* view, int16 imgId)
{
    NPage* page = (NPage*)view->page;
    NHistory* history = (NHistory*)page->history;
    NRenderNode* r;
    bd_bool ignore = N_FALSE, has;
    int l = 0;
    NRect vr;
    NRect area;

    NBK_helper_getViewableRect(page->platform, &vr);
    rect_toDoc(&vr, history_getZoom(history));

    if (page->doc->type == NEDOC_FULL) {
        // is in main body mode
        page = history_curr(history);
        if (page->doc->type == NEDOC_MAINBODY && imgId >= 0) {
            view_picChanged(page->view, imgId);
            return;
        }
    }

    r = page->view->root;
    while (r) {
        
        if (!ignore) {
            if (imgId >= 0) {
                if ((r->bgiid == imgId) ||
                    (r->type == RNT_IMAGE && ((NRenderImage*)r)->iid == imgId)) {
                    
                    has = N_FALSE;
                    if (doc_isAbsLayout(page->doc->type)) {
                        area = r->r;
                        has = N_TRUE;
                    }
                    else
                        has = renderNode_getAbsRect(r, &area);
                    
                    if (has && rect_isIntersect(&vr, &area)) {
                        nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
                        return;
                    }
                }
            }
            else {
                if (r->type == RNT_IMAGE &&
                    imagePlayer_isGif(view->imgPlayer, ((NRenderImage*)r)->iid)) {
                    
                    has = N_FALSE;
                    if (doc_isAbsLayout(page->doc->type)) {
                        area = r->r;
                        has = N_TRUE;
                    }
                    else
                        has = renderNode_getAbsRect(r, &area);
                    
                    if (has && rect_isIntersect(&vr, &area)) {
                        nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
                        return;
                    }
                }
            }
        }
        
        if (r->display == CSS_DISPLAY_NONE)
            ignore = N_TRUE;
        
        if (!ignore && r->child) {
            view->stack.lay[++l] = r;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == page->view->root)
                break;
            if (r->next)
                r = r->next;
                else {
                    r = view->stack.lay[l--];
                    ignore = N_TRUE;
                }
            }
        }
    }

void view_enablePic(NView* view, bd_bool enable)
{
    if (view->imgPlayer)
        imagePlayer_disable(view->imgPlayer, !enable);
}

void view_stop(NView* view)
{
    if (view->imgPlayer)
        imagePlayer_stopAll(view->imgPlayer);
}

void rtree_delete_node(NView* view, NRenderNode* node)
{
    NRenderNode* parent;
    NRenderNode* prev;
    NRenderNode* next;
    
    if (node == N_NULL)
        return;
    if (node->type >= RNT_LAST) // fixme
        return;
    
    parent = node->parent;
    prev = node->prev;
    next = node->next;

    if (parent && parent->child == node)
        parent->child = next;
    if (prev)
        prev->next = next;
    if (next)
        next->prev = prev;
    
    rtree_delete_tree(view, node);
}

bd_bool rtree_replace_node(NView* view, NRenderNode* old, NRenderNode* novice)
{
    NRenderNode *parent, *prev, *next;
    
    if (old == N_NULL || novice == N_NULL)
        return N_FALSE;
    
    parent = old->parent;
    prev = old->prev;
    next = old->next;
    
    novice->parent = parent;
    novice->prev = prev;
    novice->next = next;
    
    if (parent->child == old)
        parent->child = novice;
    if (prev)
        prev->next = novice;
    if (next)
        next->prev = novice;
    
    rtree_delete_tree(view, old);
    
    return N_TRUE;
}

bd_bool rtree_insert_node(NRenderNode* node, NRenderNode* novice, bd_bool before)
{
    if (node == N_NULL || novice == N_NULL)
        return N_FALSE;
    
    novice->parent = node->parent;
    
    if (before) {
        novice->prev = node->prev;
        novice->next = node;
        if (node->prev)
            node->prev->next = novice;
        else
            node->parent->child = novice;
        node->prev = novice;
    }
    else {
        novice->prev = node;
        novice->next = node->next;
        if (node->next)
            node->next->prev = novice;
        node->next = novice;
    }
    
    return N_TRUE;
}

bd_bool rtree_insert_node_as_child(NRenderNode* parent, NRenderNode* novice)
{
    if (parent == N_NULL || novice == N_NULL)
        return N_FALSE;
    
    parent->child = novice;
    novice->parent = parent;
    
    return N_TRUE;
}

void view_pause(NView* view)
{
    NPage* page = (NPage*)view->page;
    
    if (page->main) {
        imagePlayer_aniPause(view->imgPlayer);
    }
}

void view_resume(NView* view)
{
    NPage* page = (NPage*)view->page;
    
    if (page->main) {
        imagePlayer_aniResume(view->imgPlayer);
    }
}

void view_nodeDisplayChanged(NView* view, NRenderNode* rn, nid display)
{
    NRenderNode** sta;
    NRenderNode* r;
    int lv, max;
    coord dy;
    
    if (rn->type != RNT_BLOCK || rn->display == display)
        return;
    
    if (display == CSS_DISPLAY_NONE)
        dy = -rect_getHeight(&rn->r);
    else
        dy = rect_getHeight(&rn->r);
    rn->display = display;
    
    sta = view->stack.lay;
    max = 0;
    r = rn;
    sta[max++] = rn;
    while (r->parent) {
        r = r->parent;
        sta[max++] = r;
    }
    
    lv = 0;
    r = rn;
    while (r) {
        if (r != sta[lv]) {
            if (r->type == RNT_TEXT)
                renderText_adjustPos(r, dy);
            else
                renderNode_adjustPos(r, dy);
        }
        
        if (r->next) {
            r = r->next;
        }
        else {
            lv++;
            if (lv == max)
                break;
            r = sta[lv];
            r->r.b += dy;
        }
    }
    
//    view_dump_render_tree_2(view, view->root);
}

static int adjust_y_offset_abs_task(NRenderNode* rn, void* user, bd_bool* ignore)
{
    coord dy = *(coord*)user;
    if (rn->type == RNT_TEXT)
        renderText_adjustPos(rn, dy);
    else
        renderNode_adjustPos(rn, dy);
    return 0;
}

static void view_adjust_y_offset_abs(NRenderNode* root, coord dy)
{
    rtree_traverse_depth(root, adjust_y_offset_abs_task, N_NULL, &dy);
}

void view_nodeDisplayChangedFF(NView* view, NRenderNode* rn, nid display)
{
    NRenderNode** sta;
    NRenderNode* r;
    coord dy;
    int lv, max;
    
    if (rn->type != RNT_BLOCK || rn->display == display)
        return;
    
//    view_dump_render_tree_2(view, view->root);
    
    if (display == CSS_DISPLAY_NONE) {
        // convert to coordinates start from 0,0
        view_adjust_y_offset_abs(rn, -rn->r.t);
        dy = -rect_getHeight(&rn->r);
    }
    else {
        // get y offset from sibling or parent
        r = rn->prev;
        while (r && r->display == CSS_DISPLAY_NONE)
            r = r->prev;
        if (r)
            dy = r->r.b;
        else
            dy = rn->parent->r.t;
        view_adjust_y_offset_abs(rn, dy);
        dy = rect_getHeight(&rn->r);
    }
    rn->display = display;
    
    sta = view->stack.lay;
    r = rn;
    max = -1;
    while (r->parent) {
        r = r->parent;
        sta[++max] = r;
    }
    
    lv = -1;
    r = rn;
    while (r) {
        
        if (r->next) {
            r = r->next;
            if (r->display != CSS_DISPLAY_NONE)
                view_adjust_y_offset_abs(r, dy);
        }
        else {
            if (lv == max)
                break;
            r = sta[++lv];
            r->r.b += dy;
        }
    }
    
//    view_dump_render_tree_2(view, view->root);
}

void view_checkFocusVisible(NView* view)
{
    NNode* r = view->focus;
    NRenderNode* render;
    
    while (r) {
        render = (NRenderNode*)r->render;
        if (render && render->display == CSS_DISPLAY_NONE) {
            view_setFocusNode(view, N_NULL);
            return;
        }
        r = r->parent;
    }
}

#if DUMP_FUNCTIONS
static void dump_zindex_matrix(NZidxMatrix* zidx, NPage* page)
{
    int i, j;
    dump_char(page, "===== z-index martix =====", -1);
    dump_return(page);
    for (i=0; i < MAX_ZIDX_PANEL; i++) {
        dump_int(page, zidx->panels[i].z);
        dump_int(page, zidx->panels[i].use);
        dump_char(page, "|", 1);
        for (j=0; j < MAX_ZIDX_CELL_IN_PANEL; j++) {
            if (zidx->panels[i].cells[j].rn)
                dump_NRect(page, &zidx->panels[i].cells[j].rect);
            else
                dump_char(page, "x", 1);
        }
        dump_return(page);
    }
    dump_flush(page);
}
#endif

static void zindex_matrix_add(NZidxMatrix* zidx, int32 z, NRenderNode* rn, NRect rect)
{
    int i, j;
    
    if (zidx->panels[0].use == 0) {
        zidx->panels[0].z = z;
        zidx->panels[0].use = 1;
        zidx->panels[0].cells[0].rn = rn;
        zidx->panels[0].cells[0].rect = rect;
        return;
    }
    
    for (i=0; i < MAX_ZIDX_PANEL && zidx->panels[i].use; i++) {
        if (zidx->panels[i].z >= z)
            break;
    }
    if (i == MAX_ZIDX_PANEL) // fixme
        return;
    
    if (zidx->panels[i].z > z) {
        for (j = MAX_ZIDX_PANEL-1; j > i; j--) {
            NBK_memcpy(&zidx->panels[j], &zidx->panels[j-1], sizeof(NZidxPanel));
        }
        zidx->panels[i].z = z;
        zidx->panels[i].use = 0;
    }
    
    if (zidx->panels[i].use < MAX_ZIDX_CELL_IN_PANEL - 1) {
        zidx->panels[i].z = z;
        zidx->panels[i].cells[zidx->panels[i].use].rn = rn;
        zidx->panels[i].cells[zidx->panels[i].use].rect = rect;
        zidx->panels[i].use++;
    }
}

static NRect get_max_render_rect(NRenderNode* root, NRenderNode** sta)
{
    NRect rect = root->r;
    NRenderNode* r = root;
    int lv = 0;
    bd_bool ignore = N_FALSE;
    
    while (r) {
        if (!ignore && r->display) {
            if (r->zindex && r != root)
                ignore = N_TRUE;
            else if (!rect_isEmpty(&r->r))
                rect_unite(&rect, &r->r);
        }
        
        if (!ignore && r->child) {
            sta[lv++] = r;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == root)
                break;
            else if (r->next)
                r = r->next;
            else {
                r = sta[--lv];
                ignore = N_TRUE;
            }
        }
    }
    
    return rect;
}

typedef struct _NTaskBuildZindex {
    NZidxMatrix*    zidx;
    NView*          view;
} NTaskBuildZindex;

static int zindex_building(NNode* node, void* user, bd_bool* ignore)
{
    if (node->id == TAGID_DIV) {
        NTaskBuildZindex* task = (NTaskBuildZindex*)user;
        NZidxMatrix* zidx = task->zidx;
        NRenderNode* r = (NRenderNode*)node->render; 
        
        if (r->zindex) {
            NRect rect = get_max_render_rect(r, task->view->stack.lay);
            NStyle style;
            int32 z_index;
//            NCssStyle* cs = attr_getStyle(node->atts);
//            int32 z_index = cs->z_index;
            
            style_init(&style);
            node_calcStyle(task->view, node, &style);
            z_index = style.z_index;
        
            r = r->parent;
            while (r) {
                if (r->type == RNT_BLOCK && r->zindex) {
//                    cs = attr_getStyle(((NNode*)r->node)->atts);
//                    z_index += cs->z_index;
                    
                    style_init(&style);
                    node_calcStyle(task->view, (NNode*)r->node, &style);
                    z_index += style.z_index;
                }
                r = r->parent;
            }
            
            zindex_matrix_add(zidx, z_index, (NRenderNode*)node->render, rect);
        }
    }
    
    return 0;
}

void view_buildZindex(NView* view)
{
    NPage* page = (NPage*)view->page;
    NTaskBuildZindex task;
    
    if (!page->main)
        return;
    
    if (view->zidx == N_NULL)
        view->zidx = (NZidxMatrix*)NBK_malloc0(sizeof(NZidxMatrix));
    else
        NBK_memset(view->zidx, 0, sizeof(NZidxMatrix));
    
    if (page->doc->type == NEDOC_FULL) {
    task.zidx = view->zidx;
    task.view = page->view;
        node_traverse_depth(page->doc->root, zindex_building, N_NULL, &task);
    }
    view->zidx->ready = N_TRUE;
    
//    dump_zindex_matrix(view->zidx, page);
}

bd_bool view_isZindex(NView* view)
{
    return ((view->zidx && view->zidx->ready) ? N_TRUE : N_FALSE);
}

void view_clearZindex(NView* view)
{
    if (view->zidx)
        view->zidx->ready = N_FALSE;
}

void view_paintPartially(NView* view, NRenderNode* root, NRect* rect)
{
    NRenderNode* r;
    NPage* page = (NPage*)view->page;
    bd_bool ignore = N_FALSE;
    NStyle style;
    NFloat zoom;
    
    if (page->workState != NEWORK_IDLE)
        return;
    
    page->workState = NEWORK_PAINT;
    
    zoom = history_getZoom((NHistory*)page->history);
    r = root;
    view->stack.aux[0] = N_NULL;
    view->stack.ld = 0;
    while (r && !r->needLayout) {
        
        if (!ignore) {
            style_init(&style);
            style.view = view;
            style.dv = view_getRectWithOverflow(view);
            style.zoom = zoom;
            if (r->display) {
                if (r->zindex && r != root) // skip sub-trees which own z-index
                    ignore = N_TRUE;
                else if (r->isAd) {
                    renderNode_drawAdPlaceholderFF(r, &style, rect);
                    ignore = N_TRUE;
                }
                else
                    r->Paint(&view->stack, r, &style, rect);
            }
            else if (r->type == RNT_BLOCK)
                ignore = N_TRUE;
        }
        
        if (!ignore && r->child) {
            view->stack.lay[++view->stack.ld] = r;
            view->stack.aux[view->stack.ld] = N_NULL;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == root)
                break;
            else if (r->next)
                r = r->next;
            else {
                r = view->stack.lay[view->stack.ld--];
                ignore = N_TRUE;
            }
        }
    }
    
    page->workState = NEWORK_IDLE;
}

static NNode* view_find_focus_with_render(
    NRenderNode* root, coord x, coord y, NRenderNode** sta, NImagePlayer* player)
{
    NRenderNode* r = root;
    bd_bool ignore = N_FALSE;
    int16 lv = 0;
    NNode* n;
    NNode* focus = N_NULL;
    NNode* emA = N_NULL;
    
    while (r) {
        if (!ignore) {
            if (r->display) {
                if (r->zindex && r != root) {
                    ignore = N_TRUE;
                }
                else if (rect_hasPt(&r->r, x, y)) {
                    switch (r->type) {
                    case RNT_INPUT:
                    case RNT_TEXTAREA:
                    case RNT_SELECT:
                        return (NNode*)r->node;
                    case RNT_A:
                        emA = (NNode*)r->node;
                        if (r->child == N_NULL)
                            return emA;
                        break;
                    case RNT_IMAGE:
                        if (player) {
                            nid state = imagePlayer_getState(player, ((NRenderImage*)r)->iid);
                            if (state == NEIS_PENDING || state == NEIS_HAS_CACHE)
                                return (NNode*)r->node;
                        }
                        if (emA)
                            return emA;
                        break;
                    case RNT_TEXT:
                        if (emA) {
                            int i;
                            NRenderText* rt = (NRenderText*)r;
                            for (i=0; i < rt->use; i++)
                                if (rect_hasPt(&rt->lines[i].r, x, y))
                                    return emA;
                        }
                        break;
                    case RNT_BLOCK:
                    {
                        n = (NNode*)r->node;
                        if (   (n->id == TAGID_DIV && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0)
                            || (((NRenderBlock*)r)->ne_display) )
                            focus = n;
                        break;
                        }
                    default:
                    	break;
                    } // switch
                }
            }
            else
                ignore = N_TRUE;
        }
        
        if (!ignore && r->child) {
            sta[++lv] = r;
            r = r->child;
        }
        else {
            ignore = N_FALSE;
            if (r == root)
                break;
            else if (r->next) {
                if (r->type == RNT_A)
                    emA = N_NULL;
                r = r->next;
            }
            else {
                r = sta[lv--];
                ignore = N_TRUE;
            }
        }
    }
    
    return focus;
}

NNode* view_findFocusWithZindex(NView* view, coord x, coord y)
{
    int i, j;
    NPage* page = (NPage*)view->page;
    NNode* focus = N_NULL;
    NImagePlayer* player = N_NULL;
    
    if (!page->main)
        return N_NULL;
    
    if (   page->doc->type == NEDOC_FULL
        && ((NSettings*)page->settings)->clickToLoadImage )
        player = page->view->imgPlayer;
    
    for (i = MAX_ZIDX_PANEL - 1; i >= 0; i--) {
        if (view->zidx->panels[i].use && view->zidx->panels[i].z > 0) {
            for (j = view->zidx->panels[i].use - 1; j >= 0; j--) {
                if (rect_hasPt(&view->zidx->panels[i].cells[j].rect, x, y)) {
                    return view_find_focus_with_render(
                        view->zidx->panels[i].cells[j].rn, x, y, view->stack.lay, player);
                }
            }
        }
    }
    
    focus = view_find_focus_with_render(view->root, x, y, view->stack.lay, player);
    
    if (focus == N_NULL && page->subPages) {
        NPage* p = sll_first(page->subPages);
        while (p) {
            if (p->attach) {
                if (rect_hasPt(&p->view->root->r, x, y))
                    return view_find_focus_with_render(p->view->root, x, y, view->stack.lay, player);
            }
            p = sll_next(page->subPages);
        }
    }
    
    return focus;
}

void view_paintWithZindex(NView* view, NRect* rect)
{
    int i, j;
    NPage* page = (NPage*)view->page;
    NZidxMatrix* zidx = view->zidx;
    NFloat zoom = history_getZoom((NHistory*)page->history);
    
    if (!page->main)
        return;
    
    for (i=0; i < MAX_ZIDX_PANEL && zidx->panels[i].use && zidx->panels[i].z < 0; i++) {
        for (j=0; j < zidx->panels[i].use; j++) {
            view_paintPartially(view, zidx->panels[i].cells[j].rn, rect);
        }
    }
    
    view_paintPartially(view, view->root, rect);
    if (page->subPages) {
        NPage* p = sll_first(page->subPages);
        while (p) {
            if (p->attach) {
                NRect vr = p->view->frameRect;
                rect_toView(&vr, zoom);
                if (rect_isIntersect(&vr, rect)) {
                    vr.l = rect->l;
                    vr.t = rect->t;
                    vr.r = N_MIN(vr.r, rect->r);
                    vr.b = N_MIN(vr.b, rect->b);
                    if (rect_getWidth(&vr) > 0 && rect_getHeight(&vr) > 0)
                        view_paintPartially(view, p->view->root, &vr);
                }
            }
            p = sll_next(page->subPages);
        }
    }

    for (i=0; i < MAX_ZIDX_PANEL && zidx->panels[i].use; i++) {
        if (zidx->panels[i].z > 0) {
            for (j=0; j < zidx->panels[i].use; j++) {
                view_paintPartially(view, zidx->panels[i].cells[j].rn, rect);
            }
        }
    }
}

void view_paintFocusWithZindex(NView* view, NRect* rect)
{
    NRenderNode* r;
    int32 z_index = 0;
    
    if (view->focus == N_NULL)
        return;
    
    view_paintFocus(view, rect);
    
    // find panel which focus in.
    r = (NRenderNode*)view->focus->render;
    while (r && r->zindex == 0)
        r = r->parent;
    
    if (r) {
        NNode* n = (NNode*)r->node;
//        NCssStyle* style = attr_getStyle(n->atts);
        NStyle style;
        
        style_init(&style);
        node_calcStyle(view, n, &style);
//        z_index = style->z_index;
        z_index = style.z_index;
    }
    
    {
        int i, j;
        for (i=0; i < MAX_ZIDX_PANEL && view->zidx->panels[i].use; i++) {
            if (view->zidx->panels[i].z > z_index) {
                for (j=0; j < view->zidx->panels[i].use; j++) {
                    view_paintPartially(view, view->zidx->panels[i].cells[j].rn, rect);
                }
            }
        }
    }
}

// Feature: Layout text by specify width.
#if 0
typedef struct _TaskBLAdjustRtree {
    NRect*  box;
    coord   dy;
} TaskBLAdjustRtree;

static int bl_adjust_rtree(NRenderNode* render, void* user, BOOL* ignore)
{
    TaskBLAdjustRtree* task = (TaskBLAdjustRtree*)user;
    if (render->r.t > task->box->t) {
//        coord l = N_MAX(render->r.l, task->box->l);
//        coord r = N_MIN(render->r.r, task->box->r);
        if (1/*l < r*/) {
            if (render->type == RNT_TEXT)
                renderText_adjustPos(render, task->dy);
            else
                renderNode_adjustPos(render, task->dy);
        }
    }
    return 0;
}

static void bl_adjust_render_y_offset(NView* view, NRect* box, coord dy)
{
    TaskBLAdjustRtree task;
    task.box = box;
    task.dy = dy;
    rtree_traverse_depth(view->root, bl_adjust_rtree, N_NULL, &task);
}

static void bl_extent_parent_height(NRenderNode* render, coord dy)
{
    NRenderNode* r = render;
    while (r) {
        r->r.b += dy;
        r = r->parent;
    }
}

typedef struct _TaskBLAdjust {
    NPage*  page;
    coord   width;
    int     total;
} TaskBLAdjust;

static int bl_adjust_text(NNode* node, void* user, BOOL* ignore)
{
    BOOL skip = N_FALSE;
    
    if (node->id == TAGID_DIV && attr_getValueInt32(node->atts, ATTID_TC_IA) > 0)
        skip = N_TRUE;
    else if (   node->id == TAGID_A
//             || node->id == TAGID_FRAME
             || node->id == TAGID_FORM
             || node->id == TAGID_SELECT
             || node->id == TAGID_TEXTAREA)
        skip = N_TRUE;
    
    if (skip) {
        *ignore = skip;
    }
    else if (node->id == TAGID_TEXT) {
        TaskBLAdjust* task = (TaskBLAdjust*)user;
        NRenderNode* r = (NRenderNode*)node->render;
        NRect old = r->r;
        NStyle style;
        coord dy;
        
        style.view = task->page->view;
        renderText_breakLineByWidthFF(r, &style, task->width);
        
        dy = r->r.b - old.b;
        if (dy) {
            bl_extent_parent_height(r->parent->parent, dy);
            bl_adjust_render_y_offset(task->page->view, &old, dy);
            task->total++;
//            dump_wchr(task->page, node->d.text, node->len);
//            dump_return(task->page);
//            return 1;
        }
    }
    
    return 0;
}

BOOL view_layoutTextFF(NView* view)
{
    NPage* page = (NPage*)view->page;
    NRect vr;
    TaskBLAdjust task;
    
    NBK_helper_getViewableRect(page->platform, &vr);
    
    task.page = page;
    task.width = rect_getWidth(&vr);
    task.width = task.width - task.width / 10;
    task.total = 0;
    node_traverse_depth(page->doc->root, bl_adjust_text, N_NULL, &task);
    
//    dump_char(page, "adjust text node:", -1);
//    dump_int(page, task.total);
//    dump_return(page);
//    dump_flush(page);
//    
//    view_dump_render_tree_2(view, view->root);
    
    return ((task.total > 0) ? N_TRUE : N_FALSE);
}
#else
bd_bool view_layoutTextFF(NView* view)
{
    return N_FALSE;
}
#endif

NFloat view_getMinZoom(NView* view)
{
    NFloat zoom = {1, 0};
    
#if !NBK_DISABLE_ZOOM
    if (view->scrWidth > 0) {
        NRect vw;
        coord w = view_width(view);
        coord n;
        
        if (w == 0)
            return zoom;
        
        NBK_helper_getViewableRect(((NPage*)view->page)->platform, &vw);
        n = rect_getWidth(&vw) * NFLOAT_EMU / w;
        zoom.i = n / NFLOAT_EMU;
        zoom.f = n % NFLOAT_EMU;
        
//        if (zoom.i == 1 && zoom.f > 0) {
//            zoom.f = (zoom.f < 4000) ? 0 : 4000; // set to 1.4
//        }
        if (zoom.i >= 2) {
            zoom.i = 2;
            zoom.f = 0;
        }
    }
#endif
    
    return zoom;
}
