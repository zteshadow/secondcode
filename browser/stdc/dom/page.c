/*
 * page.c
 *
 *  Created on: 2010-12-25
 *      Author: wuyulun
 */

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_callback.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "page.h"
#include "node.h"
#include "attr.h"
#include "token.h"
#include "xml_helper.h"
#include "xml_tags.h"
#include "xml_atts.h"
#include "history.h"
#include "../loader/loader.h"
#include "../loader/url.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../tools/slist.h"
#include "../render/imagePlayer.h"
#include "../render/renderNode.h"
#include "../render/renderInput.h"

#define DEBUG_ATTACH        0
#define DEBUG_INC_OP        0
#define TEST_PAINT_CONSUME  0

#ifdef NBK_MEM_TEST
int page_memUsed(const NPage* page)
{
    int size = 0;
    if (page) {
        size = sizeof(NPage);
        size += doc_memUsed(page->doc);
        size += view_memUsed(page->view);
        if (page->main)
            size += sheet_memUsed(page->sheet);
        if (page->main && page->subPages) {
            int c = 0;
            NPage* p = sll_first(page->subPages);
            while (p) {
                c++;
                size += page_memUsed(p);
                p = sll_next(page->subPages);
            }
            
            dump_char((void*)page, "sub-page count:", -1);
            dump_int((void*)page, c);
            dump_return((void*)page);
        }
    }
    return size;
}

void mem_checker(const NPage* page)
{
    if (page) {
        int size = page_memUsed(page);
        
        dump_char((void*)page, "memory total:", -1);
        dump_int((void*)page, size);
        dump_return((void*)page);
        dump_flush((void*)page);
    }
}

#endif

int page_testApi(NPage* page)
{
//    uint32 n32 = 0xaabbccdd;
//    uint16 n16 = 0xaabb;
//    n32 = N_SWAP_UINT32(n32);
//    n16 = N_SWAP_UINT16(n16);
    return 0;
}

NPage* page_create(NPage* parent)
{
    NPage* p = NBK_malloc0(sizeof(NPage));
    N_ASSERT(p);
    
    p->main = (parent) ? 0 : 1;
    p->parent = parent;
    p->begin = 1;
    p->attach = (parent) ? 0 : 1;
    if (p->main)
        p->sheet = sheet_create(p);
    p->doc = doc_create(p);
    p->view = view_create(p);
    
    return p;
}

static void page_reset(NPage* page)
{
//    page->total = -1;
    page->rece = 0;

    if (page->subPages) {
        NPage* t = sll_first(page->subPages);
        while (t) {
            page_delete(&t);
            t = sll_next(page->subPages);
        }
        sll_delete(&page->subPages);
    }
}

void page_delete(NPage** page)
{
    NPage* p = *page;

    if (p->main) {
        sheet_delete(&p->sheet);
        if (p->incPage)
            page_delete(&p->incPage);
    }
    view_delete(&p->view);
    doc_delete(&p->doc);

    page_reset(p);
    
    NBK_free(p);
    *page = N_NULL;
}

void page_setScreenWidth(NPage* page, coord width)
{
    page->view->scrWidth = width;
}

NUpCmdSet page_decideRequestMode(NPage* page, const char* url, bd_bool submit)
{
    NUpCmdSet settings;
    
    settings.image = page->settings->allowImage;
    settings.incMode = page->settings->allowIncreateMode;
    
#if NBK_REQ_IMG_ALONE
    settings.image = 0;
#endif
    
    if (page->doc->root && !page->doc->finish)
        settings.incMode = 0;
    
    if (page->settings->support != NESupport) {
        if (page->settings->support == NENotSupportByFlyflow)
            settings.via = NEREV_STANDARD;
        else
            settings.via = page->settings->mode;
        page->settings->support = NESupport;
    }
    else if (submit) {
        settings.via = page->mode;
        if (page->doc->type == NEDOC_FULL_XHTML)
            settings.via = NEREV_FF_FULL_XHTML;
    }
    else {
        if (url && nbk_strfind(url, "file://") == 0) {
            settings.via = NEREV_STANDARD;
        }
        else if (url && url_inWapWhiteList(url)) {
            settings.via = NEREV_FF_FULL_XHTML;
        }
        else if (url && url_checkPhoneSite(url)) {
            settings.via = NEREV_STANDARD;
        }
        else {
            if (page->settings->initMode == NEREV_STANDARD)
                settings.via = NEREV_STANDARD;
            else
                settings.via = page->settings->mode;
        }
        
        // fixme: guess is a picture
        if (url) {
            int len = NBK_strlen(url);
            if (len > 4) {
                char* ext = (char*)url + len - 4;
                if (   nbk_strfind(ext, ".jpg") == 0
                    || nbk_strfind(ext, ".png") == 0
                    || nbk_strfind(ext, ".gif") == 0 )
                    settings.via = NEREV_STANDARD;
            }
        }
    }
    
//    dump_char(page, "decide access type", -1);
//    dump_char(page, (char*)url, -1);
//    dump_int(page, settings.via);
//    dump_return(page);
    
    return settings;
}

