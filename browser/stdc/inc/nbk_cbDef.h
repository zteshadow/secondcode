/*
 * nbk_cbDef.h
 *
 *  Created on: 2011-2-10
 *      Author: wuyulun
 */

#ifndef NBK_CBDEF_H_
#define NBK_CBDEF_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
    
enum NBKEventType {

    NBK_EVENT_NONE = 0,
    NBK_EVENT_NEW_DOC,
    NBK_EVENT_HISTORY_DOC,
    NBK_EVENT_DO_IA,
    NBK_EVENT_WAITING,
    NBK_EVENT_GOT_RESPONSE,
    NBK_EVENT_NEW_DOC_BEGIN,
    NBK_EVENT_GOT_DATA,
    NBK_EVENT_UPDATE,
    NBK_EVENT_UPDATE_PIC,
    NBK_EVENT_LAYOUT_FINISH,
    NBK_EVENT_DOWNLOAD_IMAGE,
    NBK_EVENT_PAINT_CTRL,
    NBK_EVENT_ENTER_MAINBODY,
    NBK_EVENT_QUIT_MAINBODY,
    NBK_EVENT_QUIT_MAINBODY_AFTER_CLICK,
    NBK_EVENT_LOADING_ERROR_PAGE,
    NBK_EVENT_REPOSITION,
    NBK_EVENT_GOT_INC,
    NBK_EVENT_DOWNLOAD_FILE,
    NBK_EVENT_DEBUG1,
    
    NBK_EVENT_LAST
};

typedef struct _NBK_Event_NewDoc {
    char* url;
} NBK_Event_NewDoc;

typedef struct _NBK_Event_GotResponse {
    int total;
    int received;
} NBK_Event_GotResponse;

typedef struct _NBK_Event_GotImage {
    int total;
    int curr;
    int receivedSize;
    int totalSize;
} NBK_Event_GotImage;

typedef struct _NBK_Event_RePosition {
    int x;
    int y;
    NFloat  zoom;
} NBK_Event_RePosition;

typedef struct _NBK_Event_DownloadFile {
    int     size;
    char*   url;
    char*   fileName;
    char*   cookie;
} NBK_Event_DownloadFile;

typedef enum _NBKDbgType {
    
    NBKDBG_NONE,
    NBKDBG_UINT,
    NBKDBG_INT,
    NBKDBG_WCHR,
    NBKDBG_CHAR,
    NBKDBG_RETURN,
    NBKDBG_TIME,
    NBKDBG_FLUSH,
    NBKDBG_TAB
    
} NBKDbgType;

typedef struct _NBK_DbgInfo {
    
    NBKDbgType  t;
    
    union {
        int32   si;
        uint32  ui;
        wchr*   wp;
        char*   cp;
        bd_bool    on;
    } d;
    int         len; // length of string
    
} NBK_DbgInfo;

#if 0

#define NBK_DBG_DEF()           NBK_DbgInfo __dbgInfo;
#define NBK_DBG_VAR             __dbgInfo
#define NBK_DBG_INT(i)          __dbgInfo.t = NBKDBG_INT; __dbgInfo.d.si = i;
#define NBK_DBG_UINT(n)         __dbgInfo.t = NBKDBG_UINT; __dbgInfo.d.ui = n;
#define NBK_DBG_WCHR(wc, l)     __dbgInfo.t = NBKDBG_WCHR; __dbgInfo.d.wp = wc; __dbgInfo.len = l;
#define NBK_DBG_CHAR(mb, l)     __dbgInfo.t = NBKDBG_CHAR; __dbgInfo.d.cp = mb; __dbgInfo.len = l;
#define NBK_DBG_RETURN()        __dbgInfo.t = NBKDBG_RETURN;
#define NBK_DBG_SUBMIT(page)    nbk_cb_call(NBK_EVENT_DEBUG1, &##page->cbEventNotify, &__dbgInfo);

//#else

#define NBK_DBG_DEF()
#define NBK_DBG_VAR
#define NBK_DBG_INT(i)
#define NBK_DBG_UINT(n)
#define NBK_DBG_WCHR(wc, l)
#define NBK_DBG_CHAR(mb, l)
#define NBK_DBG_RETURN()
#define NBK_DBG_SUBMIT(page)

#endif

#ifdef __cplusplus
}
#endif

#endif /* NBK_CBDEF_H_ */
