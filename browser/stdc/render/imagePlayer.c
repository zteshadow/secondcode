/*
 * imagePlayer.c
 *
 *  Created on: 2011-2-21
 *      Author: wuyulun
 */

#include "imagePlayer.h"
#include "../inc/nbk_limit.h"
#include "../inc/nbk_callback.h"
#include "../inc/nbk_cbDef.h"
#include "../inc/nbk_gdi.h"
#include "../css/color.h"
#include "../css/css_val.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../loader/url.h"
#include "../loader/upCmd.h"
#include "../dom/page.h"
#include "../dom/document.h"
#include "../dom/view.h"
#include "../dom/node.h"


#define DEBUG_SCHEDULE      0

typedef struct _NBgImage
{
    char* src;
    NPtrArray* clipLst; // clip list for source bg image need clip
    
} NBgImage;

// NImage

static void image_clear_cliplst(NBgImage* image)
{
    if (image->clipLst) {
        int i;
        for (i=0; i < image->clipLst->use; i++)
            NBK_free(image->clipLst->data[i]);
        ptrArray_delete(&image->clipLst);        
    }
}

static void bgimage_add_clipinfo(NBgImage* bgimage, NImageClipInfo* clip)
{
    int i = -1;
    NImageClipInfo* ci = N_NULL;

    if (bgimage->clipLst == N_NULL)
        bgimage->clipLst = ptrArray_create();

    i = bgimage->clipLst->use - 1;
    while (i >= 0) {

        ci = (NImageClipInfo*)bgimage->clipLst->data[i];
        if (   ci->x_repeat == clip->x_repeat
            && ci->y_repeat == clip->y_repeat
            && ci->x_t == clip->x_t
            && ci->y_t == clip->y_t
            && ci->x == clip->x
            && ci->y == clip->y
            && ci->w == clip->w
            && ci->h == clip->h )
            return;
        i--;
    }

    ci = (NImageClipInfo*) NBK_malloc(sizeof(NImageClipInfo));
    NBK_memcpy(ci, clip, sizeof(NImageClipInfo));
    ptrArray_append(bgimage->clipLst, ci);
}

static NBgImage* bgImage_create(char* src)
{
    NBgImage* im = (NBgImage*) NBK_malloc0(sizeof(NBgImage));
    N_ASSERT(im);
    im->src = src;
    return im;
}

static void bgImage_delete(NBgImage** image)
{
    NBgImage* im = *image;
    im->src = N_NULL;
    image_clear_cliplst(im);
    NBK_free(im);
    *image = N_NULL;
            }

static void mp_delete_req_list(NImagePlayer* player)
{
    int16 i = 0;
    
    if (!player->reqBglist)
        return;
    
    for (; i < player->reqBglist->use; i++) {
        NBgImage* bgim = (NBgImage*)player->reqBglist->data[i];
        bgImage_delete(&bgim);
    }
    ptrArray_delete(&player->reqBglist);
}

static char* mp_get_url_with_clip(const char* src, NImageClipInfo* ci)
{
    char* url = N_NULL;
    char clip[MAX_IMAGE_CLIP_STR_LEN];
    int16 clipLen;
    int16 len = NBK_strlen(src);

    clipLen = NBK_sprintf(clip, "_%d_%d_%d_%d_%d_%d_%d_%d",
        ci->x_repeat, ci->y_repeat, ci->x_t, ci->y_t, ci->x, ci->y, ci->w, ci->h);

    url = (char*)NBK_malloc(len + clipLen + 1);
    NBK_strcpy(url, src);
    NBK_strcpy(url + len, clip);

    return url;
}

static NImage* image_create(char* src, int8 type)
{
    NImage* im = (NImage*)NBK_malloc0(sizeof(NImage));
    N_ASSERT(im);
    im->src = src;
    im->state = NEIS_PENDING;
    im->type = type;
    im->mime = NEMIME_UNKNOWN;
    im->referId = -1;
    return im;
}

static void image_delete(NImage** image)
{
    NImage* im = *image;
    
    if (im->ownSrc)
        NBK_free(im->src);
        
    if (im->clipInfo)
        NBK_free(im->clipInfo);
    
    NBK_free(im);
    *image = N_NULL;
}

static bd_bool image_compareSrc(NImage* image, const char* src)
{
    return (NBK_strcmp(image->src, src) ? N_FALSE : N_TRUE);
}

static bd_bool image_getSize(NImage* image, NSize* size, bd_bool* fail)
{
    switch (image->state) {
    case NEIS_DECODE_FINISH:
        *size = image->size;
        *fail = N_FALSE;
        return N_TRUE;
        
    case NEIS_LOAD_FAILED:
    case NEIS_DECODE_FAILED:
        *fail = N_TRUE;
        return N_FALSE;
        
    default:
        *fail = N_FALSE;
        return N_FALSE;
    }
}

// NImagePlayer

int imagePlayer_total(NImagePlayer* player)
{
    return player->list->use;
}

static int mp_processed(NImagePlayer* player)
{
    int num = 0;
    int16 i;
    NImage* im;
    
    for (i=0; i < player->list->use; i++) {
        im = (NImage*)player->list->data[i];
        if (   im->type == NEIT_FLASH_IMAGE
            || im->state > NEIS_LOAD_NO_CACHE )
            num++;
        }
    return num;
}

static void mp_download_finish(NImagePlayer* player)
{
    if (!player->dlFinish) {
        NView* view = (NView*)player->view;
        NPage* page = (NPage*)view->page;
        NBK_Event_GotImage evt;
        player->dlFinish = 1;
        evt.curr = evt.total = imagePlayer_total(player);
        nbk_cb_call(NBK_EVENT_DOWNLOAD_IMAGE, &page->cbEventNotify, &evt);
    }
}

void imagePlayer_progress(NImagePlayer* player)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NBK_Event_GotImage evt;
    
    if (player->stop || player->dlFinish)
        return;

    evt.curr = mp_processed(player);
    evt.total = imagePlayer_total(player);
    
