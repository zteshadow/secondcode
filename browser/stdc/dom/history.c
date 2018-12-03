/*
 * history.c
 *
 *  Created on: 2011-5-8
 *      Author: migr
 */

#include "history.h"
#include "document.h"
#include "view.h"
#include "xml_tags.h"
#include "xml_atts.h"
#include "char_ref.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_callback.h"
#include "../render/imagePlayer.h"
#include "../loader/loader.h"
#include "../loader/url.h"
#include "../tools/str.h"
#include "../css/css_prop.h"
#include "../css/css_value.h"
#include "../css/color.h"

static nid l_page_id = 0;

static int l_history_refer = 0;

static void globe_init(void)
{
    if (l_history_refer == 0) {
        xml_initTags();
        xml_initAtts();
        xml_initEntityTable();
        css_initProp();
        css_initVals();
        css_initColor();
        url_initPhoneSite();
    }
    
    l_history_refer++;
}

static void globe_destory(void)
{
    l_history_refer--;
    
    if (l_history_refer == 0) {
        xml_delTags();
        xml_delAtts();
        xml_delEntityTable();
        css_delProp();
        css_delVals();
        css_delColor();
        url_delPhoneSite();
        url_freeWapWhiteList();
    }
}

NHistory* history_create(void)
{
    NHistory* h = (NHistory*)NBK_malloc0(sizeof(NHistory));
    
    h->list[0].zoom.i = 1;
    h->list[0].zoom.f = 0;
    
    h->cur = -1;
    h->page = page_create(N_NULL);
#if NBK_EXT_MAINBODY_MODE
    h->frag = page_create(N_NULL);
    imagePlayer_delete(&h->frag->view->imgPlayer);
    h->frag->view->imgPlayer = h->page->view->imgPlayer;
    h->frag->view->ownPlayer = 0;
#endif
    
    globe_init();
    
    return h;
}

void history_delete(NHistory** history)
{
    NHistory* h = *history;
    int16 i;

    for (i=0; i <= h->max; i++) {
        NBK_resourceClean(h->page->platform, h->list[i].id);
        if (h->list[i].url)
            NBK_free(h->list[i].url);
    }
    
    if (h->page)
        page_delete(&h->page);
    if (h->frag)
        page_delete(&h->frag);
    
    NBK_free(h);
    *history = N_NULL;
    
    globe_destory();
}

static void history_remember_last_pos(NHistory* history)
{
    if (history->cur != -1) {
        NRect vw;
        NBK_helper_getViewableRect(history->page->platform, &vw);
        history->list[history->cur].lastPos.x = vw.l;
        history->list[history->cur].lastPos.y = vw.t;
    }
}

NPage* history_curr(NHistory* history)
{
    if (history->mbMode) {
        return history->frag;
    }
    
    if (history->cur == -1) {
        history->cur++;
        history->max = history->cur;
        history->list[history->cur].id = ++l_page_id;
        page_setId(history->page, history->list[history->cur].id);
        history->page->history = history;
    }
    
    return history->page;
}

void history_add(NHistory* history)
{
    int16 i;
    NPoint pos = {0, 0};
    
    if (history->preId == 0)
        history->preId = ++l_page_id;
    
    history_remember_last_pos(history);
    
    // skip blank page, maybe a jump.
    if (history->cur >= 0 && history->page->doc->words == 0) {
        NBK_resourceClean(history->page->platform, history->list[history->cur].id);
        if (history->list[history->cur].url) {
            NBK_free(history->list[history->cur].url);
            history->list[history->cur].url = N_NULL;
        }
        
        for (i = history->cur; i < history->max; i++)
            history->list[i] = history->list[i + 1];
        history->cur--;
        history->max--;
    }
    
    if ((history->max < NBK_HISTORY_MAX - 1) ||
        (history->max == NBK_HISTORY_MAX - 1 && history->cur < history->max)) {
        
        // delete records after current
        for (i = history->cur + 1; i <= history->max; i++) {
            NBK_resourceClean(history->page->platform, history->list[i].id);
            history->list[i].id = 0;
            history->list[i].lastPos = pos;
            if (history->list[i].url) {
                NBK_free(history->list[i].url);
                history->list[i].url = N_NULL;
            }
        }
        history->cur++;
    }
    else {
        // shift records left
        NBK_resourceClean(history->page->platform, history->list[0].id);
        if (history->list[0].url) {
            NBK_free(history->list[0].url);
            history->list[0].url = N_NULL;
        }
        for (i=0; i < history->cur; i++) {
            history->list[i] = history->list[i + 1];
        }
    }
    
    // new item
    NBK_memset(&history->list[history->cur], 0, sizeof(NHistoryItem));
    history->list[history->cur].id = history->preId;
    history->list[history->cur].zoom.i = 1;
    
    page_setId(history->page, history->list[history->cur].id);
    history->max = history->cur;
    history->preId = 0;
}

bd_bool history_prev(NHistory* history)
{
    if (history->mbMode) {
        history->mbMode = 0;
        page_setFocusedNode(history->frag, N_NULL);
        view_pause(history->frag->view);
//        page_resetMainBody(history->frag);
        view_resume(history->page->view);
        nbk_cb_call(NBK_EVENT_QUIT_MAINBODY, &history->page->cbEventNotify, N_NULL);
        return N_TRUE;
    }
    
    if (history->cur < 1) {
        return N_FALSE;
    }
    else {
//        page_setFocusedNode(history->page, N_NULL);
        loader_stopAll(history->page->platform, page_getStopId(history->page));
        history_remember_last_pos(history);
        history->cur--;
        page_setId(history->page, history->list[history->cur].id);
        page_loadFromCache(history->page);
        return N_TRUE;
    }
}