void page_loadData(NPage* page, const char* url, const char* str, int length)
{
    loader_stopAll(page->platform, page_getStopId(page));
    view_stop(page->view);
    doc_stop(page->doc);
    nbk_cb_call(NBK_EVENT_NEW_DOC_BEGIN, &page->cbEventNotify, N_NULL);
    
    page_reset(page);
    if (page->doc->root) {
        history_add((NHistory*)page->history);
    }
    sheet_reset(page->sheet);
    doc_begin(page->doc);
    doc_setUrl(page->doc, url);
    doc_write(page->doc, str, length, N_TRUE);
    doc_end(page->doc);
    
//    sheet_dump(page->sheet);
}

static void page_load_url_normal(
    NPage* page, const char* url, nid enc, char* body, NFileUpload** files)
{
    NRequest* req;
    char *dstUrl, *referer = N_NULL;
    
    loader_stopAll(page->platform, page_getStopId(page));
    view_stop(page->view);
    doc_stop(page->doc);
    nbk_cb_call(NBK_EVENT_NEW_DOC, &page->cbEventNotify, N_NULL);
    
    dstUrl = (char*)NBK_malloc(NBK_strlen(url) + 1);
    NBK_strcpy(dstUrl, url);
    if (page->doc->url) {
        int l = NBK_strlen(page->doc->url);
        if (l) {
            referer = (char*)NBK_malloc(l + 1);
            NBK_strcpy(referer, page->doc->url);
        }
    }
    
    page->begin = 1;
    req = loader_createRequest();
    req->enc = enc;
    req->body = body;
    if (files) {
        req->files = *files; *files = N_NULL;
    }
    if (page->refresh) {
        req->pageId = page->id;
        req->method = NEREM_REFRESH;
    }
    else
        req->pageId = (page->doc->root) ? history_getPreId((NHistory*)page->history) : page->id;
    loader_setRequest(req, NEREQT_MAINDOC, N_NULL, page);
    loader_setRequestUrl(req, dstUrl, N_TRUE);
    if (referer) loader_setRequestReferer(req, referer, N_TRUE);
    req->responseCallback = page_onResponse;
    req->platform = page->platform;
        if (!loader_load(req))
            loader_deleteRequest(&req);
    }

void page_loadUrl(
    NPage* page, const char* url, nid enc, char* body, NFileUpload** files, bd_bool submit)
{
    NUpCmdSet settings = page_decideRequestMode(page, url, submit);
    
    if (settings.via == NEREV_STANDARD) {
        page_load_url_normal(page, url, enc, body, files);
    }
    else if (   settings.via == NEREV_FF_FULL
             || settings.via == NEREV_FF_FULL_XHTML
             || settings.via == NEREV_FF_NARROW
             || settings.via == NEREV_UCK ) {
    
        NUpCmd* upcmd = upcmd_create(&settings);
    upcmd_url(upcmd, url, N_NULL);
        if (settings.via == NEREV_FF_FULL_XHTML)
            settings.via = NEREV_FF_FULL;
        page_loadUrl_ff(page, &upcmd, url, N_NULL, settings.via, N_FALSE);
    if (upcmd)
        upcmd_delete(&upcmd);
}
}

