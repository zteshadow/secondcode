/*
 * nbk_gdi.c
 *
 *  Created on: 2011-1-7
 *      Author: wuyulun
 */

#include "../inc/nbk_gdi.h"
#include "unicode.h"

int32 nfloat_imul(int32 src, NFloat factor)
{
#if NBK_DISABLE_ZOOM
    return src;
#else
    return (src * factor.i) + (src * factor.f / NFLOAT_EMU);
#endif
}

int32 nfloat_idiv(int32 src, NFloat factor)
{
#if NBK_DISABLE_ZOOM
    return src;
#else
    return (src * NFLOAT_EMU) / (factor.i * NFLOAT_EMU + factor.f);
#endif
}

coord rect_getWidth(const NRect* rect)
{
    return rect->r - rect->l;
}

coord rect_getHeight(const NRect* rect)
{
    return rect->b - rect->t;
}

void rect_setWidth(NRect* rect, coord width)
{
    rect->r = rect->l + width;
}

void rect_setHeight(NRect* rect, coord height)
{
    rect->b = rect->t + height;
}

void rect_unite(NRect* dst, NRect* src)
{
    dst->l = N_MIN(dst->l, src->l);
    dst->t = N_MIN(dst->t, src->t);
    dst->r = N_MAX(dst->r, src->r);
    dst->b = N_MAX(dst->b, src->b);
}

void rect_intersect(NRect* dst, const NRect* src)
{
    coord l, t, r, b;
    
    l = N_MAX(dst->l, src->l);
    t = N_MAX(dst->t, src->t);
    r = N_MIN(dst->r, src->r);
    b = N_MIN(dst->b, src->b);
    
    dst->l = l;
    dst->t = t;
    dst->r = r;
    dst->b = b;
    
    if (dst->l > dst->r)
        dst->r = dst->l;
    if (dst->t > dst->b)
        dst->b = dst->t;
}

bd_bool rect_isIntersect(const NRect* dst, const NRect* src)
{
    coord l, t, r, b;
    
    l = N_MAX(dst->l, src->l);
    t = N_MAX(dst->t, src->t);
    r = N_MIN(dst->r, src->r);
    b = N_MIN(dst->b, src->b);
    
    return (l >= r || t >= b) ? N_FALSE : N_TRUE;
}

void rect_move(NRect* rect, coord x, coord y)
{
    coord w = rect->r - rect->l;
    coord h = rect->b - rect->t;
    rect->l = x;
    rect->r = x + w;
    rect->t = y;
    rect->b = y + h;
}

void rect_grow(NRect* rect, coord dx, coord dy)
{
    rect->l -= dx;
    rect->r += dx;
    rect->t -= dy;
    rect->b += dy;
}

coord nbk_gdi_getTextWidth_utf8(void* pfd, NFontId id, uint8* text, int length)
{
    coord width = 0;
    uint8* p = text;
    uint8* tooFar = (length == -1) ? (uint8*)N_MAX_UINT : p + length;
    wchr hz;
    int8 offset;
    
    while (*p && p < tooFar) {

        hz = uni_utf8_to_utf16(p, &offset);
        p += offset;
        width += NBK_gdi_getCharWidth(pfd, id, hz);
    }
    
    return width;
}

void nbk_gdi_drawText_utf8(void* pfd, NFontId id, const uint8* text, int length, NPoint* pos)
{
    coord w;
    uint8* p = (uint8*)text;
    uint8* tooFar = (length == -1) ? (uint8*)N_MAX_UINT : p + length;
    wchr hz;
    int8 offset;
    
    while (*p && p < tooFar) {
        
        hz = uni_utf8_to_utf16(p, &offset);
        NBK_gdi_drawText(pfd, &hz, 1, pos, 0);
        p += offset;
        w = NBK_gdi_getCharWidth(pfd, id, hz);
        pos->x += w;
    }
}

bd_bool rect_hasPt(NRect* dst, coord x, coord y)
{
    if (x >= dst->l && x < dst->r && y >= dst->t && y < dst->b)
        return N_TRUE;
    else
        return N_FALSE;
}

bd_bool rect_isContain(const NRect* dst, const NRect* src)
{
    if (   src->l >= dst->l && src->t >= dst->t
        && src->r <= dst->r && src->b <= dst->b)
        return N_TRUE;
    else
        return N_FALSE;
}

bd_bool rect_isEmpty(const NRect* rect)
{
    return ((rect->l == 0 && rect->t == 0 && rect->r == 0 && rect->b == 0) ? N_TRUE : N_FALSE);
}

void rect_toView(NRect* rect, NFloat factor)
{
#if !NBK_DISABLE_ZOOM
    rect->l = (coord)nfloat_imul(rect->l, factor);
    rect->t = (coord)nfloat_imul(rect->t, factor);
    rect->r = (coord)nfloat_imul(rect->r, factor);
    rect->b = (coord)nfloat_imul(rect->b, factor);
#endif
}

void rect_toDoc(NRect* rect, NFloat factor)
{
#if !NBK_DISABLE_ZOOM
    rect->l = (coord)nfloat_idiv(rect->l, factor);
    rect->t = (coord)nfloat_idiv(rect->t, factor);
    rect->r = (coord)nfloat_idiv(rect->r, factor);
    rect->b = (coord)nfloat_idiv(rect->b, factor);
#endif
}

void rect_set(NRect* rect, coord x, coord y, coord w, coord h)
{
    rect->l = x;
    rect->t = y;
    rect->r = rect->l + w;
    rect->b = rect->t + h;
}
