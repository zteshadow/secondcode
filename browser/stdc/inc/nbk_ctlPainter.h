#ifndef __RENDER_CUSTOM_PAINT_H__
#define __RENDER_CUSTOM_PAINT_H__

#include "nbk_gdi.h"

#ifdef __cplusplus
extern "C" {
#endif	

typedef enum _NECtrlState {
	NECS_NORMAL,
	NECS_HIGHLIGHT,
	NECS_INEDIT
} NECtrlState;

bd_bool NBK_paintText(void* pfd, const NRect* rect, NECtrlState state);
bd_bool NBK_paintCheckbox(void* pfd, const NRect* rect, NECtrlState state, bd_bool checked);
bd_bool NBK_paintRadio(void* pfd, const NRect* rect, NECtrlState state, bd_bool checked);
bd_bool NBK_paintButton(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state);
bd_bool NBK_paintSelectNormal(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state);
bd_bool NBK_paintSelectExpand(void* pfd, const wchr* items, int num, int* sel);
bd_bool NBK_paintTextarea(void* pfd, const NRect* rect, NECtrlState state);
bd_bool NBK_paintBrowse(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state);

bd_bool NBK_paintFoldBackground(void* pfd, const NRect* rect, NECtrlState state);

// for editor
bd_bool NBK_editInput(void* pfd, NFontId fontId, NRect* rect, const wchr* text, int len);
bd_bool NBK_editTextarea(void* pfd, NFontId fontId, NRect* rect, const wchr* text, int len);

// open a file select dialog by framework
bd_bool NBK_browseFile(void* pfd, const char* oldFile, char** newFile);

bd_bool NBK_dlgAlert(void* pfd, const wchr* text, int len);
bd_bool NBK_dlgConfirm(void* pfd, const wchr* text, int len, int* ret);
bd_bool NBK_dlgPrompt(void* pfd, const wchr* text, int len, int* ret, char** input);

#ifdef __cplusplus
}
#endif

#endif	// __RENDER_CUSTOM_PAINT_H__