// call by ia
void page_loadUrl_ff(
    NPage* page, NUpCmd** cmd, const char* url, NFileUpload** files, nid via, bd_bool noDeal)
{
    NRequest* req;
    char* u = N_NULL;
    
    if (url) {
        u = (char*)NBK_malloc(NBK_strlen(url) + 1);
        NBK_strcpy(u, url);
    }
    
    loader_stopAll(page->platform, page_getStopId(page));
    view_stop(page->view);
    doc_stop(page->doc);
    page->incMode = page->disSubpage = 0;
    nbk_cb_call(NBK_EVENT_NEW_DOC, &page->cbEventNotify, N_NULL);
    if (upcmd_isIa(*cmd))
        nbk_cb_call(NBK_EVENT_DO_IA, &page->cbEventNotify, N_NULL);
    
    page->begin = 1;
    req = loader_createRequest();
    if (page->refresh) {
        req->pageId = page->id;
        req->method = NEREM_REFRESH;
    }
    else
        req->pageId = (page->doc->root) ? history_getPreId((NHistory*)page->history) : page->id;
    loader_setRequest(req, NEREQT_MAINDOC, N_NULL, page);
    req->via = via;
    req->noDeal = noDeal;
    req->upcmd = *cmd; *cmd = N_NULL;
    if (files) {
        req->files = *files; *files = N_NULL;
    }
    if (u)
        loader_setRequestUrl(req, u, N_TRUE);
    req->responseCallback = page_onResponse;
    req->platform = page->platform;
    if (!loader_load(req))
        loader_deleteRequest(&req);
}

void page_loadFromCache(NPage* page)
{
    NRequest* req;
    
    view_stop(page->view);
    doc_stop(page->doc);
    nbk_cb_call(NBK_EVENT_HISTORY_DOC, &page->cbEventNotify, N_NULL);
    
    page->begin = 1;
    page->cache = 1;
    req = loader_createRequest();
    loader_setRequest(req, NEREQT_MAINDOC, N_NULL, page);
    req->method = NEREM_HISTORY;
    req->pageId = page->id;
    req->responseCallback = page_onResponse;
    req->platform = page->platform;
    if (!loader_load(req))
        loader_deleteRequest(&req);
}

void page_refresh(NPage* page)
{
    if (page->main && doc_getUrl(page->doc)) {
        page->refresh = 1;
        page_loadUrl(page, doc_getUrl(page->doc), 0, N_NULL, N_NULL, N_FALSE);
    }
}

coord page_width(NPage* page)
{
    NFloat zoom = history_getZoom((NHistory*)page->history);
    coord width = (coord)nfloat_imul(view_width(page->view), zoom);
    return width;
}

coord page_height(NPage* page)
{
    NFloat zoom = history_getZoom((NHistory*)page->history);
    coord height = (coord)nfloat_imul(view_height(page->view), zoom);
    return height;
}

bd_bool page_4ways(NBK_Page* page)
{
    return (page->doc->type == NEDOC_FULL /*|| page->doc->type == NEDOC_NARROW*/);
}

void page_paint(NPage* page, NRect* rect)
{
#if TEST_PAINT_CONSUME
    double t_time = NBK_currentMilliSeconds();
#endif
    NRect r;
    
    if (!page->doc->layoutFinish)
        return;
    
    if (page->workState != NEWORK_IDLE)
        return;
    
        // initialize background
        r.l = r.t = 0;
        r.r = rect_getWidth(rect);
        r.b = rect_getHeight(rect);
        NBK_gdi_setBrushColor(page->platform, &colorWhite);
        NBK_gdi_clearRect(page->platform, &r);
    
    if (view_isZindex(page->view)) {
        view_paintWithZindex(page->view, rect);
        view_paintFocusWithZindex(page->view, rect);
    }
    else {
        view_paint(page->view, rect);
        if (page->subPages) {
            NPage* p = sll_first(page->subPages);
            while (p) {
                if (p->attach)
                    view_paint(p->view, rect);
                p = sll_next(page->subPages);
            }
        }
        view_paintFocus(page->view, rect);
    }
    
    view_paintControl(page->view, rect); // fixme: z-index?
    
#if TEST_PAINT_CONSUME
    if (page->doc->finish) {
        dump_char(page, "page paint", -1);
        dump_int(page, (int)(NBK_currentMilliSeconds() - t_time));
        dump_NRect(page, rect);
        dump_return(page);
    }
#endif
}

void page_paintControl(NPage* page, NRect* rect)
{
    view_paintControl(page->view, rect);
}

void page_paintMainBodyBorder(NPage* page, NRect* rect, NNode* focus)
{
    bd_bool focused = N_FALSE;
    
    if (page_hasMainBody(page)) {
        NNode* n = sll_first(page->doc->mbList);
        while (n) {
            focused = N_FALSE;
            if (focus == n)
                focused = N_TRUE;

            view_paintMainBodyBorder(page->view, n, rect, focused);
            break;
        }
    }
}

void page_setEventNotify(NPage* page, NBK_CALLBACK func, void* user)
{
    page->cbEventNotify.func = func;
    page->cbEventNotify.user = user;
}

