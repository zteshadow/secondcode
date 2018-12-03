
#ifndef _CM_GDI_H
#define _CM_GDI_H


#include "cm_data_type.h"


typedef struct
{
    S32 x;
    S32 y;

} CM_POINT;

typedef struct
{
    S32 w;
    S32 h;

} CM_GDI_SIZE;

typedef struct
{
    S32 x;
    S32 y;
    S32 w;
    S32 h;

} CM_RECT;

typedef struct
{
    U8 r;
    U8 g;
    U8 b;
    U8 a;

} CM_COLOR;

typedef S32 CM_FONT;

typedef enum
{
	CM_IMG_BMP,
	CM_IMG_JPG,
	CM_IMG_GIF,
	CM_IMG_PNG,

	CM_IMG_UNKNOWN

} CM_IMG_TYPE;

typedef struct
{
    CM_IMG_TYPE type;
	S32 width;
	S32 height;
	void *user_data;

} CM_IMAGE;


#ifdef __cplusplus
extern "C"
{
#endif


void cm_gdi_draw_text(void* pfd, const U16* text, S32 len, const CM_POINT* pos);

//draw with pen
void cm_gdi_draw_rect(void* pfd, const CM_RECT* rect);

//draw with brush
void cm_gdi_fill_rect(void* pfd, const CM_RECT* rect);

//draw with pen
void cm_gdi_draw_circle(void* pfd, const CM_RECT* rect);
void cm_gdi_draw_line(void *pfd, const CM_POINT *start, const CM_POINT *end);

void cm_gdi_set_font(void* pfd, CM_FONT id);
CM_FONT cm_gdi_get_font(void* pfd, S16 pixel, CM_BOOL bold, CM_BOOL italic);
S32 cm_gdi_get_font_height(void* pfd, CM_FONT id);
S32 cm_gdi_get_font_ascent(void* pfd, CM_FONT id);
void cm_gdi_release_font(void* pfd);

S32 cm_ged_gdi_char_width(void* pfd, CM_FONT id, const U16 ch);
S32 cm_gdi_get_text_width(void* pfd, CM_FONT id, const U16* text, S32 len);

void cm_gdi_set_pen_color(void* pfd, const CM_COLOR* color);
void cm_gdi_set_brush_color(void* pfd, const CM_COLOR* color);

void cm_gdi_set_clip(void* pfd, const CM_RECT* rect);
void cm_gdi_cancel_clip(void* pfd);

void cm_gdi_get_view_rect(void* pfd, CM_RECT* view);

CM_IMAGE *cm_image_decode(const void *data, S32 len);
S32 cm_image_draw(void *h, S32 x, S32 y, CM_IMAGE *img);
void cm_image_free(CM_IMAGE *img);

void cm_gdi_update_screen(void* pfd, const CM_RECT* rect);


#ifdef __cplusplus
}
#endif


#endif //_CM_GDI_H

