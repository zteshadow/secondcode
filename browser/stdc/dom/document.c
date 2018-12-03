/*
 * document.c
 *
 *  Created on: 2010-12-25
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_limit.h"
#include "../inc/nbk_callback.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_settings.h"
#include "../inc/nbk_ctlPainter.h"
#include "document.h"
#include "xml_tags.h"
#include "xml_atts.h"
#include "page.h"
#include "history.h"
#include "attr.h"
#include "../css/cssSelector.h"
#include "../css/css_val.h"
#include "../loader/url.h"
#include "../loader/loader.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../tools/unicode.h"
#include "../render/imagePlayer.h"
#include "../render/renderNode.h"
#include "../render/renderInput.h"
#include "../render/renderSelect.h"
#include "../render/renderTextarea.h"
#include "../render/renderText.h"
#include "../render/renderBlock.h"

#define DUMP_FUNCTIONS      0
#define DUMP_DOM_TREE       0
#define DUMP_TOKEN          0

#define CREATE_RENDER_TREE  1
#define DISABLE_LAYOUT      0

#define DEBUG_INC_OP        0

#define WIDTH_FOR_MOBILE    360

enum NExtCssState {
    NEXTCSS_NONE,
    NEXTCSS_PENDING,
    NEXTCSS_LOADING,
    NEXTCSS_READY,
    NEXTCSS_ERROR,
    NEXTCSS_CANCEL
};

static void doc_do_layout(void* user);
static void extCssMgr_schedule(NExtCssMgr* mgr);

#ifdef NBK_MEM_TEST

typedef struct _NDocMemChecker {
    int     nodeTotal;
    int     nodeSize;
    int     imgTotal;
    int     textTotal;
    int     textSize;
    int     inlineStyleTotal;
    int     inlineStyleSize;
    int     renderTotal;
    int     renderSize;
} NDocMemChecker;

int doc_mem_used_checker(NNode* node, void* user, bd_bool* ignore)
{
    NDocMemChecker* chk = (NDocMemChecker*)user;
    int txtLen, styLen;
    
    chk->nodeTotal++;
    chk->nodeSize += node_memUsed(node, &txtLen, &styLen);
    
    if (node->id == TAGID_IMG)
        chk->imgTotal++;
    
    if (node->id == TAGID_TEXT) {
        chk->textTotal++;
        chk->textSize += txtLen;
    }
    
    if (styLen > 0) {
        chk->inlineStyleTotal++;
        chk->inlineStyleSize += styLen;
    }
    
    if (node->render) {
        chk->renderTotal++;
        chk->renderSize += renderNode_memUsed((NRenderNode*)node->render);
    }
    
    return 0;
}

int doc_memUsed(const NDocument* doc)
{
    int size = 0;
    if (doc) {
        size = sizeof(NDocument);
        size += xmltkn_memUsed(doc->tokenizer);
        
        if (doc->root) {
            NDocMemChecker chk;
            NPage* page = (NPage*)doc->page;
            
            NBK_memset(&chk, 0, sizeof(NDocMemChecker));
            node_traverse_depth(doc->root, doc_mem_used_checker, N_NULL, &chk);
            
            dump_char(page, "node count:", -1);
            dump_int(page, chk.nodeTotal);
            dump_return(page);
            
            dump_char(page, "text count:", -1);
            dump_int(page, chk.textTotal);
            dump_return(page);
            
            dump_char(page, "text total size:", -1);
            dump_int(page, chk.textSize);
            dump_return(page);
            
            dump_char(page, "image count:", -1);
            dump_int(page, chk.imgTotal);
            dump_return(page);
            
            dump_char(page, "inline style count:", -1);
            dump_int(page, chk.inlineStyleTotal);
            dump_return(page);
            
            dump_char(page, "inline style total size:", -1);
            dump_int(page, chk.inlineStyleSize);
            dump_return(page);
            
            dump_char(page, "dom total size:", -1);
            dump_int(page, chk.nodeSize);
            dump_return(page);

            dump_char(page, "render count:", -1);
            dump_int(page, chk.renderTotal);
            dump_return(page);
            
            dump_char(page, "rtree total size:", -1);
            dump_int(page, chk.renderSize);
            dump_return(page);
            
            size += chk.nodeSize + chk.renderSize;
            
            if (doc->ownStrPool) {
                int s = strPool_memUsed(doc->spool);
                
                dump_char(page, "string total size:", -1);
                dump_int(page, s);
                dump_return(page);
                
                size += s;
            }
        }
    }
    return size;
}
#endif

static NExtCssMgr* extCssMgr_create(void* doc)
{
    NExtCssMgr* mgr = (NExtCssMgr*)NBK_malloc0(sizeof(NExtCssMgr));
    N_ASSERT(mgr);
    mgr->cur = -1;
    mgr->doc = doc;
    return mgr;
}

static void extCssMgr_delete(NExtCssMgr** mgr)
{
    NExtCssMgr* m = *mgr;
    int i;
    
    if (m->timer) {
        tim_stop(m->timer);
        tim_delete(&m->timer);
    }
    
    if (m->cached)
        strBuf_delete(&m->cached);
    
    for (i=0; i < MAX_EXT_CSS_NUM; i++) {
        if (m->d[i].url)
            NBK_free(m->d[i].url);
        if (m->d[i].dat)
            strBuf_delete(&m->d[i].dat);
    }
    
    NBK_free(m);
    *mgr = N_NULL;
}

static bd_bool extCssMgr_addExtCss(NExtCssMgr* mgr, char* url)
{
    int i;
    
    for (i=0; i < MAX_EXT_CSS_NUM; i++)
        if (mgr->d[i].url && NBK_strcmp(mgr->d[i].url, url) == 0)
            return N_FALSE;
    
    for (i=0; i < MAX_EXT_CSS_NUM; i++)
        if (mgr->d[i].url == N_NULL) {
            mgr->d[i].url = url;
            mgr->d[i].state = NEXTCSS_PENDING;
            mgr->ready = N_FALSE;
            return N_TRUE;
        }
    
    return N_FALSE;
}

static void extCssMgr_addInnCss(NExtCssMgr* mgr, const char* style, int length)
{
    if (mgr->cached == N_NULL)
        mgr->cached = strBuf_create();
    
    strBuf_appendStr(mgr->cached, style, length);
}

static bd_bool extCssMgr_get(NExtCssMgr* mgr, char** style, int* len, bd_bool begin)
{
    mgr->use = (begin) ? 0 : mgr->use + 1;
    
    while (mgr->use < MAX_EXT_CSS_NUM) {
        if (mgr->d[mgr->use].state == NEXTCSS_READY) {
            if (strBuf_getStr(mgr->d[mgr->use].dat, style, len, N_TRUE))
                return N_TRUE;
        }
        mgr->use++;
    }
    
    if (mgr->use == MAX_EXT_CSS_NUM && mgr->cached) {
        if (strBuf_getStr(mgr->cached, style, len, N_TRUE))
            return N_TRUE;
    }
    
    return N_FALSE;
}

static void extCssMgr_onResponse(NEResponseType type, void* user, void* data, int v1, int v2)
{
    NRequest* req = (NRequest*)user;
    NExtCssMgr* mgr = (NExtCssMgr*)req->user;
    
    if (mgr->cur < 0 || mgr->cur >= MAX_EXT_CSS_NUM)
        return;

    switch(type) {
    case NEREST_HEADER:
    {
        NRespHeader* hdr = (NRespHeader*)data;
        if (hdr->mime == NEMIME_DOC_CSS) {
            if (mgr->d[mgr->cur].dat)
                strBuf_delete(&mgr->d[mgr->cur].dat);
            mgr->d[mgr->cur].dat = strBuf_create();
        }
        else
            mgr->d[mgr->cur].state = NEXTCSS_ERROR;
        break;
    }
        
    case NEREST_DATA:
    {
        if (mgr->d[mgr->cur].state == NEXTCSS_LOADING && mgr->d[mgr->cur].dat)
            strBuf_appendStr(mgr->d[mgr->cur].dat, (char*)data, v1);
        break;
    }
        
    case NEREST_COMPLETE:
    {
        if (mgr->d[mgr->cur].state == NEXTCSS_LOADING)
        mgr->d[mgr->cur].state = NEXTCSS_READY;
        extCssMgr_schedule(mgr);
        break;
    }
        
    case NEREST_ERROR:
    case NEREST_CANCEL:
    {
        mgr->d[mgr->cur].state = NEXTCSS_ERROR;
        extCssMgr_schedule(mgr);
        break;
    }
    default:
        break;
    } // switch
}

static void extCssMgr_doWork(void* user)
{
    NExtCssMgr* mgr = (NExtCssMgr*)user;
    NDocument* doc = (NDocument*)mgr->doc;
    NPage* page = (NPage*)doc->page;
    int i;
    
    tim_stop(mgr->timer);
    
    for (i=0; i < MAX_EXT_CSS_NUM; i++)
        if (mgr->d[i].state == NEXTCSS_LOADING)
            return;

    for (i=0; i < MAX_EXT_CSS_NUM; i++) {
        if (mgr->d[i].url && mgr->d[i].state == NEXTCSS_PENDING) {
            NRequest* req = loader_createRequest();
            req->pageId = page->id;
            loader_setRequest(req, NEREQT_CSS, mgr->d[i].url, mgr);
            loader_setRequestReferer(req, doc_getUrl(doc), N_FALSE);
            req->responseCallback = extCssMgr_onResponse;
            req->platform = page->platform;
            if (!history_isLastPage((NHistory*)page->history))
                req->method = NEREM_HISTORY;
            
            if (loader_load(req)) {
                mgr->d[i].state = NEXTCSS_LOADING;
                mgr->cur = i;
                return;
            }
            else {
                loader_deleteRequest(&req);
                mgr->d[i].state = NEXTCSS_ERROR;
            }
        }
    }
    
    if (mgr->cached)
        strBuf_joinAllStr(mgr->cached);
    for (i=0; i < MAX_EXT_CSS_NUM; i++) {
        if (mgr->d[i].state == NEXTCSS_READY)
            strBuf_joinAllStr(mgr->d[i].dat);
    }
    mgr->ready = N_TRUE;
    doc_do_layout(doc);
}

static void extCssMgr_schedule(NExtCssMgr* mgr)
{
    if (mgr->timer == N_NULL)
        mgr->timer = tim_create(extCssMgr_doWork, mgr);
    
    tim_stop(mgr->timer);
    tim_start(mgr->timer, 5, 5000000);
}

static bd_bool extCssMgr_ready(NExtCssMgr* mgr)
{
    return mgr->ready;
}

static void delete_loadCmd(NTimer* timer)
{
    if (timer->user) {
        NLoadCmd* cmd = (NLoadCmd*)timer->user;
        
        if (cmd->url)
            NBK_free(cmd->url);
        if (cmd->upcmd)
            upcmd_delete(&cmd->upcmd);
        if (cmd->files)
            NBK_free(cmd->files);
        
        NBK_free(cmd);
        timer->user = N_NULL;
    }
}

static char* get_onevent_go_url(NNode* card, NNode** sta, char* typeStr)
{
    char* url = N_NULL;
    nid tags[] = {TAGID_ONEVENT, 0};
    NSList* lst = node_getListByTag(card, tags, sta);
    NNode* n = sll_first(lst);
    while (n) {
        char* type = attr_getValueStr(n->atts, ATTID_TYPE);
        if (type && NBK_strcmp(type, typeStr) == 0)
            break;
        n = sll_next(lst);
    }
    sll_delete(&lst);
    if (n) {
        if ((n = node_getByTag(n, TAGID_GO, sta)) != N_NULL) {
            url = attr_getValueStr(n->atts, ATTID_HREF);
        }
    }
    return url;
}

static bd_bool doc_do_redirect(NDocument* doc)
{
    NPage* page = (NPage*)doc->page;
    
    if (doc->delayLoad > 0) {
        int delay = doc->delayLoad;
        
        doc->delayLoad = -1;
        
        if (history_isLastPage((NHistory*)page->history)) {
            // must be the last in history
            char* base = doc_getUrl(doc);
            char* url = N_NULL;
            bd_bool jump = N_FALSE;

            if (doc->delayLoadType == NEREDIR_META_REFRESH) {
                url = doc_getUrl(doc);
                jump = N_TRUE;
            }
            else if (   doc->delayLoadType == NEREDIR_WML_ONTIMER
                     || doc->delayLoadType == NEREDIR_WML_ONEVENT) {
                NNode* card = node_getByTag(doc->root, TAGID_CARD, page->view->path);
                if (card) {
                    if (doc->delayLoadType == NEREDIR_WML_ONEVENT) {
                        url = get_onevent_go_url(card, page->view->path, "onenterforward");
                    }
                    else {
                        url = attr_getValueStr(card->atts, ATTID_ONTIMER);
                        if (url == N_NULL)
                            url = get_onevent_go_url(card, page->view->path, "ontimer");
                    }
                }
                jump = (base == N_NULL || NBK_strcmp(base, url)) ? N_TRUE : N_FALSE;
            }
            if (url && jump) {
                NLoadCmd* cmd = NBK_malloc0(sizeof(NLoadCmd));
                cmd->url = url_parse(base, url);
                cmd->doc = doc;
                tim_stop(doc->loadTimer);
                delete_loadCmd(doc->loadTimer);
                doc->loadTimer->user = cmd;
                tim_start(doc->loadTimer, delay, 5000000);
                return N_TRUE;
            }
        }
    }
    
    return N_FALSE;
}

static void doc_do_layout(void* user)
{
    NDocument* doc = (NDocument*)user;
    NPage* page = (NPage*)doc->page;
    bd_bool force = N_FALSE;
    
    tim_stop(doc->layoutTimer);
    
    if (doc->stop)
        return;
    if (!page->attach)
        return;

//wuli	bd_lock(&mutex);
    
    if (doc->extCssMgr && extCssMgr_ready(doc->extCssMgr)) {
        bd_bool begin = N_TRUE;
        char* data = N_NULL;
        int len = 0, offset;
        uint8* p;
        while (extCssMgr_get(doc->extCssMgr, &data, &len, begin)) {
            begin = N_FALSE;
            p = (uint8*)data;
            offset = (*p == 0xef && *(p+1) == 0xbb && *(p+2) == 0xbf) ? 3 : 0;
            sheet_parseStyle(page->sheet, data + offset, len - offset);
        }
        extCssMgr_delete(&doc->extCssMgr);
        if (begin)
            return;
        force = N_TRUE;
//        sheet_dump(page->sheet);
    }
    
#if DISABLE_LAYOUT
    view_dump_render_tree_2(page->view, page->view->root);
#else
    {
//        int t_start = NBK_currentMilliSeconds();
    view_layout(page->view, page->view->root, force);
        if (doc->finish && doc_isAbsLayout(doc->type)) {
            NPage* mainPage = page_getMainPage(page);
//            dump_char(mainPage, "layout finish: ", -1);
//            dump_char(mainPage, doc_getUrl(page->doc), -1);
//            dump_return(mainPage);
            view_buildZindex(mainPage->view);
        }
//    dump_char(page, "layout", -1);
//        dump_int(page, NBK_currentMilliSeconds() - t_start);
//    dump_return(page);
    }
#endif
    
//wuli	bd_unlock(&mutex);

    if (doc->finish) {
        NBK_Event_RePosition evt;
        
//        doc_outputXml(doc);
//        if (page->main)
//            view_dump_render_tree_2(page->view, page->view->root);
        
#if NBK_EXT_SHORTCUT_MENU
        if (page->main && doc->type != NEDOC_MAINBODY)
            doc_buildShortcutLst(doc);
#endif
        
        doc->layoutFinish = 1;
        NBK_memset(&evt, 0, sizeof(NBK_Event_RePosition));
        if (page->main) {
            if (doc->picUpdate) {
            NRect vw;
                NBK_helper_getViewableRect(page->platform, &vw);
            evt.x = vw.l;
            evt.y = vw.t;
        }
            else {
                NPoint pos = history_getLastViewPos((NHistory*)page->history, doc_isAbsLayout(doc->type));
                evt.x = pos.x;
                evt.y = pos.y;
            }
            evt.zoom = history_getZoom((NHistory*)page->history);
        }
        nbk_cb_call(NBK_EVENT_LAYOUT_FINISH, &page->cbEventNotify, &evt);
        
        if (doc_do_redirect(doc))
                    return;
        
        if (doc->picUpdate) {
            doc->picUpdate = 0;
        }
        else {
            bd_bool loadImage = N_TRUE;
            NPage* mainPage = page_getMainPage(page);
            
//            dump_char(mainPage, "doc: layout finish", -1);
//            dump_time(mainPage);
//            dump_char(mainPage, doc_getUrl(doc), -1);
//            dump_return(mainPage);
            
            if (doc_isAbsLayout(doc->type)) {
                if (!page_isLayoutFinish(mainPage))
                    loadImage = N_FALSE;

                if (!page->main)
                    nbk_cb_call(NBK_EVENT_UPDATE_PIC, &mainPage->cbEventNotify, N_NULL);
            }
            
            if (loadImage && !doc->disable) {
                if (imagePlayer_total(mainPage->view->imgPlayer) > 0) {
//                    dump_char(mainPage, "doc: start download images", -1);
//                    dump_time(mainPage);
//                    dump_return(mainPage);
                    imagePlayer_start(mainPage->view->imgPlayer);
                }
                imagePlayer_progress(mainPage->view->imgPlayer);
            }
        }
    }
    else if (N_TRUE/*!doc->firstShow*/) {
        nbk_cb_call(NBK_EVENT_UPDATE, &page->cbEventNotify, N_NULL);
    }
    doc->firstShow = 1;
}