//#define PAGE_TEST_RELEASE
static void page_begin(NPage* page, const char* url)
{
#ifdef PAGE_TEST_RELEASE
    double t_consume;
#endif
    
    if (!page->begin)
        return;
    
#ifdef PAGE_TEST_RELEASE
    t_consume = NBK_currentMilliSeconds();
    dump_char(page, "page clear ===", -1);
    dump_return(page);
#endif

//wuli	bd_lock(&mutex);
    
    page->begin = 0;
    page->disSubpage = 0;
    page->incMode = 0;
    page->pkgOver = 0;
    page_reset(page);
    if (page->main) {
        NHistory* history = (NHistory*)page->history;
        
        if (!page->cache && !page->refresh && page->doc->root)
            history_add(history);
        if (page->refresh) {
            NFloat zoom = {1, 0};
            history_setZoom(history, zoom);
        }

        sheet_reset(page->sheet);
    }
    doc_begin(page->doc);
    if (url)
        doc_setUrl(page->doc, url);
    
    if (page->wbxml) {
        page->wbxml = 0;
        doc_attachWbxmlDecoder(page->doc, page->total);
    }
    
    nbk_cb_call(NBK_EVENT_NEW_DOC_BEGIN, &page->cbEventNotify, N_NULL);
    page->cache = 0;
    page->refresh = 0;

//wuli	bd_unlock(&mutex);
    
#ifdef PAGE_TEST_RELEASE
    dump_char(page, "page clear", -1);
    dump_int(page, (int)(NBK_currentMilliSeconds() - t_consume));
    dump_return(page);
#endif
}

static bd_bool attach_page(NPage* main, NPage* sub, const char* name)
{
    NSList* lst;
    nid tags[] = {TAGID_FRAME, 0};
    char* tc_name;
    //char* tc_name_esc;
    
    lst = node_getListByTag(main->doc->root, tags, main->view->path);
    if (lst) {
        NNode* n = sll_first(lst);
        while (n) {
            tc_name = attr_getValueStr(n->atts, ATTID_TC_NAME);
            //tc_name_esc = N_NULL;
            if (tc_name) {
//                tc_name_esc = (char*)NBK_malloc(NBK_strlen(tc_name) + 32);
//                url_unescape(tc_name_esc, tc_name, N_FALSE);
#if DEBUG_ATTACH
                {
                    NPage* p = page_getMainPage(sub);
                    dump_char(p, tc_name, -1);
                    dump_return(p);
                }
#endif
            }
            
            if (tc_name && !NBK_strcmp(tc_name, name)) {
                NRect* tc_rect = attr_getRect(n->atts, ATTID_TC_RECT);
                NNode* body = node_getByTag(main->doc->root, TAGID_BODY, main->view->path);
                NRect* body_rect = attr_getRect(body->atts, ATTID_TC_RECT);
                sub->view->frameRect = *tc_rect;
                rect_move(&sub->view->frameRect,
                    sub->view->frameRect.l + body_rect->l,
                    sub->view->frameRect.t + body_rect->t);
                n->child = sub->doc->root;
                sub->attach = 1;
            }
            
//            if (tc_name_esc)
//                NBK_free(tc_name_esc);
            if (sub->attach)
                break;

            n = sll_next(lst);
        }
        sll_delete(&lst);
    }
    
    return (bd_bool)sub->attach;
}

static void page_attach_subdoc(NPage* main, NPage* sub)
{
    char* name;
    
    if (main == sub)
        return;
    
    name = doc_getFrameName(sub->doc);
    if (name == N_NULL)
        return;
    
#if DEBUG_ATTACH
    dump_char(main, "attach", -1);
    dump_char(main, name, -1);
#endif
    
    if (attach_page(main, sub, name)) {
    }
    else if (main->subPages) {
        NPage* p = sll_first(main->subPages);
        while (p) {
            if (attach_page(p, sub, name))
                break;
            p = sll_next(main->subPages);
        }
    }
        
#if DEBUG_ATTACH
    dump_int(main, sub->attach);
    dump_return(main);
#endif
        }
        
// just use with ff-package
static NPage* pkg_get_page_by_index(NPage* page, int index)
{
    NPage* p;
    
    index--;
    
    if (index == 0)
        return page;
            
    index--;
            
    if (page->subPages == N_NULL) {
        page->subPages = sll_create();
            }
    
    p = sll_getAt(page->subPages, index);
    while (p == N_NULL) {
        NPage* np = page_create(page);
        np->sheet = page->sheet;
        np->platform = page->platform;
        np->history = page->history;
        np->settings = page->settings;
//        np->cbEventNotify = page->cbEventNotify; // deny notifcation for sub-page
        
        sll_append(page->subPages, np);
        p = sll_getAt(page->subPages, index);
        }
    return p;
}

