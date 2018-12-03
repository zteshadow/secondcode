/*
 * loader.h
 *
 *  Created on: 2011-2-3
 *      Author: wuyulun
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "../inc/config.h"
#include "upCmd.h"

#ifdef __cplusplus
extern "C" {
#endif
    
enum NEStockPage {
    
    NESPAGE_ERROR,
    NESPAGE_ERROR_404
};

enum NEMimeType {
    
    NEMIME_UNKNOWN,
    
    NEMIME_DOC_HTML,
    NEMIME_DOC_XHTML,
    NEMIME_DOC_WML,
    NEMIME_DOC_WMLC,
    NEMIME_DOC_CSS,
    NEMIME_DOC_SCRIPT,
    
    NEMIME_IMAGE_GIF,
    NEMIME_IMAGE_JPEG,
    NEMIME_IMAGE_PNG,

    NEMIME_FF_XML,
    NEMIME_FF_IMAGE,
    NEMIME_FF_BG_IMAGE,
    NEMIME_FF_INC_DATA,
    
    NEMIME_LAST
};

enum NEEncType {
    
    NEENCT_URLENCODED,
    NEENCT_MULTIPART,
    NEENCT_TEXT
};

enum NERequestType {
    
    NEREQT_NONE,
    NEREQT_MAINDOC,
    NEREQT_SUBDOC,
    NEREQT_CSS,
    NEREQT_IMAGE,
    NEREQT_IMAGE_PACK,
    NEREQT_WAP_WL
};

enum NERequestVia {
    
    NEREV_STANDARD,
    NEREV_UCK,
    NEREV_FF_FULL,
    NEREV_FF_FULL_XHTML,
    NEREV_FF_NARROW,
    NEREV_FF_FLASH,
    NEREV_FF_MULTIPICS,
    NEREV_FF_PIC,
    NEREV_SEV_WAPWL
};

enum NERequestMethod {
    
    NEREM_NORMAL,
    NEREM_REFRESH,
    NEREM_HISTORY,
    NEREM_JUST_CACHE
};

typedef enum _NEResponseType {

    NEREST_NONE,
    NEREST_HEADER,
    NEREST_DATA,
    NEREST_COMPLETE,
    NEREST_ERROR,
    NEREST_CANCEL,
    
    NEREST_PKG_DATA_HEADER,
    NEREST_PKG_DATA,
    NEREST_PKG_DATA_COMPLETE,
    
    NEREST_INC_RECEIVE,
    NEREST_INC_URL,
    NEREST_INC_OP_DELETE,
    NEREST_INC_OP_INSERT,
    NEREST_INC_OP_REPLACE,
    NEREST_INC_DATA,
    NEREST_INC_DATA_COMPLETE,
    
    NEREST_PARAM,
    
    NEREST_MSGWIN,
    
    NEREST_UNUSED
    
} NEResponseType;

enum NELoadError {
    
    NELERR_ERROR = -1,
    NELERR_NOCACHE = -2,
    NELERR_FF_NOT_SUPPORT = -3,
    NELERR_NBK_NOT_SUPPORT = -4
};

typedef struct _NFileUpload {
    
    char*   name;
    char*   path;
    int     nidx;
    
} NFileUpload;

typedef struct _NRequest {
    
    uint8   type;
    uint8   via;
    uint8   method;
    uint8   enc;
    uint8   ownUrl : 1;
    uint8   ownUser : 1;
    uint8   ownReferer : 1;
    uint8   noDeal : 1; // this document hasn't be processed by previous request
    int16   idx;    // for ff-package
    nid     pageId;
    char*   url;
    char*   referer;
    char*   body;
    void*   user;
    void*   platform;
    NUpCmd* upcmd;
    
    NFileUpload*    files;
    
    void(*responseCallback)(NEResponseType type, void* user, void* data, int length, int comprLen);
    
} NRequest, NBK_Request;

typedef struct _NRespHeader {
    
    nid     mime;
    int     content_length;
    void*   user;
    
} NRespHeader;

typedef struct _NLoader {

    int     count;
    
} NLoader;

NLoader* loader_get(void);
void loader_release(void);

NRequest* loader_createRequest(void);
void loader_deleteRequest(NRequest** req);
void loader_setRequest(NRequest* req, uint8 type, char* url, void* user);
void loader_setRequestUrl(NRequest* req, char* url, bd_bool own);
void loader_setRequestUser(NRequest* req, void* user);
void loader_setRequestReferer(NRequest* req, char* referer, bd_bool own);

bd_bool loader_load(NRequest* request);
void loader_stopAll(void* pfd, nid pageId);

void loader_onReceiveHeader(NRequest* req, NRespHeader* header);
void loader_onReceiveData(NRequest* req, char* data, int length, int comprLen);
void loader_onComplete(NRequest* req);
void loader_onCancel(NRequest* req);
void loader_onError(NRequest* req, int error);
void loader_onPackage(NRequest* req, NEResponseType type, void* data, int v1, int v2);
void loader_onPackageOver(NRequest* req);

// implemented by platform
bd_bool NBK_handleError(int error);
char* NBK_getStockPage(int type);
bd_bool NBK_resourceLoad(void* pfd, NBK_Request* req);
void NBK_resourceStopAll(void* pfd, nid pageId);

#ifdef __cplusplus
}
#endif

#endif /* LOADER_H_ */
