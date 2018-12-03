#ifndef __NBK_GDI_H__
#define __NBK_GDI_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* interface for GDI operation */

typedef int32    NFontId;

/*
typedef enum _NEBRUSHSTYLE {
    NEBRUSH_NULL,
    NEBRUSH_SOLID
} NEBRUSHSTYLE;
*/

typedef struct _NPoint {
    coord   x;
    coord   y;
} NPoint;

typedef struct _NSize {
    coord   w;
    coord   h;
} NSize;

typedef struct _NRect {
    coord   l;
    coord   t;
    coord   r;
    coord   b;
} NRect;

typedef struct _NColor {
    uint8    r;
    uint8    g;
    uint8    b;
    uint8    a;
} NColor;

/***********************************
 * implement by platform
 */

void NBK_gdi_drawText(void* pfd, const wchr* text, int length, const NPoint* pos, const int decor);
void NBK_gdi_drawRect(void* pfd, const NRect* rect);
void NBK_gdi_drawCircle(void* pfd, const NRect* rect);
void NBK_gdi_clearRect(void* pfd, const NRect* rect);

NFontId NBK_gdi_getFont(void* pfd, int16 pixel, bd_bool bold, bd_bool italic);
void NBK_gdi_useFont(void* pfd, NFontId id);
void NBK_gdi_releaseFont(void* pfd);
coord NBK_gdi_getFontHeight(void* pfd, NFontId id);
coord NBK_gdi_getFontAscent(void* pfd, NFontId id);

coord NBK_gdi_getCharWidth(void* pfd, NFontId id, const wchr ch);
coord NBK_gdi_getTextWidth(void* pfd, NFontId id, const wchr* text, int length);

void NBK_gdi_setPenColor(void* pfd, const NColor* color);
void NBK_gdi_setBrushColor(void* pfd, const NColor* color);

void NBK_gdi_setClippingRect(void* pfd, const NRect* rect);
void NBK_gdi_cancelClippingRect(void* pfd);

void NBK_helper_getViewableRect(void* pfd, NRect* view);
//void NBK_helper_getViewableDocRect(void* pfd, NRect* rect);
//coord NBK_helper_getScreenCoord(void* pfd, coord x);
//coord NBK_helper_getDocumentCoord(void* pfd, coord x);

// for editor, work in screen coordinate
coord NBK_gdi_getCharWidthByEditor(void* pfd, NFontId id, const wchr ch);
coord NBK_gdi_getFontHeightByEditor(void* pfd, NFontId id);
coord NBK_gdi_getTextWidthByEditor(void* pfd, NFontId id, const wchr* text, int length);

void NBK_gdi_drawEditorCursor(void* pfd, NPoint* pt, coord xOffset, coord cursorHeight);
void NBK_gdi_drawEditorScrollbar(void* pfd, NPoint* pt, coord xOffset, coord yOffset, NSize* size);
void NBK_gdi_drawEditorText(void* pfd, const wchr* text, int length, const NPoint* pos, coord xOffset);

/***********************************
 * implement by kernel
 */

coord rect_getWidth(const NRect* rect);
coord rect_getHeight(const NRect* rect);
void rect_setWidth(NRect* rect, coord width);
void rect_setHeight(NRect* rect, coord height);
void rect_unite(NRect* dst, NRect* src);
void rect_intersect(NRect* dst, const NRect* src);
bd_bool rect_isIntersect(const NRect* dst, const NRect* src);
void rect_move(NRect* rect, coord x, coord y);
void rect_grow(NRect* rect, coord dx, coord dy);
bd_bool rect_hasPt(NRect* dst, coord x, coord y);
bd_bool rect_isContain(const NRect* dst, const NRect* src);
bd_bool rect_isEmpty(const NRect* rect);
void rect_toView(NRect* rect, NFloat factor);
void rect_toDoc(NRect* rect, NFloat factor);
void rect_set(NRect* rect, coord x, coord y, coord w, coord h);

coord nbk_gdi_getTextWidth_utf8(void* pfd, NFontId id, uint8* text, int length);
void nbk_gdi_drawText_utf8(void* pfd, NFontId id, const uint8* text, int length, NPoint* pos);

#ifdef __cplusplus
}
#endif

#endif
