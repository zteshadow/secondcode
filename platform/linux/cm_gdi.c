
#include "cm_gdi.h"
#include "cm_debug.h"

#if defined(CM_WIN32_GDI)
#include <windows.h>
#endif


void cm_gdi_draw_text(void* pfd, const U16* text, S32 len, const CM_POINT* pos)
{
#if defined(CM_WIN32_GDI)
    TextOut((HDC)pfd, pos->x, pos->y, (LPCTSTR)text, len);
#endif //CM_WIN32_GDI
}

void cm_gdi_draw_rect(void* pfd, const CM_RECT* rect)
{
}

void cm_gdi_fill_rect(void* pfd, const CM_RECT* rect)
{
}

void cm_gdi_draw_circle(void* pfd, const CM_RECT* rect)
{
#if defined(CM_WIN32_GDI)
	Ellipse((HDC)pfd, rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
#endif //CM_WIN32_GDI
}

void cm_gdi_draw_line(void *pfd, const CM_POINT *start, const CM_POINT *end)
{
#if defined(CM_WIN32_GDI)
	MoveToEx((HDC)pfd, start->x, start->y, 0);
	LineTo((HDC)pfd, end->x, end->y);
#endif //CM_WIN32_GDI
}

void cm_gdi_set_font(void* pfd, CM_FONT id)
{
}

CM_FONT cm_gdi_get_font(void* pfd, S16 pixel, CM_BOOL bold, CM_BOOL italic)
{
    CM_FONT font = 0;
    return font;
}

S32 cm_gdi_get_font_height(void* pfd, CM_FONT id)
{
    S32 h = 0;
    return h;
}

S32 cm_gdi_get_font_ascent(void* pfd, CM_FONT id)
{
    S32 h = 0;
    return h;
}

void cm_gdi_release_font(void* pfd)
{
}

S32 cm_ged_gdi_char_width(void* pfd, CM_FONT id, const U16 ch)
{
    return 0;
}

S32 cm_gdi_get_text_width(void* pfd, CM_FONT id, const U16* text, S32 len)
{
    return 0;
}

void cm_gdi_set_pen_color(void* pfd, const CM_COLOR* color)
{
}

void cm_gdi_set_brush_color(void* pfd, const CM_COLOR* color)
{
}

void cm_gdi_set_clip(void* pfd, const CM_RECT* rect)
{
}

void cm_gdi_cancel_clip(void* pfd)
{
}

void cm_gdi_get_view_rect(void* pfd, CM_RECT* view)
{
}

CM_IMAGE *cm_image_decode(const void *data, S32 len)
{
    return 0;
}

S32 cm_image_draw(void *h, S32 x, S32 y, CM_IMAGE *img)
{
    return -1;
}

void cm_image_free(CM_IMAGE *img)
{
}

void cm_gdi_update_screen(void* pfd, const CM_RECT* rect)
{
}