#if DEBUG_SCHEDULE
    {
        char buf[64];
        NBK_sprintf(buf, "imageP: progress %d / %d", evt.curr, evt.total);
        dump_char(page, buf, -1);
        dump_return(page);
    }
#endif
    
    if (evt.curr >= evt.total)
        mp_download_finish(player);
    else {
        if (player->loadByInvoke)
            evt.curr = evt.total = 0;

    nbk_cb_call(NBK_EVENT_DOWNLOAD_IMAGE, &page->cbEventNotify, &evt);
}
}

static bd_bool mp_in_decoding(NImagePlayer* player)
{
    int i;
    NImage* im;
    
    for (i=0; i < player->list->use; i++) {
        im = (NImage*)player->list->data[i];
        if (im->state == NEIS_DECODING)
            return N_TRUE;
    }
    
    return N_FALSE;
}

static NImage* mp_get_image_to_decode(NImagePlayer* player)
{
    int i;
    NImage* im = N_NULL;
    NPage* page = (NPage*)((NView*)player->view)->page;
    
    if (doc_isAbsLayout(page->doc->type)) {
        NImage* found = N_NULL;
        int time = 0;
        for (i=0; i < player->list->use; i++, im = N_NULL) {
            im = (NImage*)player->list->data[i];
            if (im->state == NEIS_LOAD_FINISH) {
                if (im->time > time) {
                    found = im;
                    time = im->time;
                }
            }
        }
        im = found;
    }
    else {
        for (i=0; i < player->list->use; i++, im = N_NULL) {
            im = (NImage*)player->list->data[i];
            if (im->state == NEIS_LOAD_FINISH)
                break;
        }
    }
    
    return im;
}

static char* mp_asse_url_with_clip(NBgImage* image)
{
    char* url = N_NULL;
    int s, size = 4;
    
    s = NBK_strlen(image->src);
    size += s;
    if (image->clipLst)
        size += MAX_IMAGE_CLIP_STR_LEN * image->clipLst->use;
    
    url = (char*)NBK_malloc(size);
    
    NBK_strcpy(url, image->src);
    
    if (image->clipLst) {
        int i;
        NImageClipInfo* ci;
        for (i = 0; i < image->clipLst->use; i++) {
            ci = image->clipLst->data[i];
            size = NBK_sprintf(url + s, "\t%d_%d_%d_%d_%d_%d_%d_%d",
                ci->x_repeat, ci->y_repeat, ci->x_t, ci->y_t, ci->x, ci->y, ci->w, ci->h);
            s += size;
        }
    }
    
    url[s] = 0;
    
    return url;
}

static void imagePlayer_schedule_load(void* user)
{
    NImagePlayer* player = (NImagePlayer*)user;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* image;
    char* url = N_NULL;
    NRequest* req;
    NImageRequest* imgReq;
    int16 i;
    bd_bool unesc = (doc_isAbsLayout(page->doc->type)) ? N_FALSE : N_TRUE;
    
    tim_stop(player->schTimer);
    
    if (player->stop)
        return;

    if (player->multipics) {
        // request all rest images with one cmd
        NSList* lst = sll_create();
        NSList* dellst = sll_create();

        int16 j, t, num;
        
        player->multipics = 0;
        
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: multipics ...", -1);
        dump_return(page);
#endif
        
        for (j = t = num = 0; j < player->list->use; j++) {
            image = (NImage*)player->list->data[j];
            if (image->type == NEIT_FLASH_IMAGE)
                continue;
            else if (NEIT_CSS_IMAGE_CLIPED == image->type) {
                if (image->state == NEIS_PENDING)
                    image->state = NEIS_MP_LOADING;
                continue;
            }        
            num++;
            if (image->state == NEIS_PENDING) {
                url = (unesc) ? url_parse(doc_getUrl(page->doc), image->src) : image->src;
                if (url) {
                    image->state = NEIS_MP_LOADING;
                    sll_append(lst, url);
#if DEBUG_SCHEDULE
                    dump_char(page, " ", -1);
                    dump_int(page, t);
                    dump_char(page, url, -1);
                    dump_return(page);
#endif
                    t++;
                }
            }
        }
        
        if (player->reqBglist) {
            for (j = 0; j < player->reqBglist->use; j++) {
                NBgImage* bgImage = (NBgImage*) player->reqBglist->data[j];

                if (bgImage->src) {
                    if (player->reqClip) {
                        url = mp_asse_url_with_clip(bgImage);
                        t += bgImage->clipLst->use;
                        num += bgImage->clipLst->use;
                        image_clear_cliplst(bgImage);
                        sll_append(dellst, url);
                    }
                    else {
                        t++;
                        num++;
                    }
                    sll_append(lst, url);
#if DEBUG_SCHEDULE
                    dump_char(page, " ", -1);
                    dump_int(page, t);
                    dump_char(page, url, -1);
                    dump_return(page);
#endif
                    //t++;
                }
            }
            mp_delete_req_list(player);
        }
#if DEBUG_SCHEDULE
        {
            char buf[64];
            NBK_sprintf(buf, "    request %d / total %d", t, num);
            dump_char(page, buf, -1);
            dump_return(page);
        }
#endif
        
        if (t > 0) {
            NUpCmd* cmd = upcmd_create(N_NULL);
            upcmd_url(cmd, doc_getUrl(page->doc), N_NULL);
            if (player->reqClip) {
                nid tags[] = {TAGID_PARAM, 0};
                NSList* params = node_getListByTag(page->doc->root, tags, view->path);
                if (params) {
                    upcmd_iaParams(cmd, params);
                    sll_delete(&params);
                }
            }
            upcmd_multiPics(cmd, lst);

            req = loader_createRequest();
            loader_setRequest(req, NEREQT_IMAGE_PACK, N_NULL, page);
            req->pageId = page->id;
            req->referer = doc_getUrl(page->doc);
            req->via = NEREV_FF_MULTIPICS;
            req->upcmd = cmd;
            req->platform = page->platform;
            if (player->cache) req->method = NEREM_HISTORY;
            
            if (loader_load(req)) {
#if DEBUG_SCHEDULE
                dump_char(page, "imageP: request ...", -1);
                dump_return(page);
#endif
                player->passive = 1;
                player->waitMPics = 1;
            }
            else {
                player->reqRest = 1;
                player->dlFinish = 1;
                loader_deleteRequest(&req);
                imagePlayer_start(player);
            }
            
            url = sll_first(lst);
            while (url) {
                if (unesc) NBK_free(url);
                url = sll_next(lst);
            }
            url = sll_first(dellst);
            while (url) {
                NBK_free(url);
                url = sll_next(dellst);
            }
        }

        sll_delete(&lst);
        sll_delete(&dellst);

        if (t == 0) {
            player->reqRest = 1;
            player->dlFinish = 1;
//            imagePlayer_start(player);
            imagePlayer_startDecode(player);
        }
        
        return;
    }
    
    // download one by one
    
    for (i=0; i < player->list->use; i++) {
//        nid state = (player->reqRest) ? NEIS_HAS_CACHE : NEIS_PENDING;
        image = (NImage*)player->list->data[i];
        if (image->type == NEIT_FLASH_IMAGE)
            continue;
        if (   image->state == NEIS_PENDING
            /*|| image->state == NEIS_HAS_CACHE*/
            /*|| image->state == NEIS_MP_LOADING*/)
            break;
    }
    
    if (i >= player->list->use) {
        // all images load finish.
        
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: NO IMAGE NEED LOAD!", -1);
        dump_return(page);
#endif

        mp_download_finish(player);
        imagePlayer_aniResume(player);
        return;
    }
    
#if DEBUG_SCHEDULE
    dump_char(page, "imageP: schedule load...", -1);
    dump_return(page);
#endif
    
    image = (NImage*)player->list->data[i];
    image->state = NEIS_LOADING;
    
    url = (unesc) ? url_parse(doc_getUrl(page->doc), image->src) : image->src;
    
    imgReq = (NImageRequest*)NBK_malloc(sizeof(NImageRequest));
    imgReq->image = image;
    imgReq->player = player;
    
    req = loader_createRequest();
    req->referer = doc_getUrl(page->doc);
    req->pageId = page->id;
    if (image->type == NEIT_FLASH_IMAGE) req->via = NEREV_FF_FLASH;
    if (player->cache) req->method = NEREM_HISTORY;
    
    loader_setRequest(req, NEREQT_IMAGE, N_NULL, N_NULL);
    loader_setRequestUrl(req, url, unesc);
    loader_setRequestUser(req, imgReq);
    req->responseCallback = imagePlayer_onLoadResponse;
    req->platform = page->platform;
    
#if DEBUG_SCHEDULE
    dump_char(page, "imageP: load", -1);
    dump_int(page, i);
    dump_int(page, (int)req->method);
    dump_char(page, url, -1);
    dump_return(page);
#endif
    
    if (!loader_load(req)) {
        image->state = NEIS_LOAD_FAILED;
        loader_deleteRequest(&req);
        imagePlayer_start(player);
    }
}