static void doc_schedule_layout(NDocument* doc)
{
    tim_stop(doc->layoutTimer);
    if (!doc->syncMode)
        tim_start(doc->layoutTimer, 10, 5000000);
}

static void doc_do_load(void* user)
{
    NLoadCmd* cmd = (NLoadCmd*)user;
    NPage* page = (NPage*)cmd->doc->page;
    
    tim_stop(cmd->doc->loadTimer);
    
    if (cmd->upcmd) {
        // Post with private protocol
        page_loadUrl_ff(page, &cmd->upcmd, N_NULL, &cmd->files, cmd->via, cmd->noDeal);
    }
    else if (cmd->body) {
        // Post with standard protocol
        page_loadUrl(page, cmd->url, cmd->enc, cmd->body, &cmd->files, cmd->submit);
    }
    else {
        page_loadUrl(page, cmd->url, 0, N_NULL, N_NULL, cmd->submit);
    }
    
    delete_loadCmd(cmd->doc->loadTimer);
}

static void doc_delete_keywords(NDocument* doc)
{
    if (doc->ssKeywords) {
        NBK_free(doc->ssKeywords);
        doc->ssKeywords = N_NULL;
        NBK_free(doc->ssKeyList);
        doc->ssKeyList = N_NULL;
    }
}

NDocument* doc_create(void* page)
{
//    NPage* p = (NPage*)page;
    NDocument* doc = NBK_malloc0(sizeof(NDocument));
    N_ASSERT(doc);
    
    doc->page = page;
    doc->tokenizer = xmltkn_create();
    doc->tokenizer->doc = doc;
    doc->spool = strPool_create();
    doc->ownStrPool = N_TRUE;
    doc->layoutTimer = tim_create(doc_do_layout, doc);
    doc->loadTimer = tim_create(doc_do_load, N_NULL);
//    if (p->main)
//        doc->nodeAlloc = memAlloc_create(sizeof(NNode), 100);
//    else
//        doc->nodeAlloc = p->parent->doc->nodeAlloc;
    return doc;
}

void doc_delete(NDocument** doc)
{
    NDocument* d = *doc;
    NPage* p = (NPage*)d->page;
    
    if (d->shortcutLst)
        sll_delete(&d->shortcutLst);
    if (d->mbList)
        sll_delete(&d->mbList);
    
    doc_clear(d);

    if (d->stack.dat) {
        NBK_free(d->stack.dat);
        d->stack.dat = N_NULL;
    }
    
    if (d->url) {
        NBK_free(d->url);
        d->url = N_NULL;
    }
    
    if (d->title) {
        NBK_free(d->title);
        d->title = N_NULL;
    }
    
    if (d->extCssMgr)
        extCssMgr_delete(&d->extCssMgr);
    
	delete_loadCmd(d->loadTimer);
    tim_delete(&d->loadTimer);
    tim_delete(&d->layoutTimer);
    if (d->ownStrPool && d->spool)
        strPool_delete(&d->spool);
    
    if (p->main && d->nodeAlloc)
        memAlloc_delete(&d->nodeAlloc);
    
    xmltkn_delete(&d->tokenizer);
    
    doc_detachWbxmlDecoder(d);
    
    doc_delete_keywords(d);
    
    NBK_free(d);
    *doc = N_NULL;
}

void doc_begin(NDocument* doc)
{
    NBK_Page* page = (NBK_Page*)doc->page;
        
    if (doc->stack.dat == N_NULL) {
        // allocate once
        doc->stack.dat = (NNode**)NBK_malloc0(sizeof(NNode*) * MAX_TREE_DEPTH);
        N_ASSERT(doc->stack.dat);
    }
    
    doc_clear(doc);
    
    doc->type = NEDOC_UNKNOWN;
    doc->stack.top = -1;
    doc->lastChild = N_NULL;
    doc->finish = 0;
    doc->layoutFinish = 0;
    doc->inTitle = N_FALSE;
    doc->inStyle = N_FALSE;
    doc->inScript = N_FALSE;
    doc->inCtrl = N_FALSE;
    doc->inTable = N_FALSE;
    doc->words = 0;
    doc->wordProcessed = 0;
    doc->wordCount = 500;
    doc->firstShow = 0;
    doc->picUpdate = 0;
    doc->hasSpace = N_FALSE;
    doc->stop = 0;
    doc->syncMode = 0;
    doc->delayLoad = -1;
    doc->fix = N_NULL;
    doc->delayLoadType = NEREDIR_NONE;
    doc->encoding = NEENC_UNKNOWN;
    doc->gbRest = 0;
    doc->disable = 0;
    
    doc->sentMouseMove = 0;
    
    doc->lastGotFocus = N_NULL;
    
    xmltkn_reset(doc->tokenizer);
    if (doc->ownStrPool && doc->spool)
        strPool_reset(doc->spool);
    tim_stop(doc->layoutTimer);
    tim_stop(doc->loadTimer);
    
    doc_detachWbxmlDecoder(doc);
    
    if (doc->url)
        doc->url[0] = 0;
    
    if (doc->title) {
        NBK_free(doc->title);
        doc->title = N_NULL;
    }
    
    if (doc->extCssMgr)
        extCssMgr_delete(&doc->extCssMgr);
    
    if (doc->shortcutLst)
        sll_delete(&doc->shortcutLst);
    if (doc->mbList)
        sll_delete(&doc->mbList);
    
    view_begin(page->view);
    page->workState = NEWORK_IDLE;
}

static void node_delete(NDocument* doc, NNode** node)
{
    NNode* t = *node;

    if (t->ownAtts && t->atts)
        attr_delete(&t->atts);
    
    if (t->ownText && t->d.text)
        NBK_free(t->d.text);
    
	if (doc->nodeAlloc)
		memAlloc_free(doc->nodeAlloc, t);
	else
	{
		NBK_free(t);
	}
    //(doc->nodeAlloc) ? memAlloc_free(doc->nodeAlloc, t) : NBK_free(t);
    *node = N_NULL;
}

static void dom_delete_tree(NDocument* doc, NNode* root)
{
    int16 top = -1;
    NNode* n = root;
    NNode* t = N_NULL;
    bd_bool ignore = 0;
    
    while (n) {
        
        if (doc_isAbsLayout(doc->type) && n->id == TAGID_FRAME) {
            ignore = 1;
        }
        
        if (!ignore && n->child) {
            doc->stack.dat[++top] = n;
            n = n->child;
        }
        else {
            if (n == root)
                break;
            ignore = 0;
            t = n;
            if (n->next)
                n = n->next;
                else {
                    n = doc->stack.dat[top--];
                    n->child = N_NULL;
                }

            node_delete(doc, &t);
        }
    }
    if (n)
        node_delete(doc, &n);
}

//#define DOC_TEST_RELEASE
void doc_clear(NDocument* doc)
{
#ifdef DOC_TEST_RELEASE
    double t_consume = NBK_currentMilliSeconds();
#endif
    
    dom_delete_tree(doc, doc->root);
    doc->root = N_NULL;
    doc->lastGotFocus = N_NULL;
    
#ifdef DOC_TEST_RELEASE
    dump_char(doc->page, "doc clear", -1);
    dump_int(doc->page, (int)(NBK_currentMilliSeconds() - t_consume));
    dump_return(doc->page);
#endif
}

