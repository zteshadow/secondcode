/*
 * editbox : a simple editor
 *
 *  Created on: 2011-3-27
 *      Author: migr
 */

#ifndef EDITBOX_H_
#define EDITBOX_H_

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../dom/event.h"
#include "../render/renderNode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NEditBox {

    wchr*   text;
    wchr*   mask; // for password

    bd_bool    password : 1;

    int16   max;
    int16   use;
    int16   cur;
    int16   anchorPos;
//    NPoint  lastCursorPos;
    
    coord   xoff; // offset of x in paint

} NEditBox;

#ifdef NBK_MEM_TEST
int editBox_memUsed(const NEditBox* box);
#endif

NEditBox* editBox_create(bd_bool masked);
void editBox_delete(NEditBox** edit);

void editBox_setMaxLength(NEditBox* edit, int16 length);
void editBox_setText(NEditBox* edit, uint8* text, int16 length);
void editBox_setText16(NEditBox* edit, wchr* text, int16 len);
char* editBox_getText(NEditBox* edit);
int editBox_getTextLen(NEditBox* edit);
bd_bool editBox_handleKey(NEditBox* edit, NEvent* event, NRect* vr, NFontId fontId);
void editBox_draw(NEditBox* edit, NFontId fontId, NRect* pr, bd_bool op, NStyle* style);
void editBox_delSelText(NEditBox* edit);
void editBox_clearSelText(NEditBox* edit);
void editBox_clearAllText(NEditBox* edit);
void editBox_maskAll(NEditBox* edit);
void editBox_cursorChanged(NEditBox* edit, bd_bool left);

#ifdef __cplusplus
}
#endif

#endif /* EDITBOX_H_ */
