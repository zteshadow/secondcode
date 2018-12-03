/*
 * document.h
 *
 *  Created on: 2010-12-25
 *      Author: wuyulun
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "../inc/config.h"
#include "../inc/nbk_public.h"
#include "../inc/nbk_timer.h"
#include "../inc/nbk_limit.h"
#include "xml_tokenizer.h"
#include "node.h"
#include "view.h"
#include "xpath.h"
#include "event.h"
#include "wbxmlDec.h"
#include "../tools/strPool.h"
#include "../tools/strBuf.h"
#include "../tools/memAlloc.h"
#include "../loader/upCmd.h"
#include "../loader/loader.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef enum _NEDocType {
    
    NEDOC_UNKNOWN,
    //---
    NEDOC_MAINBODY,
    //---
    NEDOC_WML,
    NEDOC_XHTML,
    NEDOC_HTML,
    //---
    NEDOC_DXXML,
    NEDOC_FULL,
    NEDOC_FULL_XHTML,
    NEDOC_NARROW,
    //---
    NEDOC_LAST
    
} NEDocType;

typedef enum _NEEncoding {
    
    NEENC_UNKNOWN,
    NEENC_UTF8,
    NEENC_GB2312
    
} NEEncoding;

typedef enum _NEIncOpType {
    
    NEINCOP_NONE,
    NEINCOP_DELETE,
    NEINCOP_INSERT,
    NEINCOP_INSERT_AFTER,
    NEINCOP_INSERT_AS_CHILD,
    NEINCOP_REPLACE
    
} NEIncOpType;

typedef enum _NERedirectType {
    
     NEREDIR_NONE,
     NEREDIR_WML_ONTIMER,
     NEREDIR_WML_ONEVENT,
     NEREDIR_META_REFRESH
     
} NERedirectType;

// CSS manager

typedef struct _NExtCss {
    char*       url;
    NStrBuf*    dat;
    uint8       state;
} NExtCss;

typedef struct _NExtCssMgr {
    NExtCss     d[MAX_EXT_CSS_NUM];
    int16       cur;
    int16       use;
    NStrBuf*    cached; // inner css
    NTimer*     timer;
    void*       doc;
    bd_bool        ready;
} NExtCssMgr;

// DOM document

typedef struct _NDocument {
    
    void*       page; // not own, type is NPage
    NNode*      root;
    
    NXmlTokenizer*   tokenizer;
    
    // for build tree
    struct {
        NNode** dat;
        int16   top;
    } stack;
    NNode*  lastChild;
    
    bd_bool    finish : 1; // parse over
    bd_bool    layoutFinish : 1;
    bd_bool    inTitle : 1;
    bd_bool    inStyle : 1;
    bd_bool    inScript : 1;
    bd_bool    inCtrl : 1;
    bd_bool    inTable : 1;
    bd_bool    firstShow : 1; // is the first screen displayed
    bd_bool    picUpdate : 1;
    bd_bool    hasSpace : 1;
    bd_bool    stop : 1;
    bd_bool    syncMode : 1;
    bd_bool    ownStrPool : 1;
    bd_bool    disable : 1; // disable display
    
    bd_bool    sentMouseMove : 1; // over -> move -> move -> move -> out, just send one move
    
    NStrPool*   spool; // for attributes
    NTimer*     layoutTimer;
    int         words;
    int         wordProcessed;
    int         wordCount;
    
    NTimer*     loadTimer;
    char*       url;
    int16       urlMaxLen;
    
    nid         type; // NEDocType
    
    wchr*       title;
    
    NNode*      modNode;
    int8        incOp;
    
    NSList*     shortcutLst;
    NSList*     mbList; // list of main body
    
    int         delayLoad; // for redirect
    nid         delayLoadType; // NERedirectType
    
    nid*        fix;
    
    // for decoding
    NEEncoding  encoding;
    uint8       gbRest; // the second char in a GB
    
    // for focus grab
    NNode*      lastGotFocus;
    
    NMemAlloc*  nodeAlloc;
    
    NExtCssMgr* extCssMgr;
    
    NWbxmlDecoder*  wbxmlDecoder;
    
    // for SPREADSTRUM crop.
    // when document includes specific keywords, only display after be confirmed.
    wchr*   ssKeywords;
    wchr**  ssKeyList;
    
} NDocument;

typedef struct _NLoadCmd {
    
    NDocument*  doc;
    char*       url;
    char*       body;
    NUpCmd*     upcmd;
    nid         enc; // body encode type
    
    nid         via;
    
    NFileUpload*    files;
    
    bd_bool        noDeal : 1;
    bd_bool        submit : 1;
    
} NLoadCmd;

#ifdef NBK_MEM_TEST
int doc_memUsed(const NDocument* doc);
#endif

NDocument* doc_create(void* page);
void doc_delete(NDocument** doc);

void doc_attachWbxmlDecoder(NDocument* doc, int length);
void doc_detachWbxmlDecoder(NDocument* doc);

void doc_begin(NDocument* doc);
void doc_clear(NDocument* doc);
void doc_end(NDocument* doc);
void doc_stop(NDocument* doc);

void doc_setUrl(NDocument* doc, const char* url);
void doc_write(NDocument* doc, const char* str, int length, bd_bool final);
void doc_processToken(NDocument* doc, NToken* token);
void doc_acceptType(NDocument* doc);

char* doc_getUrl(NDocument* doc);
wchr* doc_getTitle(NDocument* doc);
nid doc_getType(NDocument* doc);

char* doc_getFrameName(NDocument* doc);

NNode* doc_getFocusNode(NDocument* doc, const NRect* area);
NNode* doc_getFocusNodeByPos(NDocument* doc, coord x, coord y);
char* doc_getFocusUrl(NDocument* doc, NNode* focus);
NNode* doc_getPrevFocusNode(NDocument* doc, NNode* curr);
NNode* doc_getNextFocusNode(NDocument* doc, NNode* curr);
void doc_getFocusNodeRect(NDocument* doc, NNode* focus, NRect* rect);
void doc_clickFocusNode(NDocument* doc, NNode* focus);

// network load
void doc_doGet(NDocument* doc, const char* url, bd_bool submit, bd_bool noDeal);
void doc_doPost(NDocument* doc, const char* url, nid enc, char* body, NFileUpload* files);
void doc_doIa(NDocument* doc, NUpCmd* upcmd, NFileUpload* files);

// local load
void doc_doBack(NDocument* doc);
void doc_jumpTo(NDocument* doc, const char* name);

bd_bool doc_doIncOp(NDocument* doc, NEIncOpType op, char* xpath);
void doc_doIncOpLastWork(NDocument* doc, void* incPage);

void doc_viewChange(NDocument* doc);
void doc_modeChange(NDocument* doc, NEDocType type);

NNode* dom_query(NDocument* doc, NXPathCell* xpath, NNode** sibling, NNode** parent, bd_bool ignoreTag);

bd_bool doc_processKey(NEvent* event);
bd_bool doc_processPen(NEvent* event);
bd_bool doc_processDomEvent(NDocument* doc, NEvent* event1, NEvent* event2);
bd_bool doc_IsEditorFull(void* page);

void doc_dumpDOM(NDocument* doc);
void doc_outputXml(NDocument* doc);

void doc_buildShortcutLst(NDocument* doc);
NSList* doc_getShortcurLst(NDocument* doc);
wchr* doc_getShortcutNodeText(NDocument* doc, NNode* node, int* len);

void doc_updateParam(NDocument* doc, char** pairs);

NStrPool* doc_getStrPool(NDocument* doc);
void doc_setStrPool(NDocument* doc, NStrPool* spool, bd_bool own);

void doc_doMsgWin(NDocument* doc, int type, char* message);

bd_bool doc_isAbsLayout(const nid type);


// interfaces used by 3rd partly

// keywords format: key1,key2,...
void DOC_setConfirmKeywords(NDocument* doc, wchr* keywords);
NSList* DOC_getNodesByKeywords(NDocument* doc, wchr* keywords);

#ifdef __cplusplus
}
#endif

#endif /* DOCUMENT_H_ */