#if DUMP_FUNCTIONS
static void dump_dom_tree(NPage* page, NNode* root)
{
    NNode** sta = (NNode**)NBK_malloc(sizeof(NNode*) * MAX_TREE_DEPTH);
    char* level = (char*)NBK_malloc(512);
    int16 l = -1;
    NNode* n = root;
    bd_bool ignore = N_FALSE;
    int i, j, k;
    
    dump_return(page);
    dump_char(page, "===== dom tree =====", -1);
    dump_return(page);
    
    while (n) {
        if (!ignore) {
            sta[l+1] = n;
            for (i=1, j=0; i <= l+1; i++) {
                
                if (sta[i]->next)
                    level[j++] = '|';
                else {
                    if (sta[i] == n)
                        level[j++] = '`';
                    else
                        level[j++] = ' ';
                }
                
                if (sta[i] == n) {
                    level[j++] = '-';
                    level[j++] = '-';
                }
                else {
                    level[j++] = ' ';
                    level[j++] = ' ';
                }
                
                level[j++] = ' ';
            }
            NBK_strcpy(&level[j], xml_getTagNames()[n->id]);
            
            j += NBK_strlen(xml_getTagNames()[n->id]);
            for (k=0; n->atts && n->atts[k].id; k++) {
                
                if (   n->atts[k].id == ATTID_ID
                    || n->atts[k].id == ATTID_CLASS
                    || n->atts[k].id == ATTID_TYPE
                    || n->atts[k].id == ATTID_NAME
                    || n->atts[k].id == ATTID_VALUE
                    || n->atts[k].id == ATTID_TC_NAME) {
                    
                    level[j++] = ' ';
                    NBK_strcpy(&level[j], xml_getAttNames()[n->atts[k].id]);
                    j += NBK_strlen(xml_getAttNames()[n->atts[k].id]);
                    level[j++] = '=';
                    NBK_strcpy(&level[j], n->atts[k].d.ps);
                    j += NBK_strlen(n->atts[k].d.ps);
                }
                else if (n->atts[k].id == ATTID_SELECTED) {
                    level[j++] = ' ';
                    NBK_strcpy(&level[j], xml_getAttNames()[n->atts[k].id]);
                    j += NBK_strlen(xml_getAttNames()[n->atts[k].id]);
                }
            }
            
            dump_char(page, level, -1);
            
//            if (attr_getRect(n->atts, ATTID_TC_RECT))
//                dump_NRect(page, attr_getRect(n->atts, ATTID_TC_RECT));
            
            if (n->id == TAGID_TEXT)
                dump_wchr(page, n->d.text, n->len);
            
            dump_return(page);
        }
        
        if (!ignore && n->child) {
            sta[++l] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == root)
                break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[l--];
                    ignore = N_TRUE;
                }
            }
        }
    
    NBK_free(level);
    NBK_free(sta);
    
    dump_flush(page);
}
#endif

void doc_dumpDOM(NDocument* doc)
{
#if DUMP_FUNCTIONS
    NPage* page = (NPage*)doc->page;
    dump_dom_tree(page, doc->root);
#endif
}

void doc_end(NDocument* doc)
{
    if (doc->finish)
        return;
    
    if (doc->wbxmlDecoder) {
        NWbxmlDecoder* decoder = doc->wbxmlDecoder;
        doc->wbxmlDecoder = N_NULL;
        if (wbxmlDecoder_decode(decoder)) {
            doc_write(doc, (char*)decoder->dst, decoder->dstPos, N_TRUE);
        }
        wbxmlDecoder_delete(&decoder);
    }
    
#if DUMP_DOM_TREE
    dump_dom_tree((NPage*)doc->page, doc->root);
#endif
    
    doc->finish = N_TRUE;
    xmltkn_abort(doc->tokenizer);
    doc_schedule_layout(doc);
}

void doc_stop(NDocument* doc)
{
    doc->stop = 1;
    
    if (doc->extCssMgr) {
        NExtCssMgr* mgr = doc->extCssMgr;
        int i;
        for (i=0; i < MAX_EXT_CSS_NUM; i++) {
            if (mgr->d[i].url && mgr->d[i].state == NEXTCSS_PENDING)
                mgr->d[i].state = NEXTCSS_CANCEL;
        }
    }
}

static NEEncoding check_encoding(const char* str, int length)
{
    NEEncoding enc = NEENC_UTF8;
    int scan = 300;
    int len = (length > scan) ? scan : length;
    
    if (nbk_strnfind_nocase(str, "encoding=\"gb2312\"", len) != -1)
        enc = NEENC_GB2312;
    else if(nbk_strnfind_nocase(str,"charset=\"gb2312\"",len) != -1)
        enc = NEENC_GB2312;
    
    return enc;
}

static char* convert_gb_to_utf8(NDocument* doc, char* data, int len, int* mbLen)
{
    char* mb = data;
    uint8* toofar = (uint8*)data + len;
    uint8* p;
    int mbl = 0, wcl = 0;
    bd_bool free = N_FALSE;
    wchr* unicode = N_NULL;
    
    if (doc->gbRest) {
        mb = (char*)NBK_malloc(len + 8);
        *mb = doc->gbRest; doc->gbRest = 0;
        NBK_strncpy(mb + 1, data, len);
        free = N_TRUE;
        toofar++;
    }
    
    p = (uint8*)mb;
    while (p < toofar) {
        if (*p <= 0x80) {
            mbl++;
            wcl++;
            p++;
        }
        else if (p + 1 < toofar) {
            mbl += 2;
            wcl++;
            p += 2;
        }
        else {
            doc->gbRest = *p;
            break;
        }
    }
    
    unicode = (wchr*)NBK_malloc(sizeof(wchr) * (wcl + 4));
    
    wcl = NBK_conv_gbk2unicode(mb, mbl, unicode, wcl);
    
    if (free)
        NBK_free(mb);
    
    mb = uni_utf16_to_utf8_str(unicode, wcl, mbLen);
    
    NBK_free(unicode);
    
    return mb;
}

void doc_write(NDocument* doc, const char* str, int length, bd_bool final)
{
    NPage* page = (NPage*)doc->page;
    char* data = (char*)str;
    int len = length;
    
    N_ASSERT(doc);
    
    if (doc->finish)
        return;
    
    if (NBK_freemem() < length * 4)
        return;
    
    if (doc->wbxmlDecoder) {
        wbxmlDecoder_addSourceData(doc->wbxmlDecoder, (uint8*)str, length);
        return;
    }
    
    if (doc->encoding == NEENC_UNKNOWN)
        doc->encoding = check_encoding(str, length);
    
//wuli	bd_lock(&mutex);

    if (page->workState != NEWORK_IDLE)
	{
        return;
	}
    
    if (doc->encoding == NEENC_GB2312)
        data = convert_gb_to_utf8(doc, (char*)str, length, &len);
    
    page->workState = NEWORK_PARSE;
    xmltkn_write(doc->tokenizer, data, len, final);
    page->workState = NEWORK_IDLE;

//wuli	bd_unlock(&mutex);
    
    if (doc->encoding == NEENC_GB2312)
        NBK_free(data);
    
//    dump_char(page, "write", -1);
//    dump_return(page);
    if (   page->main
        && !final
        && !doc->disable
        && !doc->finish
        && doc->words - doc->wordProcessed >= doc->wordCount) {
//        dump_int(page, doc->words);
//        dump_return(page);
        doc_schedule_layout(doc);
        doc->wordProcessed = doc->words;
        doc->wordCount *= 2;
    }
}

static NNode* node_create(NToken* token, NDocument* doc)
{
    NNode* node = (doc->nodeAlloc) \
        ? (NNode*)memAlloc_alloc(doc->nodeAlloc, sizeof(NNode)) \
        : (NNode*)NBK_malloc0(sizeof(NNode));
    N_ASSERT(node);
    
    node->id = token->id;
    node->space = token->space;
    
    if (doc->type == NEDOC_MAINBODY) {
        node->ownAtts = 1;
        node->ownText = 0;
    }
    else {
        node->ownAtts = 1;
        node->ownText = 1;
    }
    
    if (token->id == TAGID_TEXT) {
        node->d.text = token->d.text;
        token->d.text = N_NULL;
        node->len = token->len;
    }
    
    node->atts = token->atts;
    token->atts = N_NULL;
    
    node->nodeRef = (NNode*)token->nodeRef;
    
    switch (node->id) {
    case TAGID_A:
        node->Click = nodeA_click;
        break;
    case TAGID_BUTTON:
    case TAGID_INPUT:
        node->Click = nodeInput_click;
        break;
    case TAGID_ANCHOR:
        node->Click = nodeAnchor_click;
        break;
    case TAGID_SELECT:
        node->Click = nodeSelect_click;
        break;
    case TAGID_TEXTAREA:
        node->Click = nodeTextarea_click;
        break;
    default:
        if (doc_isAbsLayout(doc->type) || doc->type == NEDOC_DXXML) {
            switch (node->id) {
            case TAGID_DIV:
            case TAGID_SPAN:
            case TAGID_LI:
            case TAGID_H1:
            case TAGID_H2:
            case TAGID_H3:
            case TAGID_H4:
            case TAGID_H5:
            case TAGID_H6:
                node->Click = nodeDiv_click;
                break;
            case TAGID_IMG:
                node->Click = nodeImg_click;
                break;
            case TAGID_TC_ATTACHMENT:
                node->Click = nodeAttachment_click;
                break;
            }
        }
        break;
    }
    
    return node;
}

static NNode* dom_insert_as_child(NNode** parent, NNode* lastChild, NNode* node)
{
    if (*parent == N_NULL) {
        *parent = node;
    }
    else {
        if (lastChild == N_NULL) {
            // first child
            (*parent)->child = node;
            node->parent = *parent;
        }
        else {
            lastChild->next = node;
            node->parent = *parent;
            node->prev = lastChild;
        }
    }
    
    return node;
}

#if DUMP_FUNCTIONS
static void dom_dump_level(const NDocument* doc, const NNode* node)
{
    char level[64];
    char ch;
    int16 i, lv;
    lv = doc->stack.top;
    if (node->id == TAGID_TEXT)
        lv++;
    for (i=0; i <= lv; i++) {
        ch = (i == lv) ? '+' : ' ';
        level[i] = ch;
    }
    level[i++] = '-';
    level[i++] = ' ';
    NBK_strcpy(&level[i], xml_getTagNames()[node->id]);
    dump_char(doc->page, level, -1);
    if (node->id == TAGID_TEXT && !doc->inStyle)
        dump_wchr(doc->page, node->d.text, node->len);
    dump_return(doc->page);
}
#endif

static void dom_push_block(NDocument* doc, NNode* newNode)
{
    N_ASSERT(doc->stack.top < MAX_TREE_DEPTH);
    doc->stack.dat[++doc->stack.top] = newNode;
    doc->lastChild = newNode->child;
    
//    dom_dump_level(doc, newNode);
    
#if NBK_EXT_MAINBODY_MODE
    if (newNode->id == TAGID_DIV) {
        if (attr_getValueBool(newNode->atts, ATTID_TC_MAINTOPIC)) {
            if (doc->mbList == N_NULL)
                doc->mbList = sll_create();
            sll_append(doc->mbList, newNode);
        }
    }
#endif
}

static bd_bool dom_pop_block(NDocument* doc, NNode* node)
{
    NNode* n;
    nid id = node->id - TAGID_CLOSETAG;
    N_ASSERT(doc->stack.top >= 0);
    n = doc->stack.dat[doc->stack.top];
    if (n->id != id)
        return N_FALSE;
    doc->lastChild = doc->stack.dat[doc->stack.top--];
    return N_TRUE;
}

static void doc_process_single_node(NDocument* doc, NNode* node)
{
    dom_insert_as_child(&doc->stack.dat[doc->stack.top], doc->lastChild, node);
    doc->lastChild = node;
    if (node->id == TAGID_TEXT && !node->space)
        doc->words += node->len;
}

#if DUMP_FUNCTIONS
static void doc_dump_token(const NDocument* doc, const NToken* token)
{
    NPage* page = (NPage*)doc->page;
    char tag[32];

    if (token->id > TAGID_CLOSETAG)
        NBK_sprintf(tag, "</%s>", xml_getTagNames()[token->id - TAGID_CLOSETAG]);
    else
        NBK_sprintf(tag, "<%s>", xml_getTagNames()[token->id]);
    
    dump_char(page, tag, -1);
    if (token->id == TAGID_TEXT) {
        if (doc->inStyle)
            dump_char(page, token->d.str, token->len);
        else
            dump_wchr(page, token->d.text, token->len);
    }
    dump_return(page);
}
#endif

static bd_bool handle_document(nid type)
{
    if (doc_isAbsLayout(type))
        return N_TRUE;
    
    switch (type) {
    case NEDOC_UNKNOWN:
    case NEDOC_HTML:
//    case NEDOC_XHTML:
        return N_FALSE;
    }
    
    return N_TRUE;
}

static nid l_fix_select_has[] = {
    TAGID_OPTION, TAGID_OPTION + TAGID_CLOSETAG,
    TAGID_TEXT,
    TAGID_SELECT + TAGID_CLOSETAG,
    0};

static nid l_fix_table_has[] = {
    TAGID_TR, TAGID_TR + TAGID_CLOSETAG,
    TAGID_TD, TAGID_TD + TAGID_CLOSETAG,
    0};