static void inc_find_page_to_modify(NPage* page, char* url)
{
    char* u;
    
    page->modPage = N_NULL;
    
    u = doc_getUrl(page->doc);
#if DEBUG_INC_OP
    dump_char(page, "Inc >", -1);
    dump_char(page, url, -1);
    dump_return(page);
    dump_char(page, "    >", -1);
    dump_char(page, u, -1);
    dump_return(page);
#endif
    if (u && !NBK_strcmp(u, url)) {
        page->modPage = page;
        return;
    }
    
    if (page->subPages) {
        NPage* p = sll_first(page->subPages);
        while (p) {
            if (p->attach) {
                u = doc_getUrl(p->doc);
#if DEBUG_INC_OP
                dump_char(page, "    >", -1);
                dump_char(page, u, -1);
                dump_return(page);
#endif
                if (u && !NBK_strcmp(u, url)) {
                    page->modPage = p;
                    return;
                }
            }
            p = sll_next(page->subPages);
        }
    }
}

static void inc_delete_page(NPage* mainPage)
{
    if (mainPage->incPage)
        page_delete(&mainPage->incPage);
}

static void inc_create_page(NPage* mainPage)
{
    inc_delete_page(mainPage);
    mainPage->incPage = page_create(mainPage);
    mainPage->incPage->sheet = mainPage->sheet;
    mainPage->incPage->platform = mainPage->platform;
}