static void imagePlayer_schedule_load_cache(void* user)
{
    NImagePlayer* player = (NImagePlayer*)user;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* image = N_NULL;
    NRequest* req;
    NImageRequest* imgReq;
    int16 i;
    char* url;
    bd_bool unesc = (doc_isAbsLayout(page->doc->type)) ? N_FALSE : N_TRUE;
    
    tim_stop(player->schTimer);
    
    if (player->stop)
        return;
    
    for (i=0; i < player->list->use; i++, image = N_NULL) {
        image = (NImage*)player->list->data[i];
        if (image->type != NEIT_FLASH_IMAGE && image->state == NEIS_HAS_CACHE)
            break;
    }
    
    if (image == N_NULL) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: *** LOAD CACHE FINISH! request rest from net...", -1);
        dump_return(page);
#endif
        player->schTimer->func = imagePlayer_schedule_load;
        imagePlayer_start(player);
        return;
    }
    
    image->state = NEIS_LOADING;
    url = (unesc) ? url_parse(doc_getUrl(page->doc), image->src) : image->src;
    
    imgReq = (NImageRequest*)NBK_malloc(sizeof(NImageRequest));
    imgReq->image = image;
    imgReq->player = player;
    
    req = loader_createRequest();
    req->referer = doc_getUrl(page->doc);
    req->pageId = page->id;
    if (player->cache)
        req->method = NEREM_HISTORY;

    loader_setRequest(req, NEREQT_IMAGE, N_NULL, N_NULL);
    loader_setRequestUrl(req, url, unesc);
    loader_setRequestUser(req, imgReq);
    req->responseCallback = imagePlayer_onLoadResponse;
    req->platform = page->platform;
    
#if DEBUG_SCHEDULE
    dump_char(page, "imageP: load from cache", -1);
    dump_int(page, i);
    dump_int(page, (int)req->method);
    dump_char(page, url, -1);
    dump_return(page);
#endif
    
    if (!loader_load(req)) {
        image->state = NEIS_LOAD_FAILED;
        loader_deleteRequest(&req);
        imagePlayer_start(player);
    }
}

static void imagePlayer_schedule_load_flash(void* user)
{
    NImagePlayer* player = (NImagePlayer*)user;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* image;
    NRequest* req;
    NImageRequest* imgReq;
    int16 i;
    char* url;
    
    tim_stop(player->schTimer);
    
    if (player->stop)
        return;
    
    for (i=0, image = N_NULL; i < player->list->use; i++) {
        image = (NImage*)player->list->data[i];
        if (image->type == NEIT_FLASH_IMAGE && image->state == NEIS_PENDING)
            break;
    }
    
    if (image && i < player->list->use) {
        image->state = NEIS_LOADING;
        
        imgReq = (NImageRequest*)NBK_malloc(sizeof(NImageRequest));
        imgReq->image = image;
        imgReq->player = player;
        
        req = loader_createRequest();
        req->referer = doc_getUrl(page->doc);
        req->pageId = page->id;
        req->via = NEREV_FF_FLASH;
        if (player->cache)
            req->method = NEREM_HISTORY;
        
        url = url_parse(doc_getUrl(page->doc), image->src);
        
        loader_setRequest(req, NEREQT_IMAGE, N_NULL, N_NULL);
        loader_setRequestUrl(req, url, N_TRUE);
        loader_setRequestUser(req, imgReq);
        req->responseCallback = imagePlayer_onLoadResponse;
        req->platform = page->platform;
        
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: load flash", -1);
        dump_int(page, i);
        dump_int(page, (int)req->method);
        dump_char(page, url, -1);
        dump_return(page);
#endif
    
        if (!loader_load(req)) {
            image->state = NEIS_LOAD_FAILED;
            loader_deleteRequest(&req);
            imagePlayer_start(player);
        }
    }
}