// Building document tree
void doc_processToken(NDocument* doc, NToken* token)
{
    NPage* page = (NPage*)doc->page;
    NNode* node = N_NULL;
    
    if (doc->finish)
        return;
    
    if (doc->root == N_NULL && token->id == TAGID_TEXT) {
#if DUMP_TOKEN
        doc_dump_token(doc, token);
#endif
        return;
    }
    
    if (token->id == TAGID_TITLE) {
        doc->inTitle = 1;
        return;
    }
    else if (token->id == TAGID_TITLE + TAGID_CLOSETAG) {
        doc->inTitle = 0;
        return;
    }
    
    if (token->id == TAGID_STYLE) {
        doc->inStyle = N_TRUE;
        return;
    }
    else if (token->id == TAGID_STYLE + TAGID_CLOSETAG) {
        doc->inStyle = N_FALSE;
        sheet_parse(page->sheet);
//        sheet_dump(page->sheet);
        return;
    }
    
    if (token->id == TAGID_SCRIPT) {
        doc->inScript = N_TRUE;
        return;
    }
    else if (token->id == TAGID_SCRIPT + TAGID_CLOSETAG) {
        doc->inScript = N_FALSE;
        return;
    }
    
#if DUMP_TOKEN
    doc_dump_token(doc, token);
    return;
#endif

    if (token->id == TAGID_LINK) {
        char* type = attr_getValueStr(token->atts, ATTID_TYPE);
        if (type && NBK_strcmp(type, "text/css") == 0) {
            char* url = url_parse(doc_getUrl(doc), attr_getValueStr(token->atts, ATTID_HREF));
            if (doc->extCssMgr == N_NULL)
                doc->extCssMgr = extCssMgr_create(doc);
            if (extCssMgr_addExtCss(doc->extCssMgr, url))
                extCssMgr_schedule(doc->extCssMgr);
            else
                NBK_free(url);
        }
        return;
    }
    else if (token->id == TAGID_LINK + TAGID_CLOSETAG)
        return;
    
    if (doc->inCtrl) {
        if (token->id == TAGID_SELECT + TAGID_CLOSETAG)
            doc->fix = N_NULL;
    }
    
    switch (token->id) {
    case TAGID_INPUT + TAGID_CLOSETAG:
    case TAGID_BUTTON + TAGID_CLOSETAG:
    case TAGID_SELECT + TAGID_CLOSETAG:
    case TAGID_TEXTAREA + TAGID_CLOSETAG:
        doc->inCtrl = N_FALSE;
        break;
    case TAGID_TABLE + TAGID_CLOSETAG:
        doc->inTable = N_FALSE;
        break;
    case TAGID_TR + TAGID_CLOSETAG:
    case TAGID_TD + TAGID_CLOSETAG:
        doc->inTable = N_TRUE;
        break;
    }
    
    if (token->id == TAGID_BASE) {
        char* url = attr_getValueStr(token->atts, ATTID_HREF);
        if (url) {
            doc_setUrl(doc, url);
            if (page->main) {
                history_setUrl((NHistory*)page->history, url);
            }
        }
        return;
    }
    
    if (doc->inTitle && doc->title == N_NULL) {
        doc->title = token->d.text;
        token->d.text = N_NULL;
        return;
    }
    if (token->id == TAGID_CARD && doc->title == N_NULL) {
        char* t = attr_getValueStr(token->atts, ATTID_TITLE);
        if (t) {
            doc->title = uni_utf8_to_utf16_str((uint8*)t, -1, N_NULL);
        }
    }
    
    if (doc->inStyle) {
#if CREATE_RENDER_TREE
        if (doc->extCssMgr)
            extCssMgr_addInnCss(doc->extCssMgr, token->d.str, token->len);
        else
            sheet_addData(page->sheet, token->d.str, token->len);
//            sheet_parseStyle(page->sheet, token->d.str, token->len);
#endif
        return;
    }
    
    if (doc->inScript)
        return;

    if (token->id == TAGID_META) {

        char *v1, *v2;
        
        // MS spec. <meta name="MobileOptimized" content="240"/>
        v1 = attr_getValueStr(token->atts, ATTID_NAME);
        if (v1 && NBK_strcmp(v1, "MobileOptimized") == 0) {
            if (doc->type == NEDOC_UNKNOWN || doc->type == NEDOC_HTML)
                doc->type = NEDOC_XHTML;
            v2 = attr_getValueStr(token->atts, ATTID_CONTENT);
            if (v2) {
                coord ow = NBK_atoi(v2);
                if (ow >= 176)
                    page->view->optWidth = ow;
            }
        }
        else if (v1 && NBK_strcmp(v1, "viewport") == 0) {
            v2 = attr_getValueStr(token->atts, ATTID_CONTENT);
            if (v2) {
                int pos = nbk_strfind(v2, "width=");
                if (pos != -1) {
                    char* p = v2 + pos + 6;
                    coord dw;
                    if (NBK_strncmp(p, "device-width", 12) == 0)
                        dw = page->view->scrWidth;
                    else
                        dw = NBK_atoi(p);
                    if (   dw <= WIDTH_FOR_MOBILE
                        && (doc->type == NEDOC_UNKNOWN || doc->type == NEDOC_HTML))
                        doc->type = NEDOC_XHTML;
                }
            }
        }

        // <meta http-equiv="Refresh" content="5;url=http://www.badiu.com" />
        v1 = attr_getValueStr(token->atts, ATTID_HTTP_EQUIV);
        if (v1 && !NBK_strncmp_nocase(v1, "Refresh", 7)) {
            v2 = attr_getValueStr(token->atts, ATTID_CONTENT);
            if (v2) {
                int urlPos = nbk_strfind_nocase(v2, "url=");
                if (urlPos >= 0) {
                    char *url, *u;
                    int delay;
                    
                    delay = NBK_atoi(v2);
                    if (delay <= 0) delay = 1;
                    doc->delayLoad = delay * 1000000;
                    
                    u = v2 + urlPos + 4;
                    if (NBK_strlen(u) > 3) { // g.cn
                        url = url_parse(doc_getUrl(doc), u);
                    doc_setUrl(doc, url); // replace document url
                    NBK_free(url);
                        doc->delayLoadType = NEREDIR_META_REFRESH;
                    }
                    else
                        doc->delayLoad = 0;
                }
            }
        }
        return; // fixme: not create in DOM
    }
    
    // fix form controls
    if (doc->inCtrl && doc->fix) {
        int i;
        for (i=0; doc->fix[i]; i++)
            if (token->id == doc->fix[i])
                break;
        if (doc->fix[i] == 0) {
            NNode t;
            NBK_memset(&t, 0, sizeof(NNode));
            if (doc->fix == l_fix_select_has) {
                t.id = TAGID_SELECT + TAGID_CLOSETAG;
                dom_pop_block(doc, &t);
                view_processNode(page->view, &t);
            }
            doc->fix = N_NULL;
            doc->inCtrl = N_FALSE;
        }
    }

    // fix table
    if (doc->inTable) {
        int i;
        for (i=0; l_fix_table_has[i]; i++)
            if (token->id == l_fix_table_has[i])
                break;
        if (l_fix_table_has[i] == 0)
            return; // not allow in table
    }

    if (node_is_single(token->id)) {
        if (token->id < TAGID_CLOSETAG) {
            
            if (doc->stack.top == -1) {
                doc_end(doc);
                return;
            }
            
            if (token->id == TAGID_TIMER) {
                char* v = attr_getValueStr(token->atts, ATTID_VALUE);
                if (v) {
                    int s = NBK_atoi(v); // 1/10 second
                    if (s <= 0) s = 10;
                    doc->delayLoad = s * 100000;
                    doc->delayLoadType = NEREDIR_WML_ONTIMER;
                }
                return;
            }
                   
            node = node_create(token, doc);
            doc_process_single_node(doc, node);
#if CREATE_RENDER_TREE
            if (!doc->inCtrl)
                view_processNode(page->view, node);
#endif
            
            // for SPREADSTRUM
            if (doc->ssKeyList && token->id == TAGID_TEXT) {
                int i;
                for (i=0; doc->ssKeyList[i]; i++) {
                    if (NBK_wcsfind(token->d.text, doc->ssKeyList[i]) != -1) {
                        doc->disable = !NBK_ext_isDisplayDocument(doc);
                        break;
                    }
                }
            }
        }
        return; // ignore close tag of single node
    }
    
    if (token->id > TAGID_CLOSETAG) {
        NNode* t = (NNode*)token; // id must be the first member of Token and Node.
        bd_bool ret = dom_pop_block(doc, t);
        
#if CREATE_RENDER_TREE
        if (!doc->inCtrl && ret)
            view_processNode(page->view, t);
#endif
        
        if (doc->stack.top == -1) {
            // reach the end
            doc_end(doc);
        }
        return;
    }
    
    if (token->id == TAGID_TEXT && token->len == 1 && token->d.text[0] == 0x20) {
        doc->hasSpace = 1;
        return;
    }

    if (doc->hasSpace && !doc->inCtrl) {
        doc->hasSpace = 0;
        
        switch (token->id) {
        case TAGID_CARD:
        case TAGID_BODY:
        case TAGID_DIV:
        case TAGID_P:
        case TAGID_UL:
        case TAGID_OL:
        case TAGID_LI:
            break;
            
        default:
        {
            NToken tok;
            NBK_memset(&tok, 0, sizeof(NToken));
            tok.id = TAGID_TEXT;
            tok.d.text = (wchr*)NBK_malloc(sizeof(wchr) * 2);
            tok.d.text[0] = 0x20; tok.d.text[1] = 0;
            tok.len = 1;
            node = node_create(&tok, doc);
            doc_process_single_node(doc, node);
#if CREATE_RENDER_TREE
            if (!doc->inCtrl)
                view_processNode(page->view, node);
#endif
        }
            break;
        }
    }
    
    node = node_create(token, doc);
    
    if (token->id == TAGID_TEXT) {
        doc_process_single_node(doc, node);
#if CREATE_RENDER_TREE
        if (!doc->inCtrl)
            view_processNode(page->view, node);
#endif
        return;
    }
    
    if (node->id == TAGID_ONEVENT) {
        char* type = attr_getValueStr(node->atts, ATTID_TYPE);
        if (type && NBK_strcmp(type, "onenterforward") == 0) {
            doc->delayLoad = 1000000;
            doc->delayLoadType = NEREDIR_WML_ONEVENT;
        }
    }
    
    if (doc->root == N_NULL) {
        dom_insert_as_child(&doc->root, N_NULL, node);
    }
    else {
        dom_insert_as_child(&doc->stack.dat[doc->stack.top], doc->lastChild, node);
    }
    dom_push_block(doc, node);
    
#if CREATE_RENDER_TREE
    if (!doc->inCtrl)
        view_processNode(page->view, node);
#endif
    
    switch (token->id) {
    case TAGID_INPUT:
    case TAGID_BUTTON:
    case TAGID_SELECT:
    case TAGID_TEXTAREA:
        doc->inCtrl = N_TRUE;
        break;
    case TAGID_TABLE:
    case TAGID_TR:
        doc->inTable = N_TRUE;
        break;
    case TAGID_TD:
        doc->inTable = N_FALSE;
        break;
    }
    
    if (token->id == TAGID_SELECT)
        doc->fix = l_fix_select_has;
}

void doc_acceptType(NDocument* doc)
{
    NPage* page = (NPage*)doc->page;
    
    // document can't handle, request to TC
    if (!handle_document(doc->type)) {
        doc->finish = 1;
        xmltkn_abort(doc->tokenizer);
        if (doc_getUrl(doc)) {
            page->settings->support = NENotSupportByNbk;
            doc_doGet(doc, doc_getUrl(doc), N_FALSE, N_TRUE);
        }
        else if (!NBK_handleError(NELERR_NBK_NOT_SUPPORT)){
            doc_doGet(page->doc, NBK_getStockPage(NESPAGE_ERROR), N_FALSE, N_FALSE);
        }
    }
}

static NRenderNode* doc_get_first_render_node(NNode* node, NNode** sta)
{
    NRenderNode* r = (NRenderNode*)node->render;
    
    if (r && !r->needLayout) {
        return r;
    }
    // for element A, it not has a render node
    else {
        NNode* n = node;
        int16 idx = -1;
        bd_bool ignore = N_FALSE;
        
        while (n) {
            
            r = (NRenderNode*)n->render;
            if (!ignore && r && !r->needLayout)
                return r;
            
            if (!ignore && n->child) {
                sta[++idx] = n;
                n = n->child;
            }
            else {
                ignore = N_FALSE;
                if (n == node)
                    break;
                if (n->next)
                    n = n->next;
                    else {
                        n = sta[idx--];
                        ignore = N_TRUE;
                    }
                }
            }
        }
    return N_NULL;
}

