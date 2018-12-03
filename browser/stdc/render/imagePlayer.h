/*
 * imagePlayer.h
 *
 *  Created on: 2011-2-21
 *      Author: wuyulun
 */

#ifndef IMAGEPLAYER_H_
#define IMAGEPLAYER_H_

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../inc/nbk_timer.h"
#include "../tools/ptrArray.h"
#include "../tools/slist.h"
#include "../render/renderNode.h"
#include "../loader/loader.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define IMAGE_CUTE_ID   30000
    
enum NEImageState {

    NEIS_NONE,
    NEIS_HAS_CACHE,
    NEIS_PENDING,
    NEIS_LOADING,
    NEIS_MP_LOADING,
    NEIS_LOAD_NO_CACHE,
    NEIS_LOAD_FINISH,
    NEIS_LOAD_FAILED,
    NEIS_DECODING,
    NEIS_DECODE_FINISH,
    NEIS_DECODE_FAILED,
    NEIS_CANCEL
};

enum NEImageType {
    
    NEIT_NONE,
    NEIT_IMAGE,
    NEIT_CSS_IMAGE,
    NEIT_CSS_IMAGE_CLIPED,
    NEIT_FLASH_IMAGE
};

typedef struct _NImageClipInfo {
    
    uint8 repeat;
    
    uint8 x_repeat : 1;  // X position repeat or not
    uint8 y_repeat : 1;  // Y position repeat or not
    
    uint8 x_t : 1;  // X position X is precent
    uint8 y_t : 1;  // Y position X is precent

    coord x;    // X background position(before clip)
    coord y;    // Y background position(before clip)
    coord w;    // element width
    coord h;    // element height

    coord new_x;    //X background position(after clip)
    coord new_y;    //X background position(after clip)

} NImageClipInfo;

typedef struct _NImage {
    
    int8    curr;   // current frame
    int8    total;  // total of frame
    int8    state;
    int8    type;
    
    bd_bool    ownSrc : 1;
    
    nid     pageId;
    int16   id;
    int16   referId;    // for ff same clip image
    
    nid     mime;
    char*   src;
    NSize   size;
    int     length;
    int     received;
        
    int     time; // track last used time.
    
    NImageClipInfo* clipInfo; // for result of clip bg image from ff-package or FSCache
    
} NImage;

typedef struct _NImagePlayer {
    
    nid         pageId;
    
    void*       view;
    NPtrArray*  list;
    NSList*     srcLst;
    
    NTimer*     aniTimer; // for animation
    NTimer*     schTimer; // for load
    NTimer*     decTimer; // for decode
    
    bd_bool        disable : 1;
    bd_bool        loadByInvoke : 1;
    bd_bool        stop : 1;
    bd_bool        passive : 1;
    bd_bool        cache : 1;
    bd_bool        multipics : 1;
    bd_bool        reqRest : 1; // request these images when multipics rest
    bd_bool        dlFinish : 1; // don't send progrss
    bd_bool        waitMPics : 1;
    bd_bool        reqClip : 1; // request image with clip info
    
    int         ticks;
    int         time;
    int         interval; // update interval
    int         numDecoded;
    
    NPtrArray*  reqBglist;
    
} NImagePlayer;

typedef struct _NImageRequest {
    
    NImage*         image;
    NImagePlayer*   player;
    
} NImageRequest;

typedef struct _NImageDrawInfo {
    
    int16   id;
    
    NRect   r; // tc_rect
    NRect*  pr; // paint rect
    NRect*  vr; // view rect
    
    uint8   repeat;
    uint8   ox_t;
    uint8   oy_t;
    coord   ox;
    coord   oy;
    
    NFloat* zoom;
    
} NImageDrawInfo;

NImagePlayer* imagePlayer_create(void* view);
void imagePlayer_delete(NImagePlayer** player);
void imagePlayer_reset(NImagePlayer* player);
void imagePlayer_disable(NImagePlayer* player, bd_bool disable);
void imagePlayer_stopAll(NImagePlayer* player);

int16 imagePlayer_getId(NImagePlayer* player, const char* src, int8 type, NImageClipInfo* ci);
bd_bool imagePlayer_getSize(NImagePlayer* player, const int16 id, NSize* size, bd_bool* fail);
void imagePlayer_draw(NImagePlayer* player, const int16 id, const NRect* rect);
void imagePlayer_drawBg(NImagePlayer* player, const NImageDrawInfo di);
bd_bool imagePlayer_isGif(NImagePlayer* player, const int16 id);
void imagePlayer_setDraw(NImagePlayer* player, const int16 id);
bd_bool imagePlayer_isClipped(NImagePlayer* player, int16 id);
int16 imagePlayer_getIdClipped(NImagePlayer* player, const NImageDrawInfo* info);
bd_bool imagePlayer_getDrawInfo(NImagePlayer* player, int16 id, NImageDrawInfo* info);
nid imagePlayer_getState(NImagePlayer* player, int16 id);

void imagePlayer_onLoadResponse(NEResponseType type, void* user, void* data, int, int);
void imagePlayer_onDecodeOver(NImagePlayer* player, int16 id, bd_bool succeed);

void imagePlayer_start(NImagePlayer* player);
void imagePlayer_startDecode(NImagePlayer* player);
void imagePlayer_aniPause(NImagePlayer* player);
void imagePlayer_aniResume(NImagePlayer* player);
void imagePlayer_loadImage(NImagePlayer* player, int16 id);

int imagePlayer_total(NImagePlayer* player);
void imagePlayer_progress(NImagePlayer* player);

NRequest* imagePlayer_getFakeRequestForFF(NRequest* req, char* url, const char* clip, const char* refer);
void imagePlayer_onFFPackageOver(NRequest* req);

// implemented by platform

void NBK_imageCreate(void* pfd, NImagePlayer* player, NImage* image);
void NBK_imageDelete(void* pfd, NImagePlayer* player, NImage* image);
void NBK_imageDecode(void* pfd, NImagePlayer* player, NImage* image);
void NBK_imageDraw(void* pfd, NImagePlayer* player, NImage* image, const NRect* dest);
void NBK_imageDrawBg(void* pfd, NImagePlayer* player, NImage* image, const NRect* dest, int rx, int ry);
//BOOL NBK_imageGetColorByPos(void* pfd, NImagePlayer* player, NImage* image, coord x, coord y, NColor* color);

void NBK_onImageData(void* pfd, NImage* image, void* data, int length);
void NBK_onImageDataEnd(void* pfd, NImage* image, bd_bool succeed);

void NBK_imageStopAll(void* pfd);

bd_bool NBK_isImageCached(const char* url);

// for placeholder
void NBK_drawAdPlaceHolder(void* pfd, NRect* rect);


#ifdef __cplusplus
}
#endif

#endif /* IMAGEPLAYER_H_ */