static void imagePlayer_schedule_animate(void* user)
{
    NImagePlayer* player = (NImagePlayer*)user;
    NView* view = (NView*)player->view;
    //NPage* page = (NPage*)view->page;
    NImage* im;
    int16 i;
    bd_bool ant = N_FALSE;
    
    tim_stop(player->aniTimer);
    
    if (player->stop)
        return;
    
    for (i=0; i < player->list->use; i++) {
        im = (NImage*)player->list->data[i];
        if (im->state == NEIS_DECODE_FINISH && im->total > 1) {
            im->curr = (im->curr + 1 == im->total) ? 0 : im->curr + 1;
            ant = N_TRUE;
        }
    }
    
    if (ant) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: animating...", -1);
        dump_return(page);
#endif
        view_picUpdate(view, -1);
        tim_start(player->aniTimer, 1000000, 5000000);
    }
}

static void imagePlayer_schedule_decode(void* user)
{
    NImagePlayer* player = (NImagePlayer*)user;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* im;
    
    tim_stop(player->decTimer);
    
    if (player->stop)
        return;

    im = mp_get_image_to_decode(player);

    if (im) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: decoding...", -1);
        dump_int(page, im->id);
        dump_return(page);
#endif
        im->state = NEIS_DECODING;
        NBK_imageDecode(page->platform, player, im);
    }
}

static bd_bool imagePlayer_checkMemory(NImagePlayer* player)
{
    if (NBK_freemem() < MINI_MEMORY_FOR_PIC_DECODE) {
        NView* view = (NView*)player->view;
        NPage* page = (NPage*)view->page;
        
        mp_download_finish(player);        
        dump_time(page);
        dump_char(page, "ImageP [ memory insufficient ]", -1);
        dump_return(page);
        dump_flush(page);
        
        player->stop = 1;
        return N_FALSE;
    }
    else
        return N_TRUE;
}

void imagePlayer_startDecode(NImagePlayer* player)
{
    NImage* im;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    
    if (player->decTimer->run)
        return;
    
    if (player->stop)
        return;
    
    if (!imagePlayer_checkMemory(player))
        return;
    
    if (mp_in_decoding(player))
        return;

    im = mp_get_image_to_decode(player);
    
    if (im) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: start decode...", -1);
        dump_return(page);
#endif
        tim_stop(player->decTimer);
        tim_start(player->decTimer, 5, 5000000);
    }
    else if (player->numDecoded) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: NO IMAGE NEED DECODE!", -1);
        dump_int(page, player->numDecoded);
        dump_return(page);
#endif
        nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
        player->numDecoded = 0;
    }
}

static void mp_update_refer_id(NImagePlayer* player, int16 id, const char* refer)
{
    int16 i;
    NImage* image;
    NImage* imageIter;
    
    if (id < 0 || id >= player->list->use)
        return;
    
    image = (NImage*) player->list->data[id];
    
    if (refer) {
        i = player->list->use - 1;
        while (i >= 0) {
            imageIter = (NImage*) player->list->data[i];
            if (image_compareSrc(imageIter, refer)) {
                image->referId = i;
                break;
            }
            i--;
        }
    }
}

static NImage* mp_get_refer_image(NImagePlayer* player, int16 id)
{
    NImage* im = (NImage*)player->list->data[id];
    if (im->referId != -1)
        im = (NImage*)player->list->data[im->referId];
    return im;
}

NImagePlayer* imagePlayer_create(void* view)
{
    NImagePlayer* player = (NImagePlayer*)NBK_malloc0(sizeof(NImagePlayer));
    N_ASSERT(player);
    
    player->view = view;
    player->list = ptrArray_create();
    player->schTimer = tim_create(imagePlayer_schedule_load, player);
    player->decTimer = tim_create(imagePlayer_schedule_decode, player);
    player->aniTimer = tim_create(imagePlayer_schedule_animate, player);
    
    return player;
}

void imagePlayer_delete(NImagePlayer** player)
{
    NImagePlayer* pl = *player;
    
    mp_delete_req_list(pl);
    
    imagePlayer_reset(pl);
    
    tim_delete(&pl->schTimer);
    tim_delete(&pl->decTimer);
    tim_delete(&pl->aniTimer);
    
    ptrArray_delete(&pl->list);
    
    NBK_free(pl);
    *player = N_NULL;
}

void imagePlayer_reset(NImagePlayer* player)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    int16 i;
    NImage* im;
    
    if (page->platform == N_NULL)
        return;
    
    mp_delete_req_list(player);

    tim_stop(player->aniTimer);
    tim_stop(player->schTimer);
    tim_stop(player->decTimer);
    player->schTimer->func = imagePlayer_schedule_load_cache;
    
    for (i=0; i < player->list->use; i++) {
        im = (NImage*)player->list->data[i];
        NBK_imageDelete(page->platform, player, im);
        image_delete(&im);
    }
    ptrArray_reset(player->list);
    
    if (player->srcLst) {
        char* url = sll_first(player->srcLst);
        while (url) {
            NBK_free(url);
            url = sll_next(player->srcLst);
        }
        sll_delete(&player->srcLst);
    }
    
    player->stop = 0;
    player->passive = 0;
    player->cache = 0;
    player->multipics = 0;
    player->reqRest = 0;
    player->dlFinish = 0;
    player->ticks = 0;
    player->waitMPics = 0;
    player->reqClip = 0;
    player->time = NBK_currentMilliSeconds();
    player->interval = 1000;
    player->numDecoded = 0;

    NBK_imageStopAll(page->platform);
    