NNode* doc_getFocusNode(NDocument* doc, const NRect* area)
{
    NPage* page = (NPage*)doc->page;
    NView* view = page->view;
    NNode** sta = view->path;
    int16 idx = -1;
    NNode* n = doc->root;
    bd_bool ignore = N_FALSE, abort = N_FALSE;
    NRenderNode* r;
    NRect rr, vw;
    bd_bool absLayout = doc_isAbsLayout(doc->type);
    
    if (n == N_NULL)
        return N_NULL;
    
    vw = *area;
    rect_toDoc(&vw, history_getZoom((NHistory*)page->history));
    
    while (n) {
        
        if (!ignore) {
            switch (n->id) {
            case TAGID_A:
            case TAGID_ANCHOR:
            {
                r = doc_get_first_render_node(n, (NNode**)view->stack.aux);
                if (r) {
                    if (absLayout)
                        rr = r->r;
                    else
                    renderNode_absRect(r, &rr);
                    if (rr.l >= vw.l && rr.t >= vw.t)
                        return n;
                    if (rr.t >= vw.b)
                        abort = N_TRUE;
                }
                ignore = N_TRUE;
                break;
            }
            case TAGID_INPUT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderInput_isVisible((NRenderInput*)r)) {
                    if (absLayout)
                        rr = r->r;
                    else
                    renderNode_absRect(r, &rr);
                    if (rect_isContain(&vw, &rr))
                        return n;
                    if (rr.t >= vw.b)
                        abort = N_TRUE;
                }
                break;
            }
            case TAGID_TEXTAREA:
            case TAGID_SELECT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout) {
                    if (absLayout)
                        rr = r->r;
                    else
                    renderNode_absRect(r, &rr);
                    if (rect_isContain(&vw, &rr))
                        return n;
                    if (rr.t >= vw.b)
                        abort = N_TRUE;
                }
                break;
            }
            case TAGID_DIV:
            {
                r = (NRenderNode*)n->render;
                if (r->display == CSS_DISPLAY_NONE)
                    ignore = N_TRUE;
                else if (doc->type == NEDOC_NARROW) {
                    if (   n->Click
                        && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0
                        && rect_isIntersect(&vw, &r->r) )
                        return n;
                    else if (((NRenderBlock*)r)->ne_display && rect_isContain(&vw, &r->r))
                        return n;
                }
                    break;
                }
            default:
                if (doc->type == NEDOC_DXXML) {
                    if (n->Click && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0) {
                        r = doc_get_first_render_node(n, (NNode**)view->stack.aux);
                        if (r) {
                            renderNode_absRect(r, &rr);
                            if (rr.l >= vw.l && rr.t >= vw.t)
                                return n;
                            if (rr.t >= vw.b)
                                abort = N_TRUE;
                        }
                        ignore = N_TRUE;
                    }
                }
                break;
            }
        }
        
        if (abort)
            break;
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == doc->root)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }
    
    return N_NULL;
}

static bd_bool doc_child_include_pos(NNode* node, coord x, coord y, NNode** sta, bd_bool abs)
{
    NNode* n = node;
    int16 l = -1;
    bd_bool ignore = N_FALSE;
    NRect rect;
    NPoint pt;
    
    pt.x = x; pt.y = y;
    
    while (n) {
        
        if (!ignore && n->render) {
            NRenderNode* r = (NRenderNode*)n->render;
            if (!r->needLayout && (r->type == RNT_TEXT || r->type == RNT_IMAGE)) {
                if (abs) {
                    if (rect_hasPt(&r->r, x, y))
                        return N_TRUE;
                }
                else {
                    if (r->type == RNT_TEXT) {
                        if (renderText_hasPt((NRenderText*)r, pt))
                            return N_TRUE;
                    }
                    else if (renderNode_getAbsRect(r, &rect) && rect_hasPt(&rect, x, y))
                        return N_TRUE;
                }
            }
        }
        
        if (!ignore && n->child) {
            sta[++l] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == node)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[l--];
                    ignore = N_TRUE;
                }
            }
        }
    
    return N_FALSE;
}

static NNode* doc_get_focus_node_by_pos(NDocument* doc, coord x, coord y)
{
    NPage* page = (NPage*)doc->page;
    NView* view = page->view;
    NNode** sta = view->path;
    int16 idx = -1;
    NNode* n = doc->root;
    NRenderNode* r;
    bd_bool ignore = N_FALSE;
    NRect rect;
    
    if (n == N_NULL)
        return N_NULL;
    
    while (n) {

        if (!ignore) {
            
            switch (n->id) {
            case TAGID_A:
            case TAGID_ANCHOR:
            {
                r = (NRenderNode*)n->render;
                if (doc_child_include_pos(n, x, y, (NNode**)view->stack.aux, N_FALSE)) {
                    return n;
                }
                
                ignore = N_TRUE;
                break;
            }
            case TAGID_BUTTON:
            case TAGID_INPUT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderInput_isVisible((NRenderInput*)r) && renderNode_getAbsRect(r, &rect)) {
                    if (rect_hasPt(&rect, x, y))
                        return n;
                }
                break;
            }
            case TAGID_TEXTAREA:
            case TAGID_SELECT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderNode_getAbsRect(r, &rect)) {
                    if (rect_hasPt(&rect, x, y))
                        return n;
                }
                ignore = N_TRUE;
                break;
            }
            case TAGID_DIV:
            {
                NRenderBlock* rb = (NRenderBlock*)n->render;
                r = (NRenderNode*)n->render;
                if (r->display == CSS_DISPLAY_NONE) {
                    ignore = N_TRUE;
                }
                else if (   !r->needLayout
                         && rb->ne_display
                         && renderNode_getAbsRect(r, &rect)) {
                    if (rect_hasPt(&rect, x, y))
                        return n;
                }
                    break;
            }
            } // switch
        }
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == doc->root)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }

    return N_NULL;
}

static NNode* doc_get_focus_node_by_pos_ff(NDocument* doc, coord x, coord y)
{
    NPage* page = (NPage*)doc->page;
    NView* view = page->view;
    NNode** sta = view->path;
    int16 idx = -1;
    NNode* n = doc->root;
    NRenderNode* r;
    bd_bool ignore = N_FALSE;
    NNode* last = N_NULL; // FIXME: search all dom to get node focused.
    
    if (n == N_NULL)
        return N_NULL;
    
    while (n) {

        if (!ignore) {
            
            switch (n->id) {
            case TAGID_A:
            {
                r = (NRenderNode*)n->render;
                if (doc_child_include_pos(n, x, y, (NNode**)view->stack.aux, N_TRUE)) {
                    last = n;
                }
                
                ignore = N_TRUE;
                break;
            }
            case TAGID_BUTTON:
            case TAGID_INPUT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderInput_isVisible((NRenderInput*)r) && rect_hasPt(&r->r, x, y)) {
                    last = n;
                }
                break;
            }
            case TAGID_TEXTAREA:
            case TAGID_SELECT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && rect_hasPt(&r->r, x, y)) {
                    last = n;
                }
                ignore = N_TRUE;
                break;
            }
            case TAGID_DIV:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0) {
                    if (rect_hasPt(&r->r, x, y)) {
                        last = n; // FIXME: temp
                    }
                }
                break;
            }
            }
        }
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == doc->root)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }

    return last;
}

NNode* doc_getFocusNodeByPos(NDocument* doc, coord x, coord y)
{
    NPage* page = (NPage*)doc->page;
    NFloat zoom = history_getZoom((NHistory*)page->history);
    x = (coord)nfloat_idiv(x, zoom);
    y = (coord)nfloat_idiv(y, zoom);
    
    if (doc_isAbsLayout(doc->type)) {
        
//        if (doc->lastGotFocus && doc->lastGotFocus->id != TAGID_DIV) {
//            NRenderNode* r = (NRenderNode*)doc->lastGotFocus->render;
//            if (r && rect_hasPt(&r->r, x, y))
//                return doc->lastGotFocus;
//        }
        
        if (view_isZindex(page->view))
            doc->lastGotFocus = view_findFocusWithZindex(page->view, x, y);
        else
        doc->lastGotFocus = doc_get_focus_node_by_pos_ff(doc, x, y);
        return doc->lastGotFocus;
    }
    else
        return doc_get_focus_node_by_pos(doc, x, y);
}

char* doc_getFocusUrl(NDocument* doc, NNode* focus)
{
    NNode* n = focus;
    
    while (n && n->id != TAGID_A && n->id != TAGID_ANCHOR)
        n = n->parent;
    
    if (n) {
        char* href = N_NULL;
        
        if (n->id == TAGID_A) {
            href = attr_getValueStr(n->atts, ATTID_HREF);
        }
        else if (n->id == TAGID_ANCHOR) {
            NPage* page = (NPage*)doc->page;
            NNode* go = node_getByTag(n, TAGID_GO, page->view->path);
            if (go)
                href = attr_getValueStr(go->atts, ATTID_HREF);
        }
        
        if (href)
            return url_parse(doc->url, href);
    }
    
    return N_NULL;
}

static int16 doc_build_path(NNode* node, NNode** sta, NNode** sta2)
{
    NNode* n = node;
    int16 i = -1, idx = -1;
    
    while (n->parent) {
        n = n->parent;
        sta2[++i] = n;
    }

    for (; i > -1; i--) {
        sta[++idx] = sta2[i];
    }
    
    return idx;
}

NNode* doc_getPrevFocusNode(NDocument* doc, NNode* curr)
{
    NPage* page = (NPage*)doc->page;
    NView* view = page->view;
    NNode** sta = view->path;
    int16 idx = -1;
    NNode* n = doc->root;
    NRenderNode* r;
    bd_bool ignore = N_FALSE;
    NNode* last = N_NULL;
    
    if (n == N_NULL)
        return n;
    
    while (n) {
        if (!ignore) {
            switch (n->id) {
            case TAGID_A:
            case TAGID_ANCHOR:
            {
                if (doc_get_first_render_node(n, (NNode**)view->stack.aux)) {
                    if (n == curr)
                        return last;
                    else
                        last = n;
                }
                ignore = N_TRUE;
                break;
            }
            case TAGID_INPUT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderInput_isVisible((NRenderInput*)r)) {
                    if (n == curr)
                        return last;
                    else
                        last = n;
                }
                break;
            }
            case TAGID_TEXTAREA:
            case TAGID_SELECT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout) {
                    if (n == curr)
                        return last;
                    else
                        last = n;
                }
                break;
            }
            case TAGID_DIV:
            {
                r = (NRenderNode*)n->render;
                if (r->display == CSS_DISPLAY_NONE)
                    ignore = N_TRUE;
                else if (doc->type == NEDOC_NARROW) {
                    if (   (n->Click && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0)
                        || ((NRenderBlock*)r)->ne_display ) {
                        if (n == curr)
                            return last;
                        else
                            last = n;
                    }
                }
                    break;
                }
            default:
                if (doc->type == NEDOC_DXXML) {
                    if (n->Click && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0) {
                        if (doc_get_first_render_node(n, (NNode**)view->stack.aux)) {
                            if (n == curr)
                                return last;
                            else
                                last = n;
                        }
                        ignore = (n->id == TAGID_DIV || n->id == TAGID_SPAN) ? N_FALSE : N_TRUE;
                    }
                }
                break;
            }
        }
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == doc->root)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }
    
    return N_NULL;
}

NNode* doc_getNextFocusNode(NDocument* doc, NNode* curr)
{
    NPage* page = (NPage*)doc->page;
    NView* view = page->view;
    NNode** sta = view->path;
    int16 idx;
    NNode* n = curr;
    NRenderNode* r;
    bd_bool ignore = N_TRUE;
    
    if (n == N_NULL)
        return n;
    
    idx = doc_build_path(n, sta, (NNode**)view->stack.aux);
    if ((n->id == TAGID_DIV || n->id == TAGID_SPAN) && n->child) {
        sta[++idx] = n;
        n = n->child;
        ignore = N_FALSE;
    }
    
    while (n) {
        if (!ignore) {
            switch (n->id) {
            case TAGID_A:
            case TAGID_ANCHOR:
            {
                if (doc_get_first_render_node(n, (NNode**)view->stack.aux)) {
                    return n;
                }
                ignore = N_TRUE;
                break;
            }
            case TAGID_INPUT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout && renderInput_isVisible((NRenderInput*)r))
                    return n;
                break;
            }
            case TAGID_TEXTAREA:
            case TAGID_SELECT:
            {
                r = (NRenderNode*)n->render;
                if (!r->needLayout)
                    return n;
                break;
            }
            case TAGID_DIV:
            {
                r = (NRenderNode*)n->render;
                if (r->display == CSS_DISPLAY_NONE)
                    ignore = N_TRUE;
                else if (doc->type == NEDOC_NARROW) {
                    if (   (n->Click && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0)
                        || ((NRenderBlock*)r)->ne_display )
                        return n;
                }
                    break;
                }
            default:
                if (doc->type == NEDOC_DXXML) {
                    if (n->Click && attr_getValueInt32(n->atts, ATTID_TC_IA) > 0) {
                        if (doc_get_first_render_node(n, (NNode**)view->stack.aux)) {
                            return n;
                        }
                        ignore = N_TRUE;
                    }
                }
                break;
            }
        }
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
            if (n == doc->root)
                    break;
            if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }
    
    return N_NULL;
}

