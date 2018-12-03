/*
 * page.h
 *
 *  Created on: 2010-12-25
 *      Author: wuyulun
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "../inc/config.h"
#include "../inc/nbk_settings.h"
#include "../inc/nbk_gdi.h"
#include "../inc/nbk_callback.h"
#include "../css/cssSelector.h"
#include "../loader/loader.h"
#include "../loader/upCmd.h"
#include "../tools/slist.h"
#include "document.h"
#include "view.h"

#ifdef __cplusplus
extern "C" {
#endif
    
enum NEWORKSTATE {
    
    NEWORK_IDLE,
    NEWORK_PARSE,
    NEWORK_LAYOUT,
    NEWORK_PAINT
};

typedef struct _NBK_Page {
    
    nid         id;
    
    uint8       workState;
    uint8       mode;
    
    uint8       main : 1;
    uint8       begin : 1;
    uint8       attach : 1;
    uint8       disSubpage : 1;
    uint8       cache : 1;
    uint8       refresh : 1;
    uint8       wbxml : 1;
    uint8       pkgOver : 1;
    
    uint8       incMode; // counter
    
    NSettings*  settings;
    
    NDocument*  doc;
    NView*      view;
    NSheet*     sheet;
    
    int         total;
    int         rece; // data received
    
    NBK_Callback    cbEventNotify; // for all events from page
    
    void*       platform;
    
    struct _NBK_Page*   parent;
    NSList*             subPages;
    
    struct _NBK_Page*   modPage;
    struct _NBK_Page*   incPage;
    
    void*       history;
    
} NBK_Page, NPage;

#ifdef NBK_MEM_TEST
int page_memUsed(const NPage* page);
void mem_checker(const NPage* page);
#endif

int page_testApi(NBK_Page* page); // for test

NBK_Page* page_create(NPage* parent);
void page_delete(NBK_Page** page);

void page_setPlatformData(NBK_Page* page, void* platform);
void page_setScreenWidth(NBK_Page* page, coord width);
void page_setId(NPage* page, nid id);

void page_enablePic(NPage* page, bd_bool enable);

NUpCmdSet page_decideRequestMode(NPage* page, const char* url, bd_bool submit);

void page_loadData(NPage* page, const char* url, const char* str, int length);
void page_loadUrl(NPage* page, const char* url, nid enc, char* body, NFileUpload** files, bd_bool submit);
void page_loadUrl_ff(NPage* page, NUpCmd** cmd, const char* url, NFileUpload** files, nid via, bd_bool noDeal);
void page_loadFromCache(NPage* page);
void page_refresh(NPage* page);

bd_bool page_4ways(NBK_Page* page);

coord page_width(NBK_Page* page);
coord page_height(NBK_Page* page);

void page_paint(NBK_Page* page, NRect* rect);
void page_paintControl(NBK_Page* page, NRect* rect);
void page_paintMainBodyBorder(NBK_Page* page, NRect* rect, NNode* focus);

void page_setEventNotify(NBK_Page* page, NBK_CALLBACK func, void* user);

void page_onResponse(NEResponseType type, void* user, void* data, int length, int comprLen);
void page_onFFPackageOver(NRequest* request);

void page_stop(NPage* page);

void page_setFocusedNode(NPage* page, NNode* node);

bd_bool page_hasMainBody(NPage* page);
void page_createMainBody(NPage* page, NNode* root, NPage* mainBody);
void page_resetMainBody(NPage* mainBody);
NNode* page_getMainBodyByPos(NPage* page, coord x, coord y);
bd_bool page_isPaintMainBodyBorder(NPage* page, NRect* rect);
bd_bool page_isMainBodyMode(NPage* page);

void page_updateCtrlNode(NPage* page, NNode* node,NRect* rect);
bd_bool page_isCtrlNode(NPage* page, NNode* node);

NPage* page_getMainPage(NPage* page);
nid page_getStopId(NPage* page);

void page_layout(NPage* page, bd_bool force);
bd_bool page_layoutTextFF(NPage* page);
bd_bool page_isLayoutFinish(NPage* page); // ff only

#ifdef __cplusplus
}
#endif

#endif /* PAGE_H_ */