#if DEBUG_SCHEDULE
    dump_char(page, "==========", -1);
    dump_return(page);
    dump_flush(page);
#endif
}

static void mp_add_bgclipinfo(NImagePlayer* player, NBgImage* image, NImageClipInfo* ci)
{
    if (player->multipics && ci) {
        player->reqClip = 1;
        bgimage_add_clipinfo(image, ci);
    }
}

static void convert_repeat(const uint8 repeat, uint8* x_repeat, uint8* y_repeat)
{
    switch (repeat) {
    case CSS_REPEAT_NO:
    {
        *x_repeat = 0;
        *y_repeat = 0;
        break;
    }
    case CSS_REPEAT_X:
    {
        *x_repeat = 1;
        *y_repeat = 0;
        break;
    }
    case CSS_REPEAT_Y:
    {
        *x_repeat = 0;
        *y_repeat = 1;
        break;
    }
    default:
    {
        *x_repeat = 1;
        *y_repeat = 1;
    }
    }
}

int16 imagePlayer_getId(NImagePlayer* player, const char* src, int8 type, NImageClipInfo* ci)
{
    int16 i;
    NImage* image;
    NView* view = (NView*) player->view;
    NPage* page = (NPage*) view->page;
    int8 newType = type;
    char* url = (char*)src;
    bd_bool unesc = (doc_isAbsLayout(page->doc->type)) ? N_FALSE : N_TRUE;

    if (player->disable)
        return -1; // ignore image request
    
    if (player->loadByInvoke && type != NEIT_IMAGE)
        return -1;
    
//    if (type == NEIT_FLASH_IMAGE)
//        return -1;

    if (ci && ci->w && ci->h) {
        uint8 xr, yr;
        convert_repeat(ci->repeat, &xr, &yr);
        ci->x_repeat = xr;
        ci->y_repeat = yr;
    
        newType = NEIT_CSS_IMAGE_CLIPED;
        url = mp_get_url_with_clip(src, ci); // url = url + clip
    }
    
    for (i = 0; i < player->list->use; i++) {
        image = (NImage*) player->list->data[i];
        if (image_compareSrc(image, url)) {
            if (url != src)
                NBK_free(url);
            return i; // already exist
        }
    }
    
    // not found insert new one
    image = image_create(url, newType);
    ptrArray_append(player->list, image);
    image->pageId = player->pageId;
    image->id = player->list->use - 1;
    image->state = NEIS_PENDING;
    image->ownSrc = (url != src) ? N_TRUE : N_FALSE;
    NBK_imageCreate(page->platform, player, image);
    
    if (image->clipInfo == N_NULL && ci && ci->w && ci->h) {
        image->clipInfo = (NImageClipInfo*)NBK_malloc(sizeof(NImageClipInfo));
        NBK_memcpy(image->clipInfo, ci, sizeof(NImageClipInfo));
    }
    
    if (unesc)
        url = url_parse(doc_getUrl(page->doc), src);

    if (NBK_isImageCached(url)) {
        image->state = NEIS_HAS_CACHE;
    }
    else if (ci && ci->w && ci->h) {
        NBgImage* bgIm = N_NULL;
        bd_bool add = N_TRUE;
        if (!player->reqBglist)
            player->reqBglist = ptrArray_create();

        for (i = 0; i < player->reqBglist->use; i++) {
            bgIm = (NBgImage*) player->reqBglist->data[i];

            if (!NBK_strcmp(bgIm->src, src)) {
                mp_add_bgclipinfo(player, bgIm, ci);
                add = N_FALSE;
                break;
            }
        }

        if (add) {
            bgIm = bgImage_create((char*)src);
            ptrArray_append(player->reqBglist, bgIm);
            mp_add_bgclipinfo(player, bgIm, ci);
        }
    }
    
    if (unesc)
        NBK_free(url);
    
#if DEBUG_SCHEDULE
    dump_char(page, "ImageP: add", -1);
    dump_int(page, image->id);
    dump_int(page, image->type);
    dump_int(page, image->state);
    dump_char(page, image->src, -1);
    dump_return(page);
#endif
    
    return image->id;
}

bd_bool imagePlayer_getSize(NImagePlayer* player, const int16 id, NSize* size, bd_bool* fail)
{
    NImage* im;
    
    if (id < 0 || id >= player->list->use)
        return N_FALSE;
    
    im = mp_get_refer_image(player, id);
    
    return image_getSize(im, size, fail);
}

void imagePlayer_draw(NImagePlayer* player, const int16 id, const NRect* rect)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* im;
    
    if (id < 0 || id >= player->list->use)
        return;
    
    im = (NImage*)player->list->data[id];
    im->time = NBK_currentMilliSeconds() - player->time;
    if (   im->type != NEIT_FLASH_IMAGE
        && (   im->state == NEIS_LOAD_FINISH
            || (player->loadByInvoke && im->state < NEIS_DECODE_FINISH) ) ) {
        NColor fill[4] = {{228, 197, 184, 255},
                          {241, 242, 242, 255},
                          {229, 222, 200, 255},
                          {225, 240, 219, 255}};
        NBK_gdi_setBrushColor(page->platform, &fill[id % 4]);
        NBK_gdi_clearRect(page->platform, rect);
        if (!player->loadByInvoke)
        imagePlayer_startDecode(player);
    }
    else
        NBK_imageDraw(page->platform, player, im, rect);
}