void doc_getFocusNodeRect(NDocument* doc, NNode* focus, NRect* rect)
{
    NPage* page = (NPage*)doc->page;
    NView* view = (NView*)page->view;
    NNode** sta = view->path;
    int16 idx = -1;
    NNode* n = focus;
    NRenderNode* r;
    coord x, y;
    bd_bool ignore = N_FALSE;
    NRect a;
    
    if (n == N_NULL || rect == N_NULL)
        return;
    
    rect->l = rect->t = N_MAX_COORD;
    rect->r = rect->b = 0;
    
    if (n->render) {
        r = (NRenderNode*)n->render;
        if (doc_isAbsLayout(doc->type)) {
            *rect = r->r;
        }
        else {
            renderNode_getAbsPos(r->parent, &x, &y);
            *rect = r->r;
            rect_move(rect, rect->l + x, rect->t + y);
        }
    }
    else {
    while (n) {
        
        if (!ignore && n->render) {
            r = (NRenderNode*)n->render;
                if (doc_isAbsLayout(doc->type)) {
                a = r->r;
            }
            else {
                    renderNode_getAbsPos(r->parent, &x, &y);
                    a = r->r;
                    rect_move(&a, a.l + x, a.t + y);
            }
            rect_unite(rect, &a);
            ignore = N_TRUE;
        }
        
        if (!ignore && n->child) {
            sta[++idx] = n;
            n = n->child;
        }
        else {
            ignore = N_FALSE;
                if (n == focus)
                    break;
                if (n->next)
                n = n->next;
                else {
                    n = sta[idx--];
                    ignore = N_TRUE;
                }
            }
        }
    }
    
    rect_toView(rect, history_getZoom((NHistory*)page->history));
}

void doc_doGet(NDocument* doc, const char* url, bd_bool submit, bd_bool noDeal)
{
    NPage* page = (NPage*)doc->page;
    NLoadCmd* cmd = NBK_malloc0(sizeof(NLoadCmd));
    char* base = doc_getUrl(doc);
    char* u = N_NULL;
    NUpCmdSet settings;
    
    u = url_parse(base, url);
    settings = page_decideRequestMode(page, u, submit);
    
    cmd->doc = doc;
    cmd->noDeal = noDeal;
    cmd->via = (settings.via == NEREV_FF_FULL_XHTML) ? NEREV_FF_FULL : settings.via;
    cmd->submit = submit;
    
    if (   settings.via == NEREV_FF_FULL
        || settings.via == NEREV_FF_FULL_XHTML
        || settings.via == NEREV_FF_NARROW
        || settings.via == NEREV_UCK ) {
        
        cmd->upcmd = upcmd_create(&settings);
        cmd->upcmd->node = view_getFocusNode(page->view); // record for request again when server not support
        upcmd_url(cmd->upcmd, u, base);
        if (doc_isAbsLayout(doc->type)) {
            nid tags[] = {TAGID_PARAM, 0};
            NSList* lst = node_getListByTag(doc->root, tags, page->view->path);
            upcmd_iaParams(cmd->upcmd, lst);
            sll_delete(&lst);
        }
        NBK_free(u);
    }
    else {
        cmd->url = u;
        if (cmd->url == N_NULL) {
            NBK_free(cmd);
            return;
        }
    }
    
    tim_stop(doc->loadTimer);
    delete_loadCmd(doc->loadTimer);
    doc->loadTimer->user = cmd;
    tim_start(doc->loadTimer, 5, 5000000);
}

void doc_doPost(NDocument* doc, const char* url, nid enc, char* body, NFileUpload* files)
{
    NPage* page = (NPage*)doc->page;
    NLoadCmd* cmd = NBK_malloc0(sizeof(NLoadCmd));
    
    cmd->doc = doc;
    cmd->body = body;
    cmd->enc = enc;
    cmd->files = files;
    cmd->via = NEREV_STANDARD;
    cmd->submit = 1;
    
    if (   page->mode == NEREV_FF_FULL
        || page->mode == NEREV_FF_NARROW
        || page->mode == NEREV_UCK) {
        NBK_free(cmd);
        return;
    }
    else {
        cmd->url = url_parse(doc->url, url);
        if (cmd->url == N_NULL) {
            NBK_free(cmd);
            return;
        }
    }
    
    tim_stop(doc->loadTimer);
    delete_loadCmd(doc->loadTimer);
    doc->loadTimer->user = cmd;
    tim_start(doc->loadTimer, 5, 5000000);
}

void doc_doIa(NDocument* doc, NUpCmd* upcmd, NFileUpload* files)
{
    NPage* page = (NPage*)doc->page;
    NLoadCmd* cmd = NBK_malloc0(sizeof(NLoadCmd));
    
    cmd->doc = doc;
    cmd->upcmd = upcmd;
    cmd->files = files;
    cmd->via = page->mode;
    cmd->submit = 1;
    
    tim_stop(doc->loadTimer);
    delete_loadCmd(doc->loadTimer);
    doc->loadTimer->user = cmd;
    tim_start(doc->loadTimer, 5, 5000000);
}

static void doc_do_back(void* user)
{
    NLoadCmd* cmd = (NLoadCmd*)user;
    tim_stop(cmd->doc->loadTimer);
    cmd->doc->loadTimer->func = doc_do_load;
    history_prev((NHistory*)((NPage*)cmd->doc->page)->history);
    delete_loadCmd(cmd->doc->loadTimer);
}

void doc_doBack(NDocument* doc)
{
    NLoadCmd* cmd = NBK_malloc0(sizeof(NLoadCmd));
    cmd->doc = doc;
    tim_stop(doc->loadTimer);
    delete_loadCmd(doc->loadTimer);
    doc->loadTimer->func = doc_do_back;
    doc->loadTimer->user = cmd;
    tim_start(doc->loadTimer, 5, 5000000);
}

void doc_setUrl(NDocument* doc, const char* url)
{
    int len = NBK_strlen(url) + 1;
    int size = doc->urlMaxLen;

    if (len > size) {
        while (len > size) {
            if (size == 0)
                size = 128;
            else
                size <<= 1;
        }
        
        if (doc->url)
            NBK_free(doc->url);
        doc->url = (char*)NBK_malloc(size);
        doc->urlMaxLen = (int16)size;
    }
    
    NBK_strcpy(doc->url, url);
}

char* doc_getUrl(NDocument* doc)
{
    if (doc->url && doc->url[0])
        return doc->url;
    else
    return N_NULL;
}

wchr* doc_getTitle(NDocument* doc)
{
    return doc->title;
}

nid doc_getType(NDocument* doc)
{
    return doc->type;
}

void doc_clickFocusNode(NDocument* doc, NNode* focus)
{
    if (focus && focus->Click) {
        if (doc->type == NEDOC_MAINBODY) {
            NPage* page;
            NPage* mbPage = (NPage*)doc->page;
            NHistory* history = (NHistory*)mbPage->history;
            history_quitMainBodyWithClick(history);
            page = history_curr(history);
            view_setFocusNode(page->view, focus->nodeRef);
            doc_clickFocusNode(page->doc, focus->nodeRef);
        }
        else {
            focus->Click(focus, doc);
        }
    }
}

void doc_viewChange(NDocument* doc)
{
    doc->picUpdate = 1;
    doc->layoutFinish = 0;
    doc_schedule_layout(doc);
}

void doc_modeChange(NDocument* doc, NEDocType type)
{
    NPage* page = (NPage*)doc->page;
    
    doc->type = type;
    
    if (type == NEDOC_FULL) {
        page->mode = NEREV_FF_FULL;
        if (page->main) {
#if NBK_REQ_IMG_ALONE
            page->view->imgPlayer->passive = 0;
            page->view->imgPlayer->multipics = 1;
#else
            page->view->imgPlayer->passive = 1;
            page->view->imgPlayer->multipics = 0;
#endif
            page->view->imgPlayer->interval = 2000;
            page->view->imgPlayer->loadByInvoke = ((NSettings*)page->settings)->clickToLoadImage;
        }
    }
    else if (type == NEDOC_FULL_XHTML) {
        page->mode = NEREV_FF_FULL;
        if (page->main) {
            page->view->imgPlayer->passive = 0;
            page->view->imgPlayer->multipics = 1;
        }
    }
    else if (type == NEDOC_NARROW) {
        page->mode = NEREV_FF_NARROW;
        if (page->main) {
#if NBK_REQ_IMG_ALONE
            page->view->imgPlayer->passive = 0;
            page->view->imgPlayer->multipics = 1;
#else
            page->view->imgPlayer->passive = 1;
            page->view->imgPlayer->multipics = 0;
#endif
            page->view->imgPlayer->interval = 500;
        }
    }
    else if (type == NEDOC_DXXML) {
        page->mode = NEREV_UCK;
        if (page->main) {
            page->view->imgPlayer->passive = 0;
            page->view->imgPlayer->multipics = 1;
        }
    }
    else {
        page->mode = NEREV_STANDARD;
        if (page->main) {
            page->view->imgPlayer->passive = 0;
            page->view->imgPlayer->multipics = 0;
            page->doc->wordCount = 200;
        }
    }
}

// when path exists except the leaf, sibling set to last valid sibling
NNode* dom_query(NDocument* doc, NXPathCell* xpath, NNode** sibling, NNode** parent, bd_bool ignoreTag)
{
    NNode *body, *n, *t, *l;
    int i, j;
    bd_bool found;

    if (doc->root == N_NULL || doc->root->child == N_NULL)
        return N_NULL;
    
    if (sibling)
        *sibling = N_NULL;
    if (parent)
        *parent = N_NULL;
    
    body = node_getByTag(doc->root, TAGID_BODY, doc->stack.dat);
    if (body == N_NULL)
        return N_NULL;
    if (body->child == N_NULL) {
        if (ignoreTag && xpath[0].id && !xpath[1].id)
            *parent = body;
        return N_NULL;
    }
    
    i = j = 0;
    n = body->child;
    t = l = N_NULL;
    while (n && xpath[i].id) {
        
        if (ignoreTag && xpath[i+1].id == 0) {
            xpath[i].id = N_INVALID_ID;
        }
        
        j = 0;
        found = 0;
        while (n) {
            if (xpath[i].id == N_INVALID_ID || n->id == xpath[i].id) {
                j++;
//                n->test = j;
                l = n;
                if (j == xpath[i].idx) {
                    found = 1;
                    break;
                }
            }
            n = n->next;
        }
        
        if (found) {
            t = n;
            n = n->child;
            i++;
        }
    }
    
    if (t && xpath[i].id == 0)
        return t;
    
    if (!n && (j == xpath[i].idx - 1) && (xpath[i + 1].id == 0) && sibling)
        *sibling = l;
    
    if (ignoreTag && xpath[i+1].id == 0 && t && parent)
        *parent = t;

    return N_NULL;
}

static void dom_delete_node(NDocument* doc, NNode* node)
{
    NNode* parent = node->parent;
    NNode* prev = node->prev;
    NNode* next = node->next;
    
    if (parent && parent->child == node)
        parent->child = next;
    if (prev)
        prev->next = next;
    if (next)
        next->prev = prev;
    
    dom_delete_tree(doc, node);
}

static bd_bool dom_replace_node(NDocument* doc, NNode* old, NNode* novice)
{
    NNode* parent = old->parent;
    NNode* prev = old->prev;
    NNode* next = old->next;
    
    novice->parent = parent;
    novice->prev = prev;
    novice->next = next;
    
    if (parent->child == old)
        parent->child = novice;
    if (prev)
        prev->next = novice;
    if (next)
        next->prev = novice;
    
    dom_delete_tree(doc, old);
    
    return N_TRUE;
}

static bd_bool dom_insert_node(NNode* node, NNode* novice, bd_bool before)
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

static bd_bool dom_insert_node_as_child(NNode* parent, NNode* novice)
{
    if (parent == N_NULL || novice == N_NULL)
        return N_FALSE;
    
    parent->child = novice;
    novice->parent = parent;
    
    return N_TRUE;
}

bd_bool doc_IsEditorFull(void* page)
{
    NPage* p = (NPage*) page;
    bd_bool ret = N_TRUE;
    if (p->view->focus) {
        NRenderNode* r = (NRenderNode*) p->view->focus->render;
        if (r == N_NULL)
            return ret;

        if(RNT_INPUT == r->type) {
            NRenderInput* ri = (NRenderInput*) r;
            if (ri->editor->cur < ri->editor->max - 1)
                ret = N_FALSE;
        }

        else if(RNT_TEXTAREA == r->type)
        {
            NRenderTextarea* rt = (NRenderTextarea*) r;
            if (rt->editor->cur < rt->editor->max)
                ret = N_FALSE;
        }
    }
    return ret;
}

bd_bool doc_processKey(NEvent* event)
{
    NPage* page = (NPage*)event->page;
    bd_bool ret = N_FALSE;
    
    if (page->view->focus) {
        NRenderNode* r = (NRenderNode*)page->view->focus->render;
        if (r == N_NULL)
            return N_FALSE;
        
        switch (r->type) {
        case RNT_INPUT:
            ret = renderInput_processKey(r, event);
            if (ret)
                nbk_cb_call(NBK_EVENT_PAINT_CTRL, &page->cbEventNotify, N_NULL);
            break;
            
        case RNT_TEXTAREA:
            ret = renderTextarea_processKey(r, event);
            if (ret)
                nbk_cb_call(NBK_EVENT_PAINT_CTRL, &page->cbEventNotify, N_NULL);
            break;
            
        case RNT_SELECT:
            ret = renderSelect_processKey(r, event);
            if (ret)
                    nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
            break;
        }
    }

    return ret;
}

