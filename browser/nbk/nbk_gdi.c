
#include "nbk_gdi.h"
#include "cm_gdi.h"
#include "cm_debug.h"

#include "nbk_web_view.h"


void NBK_gdi_drawText(void* pfd, const wchr* text, int length, const NPoint* pos, const int decor)
{
    CM_POINT point;

    point.x = pos->x;
    point.y = pos->y;
    
    cm_gdi_draw_text(((nbk_web_view *)pfd)->graphic, text, length, &point);
}

void NBK_gdi_drawRect(void* pfd, const NRect* rect)
{
    CM_RECT frame;
    
    frame.x = rect->l;
    frame.y = rect->t;
    frame.w = rect->r - rect->l;
    frame.h = rect->b - rect->t;
    
    cm_gdi_draw_rect(((nbk_web_view *)pfd)->graphic, &frame);
}

void NBK_gdi_drawCircle(void* pfd, const NRect* rect)
{
    CM_RECT frame;
    
    frame.x = rect->l;
    frame.y = rect->t;
    frame.w = rect->r - rect->l;
    frame.h = rect->b - rect->t;
    
    cm_gdi_draw_circle(((nbk_web_view *)pfd)->graphic, &frame);    
}

void NBK_gdi_clearRect(void* pfd, const NRect* rect)
{
    CM_RECT frame;
    CM_COLOR color;
    
    frame.x = rect->l;
    frame.y = rect->t;
    frame.w = rect->r - rect->l;
    frame.h = rect->b - rect->t;
    
    color.r = 0xFF;
    color.g = 0xFF;
    color.b = 0xFF;
    color.a = 0xFF;
    
    //cm_gdi_set_brush_color(((nbk_web_view *)pfd)->graphic, &color);
    cm_gdi_fill_rect(((nbk_web_view *)pfd)->graphic, &frame);
}

NFontId NBK_gdi_getFont(void* pfd, int16 pixel, bd_bool bold, bd_bool italic)
{
    CM_FONT id = cm_gdi_get_font(((nbk_web_view *)pfd)->graphic, pixel, bold, italic);
    return (NFontId)id;
}

void NBK_gdi_useFont(void* pfd, NFontId id)
{
    cm_gdi_set_font(((nbk_web_view *)pfd)->graphic, (CM_FONT)id);
}

void NBK_gdi_releaseFont(void* pfd)
{
    cm_gdi_release_font(((nbk_web_view *)pfd)->graphic);
}

coord NBK_gdi_getFontHeight(void* pfd, NFontId id)
{
    S32 h = cm_gdi_get_font_height(((nbk_web_view *)pfd)->graphic, (CM_FONT)id);
    return (coord)h;
}

coord NBK_gdi_getFontAscent(void* pfd, NFontId id)
{
    S32 h = cm_gdi_get_font_ascent(((nbk_web_view *)pfd)->graphic, (CM_FONT)id);
    return (coord)h;
}

coord NBK_gdi_getCharWidth(void* pfd, NFontId id, const wchr ch)
{
    S32 w = cm_ged_gdi_char_width(((nbk_web_view *)pfd)->graphic, (CM_FONT)id, ch);
    return (coord)w;
}

coord NBK_gdi_getTextWidth(void* pfd, NFontId id, const wchr* text, int length)
{
    S32 w = cm_gdi_get_text_width(((nbk_web_view *)pfd)->graphic, (CM_FONT)id, text, length);
    return (coord)w;
}

void NBK_gdi_setPenColor(void* pfd, const NColor* color)
{
    CM_COLOR c;
    
    c.r = color->r;
    c.g = color->g;
    c.b = color->b;
    c.a = color->a;
    
    cm_gdi_set_pen_color(((nbk_web_view *)pfd)->graphic, &c);
}

void NBK_gdi_setBrushColor(void* pfd, const NColor* color)
{
    CM_COLOR c;
    
    c.r = color->r;
    c.g = color->g;
    c.b = color->b;
    c.a = color->a;
    
    cm_gdi_set_brush_color(((nbk_web_view *)pfd)->graphic, &c);
}

void NBK_gdi_setClippingRect(void* pfd, const NRect* rect)
{
    CM_RECT frame;
    
    frame.x = rect->l;
    frame.y = rect->t;
    frame.w = rect->r - rect->l;
    frame.h = rect->b - rect->t;
    
    cm_gdi_set_clip(((nbk_web_view *)pfd)->graphic, &frame);
}

void NBK_gdi_cancelClippingRect(void* pfd)
{
    CM_RECT frame;
    
    cm_gdi_get_view_rect(((nbk_web_view *)pfd)->graphic, &frame);
    
    cm_gdi_set_clip(((nbk_web_view *)pfd)->graphic, &frame);    
}

coord NBK_gdi_getCharWidthByEditor(void* pfd, NFontId id, const wchr ch)
{
    coord cord = NBK_gdi_getCharWidth(pfd, id, ch);
    return cord;
}

coord NBK_gdi_getFontHeightByEditor(void* pfd, NFontId id)
{
    coord cord = NBK_gdi_getFontHeight(pfd, id);
    return cord;
}

coord NBK_gdi_getTextWidthByEditor(void* pfd, NFontId id, const wchr* text, int length)
{
    coord cord = NBK_gdi_getTextWidth(pfd, id, text, length);
    return cord;
}

void NBK_gdi_drawEditorCursor(void* pfd, NPoint* pt, coord xOffset, coord cursorHeight)
{
    CM_ASSERT(0);
}

void NBK_gdi_drawEditorScrollbar(void* pfd, NPoint* pt, coord xOffset, coord yOffset, NSize* size)
{
    CM_ASSERT(0);
}

void NBK_gdi_drawEditorText(void* pfd, const wchr* text, int length, const NPoint* pos, coord xOffset)
{
    NBK_gdi_drawText(pfd, text, length, pos, xOffset);
}