void imagePlayer_drawBg(NImagePlayer* player, const NImageDrawInfo di)
{
    NPage* page = (NPage*)((NView*)player->view)->page;
    NImage* im;
    coord x, y, w, h, iw, ih;
    NRect r, cl;
    int rx = 0, ry = 0;

    if (di.id < 0 || di.id >= player->list->use)
        return;
    
    im = mp_get_refer_image(player, di.id);
    if (im->state == NEIS_LOAD_FINISH) {
        imagePlayer_startDecode(player);
        return;
    }
    if (im->state != NEIS_DECODE_FINISH)
        return;

    w = rect_getWidth(di.vr);
    h = rect_getHeight(di.vr);
    
    cl = *(di.pr);
    cl.l = (cl.l < 0) ? 0 : cl.l;
    cl.t = (cl.t < 0) ? 0 : cl.t;
    cl.r = (cl.r > w) ? w : cl.r;
    cl.b = (cl.b > h) ? h : cl.b;
    NBK_gdi_setClippingRect(page->platform, &cl);
    
    x = di.r.l + di.ox;
    y = di.r.t + di.oy;
    
    iw = im->size.w;
    ih = im->size.h;
    
    // the first area to draw bg
    r.l = x;
    r.t = y;
    r.r = r.l + iw;
    r.b = r.t + ih;
    rect_toView(&r, *(di.zoom));
    rect_move(&r, r.l - di.vr->l, r.t - di.vr->t);
    
    iw = rect_getWidth(&r);
    ih = rect_getHeight(&r);
    if (iw == 0 || ih == 0)
        return;
    
    w = rect_getWidth(di.pr);
    h = rect_getWidth(di.pr);

    if (di.repeat == CSS_REPEAT_NO) {
        NBK_imageDrawBg(page->platform, player, im, &r, 0, 0);
    }
    else if (di.repeat == CSS_REPEAT_X) {
        rx = w / iw;
        if (w % iw) rx++;
        NBK_imageDrawBg(page->platform, player, im, &r, rx, 0);
    }
    else if (di.repeat == CSS_REPEAT_Y) {
        ry = h / ih;
        if (h % ih) ry++;
        NBK_imageDrawBg(page->platform, player, im, &r, 0, ry);
    }
    else {
        rx = w / iw;
        if (w % iw) rx++;
        ry = h / ih;
        if (h % ih) ry++;
        NBK_imageDrawBg(page->platform, player, im, &r, rx, ry);
    }
    
    NBK_gdi_cancelClippingRect(page->platform);
}

void imagePlayer_onLoadResponse(NEResponseType type, void* user, void* data, int v1, int v2)
{
    NRequest* r = (NRequest*)user;
    NImageRequest* req = (NImageRequest*)r->user;
    NImage* image = req->image;
    NImagePlayer* player = req->player;
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    
    switch (type) {
    case NEREST_HEADER:
    {
        NRespHeader* hdr = (NRespHeader*)data;
        image->mime = hdr->mime;
        image->length = hdr->content_length;
        image->received = 0;
        if (hdr->user) {
            if (image->clipInfo) {
                NImageClipInfo* ci = (NImageClipInfo*) hdr->user;
                image->clipInfo->new_x = ci->new_x;
                image->clipInfo->new_y = ci->new_y;
            }
            else {
                image->clipInfo = (NImageClipInfo*) NBK_malloc0(sizeof(NImageClipInfo));
                NBK_memcpy((void*)image->clipInfo, hdr->user, sizeof(NImageClipInfo));
            }
        }
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: got header", -1);
        dump_int(page, image->id);
        dump_int(page, image->length);
        if (hdr->user) {
            char bgpos[32];
            NBK_sprintf(bgpos, "bgpos(%ld,%ld)", image->clipInfo->new_x, image->clipInfo->new_y);
            dump_char(page, bgpos, -1);
        }
        dump_return(page);
#endif
        break;
    }

    case NEREST_DATA:
    {
        image->received += v1;
        NBK_onImageData(page->platform, image, data, v1);
        break;
    }
        
    case NEREST_COMPLETE:
    {
        if (image->received > 0) {
            image->state = NEIS_LOAD_FINISH;
            NBK_onImageDataEnd(page->platform, image, N_TRUE);
            imagePlayer_startDecode(player);
        }
        else {
            image->state = NEIS_LOAD_FAILED;
            NBK_onImageDataEnd(page->platform, image, N_FALSE);
        }
        imagePlayer_progress(player);
        imagePlayer_start(player);
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: download complete!", -1);
        dump_int(page, image->id);
        dump_int(page, image->received);
        dump_return(page);
#endif
        break;
    }
        
    case NEREST_ERROR:
    case NEREST_CANCEL:
    {
        image->state = (v1 == NELERR_NOCACHE) ? NEIS_LOAD_NO_CACHE : NEIS_LOAD_FAILED;
        NBK_onImageDataEnd(page->platform, image, N_FALSE);
        imagePlayer_progress(player);
        imagePlayer_startDecode(player);
        imagePlayer_start(player);
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: download error, process next one", -1);
        dump_int(page, image->id);
        dump_int(page, image->state);
        dump_return(page);
#endif
        break;
    }
    
    default:
        break;
    }
}

void imagePlayer_onDecodeOver(NImagePlayer* player, int16 id, bd_bool succeed)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page; // always main page
    NImage* im = (NImage*)player->list->data[id];

    im->state = (succeed) ? NEIS_DECODE_FINISH : NEIS_DECODE_FAILED;
    
    if (player->stop)
        return;
    
#if DEBUG_SCHEDULE
    dump_char(page, "imageP: decoded!", -1);
    dump_int(page, id);
    dump_char(page, "ret", -1);
    dump_int(page, succeed);
    dump_return(page);
#endif
    if (succeed) {
        if (doc_isAbsLayout(page->doc->type)) {
            player->numDecoded++;

            if (im->type == NEIT_IMAGE && page_isMainBodyMode(page)) {
                view_picUpdate(view, im->id);
            }

            if (player->ticks == 0) {
                player->ticks = NBK_currentMilliSeconds();
            }
            else {
                int now = NBK_currentMilliSeconds();
                if (now - player->ticks >= player->interval) {
#if DEBUG_SCHEDULE
                    dump_char(page, "\tupdate view ...", -1);
                    dump_int(page, player->numDecoded);
                    dump_return(page);
#endif
                    player->ticks = now;
                    nbk_cb_call(NBK_EVENT_UPDATE_PIC, &page->cbEventNotify, N_NULL);
                    player->numDecoded = 0;
                }
            }
        }
        else if (im->type == NEIT_CSS_IMAGE)
            view_picUpdate(view, im->id);
        else
            view_picChanged(view, im->id);
    }
    
    imagePlayer_startDecode(player);
}