void page_onResponse(NEResponseType type, void* user, void* data, int v1, int v2)
{
    NRequest* req = (NRequest*)user;
    NPage* page = (NPage*)req->user;
    
    switch (type) {
    case NEREST_HEADER:
    {
        NRespHeader* hdr = (NRespHeader*)data;
        if (hdr->mime == NEMIME_DOC_WMLC) {
            page->wbxml = 1;
        }
        page->total = hdr->content_length;
        nbk_cb_call(NBK_EVENT_GOT_RESPONSE, &page->cbEventNotify, N_NULL);
        break;
    }
        
    case NEREST_DATA:
    {
        NBK_Event_GotResponse evtData;
        page_begin(page, req->url);
        page->rece += v2;
        doc_write(page->doc, (char*)data, v1, N_FALSE);
        evtData.received = page->rece;
        evtData.total = page->total;
        nbk_cb_call(NBK_EVENT_GOT_DATA, &page->cbEventNotify, &evtData);
        break;
    }
        
    case NEREST_COMPLETE:
    {
        doc_end(page->doc);
//        sheet_dump(page->sheet);
        break;
    }
        
    case NEREST_ERROR:
    {
        doc_end(page->doc);
        
        switch (v1) {
        case NELERR_FF_NOT_SUPPORT:
            if (!req->noDeal) {
                if (req->via == NEREV_FF_FULL || req->via == NEREV_FF_NARROW) {
                    page->settings->support = NENotSupportByFlyflow;
                if (req->upcmd->node) {
                    NNode* n = (NNode*)req->upcmd->node;
                    char* url = attr_getValueStr(n->atts, ATTID_HREF);
                        if (url)
                            doc_doGet(page->doc, url, N_FALSE, N_TRUE);
                }
                else if (req->url) {
                        doc_doGet(page->doc, req->url, N_FALSE, N_TRUE);
                    }
                }
                    break;
                }
            
        default:
        {
            if (!NBK_handleError(v1)) {
                char* errorPage = NBK_getStockPage((v1 == 404) ? NESPAGE_ERROR_404 : NESPAGE_ERROR);
                doc_doGet(page->doc, errorPage, N_FALSE, N_FALSE);
            nbk_cb_call(NBK_EVENT_LOADING_ERROR_PAGE, &page->cbEventNotify, N_NULL);
            }
            break;
        }
        }
        break;
    }
        
    case NEREST_CANCEL:
    {
        doc_end(page->doc);
        break;
    }
        
    case NEREST_PKG_DATA_HEADER:
    {
        if (!page->disSubpage) {
            NRespHeader* hdr = (NRespHeader*)data;
            NPage* ap = pkg_get_page_by_index(page, req->idx);
            ap->total = hdr->content_length;
            nbk_cb_call(NBK_EVENT_GOT_RESPONSE, &ap->cbEventNotify, N_NULL);
        }
        break;
    }
    
    case NEREST_PKG_DATA:
    {
        if (!page->disSubpage) {
            NBK_Event_GotResponse evtData;
            NPage* ap = pkg_get_page_by_index(page, req->idx);
            page_begin(ap, N_NULL);
            ap->rece += v2;
            doc_write(ap->doc, (char*)data, v1, N_FALSE);
            evtData.received = ap->rece;
            evtData.total = ap->total;
            nbk_cb_call(NBK_EVENT_GOT_DATA, &ap->cbEventNotify, &evtData);
        }
        break;
    }
    
    case NEREST_PKG_DATA_COMPLETE:
    {
        if (!page->disSubpage) {
            NPage* ap = pkg_get_page_by_index(page, req->idx);
            doc_end(ap->doc);
            page_attach_subdoc(page, ap);
        }
        break;
    }
    
    case NEREST_INC_RECEIVE:
    {
        NBK_Event_GotResponse* got = (NBK_Event_GotResponse*)data;
        nbk_cb_call(NBK_EVENT_GOT_DATA, &page->cbEventNotify, got);
        break;
    }
    
    case NEREST_INC_URL:
    {
        if (page->incMode == 0)
            nbk_cb_call(NBK_EVENT_GOT_INC, &page->cbEventNotify, N_NULL);
        
        page->disSubpage = 1;
        page->incMode++;
        view_clearZindex(page->view);
        if (page->incMode > 1) {
            view_buildZindex(page->view);
            nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
        }
        page_setFocusedNode(page, N_NULL);
        inc_find_page_to_modify(page, (char*)data);
//        if (page->modPage && page->modPage->main)
//            doc_outputXml(page->modPage->doc);
        break;
    }
    
    case NEREST_INC_OP_DELETE:
    {
        inc_delete_page(page);
        if (page->modPage) {
            doc_doIncOp(page->modPage->doc, NEINCOP_DELETE, (char*)data);
        }
        break;
    }
    
    case NEREST_INC_OP_INSERT:
    case NEREST_INC_OP_REPLACE:
    {
        NEIncOpType ot = (type == NEREST_INC_OP_INSERT) ? NEINCOP_INSERT : NEINCOP_REPLACE;
        inc_delete_page(page);
        if (page->modPage) {
            if (doc_doIncOp(page->modPage->doc, ot, (char*)data)) {
                inc_create_page(page);
                page_begin(page->incPage, N_NULL);
                doc_setStrPool(page->incPage->doc, doc_getStrPool(page->modPage->doc), N_FALSE);
                page->incPage->mode = page->mode;
                page->incPage->doc->type = page->doc->type;
                page->incPage->id = page->id;
            }
        }
        break;
    }
    
    case NEREST_INC_DATA:
    {
        if (page->incPage) {
            if (page->incPage->doc->root == N_NULL) {
                char body[] = "<body>";
                doc_write(page->incPage->doc, body, NBK_strlen(body), N_FALSE);
            }
            if (page->modPage->doc->modNode->id == TAGID_OPTION)
                page->incPage->doc->inCtrl = 1;
            doc_write(page->incPage->doc, (char*)data, v1, N_FALSE);
        }
        break;
    }
    
    case NEREST_INC_DATA_COMPLETE:
    {
        if (page->incPage) {
            page->incPage->doc->finish = N_TRUE;
            page->incPage->doc->layoutFinish = N_TRUE;
            view_layout(page->incPage->view, page->incPage->view->root, N_FALSE);
            doc_doIncOpLastWork(page->modPage->doc, page->incPage);
            inc_delete_page(page);
        }
        break;
    }
    
    case NEREST_PARAM:
    {
        doc_updateParam(page->doc, (char**)data);
        break;
    }
    
    case NEREST_MSGWIN:
    {
        doc_doMsgWin(page->doc, v2, (char*)data);
        break;
    }
    
    default:
        break;
    }
}

void page_onFFPackageOver(NRequest* request)
{
    NPage* page = (NPage*)request->user;
    
    page->pkgOver = 1;
    
//    dump_char(page, "package over!", -1);
//    dump_return(page);
    
    if (page->incMode) {
        NBK_Event_GotImage evt;
        page->incMode = page->disSubpage = 0;
        view_buildZindex(page->view);
        // decoding new images.
        page->view->imgPlayer->stop = 0;
        imagePlayer_startDecode(page->view->imgPlayer);
        evt.curr = evt.total = 0;
        nbk_cb_call(NBK_EVENT_DOWNLOAD_IMAGE, &page->cbEventNotify, &evt); // fixme: temp
        nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
    }
    else {
#if NBK_REQ_IMG_ALONE
        if (page_isLayoutFinish(page)) {
            if (imagePlayer_total(page->view->imgPlayer) > 0) {
//                dump_char(page, "page: start download images", -1);
//                dump_time(page);
//                dump_return(page);
                imagePlayer_start(page->view->imgPlayer);
            }
            imagePlayer_progress(page->view->imgPlayer);
        }
#endif
    }
}