bd_bool history_next(NHistory* history)
{
    if (history->cur >= history->max) {
        return N_FALSE;
    }
    else {
//        page_setFocusedNode(history->page, N_NULL);
        loader_stopAll(history->page->platform, page_getStopId(history->page));
        history_remember_last_pos(history);
        history->cur++;
        page_setId(history->page, history->list[history->cur].id);
        page_loadFromCache(history->page);
        return N_TRUE;
    }
}

bd_bool history_goto(NHistory* history, int16 where)
{
    int16 i = where - 1;
    if (i >= 0 && i <= history->max) {
//        page_setFocusedNode(history->page, N_NULL);
        loader_stopAll(history->page->platform, page_getStopId(history->page));
        history->cur = i;
        page_setId(history->page, history->list[history->cur].id);
        page_loadFromCache(history->page);
        return N_TRUE;
    }
    else
        return N_FALSE;
}

nid history_getPreId(NHistory* history)
{
        history->preId = ++l_page_id;
    return history->preId;
}

void history_getRange(NHistory* history, int16* cur, int16* used)
{
    int16 c, u;
    
    c = history->cur;
    u = history->max;
    
    if (history->page->doc->root) {
        c++;
        u++;
    }
    
    *cur = c;
    *used = u;
}

void history_setZoom(NHistory* history, NFloat zoom)
{
    if (history->cur != -1 && !history->mbMode)
        history->list[history->cur].zoom = zoom;
}

NFloat history_getZoom(NHistory* history)
{
    NFloat zoom = {1, 0};
    if (history->mbMode) {
//        NRect vw;
//        coord n;
//        NBK_helper_getViewableRect(history->frag->platform, &vw);
//        n = rect_getWidth(&vw) * NFLOAT_EMU / history->frag->view->scrWidth;
//        zoom.i = n / NFLOAT_EMU;
//        zoom.f = n % NFLOAT_EMU;
        return zoom;
    }
    else {
    if (history->cur != -1)
            return history->list[history->cur].zoom;
        else
            return zoom;
    }
}

void history_setMainBodyWidth(NHistory* history, coord w)
{
    page_setScreenWidth(history->frag, w);
}

void history_enterMainBodyMode(NHistory* history, NNode* body)
{
    if (!page_hasMainBody(history->page))
        return;
    
    if (history->mbMode)
        return;
    
    history->mbMode = 1;
    view_pause(history->page->view);
    page_createMainBody(history->page, body, history->frag);
    nbk_cb_call(NBK_EVENT_ENTER_MAINBODY, &history->page->cbEventNotify, N_NULL);
}

bd_bool history_mainBodyMode(NHistory* history)
{
    return history->mbMode;
}

void history_setPlatformData(NHistory* history, void* platform)
{
    page_setPlatformData(history->page, platform);
    if (history->frag)
        page_setPlatformData(history->frag, platform);
}

void history_setSettings(NHistory* history, NSettings* settings)
{
    history->page->settings = settings;
    if (history->frag)
        history->frag->settings = settings;
}

NNode* history_getMainBodyByPos(NHistory* history, coord x, coord y)
{
    NFloat zoom = history_getZoom(history);
    x = (coord)nfloat_idiv(x, zoom);
    y = (coord)nfloat_idiv(y, zoom);
    return page_getMainBodyByPos(history->page, x, y);
}

void history_enablePic(NHistory* history, bd_bool enable)
{
    if (history->page)
        page_enablePic(history->page, enable);
    if (history->frag)
        page_enablePic(history->frag, enable);
}

void history_quitMainBodyWithClick(NHistory* history)
{
    history->mbMode = 0;
    page_setFocusedNode(history->frag, N_NULL);
    view_pause(history->frag->view);
    nbk_cb_call(NBK_EVENT_QUIT_MAINBODY_AFTER_CLICK, &history->page->cbEventNotify, N_NULL);
}

NPoint history_getLastViewPos(NHistory* history, bd_bool checkUrl)
{
    if (history->cur != -1) {
        // if url of current page is same as last, use the pos from the last.
        if (   checkUrl
            && history->cur > 0
            && history->list[history->cur].url
            && history->list[history->cur - 1].url
            && !NBK_strcmp(history->list[history->cur].url, history->list[history->cur - 1].url) )
            return history->list[history->cur -1].lastPos;
    else
            return history->list[history->cur].lastPos;
    }
    else {
        NPoint pos = {0, 0};
        return pos;
    }
}

bd_bool history_isLastPage(NHistory* history)
{
    return (history->cur == history->max) ? N_TRUE : N_FALSE;
}

void history_setUrl(NHistory* history, const char* url)
{
    if (   !history->mbMode
        && url
        && history->cur >= 0
        && history->cur <= history->max) {
        
        if (history->list[history->cur].url)
            NBK_free(history->list[history->cur].url);
        history->list[history->cur].url = (char*)NBK_malloc(NBK_strlen(url) + 2);
        NBK_strcpy(history->list[history->cur].url, url);
    }
}
