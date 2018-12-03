/*
 * history.h
 *
 *  Created on: 2011-5-8
 *      Author: migr
 */

#ifndef HISTORY_H_
#define HISTORY_H_

#include "../inc/config.h"
#include "../inc/nbk_settings.h"
#include "../inc/nbk_gdi.h"
#include "page.h"

#define NBK_HISTORY_MAX     16

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NHistoryItem {
    nid     id;
    NFloat  zoom; // zoom level used
    NPoint  lastPos; // last view position
    char*   url;
} NHistoryItem;

typedef struct _NHistory {
    
    NPage*          page;
    int16           cur;
    int16           max;
    NHistoryItem    list[NBK_HISTORY_MAX];
    nid             preId;
    
    bd_bool            mbMode : 1;
    NPage*          frag; // for main body mode
    
} NHistory, NBK_History;

NHistory* history_create(void);
void history_delete(NHistory** history);

NPage* history_curr(NHistory* history);
void history_add(NHistory* history);
bd_bool history_prev(NHistory* history);
bd_bool history_next(NHistory* history);
bd_bool history_goto(NHistory* history, int16 where);
nid history_getPreId(NHistory* history);

void history_getRange(NHistory* history, int16* cur, int16* used);

void history_setZoom(NHistory* history, NFloat zoom);
NFloat history_getZoom(NHistory* history);

bd_bool history_mainBodyMode(NHistory* history);
void history_setMainBodyWidth(NHistory* history, coord w);
void history_enterMainBodyMode(NHistory* history, NNode* body);
NNode* history_getMainBodyByPos(NHistory* history, coord x, coord y);

void history_setPlatformData(NHistory* history, void* platform);
void history_setSettings(NHistory* history, NSettings* settings);
void history_enablePic(NHistory* history, bd_bool enable);

void history_quitMainBodyWithClick(NHistory* history);

NPoint history_getLastViewPos(NHistory* history, bd_bool checkUrl);
bd_bool history_isLastPage(NHistory* history);

void history_setUrl(NHistory* history, const char* url);

// Implemented by platform.

void NBK_resourceClean(void* pfd, nid id);
    
#ifdef __cplusplus
}
#endif

#endif /* HISTORY_H_ */