// event position in view coordinate
bd_bool doc_processPen(NEvent* event)
{
    return doc_processKey(event);
    
#if 0
    if (   doc_isAbsLayout(page->doc->type)
        && event->type == NEEVENT_PEN && event->d.penEvent.type == NEPEN_MOVE) {
        NNode* old = view_getFocusNode(page->view);
        NNode* found = doc_getFocusNodeByPos(page->doc, event->d.penEvent.pos.x, event->d.penEvent.pos.y);
        NEvent evt1, evt2;
        
        page->doc->lastGotFocus = found;

        NBK_memset(&evt1, 0, sizeof(NEvent));
        NBK_memset(&evt2, 0, sizeof(NEvent));
        
        if (old && found) {
            if (old == found) {
                if (!page->doc->sentMouseMove) {
                    evt1.type = NEEVENT_DOM;
                    evt1.page = page;
                    evt1.d.domEvent.type = NEDOM_MOUSE_MOVE;
                    evt1.d.domEvent.node = old;
                    
                    ret = doc_processDomEvent(page->doc, &evt1, N_NULL);
                    
                    page->doc->sentMouseMove = 1;
                }
            }
            else { // switch to new focus
                evt1.type = NEEVENT_DOM;
                evt1.page = page;
                evt1.d.domEvent.type = NEDOM_MOUSE_OUT;
                evt1.d.domEvent.node = old;
                
                evt2.type = NEEVENT_DOM;
                evt2.page = page;
                evt2.d.domEvent.type = NEDOM_MOUSE_OVER;
                evt2.d.domEvent.node = found;
                
                ret = doc_processDomEvent(page->doc, &evt1, &evt2);
                
                page->doc->sentMouseMove = 0;
            }
        }
        else if (!old && found) {
            evt1.type = NEEVENT_DOM;
            evt1.page = page;
            evt1.d.domEvent.type = NEDOM_MOUSE_OVER;
            evt1.d.domEvent.node = found;
            
            ret = doc_processDomEvent(page->doc, &evt1, N_NULL);
            
            page->doc->sentMouseMove = 0;
        }
        else if (old && !found) {
            evt1.type = NEEVENT_DOM;
            evt1.page = page;
            evt1.d.domEvent.type = NEDOM_MOUSE_OUT;
            evt1.d.domEvent.node = old;
            
            ret = doc_processDomEvent(page->doc, &evt1, N_NULL);
        }
    }
    
    return ret;
#endif
}

bd_bool doc_processDomEvent(NDocument* doc, NEvent* event1, NEvent* event2)
{
    bd_bool ret = N_FALSE;
    int32 ia;
    uint32 mod;
    NNode* node;
    
    node = (NNode*)event1->d.domEvent.node;
    ia = attr_getValueInt32(node->atts, ATTID_TC_IA);
    mod = node_getEventTypeForIa(event1);
    if (ia == -1 || !((uint32)ia & mod))
        event1 = N_NULL;
    
    if (event2) {
        node = (NNode*)event2->d.domEvent.node;
        ia = attr_getValueInt32(node->atts, ATTID_TC_IA);
        mod = node_getEventTypeForIa(event2);
        if (ia == -1 || !((uint32)ia & mod))
            event2 = N_NULL;
    }
    
    if (event1 && event2) {
        node_active_with_ia(doc, event1, event2);
        ret = N_TRUE;
    }
    else if (event1) {
        node_active_with_ia(doc, event1, N_NULL);
        ret = N_TRUE;
    }
    else if (event2) {
        node_active_with_ia(doc, event2, N_NULL);
        ret = N_TRUE;
    }
    
    return ret;
}

char* doc_getFrameName(NDocument* doc)
{
    NPage* page = (NPage*)doc->page;
    NNode* n = node_getByTag(doc->root, TAGID_BODY, page->view->path);
    if (n) {
        char* name = attr_getValueStr(n->atts, ATTID_NAME);
        return name;
    }
    else
        return N_NULL;
}

bd_bool doc_doIncOp(NDocument* doc, NEIncOpType op, char* xpath)
{
    NPage* page = (NPage*)doc->page;
    NXPathCell* xp = xpath_parse(xpath, -1);
    NNode* slibing = N_NULL;
    NNode* parent = N_NULL;

    doc->modNode = N_NULL;

    if (xp == N_NULL) {
        doc->incOp = NEINCOP_NONE;
        return N_FALSE;
    }
    
    doc->modNode = dom_query(doc, xp, &slibing, &parent, (op == NEINCOP_INSERT) ? 1 : 0);
    xpath_free(&xp);
    
    if (op == NEINCOP_INSERT && (doc->modNode || slibing || parent)) {
        if (doc->modNode == N_NULL) {
            if (slibing) {
                doc->modNode = slibing;
                doc->incOp = NEINCOP_INSERT_AFTER;
            }
            else {
                doc->modNode = parent;
                doc->incOp = NEINCOP_INSERT_AS_CHILD;
            }
        }
        else {
            doc->incOp = NEINCOP_INSERT;
        }
    }
    else if (doc->modNode == N_NULL) {
        doc->incOp = NEINCOP_NONE;
#if DEBUG_INC_OP
        dump_char(page, "doc-inc-op: not found!", -1);
        dump_int(page, op);
        dump_char(page, xpath, -1);
        dump_return(page);
#endif
        return N_FALSE;
    }
    else if (doc->modNode->render == N_NULL) {
#if DEBUG_INC_OP
        dump_char(page, "doc-inc-op: no render!", -1);
        dump_int(page, op);
#endif
        if (   doc->modNode->id == TAGID_OPTION
            || doc->modNode->id == TAGID_FRAME) {
#if DEBUG_INC_OP
            dump_char(page, "delete node only", -1);
        dump_return(page);
#endif
        }
        else {
#if DEBUG_INC_OP
            dump_char(page, "skip", -1);
            dump_return(page);
#endif
            doc->incOp = NEINCOP_NONE;
            doc->modNode = N_NULL;
            return N_FALSE;
        }
    }
    
    switch (op) {
    case NEINCOP_DELETE:
    {
//        dump_dom_tree(page, doc->root);
        rtree_delete_node(page->view, (NRenderNode*)doc->modNode->render);
        dom_delete_node(doc, doc->modNode);
        doc->incOp = NEINCOP_NONE;
        doc->modNode = N_NULL;
//        dump_dom_tree(page, doc->root);
        break;
    }
    case NEINCOP_REPLACE:
    {
        doc->incOp = NEINCOP_REPLACE;
        break;
    }
    default:
        break;
    }
    
#if DEBUG_INC_OP
    dump_char(page, "doc-inc-op: successed!", -1);
    dump_int(page, doc->incOp);
    dump_char(page, xpath, -1);
    dump_return(page);
#endif
    
    return N_TRUE;
}

void doc_doIncOpLastWork(NDocument* doc, void* incPage)
{
    NPage* mp = (NPage*)doc->page;
    NPage* ip = (NPage*)incPage;
    
    switch (doc->incOp) {
    case NEINCOP_INSERT:
    case NEINCOP_INSERT_AFTER:
    {
        bd_bool before = (doc->incOp == NEINCOP_INSERT) ? 1 : 0;
//        dump_dom_tree(mp, doc->root);
        if (rtree_insert_node((NRenderNode*)doc->modNode->render, ip->view->root->child, before))
            ip->view->root->child = N_NULL;
        if (dom_insert_node(doc->modNode, ip->doc->root->child, before))
            ip->doc->root->child = N_NULL;
//        dump_dom_tree(mp, doc->root);
//        view_dump_render_tree_2(mp->view, mp->view->root);
        doc->incOp = NEINCOP_NONE;
        doc->modNode = N_NULL;
        break;
    }
    case NEINCOP_INSERT_AS_CHILD:
    {
        if (rtree_insert_node_as_child((NRenderNode*)doc->modNode->render, ip->view->root->child))
            ip->view->root->child = N_NULL;
        if (dom_insert_node_as_child(doc->modNode, ip->doc->root->child))
            ip->doc->root->child = N_NULL;
//        dump_dom_tree(mp, doc->root);
        doc->incOp = NEINCOP_NONE;
        doc->modNode = N_NULL;
        break;
    }
    case NEINCOP_REPLACE:
    {
        if (rtree_replace_node(mp->view, (NRenderNode*)doc->modNode->render, ip->view->root->child))
            ip->view->root->child = N_NULL;
        if (dom_replace_node(doc, doc->modNode, ip->doc->root->child))
            ip->doc->root->child = N_NULL;
        doc->incOp = NEINCOP_NONE;
        doc->modNode = N_NULL;
        break;
    }
    } // switch
}

const wchr nbk_key_prev[] = {0x4E0A, 0x0000}; // shang
const wchr nbk_key_next[] = {0x4E0B, 0x0000}; // xia
const wchr nbk_key_page[] = {0x9875, 0x0000}; // ye
const wchr nbk_key_prev_2[] = {0x2039, 0x2039, 0x0000};
const wchr nbk_key_next_2[] = {0x203a, 0x203a, 0x0000};
const char nbk_bbs_newthread[] = "action=newthread";
const char nbk_bbs_reply[] = "action=reply";

const int minKeylen = 2;
const int maxKeylen = 6;

typedef struct _NTaskFindShortcut {
    bd_bool    absLayout : 1;
    bd_bool    prevPage : 1;
    bd_bool    nextPage : 1;
    bd_bool    newthread : 1;
    bd_bool    reply : 1;
    NSList* lst;
    NPage*  page;
} NTaskFindShortcut;

static int find_shortcut(NNode* node, void* user, bd_bool* ignore)
{
    if (node->id == TAGID_A) {
        NTaskFindShortcut* task = (NTaskFindShortcut*)user;
        NNode* text = N_NULL;
        char* href = attr_getValueStr(node->atts, ATTID_HREF);
        bd_bool found = N_FALSE;

        // check hyperlink
        if (href) {
            if (!task->newthread && !found) {
                if (nbk_strfind(href, nbk_bbs_newthread) > 0 ) {
                    task->newthread = 1;
                    found = N_TRUE;
                }
            }
            if (!task->reply && !found) {
                if (nbk_strfind(href, nbk_bbs_reply) > 0) {
                    task->reply = 1;
                    found = N_TRUE;
                }
            }
        }
        
        // get text of hyperlink
        if (task->absLayout) {
            NNode* span = (node->child && node->child->id == TAGID_SPAN) ? node->child : N_NULL;
            if (span && span->child && span->child->id == TAGID_TEXT)
                text = span->child;
        }
        else {
            if (node->child && node->child->id == TAGID_TEXT)
                text = node->child;
        }

        if (text && text->len >= minKeylen && text->len <= maxKeylen) {
            if (   !task->prevPage
                && !found
                && NBK_wcsfind(text->d.text, nbk_key_prev) == 0
                && NBK_wcsfind(text->d.text, nbk_key_page) > 0 ) {
                task->prevPage = 1;
                found = N_TRUE;
            }
            if (   !task->nextPage
                && !found
                && NBK_wcsfind(text->d.text, nbk_key_next) == 0
                && NBK_wcsfind(text->d.text, nbk_key_page) > 0 ) {
                task->nextPage = 1;
                found = N_TRUE;
                    }
            
            if (   !task->prevPage
                && !found
                && NBK_wcsfind(text->d.text, nbk_key_prev_2) == 0 ) {
                task->prevPage = 1;
                found = N_TRUE;
            }
            if (   !task->nextPage
                && !found
                && NBK_wcsfind(text->d.text, nbk_key_next_2) == 0 ) {
                task->nextPage = 1;
                found = N_TRUE;
                    }
                }
        
        if (found) {
            sll_append(task->lst, node);
            
//            dump_char(task->page, "find shortcut", -1);
//            dump_char(task->page, href, -1);
//            dump_return(task->page);
            }

        *ignore = N_TRUE;
                }
    
    return 0;
            }

void doc_buildShortcutLst(NDocument* doc)
{
    if (doc && doc->shortcutLst == N_NULL) {
        NTaskFindShortcut task;
        
        doc->shortcutLst = sll_create();
        
        NBK_memset(&task, 0, sizeof(NTaskFindShortcut));
        task.lst = doc->shortcutLst;
        task.absLayout = doc_isAbsLayout(doc->type);
        task.page = (NPage*)doc->page;
        
        node_traverse_depth(doc->root, find_shortcut, N_NULL, &task);
    }
}

NSList* doc_getShortcurLst(NDocument* doc)
{
    return (doc) ? doc->shortcutLst : N_NULL;
    }