void page_setPlatformData(NBK_Page* page, void* platform)
{
    page->platform = platform;
}

void page_stop(NPage* page)
{
    loader_stopAll(page->platform, page_getStopId(page));
    view_stop(page->view);
}

void page_enablePic(NPage* page, bd_bool enable)
{
    if (page->view)
        view_enablePic(page->view, enable);
}

void page_setFocusedNode(NPage* page, NNode* node)
{
    if (page->main == N_FALSE)
        return;

    if(page->view->focus == node)
        return;
    
    // if last node focused is in editing, changes the state.
    node_cancelEditing(page->view->focus, page);
    
    page->view->focus = node;
    page->doc->lastGotFocus = node;
    
    if (page->subPages) {
        NPage* p = sll_first(page->subPages);
        while (p) {
            p->view->focus = node;
            p = sll_next(page->subPages);
        }
    }
}

bd_bool page_hasMainBody(NPage* page)
{
    bd_bool has = N_FALSE;
    
    if (page->main) {
        if (page->doc->mbList)
            has = N_TRUE;
    }
    
    return has;
}

typedef struct _NMainBodyCreateTask {
    NPage*  page;
    bd_bool    ignore;
} NMainBodyCreateTask;

static int create_main_body_tag_start(NNode* node, void* user, bd_bool* ignore)
{
    NMainBodyCreateTask* task = (NMainBodyCreateTask*)user;
    NToken token;
    char* cls;
    
    if (task->ignore)
        return 0;
    else if (node->id == TAGID_FORM || node->id == TAGID_FRAME) {
        task->ignore = N_TRUE;
        return 0;
    }
    
    NBK_memset(&token, 0, sizeof(NToken));
    token.id = node->id;
    token.nodeRef = node;
    cls = attr_getValueStr(node->atts, ATTID_CLASS);
    if (cls && !NBK_strcmp(cls, "tc_title")) {
        token.atts = (NAttr*)NBK_malloc0(sizeof(NAttr) * 2);
        token.atts[0].id = ATTID_CLASS;
        token.atts[0].d.ps = cls;
    }
    if (node->len) {
        token.len = node->len;
        token.d.text = node->d.text;
    }
    
    doc_processToken(task->page->doc, &token);
    
    return 0;
}

static int create_main_body_tag_end(NNode* node, void* user)
{
    NMainBodyCreateTask* task = (NMainBodyCreateTask*)user;
    NToken token;
    
    if (task->ignore) {
        if (node->id == TAGID_FORM || node->id == TAGID_FRAME)
            task->ignore = N_FALSE;
        return 0;
    }
    
    NBK_memset(&token, 0, sizeof(NToken));
    token.id = node->id + TAGID_CLOSETAG;
    
    doc_processToken(task->page->doc, &token);
    
    return 0;
}

void page_createMainBody(NPage* page, NNode* root, NPage* mainBody)
{
    NMainBodyCreateTask task;
    
//    doc_dumpDOM(page->doc);

    page_resetMainBody(mainBody);
    
    mainBody->doc->type = NEDOC_MAINBODY;
    mainBody->id = page->id;
    mainBody->view->imgPlayer->pageId = page->id;
    mainBody->cache = 1;
    mainBody->cbEventNotify.func = N_NULL;
    mainBody->doc->syncMode = 1;
    mainBody->history = page->history;
    
    task.page = mainBody;
    task.ignore = N_FALSE;
    node_traverse_depth(root,
        create_main_body_tag_start,
        create_main_body_tag_end,
        &task);
    
    mainBody->doc->syncMode = 0;
    mainBody->doc->finish = 0;
    mainBody->cbEventNotify = page->cbEventNotify;
    doc_end(mainBody->doc);
    
//    doc_dumpDOM(mainBody->doc);
}

void page_resetMainBody(NPage* mainBody)
{
    char* style = (char*)NBK_malloc(512);
    NBK_strcpy(style,
        "* {line-height:150%;color:#333;background:#efefef;} " \
        "div {padding:0px 5px 10px;} " \
        ".tc_title {font-weight:bold;text-align:center;padding:10px;}");
    
    page_reset(mainBody);
    doc_begin(mainBody->doc);
    sheet_parseStyle(mainBody->sheet, style, -1);
    
    NBK_free(style);
}