void imagePlayer_start(NImagePlayer* player)
{
    if (player->schTimer->run)
        return;
    
    if (player->stop || player->passive || player->loadByInvoke)
        return;

    if (!imagePlayer_checkMemory(player))
        return;
    
    tim_stop(player->schTimer);
    tim_start(player->schTimer, 5, 5000000);
}

void imagePlayer_disable(NImagePlayer* player, bd_bool disable)
{
    player->disable = disable;
}

void imagePlayer_stopAll(NImagePlayer* player)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    int i;
    
    tim_stop(player->aniTimer);
    tim_stop(player->schTimer);
    tim_stop(player->decTimer);
    
    NBK_imageStopAll(page->platform);
    player->stop = 1;
    
    for (i=0; i < player->list->use; i++) {
        NImage* im = (NImage*)player->list->data[i];
        
        switch (im->state) {
        case NEIS_HAS_CACHE:
        case NEIS_PENDING:
        case NEIS_LOADING:
        case NEIS_MP_LOADING:
            im->state = NEIS_CANCEL;
            break;
        case NEIS_DECODING:
//            dump_char(page, "ImageP: decodeing => load", -1);
//            dump_int(page, i);
//            dump_return(page);
            im->state = NEIS_LOAD_FINISH;
            break;
    }
}
}

NRequest* imagePlayer_getFakeRequestForFF(
    NRequest* req, char* url, const char* clip, const char* refer)
{
    NPage* page = (NPage*)req->user;
    NImagePlayer* player = page->view->imgPlayer;
    int16 i;
    NImage* im;
    NRequest* fakeReq = N_NULL;
    NImageRequest* imgReq;
    bd_bool unesc = (doc_isAbsLayout(page->doc->type)) ? N_FALSE : N_TRUE;
    bd_bool found;
    char* imgUrl;

#if DEBUG_SCHEDULE
    if (refer) {
        dump_char(page, "ImageP: same image data to other", -1);
        dump_char(page, url, -1);
        dump_return(page);
    }
#endif
            
    for (i = 0; i < player->list->use; i++) {
        im = (NImage*) player->list->data[i];

        if (im->type == NEIT_FLASH_IMAGE)
            continue;

        found = N_FALSE;
        imgUrl = (unesc) ? url_parse(doc_getUrl(page->doc), im->src) : im->src;
        if (imgUrl && NBK_strcmp(imgUrl, url) == 0) {
            found = N_TRUE;
#if DEBUG_SCHEDULE
            dump_char(page, "ImageP: found request", -1);
            dump_char(page, url, -1);
            dump_int(page, i);
            dump_char(page, (char*)clip, -1);
            dump_return(page);
#endif
        }

        if (found && (im->state == NEIS_MP_LOADING || im->state == NEIS_PENDING)) {
            imgReq = (NImageRequest*) NBK_malloc(sizeof(NImageRequest));
            imgReq->image = im;
            imgReq->player = player;

            im->state = NEIS_LOADING;

            fakeReq = loader_createRequest();
            loader_setRequest(fakeReq, NEREQT_IMAGE, N_NULL, N_NULL);
            loader_setRequestUser(fakeReq, imgReq);
            fakeReq->responseCallback = imagePlayer_onLoadResponse;
        }

        if (imgUrl && unesc)
            NBK_free(imgUrl);
        if (found) {
            if (refer)
                mp_update_refer_id(player, i, refer);

            return fakeReq;
        }
    } 
    
    if (fakeReq == N_NULL) { // the image need later
        if (clip) {
            i = imagePlayer_getId(player, url, NEIT_CSS_IMAGE, N_NULL); 
            mp_update_refer_id(player, i, refer);
        }
        else {
            char* urlCopy = NBK_malloc(NBK_strlen(url) + 1);
            NBK_strcpy(urlCopy, url);
            i = imagePlayer_getId(player, urlCopy, NEIT_IMAGE, N_NULL);
            if (player->srcLst == N_NULL)
                player->srcLst = sll_create();
            sll_append(player->srcLst, urlCopy);
        }
        
        if (i != -1) {
#if DEBUG_SCHEDULE
            dump_char(page, "ImageP: fake request", -1);
            dump_int(page, i);
            dump_char(page, url, -1);
            dump_return(page);
#endif
            im = (NImage*)player->list->data[i];
            
            imgReq = (NImageRequest*)NBK_malloc(sizeof(NImageRequest));
            imgReq->image = im;
            imgReq->player = player;
            
            im->state = NEIS_LOADING;
            
            fakeReq = loader_createRequest();
            loader_setRequest(fakeReq, NEREQT_IMAGE, N_NULL, N_NULL);
            loader_setRequestUser(fakeReq, imgReq);
            fakeReq->responseCallback = imagePlayer_onLoadResponse;
        }
    }
    
    return fakeReq;
}

void imagePlayer_onFFPackageOver(NRequest* req)
{
    NPage* page = (NPage*)req->user;
    NImagePlayer* player = page->view->imgPlayer;
    int i, num;

    if (!player->waitMPics && !player->multipics)
        mp_download_finish(player);

    if (req->via == NEREV_FF_MULTIPICS) {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: *** image package over ***", -1);
        dump_return(page);
#endif
        player->passive = 0;
        player->reqRest = 1;
        player->waitMPics = 0;
        
        mp_download_finish(player);
        
        for (i = num = 0; i < player->list->use; i++) {
            NImage* im = (NImage*) player->list->data[i];
            if (im->state == NEIS_MP_LOADING) {
                im->state = NEIS_LOAD_FAILED;
#if DEBUG_SCHEDULE            
                dump_char(page, "imageP: Multi-Pics not response", -1);
                dump_int(page, im->state);
                dump_char(page, im->src, -1);
                dump_return(page);
#endif  
                num++;
            }
        }
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: Multi-Pics missing", -1);        
        dump_int(page, num);
        dump_return(page);
#endif
        
        imagePlayer_start(player);
    }
    else {
#if DEBUG_SCHEDULE
        dump_char(page, "imageP: *** package over ***", -1);
//        dump_time(page);
        dump_return(page);
#endif
        
        if (player->multipics == 0) {
            player->passive = 0;
            player->reqRest = 1;
            player->dlFinish = 1;
            
//            player->schTimer->func = imagePlayer_schedule_load_flash;
//            imagePlayer_start(player);
        }
    }
}