wchr* doc_getShortcutNodeText(NDocument* doc, NNode* node, int* len)
{
    wchr* nbk_label_newthread = L"\x65b0\x8d34";
    wchr* nbk_label_reply = L"\x56de\x590d";
    wchr* nbk_label_prev_page = L"\x4e0a\x4e00\x9875";
    wchr* nbk_label_next_page = L"\x4e0b\x4e00\x9875";
    
    NNode* text = N_NULL;
    char* href = attr_getValueStr(node->atts, ATTID_HREF);
    
    if (href) {
        if (nbk_strfind(href, nbk_bbs_newthread) > 0) {
            *len = 2;
            return nbk_label_newthread;
        }
        if (nbk_strfind(href, nbk_bbs_reply) > 0) {
            *len = 2;
            return nbk_label_reply;
        }
}

    if (doc_isAbsLayout(doc->type)) {
        NNode* span = (node->child && node->child->id == TAGID_SPAN) ? node->child : N_NULL;
        if (span && span->child && span->child->id == TAGID_TEXT)
            text = span->child;
        }
    else {
        if (node->child && TAGID_TEXT == node->child->id)
            text = node->child;
    }

    if (text && text->len) {
        if (NBK_wcsfind(text->d.text, nbk_key_prev_2) == 0) {
            *len = 3;
            return nbk_label_prev_page;
        }
        if (NBK_wcsfind(text->d.text, nbk_key_next_2) == 0) {
            *len = 3;
            return nbk_label_next_page;
    }

        *len = text->len;
        return text->d.text;
    }

    *len = 0;
    return N_NULL;
}

void doc_updateParam(NDocument* doc, char** pairs)
{
    NPage* page = (NPage*)doc->page;
    NNode** sta = page->view->path;
    NNode* head;
    nid tags[] = {TAGID_PARAM, 0};
    NSList* lst;
    NNode* n;
    int i;
    
//    for (i=0; pairs[i]; i += 2) {
//        dump_char(page, pairs[i], -1);
//        dump_char(page, pairs[i+1], -1);
//        dump_return(page);
//    }
    
    head = node_getByTag(doc->root, TAGID_HEAD, sta);
    if (head == N_NULL)
        return;
    
    lst = node_getListByTag(head, tags, sta);
    n = sll_first(lst);
    if (n) {
        // replace
        while (n) {
            char* name = attr_getValueStr(n->atts, ATTID_ID);
            if (name) {
                for (i=0; pairs[i]; i += 2) {
                    if (NBK_strcmp(name, pairs[i]) == 0) {                       
                        char* old = attr_getValueStr(n->atts, ATTID_VALUE);
                        if (old == N_NULL || NBK_strcmp(old, pairs[i+1]))
                        attr_setValueStr(n->atts, ATTID_VALUE, doc->spool, pairs[i+1]);
                        break;
                    }
                }
            }
            n = sll_next(lst);
        }
    }
    else {
        // create a new param
        NNode* param = (NNode*)NBK_malloc0(sizeof(NNode));
        NAttr* atts = (NAttr*)NBK_malloc0(sizeof(NAttr) * 3);
        
        param->id = TAGID_PARAM;
        param->atts = atts;
        param->ownAtts = 1;
        atts[0].id = ATTID_ID;
        atts[0].d.ps = strPool_save(doc->spool, pairs[0], -1);
        atts[1].id = ATTID_VALUE;
        atts[1].d.ps = strPool_save(doc->spool, pairs[1], -1);
        
        // find insert place
        if (head->child) {
            n = head->child;
            while (n->next)
                n = n->next;

            n->next = param;
            param->prev = n;
            param->parent = head;
        }
        else {
            head->child = param;
            param->parent = head;
        }
    }
    sll_delete(&lst);
}

typedef struct _NTaskJumpFindRet {
    char*   name;
    NNode*  node;
    coord   x;
    coord   y;
} NTaskJumpFindRet;

static int jump_find_anchor(NNode* node, void* user, bd_bool* ignore)
{
    NTaskJumpFindRet* find = (NTaskJumpFindRet*)user;
    
    if (find->node) {
        if (node->render) {
            NRenderNode* r = (NRenderNode*)node->render;
            NRect rr;
            renderNode_absRect(r, &rr);
            find->x = rr.l;
            find->y = rr.t;
            return 1;
        }
    }
    else if (node->id == TAGID_A) {
        char* name = attr_getValueStr(node->atts, ATTID_NAME);
        if (name == N_NULL)
            name = attr_getValueStr(node->atts, ATTID_ID);
        if (name && NBK_strcmp(find->name, name) == 0)
            find->node = node;
    }
    
    return 0;
}

void doc_jumpTo(NDocument* doc, const char* name)
{
    NPage* page = (NPage*)doc->page;
    NTaskJumpFindRet find;

    if (NBK_strcmp(name, "top") == 0) {
        find.node = doc->root;
        find.x = find.y = 0;
    }
    else {
        find.name = (char*)name;
        find.node = N_NULL;
        find.x = find.y = -1;
        node_traverse_depth(doc->root, jump_find_anchor, N_NULL, &find);
    }
    
    if (find.node && find.x >= 0 && find.y >= 0) {
        NBK_Event_RePosition evt;
        evt.x = find.x;
        evt.y = find.y;
        evt.zoom = history_getZoom((NHistory*)page->history);
        nbk_cb_call(NBK_EVENT_REPOSITION, &page->cbEventNotify, &evt);
    }
}

NStrPool* doc_getStrPool(NDocument* doc)
{
    return doc->spool;
}

void doc_setStrPool(NDocument* doc, NStrPool* spool, bd_bool own)
{
    if (doc->ownStrPool && doc->spool) {
        strPool_reset(doc->spool);
        strPool_delete(&doc->spool);
    }
    
    doc->spool = spool;
    doc->ownStrPool = own;
}

typedef struct _NMsgwinCmd {
    NDocument*  doc;
    int         type;
    wchr*       text;
    int         len;
} NMsgwinCmd;

static void doc_do_msgwin(void* user)
{
    NMsgwinCmd* cmd = (NMsgwinCmd*)user;
    NPage* page = (NPage*)cmd->doc->page;
    
    tim_stop(cmd->doc->loadTimer);
    tim_setCallback(cmd->doc->loadTimer, doc_do_load, N_NULL); // restore
    
    if (cmd->type == 1) {
        // alert
        NBK_dlgAlert(page->platform, cmd->text, cmd->len);
    }
    else if (cmd->type == 2) {
        // confirm
        int ret = 0;
        if (NBK_dlgConfirm(page->platform, cmd->text, cmd->len, &ret)) {
            NNode* focus = (NNode*)view_getFocusNode(page->view);
            if (focus) {
                NEvent evt;
                NBK_memset(&evt, 0, sizeof(NEvent));
                evt.type = NEEVENT_DOM;
                evt.page = page;
                evt.d.domEvent.type = NEDOM_DIALOG;
                evt.d.domEvent.node = focus;
                evt.d.domEvent.d.dlg.type = 2;
                evt.d.domEvent.d.dlg.choose = ret;
                node_active_with_ia(page->doc, &evt, N_NULL);
            }
        }
    }
    else if (cmd->type == 3) {
        // prompt
        int ret = 0;
        char* input = N_NULL;
        if (NBK_dlgPrompt(page->platform, cmd->text, cmd->len, &ret, &input)) {
            NNode* focus = (NNode*)view_getFocusNode(page->view);
            if (focus) {
                NEvent evt;
                NBK_memset(&evt, 0, sizeof(NEvent));
                evt.type = NEEVENT_DOM;
                evt.page = page;
                evt.d.domEvent.type = NEDOM_DIALOG;
                evt.d.domEvent.node = focus;
                evt.d.domEvent.d.dlg.type = 3;
                evt.d.domEvent.d.dlg.choose = ret;
                if (ret)
                    evt.d.domEvent.d.dlg.input = input;
                node_active_with_ia(page->doc, &evt, N_NULL);
            }
            if (input)
                NBK_free(input);
        }
    }
    
    if (cmd->text)
        NBK_free(cmd->text);
    NBK_free(cmd);
}

void doc_doMsgWin(NDocument* doc, int type, char* message)
{
    NMsgwinCmd* cmd = (NMsgwinCmd*)NBK_malloc0(sizeof(NMsgwinCmd));
    
    cmd->doc = doc;
    cmd->type = type;
    if (message)
        cmd->text = uni_utf8_to_utf16_str((uint8*)message, -1, &cmd->len);
    
    tim_setCallback(doc->loadTimer, doc_do_msgwin, cmd);
    tim_start(doc->loadTimer, 5, 10000000);
}

#if DUMP_FUNCTIONS
static int output_xml_start_tag(NNode* node, void* user, bd_bool* ignore)
{
    if (node->id != TAGID_TEXT) {
        NPage* page = (NPage*)user;
        char tag[MAX_TAG_LEN + 1];
        int i;
        tag[0] = '<';
        NBK_strcpy(&tag[1], xml_getTagNames()[node->id]);
        i = NBK_strlen(tag);
        if (node_is_single(node->id))
            tag[i++] = '/';
        tag[i++] = '>';
        tag[i] = 0;
        dump_char(page, tag, i);
    }
    return 0;
}

static int output_xml_end_tag(NNode* node, void* user)
{
    NPage* page = (NPage*)user;
    char tag[MAX_TAG_LEN + 1];
    int i;
    tag[0] = '<';
    tag[1] = '/';
    NBK_strcpy(&tag[2], xml_getTagNames()[node->id]);
    i = NBK_strlen(tag);
    tag[i++] = '>';
    tag[i] = 0;
    dump_char(page, tag, i);
    return 0;
}
#endif

void doc_outputXml(NDocument* doc)
{
#if DUMP_FUNCTIONS
    NPage* page = (NPage*)doc->page;
    dump_tab(page, N_FALSE);
    dump_return(page);
    dump_return(page);
    node_traverse_depth(doc->root, output_xml_start_tag, output_xml_end_tag, page);
    dump_return(page);
    dump_return(page);
    dump_tab(page, N_TRUE);
    dump_flush(page);
#endif
}

bd_bool doc_isAbsLayout(const nid type)
{
    if (   type == NEDOC_FULL
        || type == NEDOC_FULL_XHTML
        || type == NEDOC_NARROW)
        return N_TRUE;
    else
        return N_FALSE;
}

void doc_attachWbxmlDecoder(NDocument* doc, int length)
{
    if (doc->wbxmlDecoder)
        doc_detachWbxmlDecoder(doc);
    
    doc->wbxmlDecoder = wbxmlDecoder_create();
    doc->wbxmlDecoder->page = doc->page; // for log
    wbxmlDecoder_setSourceLength(doc->wbxmlDecoder, length);
}

void doc_detachWbxmlDecoder(NDocument* doc)
{
    if (doc->wbxmlDecoder)
        wbxmlDecoder_delete(&doc->wbxmlDecoder);
}


///////////////////////////////////////////////////////////////////////////////
// interfaces used by 3rd partly

static wchr** split_keywords(wchr* keywords)
{
    wchr** names;
    wchr* p;
    int i;
    
    p = keywords;
    i = 1;
    while (*p) {
        if (*p == ',')
            i++;
        p++;
    }
    
    names = (wchr**)NBK_malloc0(sizeof(wchr*) * (i + 1));
    i = 0;
    p = keywords;
    while (*p) {
        if (*p == ',') {
            *p = 0;
            i++;
        }
        else if (names[i] == N_NULL) {
            names[i] = p;
        }
        p++;
    };
    
    return names;
}

void DOC_setConfirmKeywords(NDocument* doc, wchr* keywords)
{
    doc_delete_keywords(doc);
    
    if (keywords == N_NULL)
        return;
    
    doc->ssKeywords = (wchr*)NBK_malloc(sizeof(wchr) * (NBK_wcslen(keywords) + 1));
    NBK_wcscpy(doc->ssKeywords, keywords);
    doc->ssKeyList = split_keywords(doc->ssKeywords);
}

typedef struct _NTask3FindKeys {
    NSList* lst;
    wchr** names;
} NTask3FindKeys;

static int task3_find_keys(NNode* node, void* user, bd_bool* ignore)
{
    NTask3FindKeys* task = (NTask3FindKeys*)user;
    
    if (node->id == TAGID_TEXT) {
        int i;
        bd_bool found = N_FALSE;
        for (i = 0; task->names[i]; i++) {
            if (NBK_wcsfind(node->d.text, task->names[i]) != -1) {
                found = N_TRUE;
                break;
            }
        }
        if (found) {
            NNode* n = node->parent;
            while (n) {
                if (n->id == TAGID_A || n->id == TAGID_ANCHOR) {
                    NBK_NODE* nn = (NBK_NODE*)NBK_malloc0(sizeof(NBK_NODE));
                    nn->type = NEPUBDATA_NODE;
                    nn->p = n;
                    sll_append(task->lst, nn);
                    break;
                }
                n = n->parent;
            }
        }
    }
    
    return 0;
}

NSList* DOC_getNodesByKeywords(NDocument* doc, wchr* keywords)
{
    NSList* lst = N_NULL;
    wchr** names = N_NULL;
    wchr *p, *toofar;
    NTask3FindKeys task;
    
    if (keywords == N_NULL)
        return N_NULL;

    // split keywords
    p = keywords;
    toofar = p + NBK_wcslen(p);
    names = split_keywords(keywords);
    
    // searching
    lst = sll_create();
    task.lst = lst;
    task.names = names;
    node_traverse_depth(doc->root, task3_find_keys, N_NULL, &task);
    
    // restore keywords
    p = keywords;
    while (p < toofar) {
        if (*p == 0)
            *p = ',';
        p++;
    }
    
    NBK_free(names);
    
    return lst;
}