NNode* page_getMainBodyByPos(NPage* page, coord x, coord y)
{
    if (page_hasMainBody(page)) {
        NNode* n = sll_first(page->doc->mbList);
        while (n) {
            NRenderNode* rn = (NRenderNode*)n->render;
            if (rn && rect_hasPt(&rn->r, x, y))
                return n;
            n = sll_next(page->doc->mbList);
        }
    }
    return N_NULL;
}

bd_bool page_isPaintMainBodyBorder(NPage* page, NRect* rect)
{
    bd_bool ret = N_FALSE;
    rect_toDoc(rect, history_getZoom((NHistory*)page->history));
    
    if (page_hasMainBody(page)) {
        NNode* n = sll_first(page->doc->mbList);
        while (n) {
            NRenderNode* rn = (NRenderNode*)n->render;
            //rn->r.l >= rect->l && rn->r.r <= rect->r
            if (rn && (rect_getWidth(&rn->r) < rect_getWidth(rect) || rect_getHeight(&rn->r)
                < rect_getHeight(rect))) {
                ret = N_TRUE;
                break;
            }
            n = sll_next(page->doc->mbList);
        }
    }
    
    return ret;
}

bd_bool page_isMainBodyMode(NPage* page)
{
    bd_bool ret = N_FALSE;
    
    if (page->doc->type == NEDOC_FULL) {
        NPage* p = history_curr((NHistory*)page->history);
        if (p->doc->type == NEDOC_MAINBODY)
            ret = N_TRUE;
    }
    
    return ret;
}

void page_setId(NPage* page, nid id)
{
    page->id = id;
    page->view->imgPlayer->pageId = id;
}

void page_updateCtrlNode(NPage* page, NNode* node, NRect* rect)
{
    NStyle style;
    NRenderNode* rn = N_NULL;

    if (node) {
        rn = (NRenderNode*) node->render;

        style_init(&style);
        style.view = page->view;
        style.dv = view_getRectWithOverflow(page->view);
        rn->Paint(&page->view->stack, rn, &style, rect);
    }
}

bd_bool page_isCtrlNode(NPage* page, NNode* node)
{
    NRenderNode* rn = N_NULL;
    NRenderInput* ri = N_NULL;

    bd_bool bCtrl = N_FALSE;
    if (node) {
        rn = (NRenderNode*) node->render;

        if (rn && RNT_INPUT == rn->type) {

            ri = (NRenderInput*) rn;
            if (ri) {
                switch (ri->type) {
                case NEINPUT_SUBMIT:
                case NEINPUT_RESET:
                case NEINPUT_BUTTON:
                case NEINPUT_IMAGE:
                case NEINPUT_RADIO:
                case NEINPUT_CHECKBOX:
                    bCtrl = N_TRUE;
                    break;
                }
            }
        }
        else if (rn && RNT_SELECT == rn->type) {
            bCtrl = N_TRUE;
        }
    }

    return bCtrl;
}

NPage* page_getMainPage(NPage* page)
{
    NPage* p = page;
    while (p->parent)
        p = p->parent;
    return p;
}

nid page_getStopId(NPage* page)
{
    NHistory* history = (NHistory*)page->history;
    return (history->preId) ? history->preId : page->id;
}

void page_layout(NPage* page, bd_bool force)
{
    view_layout(page->view, page->view->root, force);
    if (page->subPages) {
        NPage* p = sll_first(page->subPages);
        while (p) {
            view_layout(p->view, p->view->root, force);
            p = sll_next(page->subPages);
        }
    }
    if (doc_isAbsLayout(page->doc->type))
        view_buildZindex(page->view);
}

bd_bool page_layoutTextFF(NPage* page)
{
    bd_bool ret = N_FALSE;
    
    if (page->doc->type == NEDOC_FULL) {
        ret = view_layoutTextFF(page->view);
        if (ret)
            view_buildZindex(page->view);
    }
    
    return ret;
}

bd_bool page_isLayoutFinish(NPage* page)
{
    bd_bool finish = N_TRUE;
    
    if (page->pkgOver) {
        if (!page->doc->layoutFinish) {
            finish = N_FALSE;
        }
        else if (page->subPages) {
            NPage* p = sll_first(page->subPages);
            while (p) {
                if (p->attach && !p->doc->layoutFinish) {
                    finish = N_FALSE;
                    break;
                }
                p = sll_next(page->subPages);
            }
        }
    }
    else
        finish = N_FALSE;
    
    return finish;
}