void imagePlayer_aniPause(NImagePlayer* player)
{
    if (!player->aniTimer->run)
        return;
    
    tim_stop(player->aniTimer);
}

void imagePlayer_aniResume(NImagePlayer* player)
{
    if (player->aniTimer->run)
        return;
    if (!player->dlFinish)
        return;
    
    tim_start(player->aniTimer, 1000000, 5000000);
}

bd_bool imagePlayer_isGif(NImagePlayer* player, const int16 id)
{
    if (id < 0 || id >= player->list->use)
        return N_FALSE;
    
    return (((NImage*)player->list->data[id])->total > 1) ? N_TRUE : N_FALSE;
}

void imagePlayer_setDraw(NImagePlayer* player, const int16 id)
{   
    if (id >= 0 && id < player->list->use) {
        NImage* im = mp_get_refer_image(player, id);
        im->time = NBK_currentMilliSeconds() - player->time;
        if (im->state == NEIS_LOAD_FINISH)
            imagePlayer_startDecode(player);
    }
}

bd_bool imagePlayer_isClipped(NImagePlayer* player, int16 id)
{
    NImage* im;
    
    if (id >= 0 && id < player->list->use) {
        
        im = mp_get_refer_image(player, id);
        if (im->state != NEIS_DECODE_FINISH)
            return N_FALSE;
        
        im = (NImage*)player->list->data[id];
        if (im->type == NEIT_CSS_IMAGE_CLIPED || im->clipInfo)
            return N_TRUE;
    }
    
    return N_FALSE;
}

int16 imagePlayer_getIdClipped(NImagePlayer* player, const NImageDrawInfo* info)
{
#if 0
    NImage* im;
    int16 ret = -1, i;
    coord w = rect_getWidth(&info->r);
    coord h = rect_getHeight(&info->r);
    uint8 x_t = (info->ox_t == CSS_POS_PERCENT) ? 1 : 0;
    uint8 y_t = (info->oy_t == CSS_POS_PERCENT) ? 1 : 0;
    uint8 x_repeat, y_repeat;
    
    convert_repeat(info->repeat, &x_repeat, &y_repeat);
    
    im = player->list->data[info->id];
    
    for (i = 0; i < player->list->use; i++) {
        NImage* image = (NImage*)player->list->data[i];
        if (image->clipInfo) { 
            if (   image->clipInfo->x_repeat == x_repeat
                && image->clipInfo->y_repeat == y_repeat   
                && image->clipInfo->x_t == x_t
                && image->clipInfo->y_t == y_t
                && image->clipInfo->x == info->ox
                && image->clipInfo->y == info->oy
                && image->clipInfo->w == w
                && image->clipInfo->h == h
                && nbk_strfind(image->src, im->src) == 0 ) {
                ret = i;
                break;
                }
            }
        }
    
    return ret;
#else
    return info->id;
#endif
}

bd_bool imagePlayer_getDrawInfo(NImagePlayer* player, int16 id, NImageDrawInfo* info)
{
    bd_bool ret = N_FALSE;
    
    if (id >= 0 && id < player->list->use) {
        NImage* image = (NImage*)player->list->data[id];
        if (image->clipInfo) {
        info->ox = image->clipInfo->new_x;
        info->oy = image->clipInfo->new_y;
            image->time = NBK_currentMilliSeconds() - player->time;
        ret = N_TRUE;
    }
    }
    return ret;
}

//BOOL imagePlayer_getColorByPos(NImagePlayer* player, int16 id, coord x, coord y, NColor* color)
//{
//    BOOL ret = N_FALSE;
//    
//    if (id >= 0 && id < player->list->use) {
//        NView* view = (NView*)player->view;
//        NPage* page = (NPage*)view->page;
//        NImage* image = player->list->data[id];
//        ret = NBK_imageGetColorByPos(page->platform, player, image, x, y, color);
//    }
//    
//    return ret;
//}

void imagePlayer_loadImage(NImagePlayer* player, int16 id)
{
    NView* view = (NView*)player->view;
    NPage* page = (NPage*)view->page;
    NImage* image;
    NRequest* req;
    NImageRequest* imgReq;
    
    if (id < 0 || id >= player->list->use)
        return;
    
    image = (NImage*)player->list->data[id];
    if (   image->state != NEIS_PENDING
        && image->state != NEIS_HAS_CACHE )
        return;
    
    image->state = NEIS_LOADING;
    
    imgReq = (NImageRequest*)NBK_malloc(sizeof(NImageRequest));
    imgReq->image = image;
    imgReq->player = player;
    
    req = loader_createRequest();
    req->referer = doc_getUrl(page->doc);
    req->pageId = page->id;
    req->via = NEREV_FF_PIC;
    if (player->cache) req->method = NEREM_HISTORY;
    
    loader_setRequest(req, NEREQT_IMAGE, image->src, N_NULL);
    loader_setRequestUser(req, imgReq);
    req->responseCallback = imagePlayer_onLoadResponse;
    req->platform = page->platform;
    
    if (!loader_load(req)) {
        image->state = NEIS_LOAD_FAILED;
        loader_deleteRequest(&req);
    }
}

nid imagePlayer_getState(NImagePlayer* player, int16 id)
{
    if (id >=0 && id < player->list->use)
        return ((NImage*)player->list->data[id])->state;
    else
        return NEIS_NONE;
}
