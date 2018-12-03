/*
 * css_helper.c
 *
 *  Created on: 2011-1-4
 *      Author: wuyulun
 */

#include "css_helper.h"
#include "../inc/nbk_limit.h"
#include "../tools/str.h"
#include "../tools/dump.h"
#include "../dom/xml_helper.h"
#include "../dom/page.h"
#include "../render/renderNode.h"

#include "color.h"
#include "css_val.h"
#include "css_prop.h"
#include "css_value.h"

#define DEBUG_PARSE     0

#define CSS_IS_NUM(c) (c == '-' || c == '.' || (c >= '0' && c <= '9'))

static uint8* css_find_sep(const uint8* s, const uint8* e, bd_bool* end)
{
    uint8* p = (uint8*)s;
    uint8 de = 0;
    int8 lv = 0;
    
    while (*p && p < e) {

        if (*p == '(') {
            de = ')';
            lv++;
        }
        
        if (de == 0) {
            if (*p <= 0x20 || *p == ';' || *p == '}' || *p == ',') {
                *end = N_FALSE;
                return p;
            }
        }
        else if (*p == de) {
            lv--;
            if (lv <= 0)
            de = 0;
        }
        
        p++;
    }
    *end = N_TRUE;
    return p;
}

static bd_bool compare_prop(const char* p1, int len, const char* p2)
{
    if (len >= 0) {
        int i, len2;
        
        len2 = NBK_strlen(p2);
        if (len != len2)
            return N_FALSE;
        
        for (i=0; i < len; i++)
            if (p1[i] != p2[i])
                return N_FALSE;
        return N_TRUE;
    }
    else {
        return ((NBK_strcmp(p1, p2) == 0) ? N_TRUE : N_FALSE);
    }
}

nid css_getPropertyId(const char* prop, int len)
{
    char p[MAX_CSS_PROP_LEN + 1];
    if (len > MAX_CSS_PROP_LEN)
        return N_INVALID_ID;
    NBK_strncpy(p, prop, len);
    return binary_search_id(css_getPropNames(), CSSID_LAST, p);
}

bd_bool css_parseColor(const char* colorStr, int len, NColor* color)
{
    char* p = (char*)colorStr;
    
    // case 1: #xxxxxx
    if (*p == '#') {
        char hex[8];
        char *q, *tooFar;
        uint32 value;
        bd_bool end;

        tooFar = (len == -1) ? (char*)N_MAX_UINT : p + len;
        q = (char*)css_find_sep((uint8*)++p, (uint8*)tooFar, &end);
        if (q - p == 6)
            NBK_strncpy(hex, p, 6);
        else if (q - p == 3) {
            hex[0] = hex[1] = *p++;
            hex[2] = hex[3] = *p++;
            hex[4] = hex[5] = *p++;
            hex[6] = 0;
        }
        else
            return N_FALSE;
        
        value = NBK_htoi(hex);
        color->r = (value & 0xff0000) >> 16;
        color->g = (value & 0xff00) >> 8;
        color->b = (value & 0xff);
        color->a = 255;
        
        return N_TRUE;
    }
    // case 2: color name
    else {
        nid id;
        char name[MAX_COLOR_LEN + 1];
        int ln = (len == -1) ? NBK_strlen(p) : len;
        if (ln > MAX_COLOR_LEN)
            return N_FALSE;
        NBK_strncpy(name, p, ln);
        str_toLower(p, ln);
        id = binary_search_id(css_getColorNames(), CC_LASTCOLOR, name);
        if (id != N_INVALID_ID) {
            *color = css_getColorValues()[id];
            return N_TRUE;
        }
    }
    
    return N_FALSE;
}

static bd_bool css_parse_rgba(const char* colorStr, int len, NColor* color)
{
    // case: rgb(255, 0, 255)
    // case: rgba(128,128,128,0)
    
    char* p = (char*)colorStr;
    char* q;
    char* toofar = (len == -1) ? (char*)N_MAX_UINT : p + len;
    bd_bool end, hasAlpha;
    
    if (*(p+3) == 'a') {
        hasAlpha = N_TRUE;
        p += 5;
    }
    else {
        hasAlpha = N_FALSE;
    p += 4;
    }
    
    color->r = NBK_atoi(p);
    
    q = (char*)css_find_sep((uint8*)p, (uint8*)toofar, &end);
    p = q + 1;
    color->g = NBK_atoi(p);
    
    p = (char*)str_skip_invisible_char((uint8*)p, (uint8*)toofar, &end);
    q = (char*)css_find_sep((uint8*)p, (uint8*)toofar, &end);
    p = q + 1;
    color->b = NBK_atoi(p);
    
    if (hasAlpha) {
        p = (char*)str_skip_invisible_char((uint8*)p, (uint8*)toofar, &end);
        q = (char*)css_find_sep((uint8*)p, (uint8*)toofar, &end);
        p = q + 1;
        color->a = NBK_atoi(p);
    }
    else
        color->a = 255; // opacity
    
    return N_TRUE;
}

enum NEParseDeclState {
    PDS_PRO_BEGIN,
    PDS_PRO_END,
    PDS_VAL_BEGIN,
    PDS_VAL_END
};

int css_parseDecl(const char* style, int len, NCssDecl decl[], int max)
{
    uint8 *p, *tooFar;
    int i = 0;
    nid stat = PDS_PRO_BEGIN;
    char de = ';';

    p = (uint8*)style;
    tooFar = (len == -1) ? (uint8*)N_MAX_UINT : (uint8*)style + len;
    
    NBK_memset(decl, 0, sizeof(NCssDecl) * max);
    while (*p && p < tooFar && i < max) {
        
        if (*p <= 0x20) {
            if (stat == PDS_PRO_BEGIN || stat == PDS_VAL_BEGIN) {
                p++;
                continue;
            }
            if (stat == PDS_PRO_END) {
                decl[i].p.l = p - (uint8*)decl[i].p.s;
                stat = PDS_VAL_BEGIN;
            }
        }
        else if (*p == ':') {
            if (stat == PDS_PRO_END) {
                decl[i].p.l = p - (uint8*)decl[i].p.s;
                stat = PDS_VAL_BEGIN;
                de = ';';
            }
        }
        else if (*p == de) {
            if (de == ')')
                de = ';';
            else {
            if (decl[i].v.s)
                decl[i].v.l = p - (uint8*)decl[i].v.s;
            stat = PDS_PRO_BEGIN;
        }
        }
        else {
            switch (stat) {
            case PDS_PRO_BEGIN:
                decl[i].p.s = (char*)p;
                stat = PDS_PRO_END;
                break;
            case PDS_VAL_BEGIN:
                decl[i].v.s = (char*)p;
                stat = PDS_VAL_END;
                if (NBK_strncmp((char*)p, "url(", 4) == 0)
                    de = ')';
                break;
            }
        }
        p++;
        if (stat == PDS_PRO_BEGIN)
            i++;
    }
    if (stat == PDS_VAL_END) {
        decl[i].v.l = p - (uint8*)decl[i].v.s;
        i++;
    }
    
    return i;
}

//#define BOX_MAX_VAL_NUM 4
//#define BOX_MAX_VAL_LEN 8
//
//BOOL css_parseBox(const char* valStr, int len, NCssBox* box)
//{
//    uint8 vals[BOX_MAX_VAL_NUM][BOX_MAX_VAL_LEN];
//    int i, j, n;
//    uint8* p = (uint8*)valStr;
//    uint8* tooFar = (len == -1) ? (uint8*)N_MAX_UINT : (uint8*)valStr + len;
//
//    NBK_memset(vals, 0, BOX_MAX_VAL_NUM * BOX_MAX_VAL_LEN);
//    i = j = 0;
//    while (*p && p < tooFar && i < BOX_MAX_VAL_NUM && j < BOX_MAX_VAL_LEN - 1) {
//        
//        if (*p <= 0x20) {
//            if (j) {
//                vals[i][j] = 0;
//                i++;
//                j = 0;
//            }
//        }
//        else {
//            vals[i][j] = *p;
//            j++;
//        }
//        p++;
//    }
//    if (i < BOX_MAX_VAL_NUM) {
//        vals[i][j] = 0;
//        if (j) i++;
//        n = i;
//    }
//    else {
//        n = BOX_MAX_VAL_NUM;
//    }
//    
//    switch (n) {
//    case 1:
//        box->t = box->b = box->l = box->r = NBK_atoi((char*)vals[0]);
//        break;
//        
//    case 2:
//        box->t = box->b = NBK_atoi((char*)vals[0]);
//        box->l = box->r = NBK_atoi((char*)vals[1]);
//        break;
//        
//    case 3:
//        box->t = NBK_atoi((char*)vals[0]);
//        box->l = box->r = NBK_atoi((char*)vals[1]);
//        box->b = NBK_atoi((char*)vals[2]);
//        break;
//        
//    case 4:
//        box->t = NBK_atoi((char*)vals[0]);
//        box->r = NBK_atoi((char*)vals[1]);
//        box->b = NBK_atoi((char*)vals[2]);
//        box->l = NBK_atoi((char*)vals[3]);
//        break;
//        
//    default:
//        return N_FALSE;
//    }
//    
//    return N_TRUE;
//}

//BOOL css_parseFontSize(const char* valStr, int len, int8* size)
//{
//    char v[16];
//    int n;
//    
//    NBK_strncpy(v, valStr, len);
//    n = NBK_atoi(v);
//    if (n <= 0 || n > MAX_FONT_SIZE)
//        return N_FALSE;
//    
//    *size = (int8)n;
//    return N_TRUE;
//}
    
//uint8 css_parseFloat(const char* str, int len)
//{
//    uint8 rt = CSS_FLOAT_NONE;
//    
//    if (compare_prop(str, len, "left"))
//        rt = CSS_FLOAT_LEFT;
//    else if (compare_prop(str, len, "right"))
//        rt = CSS_FLOAT_RIGHT;
//    
//    return rt;
//}

uint8 css_parseAlign(const char* str, int len)
{
    uint8 rt = CSS_ALIGN_LEFT;
    
    if (compare_prop(str, len, "center"))
        rt = CSS_ALIGN_CENTER;
    else if (compare_prop(str, len, "right"))
        rt = CSS_ALIGN_RIGHT;
    
    return rt;
}

void css_addStyle(NCssStyle* dst, const NCssStyle* src)
{
    dst->flo = src->flo;
    dst->clr = src->clr;
    dst->font_weight = src->font_weight;
    dst->z_index = src->z_index;
    
    if (src->width.type)
        dst->width = src->width;
    if (src->height.type)
        dst->height = src->height;
    if (src->max_width.type)
        dst->max_width = src->max_width;
    
    if (src->text_indent.type)
        dst->text_indent = src->text_indent;
    if (src->line_height.type)
        dst->line_height = src->line_height;
    
    if (src->hasDisplay) {
        dst->hasDisplay = 1;
        dst->display = src->display;
    }

    if (src->hasTextAlign) {
        dst->hasTextAlign = 1;
        dst->text_align = src->text_align;
    }
    
    if (src->hasTextDecor) {
        dst->hasTextDecor = 1;
        dst->text_decor = src->text_decor;
    }
    
    if (src->hasVertAlign) {
        dst->hasVertAlign = 1;
        dst->vert_align = src->vert_align;
    }
    
    if (src->hasFontsize) {
        dst->hasFontsize = 1;
        dst->font_size = src->font_size;
    }
    
    if (src->hasColor) {
        dst->hasColor = 1;
        dst->color = src->color;
    }
    
    if (src->hasBgcolor) {
        dst->hasBgcolor = 1;
        dst->background_color = src->background_color;
    }
    
    if (src->hasBrcolorL) {
        dst->hasBrcolorL = 1;
        dst->border_color_l = src->border_color_l;
    }
    
    if (src->hasBrcolorR) {
        dst->hasBrcolorR = 1;
        dst->border_color_r = src->border_color_r;
    }
    
    if (src->hasBrcolorT) {
        dst->hasBrcolorT = 1;
        dst->border_color_t = src->border_color_t;
    }
    
    if (src->hasBrcolorB) {
        dst->hasBrcolorB = 1;
        dst->border_color_b = src->border_color_b;
    }
    
    if (src->hasMargin) {
        dst->hasMargin = 1;
        css_boxAddSide(&dst->margin, &src->margin);
    }
    
    if (src->hasBorder) {
        dst->hasBorder = 1;
        css_boxAddSide(&dst->border, &src->border);
    }
    
    if (src->hasPadding) {
        dst->hasPadding = 1;
        css_boxAddSide(&dst->padding, &src->padding);
    }
    
    if (src->hasOverflow) {
        dst->hasOverflow = 1;
        dst->overflow = src->overflow;
    }
    
    if (src->bgImage) {
        dst->bgImage = src->bgImage;
        dst->bg_repeat = src->bg_repeat;
        dst->bg_x = src->bg_x; dst->bg_x_t = src->bg_x_t;
        dst->bg_y = src->bg_y; dst->bg_y_t = src->bg_y_t;
    }
    
    if (src->hasOpacity) {
        dst->hasOpacity = 1;
        dst->opacity = src->opacity;
    }
}

static void css_parse_url(const char* def, int len, char** url)
{
    char* p = (char*)def;
    char* tooFar = (len == -1) ? (char*)N_MAX_UINT : p + len;
    char t, *q;

    *url = N_NULL;
    p += 4;
    if (NBK_strncmp(p, "data:", 5) == 0)
        return;
    t = (*p == '\'' || *p == '\"') ? *p++ : ')';
    q = p;
    while (*p && p < tooFar && *p != t) p++;
    if (*p == t) {
        *url = (char*)NBK_malloc(p - q + 1);
        NBK_strncpy(*url, q, p - q);
    }
}

//void css_parseBackground(const char* str, int len, NCssStyle* cs)
//{
//    uint8* p = (uint8*)str;
//    uint8* tooFar = (len == -1) ? (uint8*)N_MAX_UINT : p + len;
//    uint8* q;
//    BOOL end = N_FALSE;
//
//    while (!end) {
//        q = css_find_sep(p, tooFar, &end);
//        if (q > p) {
//            
//            if (*p == 'u') {
//                css_parse_url((char*)p, q - p, &cs->bgImage);
//            }
//            else if (css_parseColor((char*)p, q - p, &cs->background_color))
//                cs->hasBgcolor = 1;
//            
//            p = str_skip_invisible_char(q, tooFar, &end);
//        }
//        else
//            break;
//    }
//}

void css_parseNumber(const char* str, int len, NCssValue* val)
{
    char v[16];
    char* p = (char*)str;
    char* tooFar = (len == -1) ? (char*)N_MAX_UINT : p + len;
    int i = 0;
    bd_bool flo = N_FALSE;
    
    while (*p && p < tooFar) {
        if (CSS_IS_NUM(*p)) {
            v[i++] = *p;
            if (*p == '.')
                flo = N_TRUE;
        }
        else
            break;
        p++;
    }
    v[i] = 0;
    
    if (*p == 'e' && *(p+1) == 'm') {
        val->d.flo = NBK_atof(v);
        val->type = NECVT_EM;
    }
    else if (*p == '%') {
        val->d.perc = (int16)NBK_atoi(v);
        val->type = NECVT_PERCENT;
    }
    else if (flo) {
        val->d.flo = NBK_atof(v);
        val->type = NECVT_FLOAT;
    }
    else {
        val->d.i32 = NBK_atoi(v);
        val->type = NECVT_INT;
    }
}

int css_parseValues(const char* str, int len, NCssValue vals[], int max, void* user)
{
    //NPage* page = (NPage*)user;
    uint8* p = (uint8*)str;
    uint8* tooFar = (len == -1) ? (uint8*)N_MAX_UINT : p + len;
    uint8* q;
    bd_bool end = N_FALSE;
    int i = 0;
    nid id;
    uint8 t;

    NBK_memset(vals, 0, sizeof(NCssValue) * max);
    while (!end && i < max) {
        q = css_find_sep(p, tooFar, &end);
        if (q > p) {
#if DEBUG_PARSE
            if (page) {
                dump_char(page, (char*)p, q - p);
                dump_return(page);
            }
#endif
            
            if (*p == '-' && *(p+1) == 'w') { // -webkit
#if DEBUG_PARSE
                if (page) {
                    dump_char(page, "\tskip", -1);
                    dump_return(page);
                }
#endif
            }
            else if (CSS_IS_NUM(*p)) {
                css_parseNumber((char*)p, q - p, &vals[i]);
            }
            else if (*p == '#') {
                if (css_parseColor((char*)p, q - p, &vals[i].d.color))
                    vals[i].type = NECVT_COLOR;
            }
            else if (NBK_strncmp_nocase((char*)p, "rgb", 3) == 0) {
                if (css_parse_rgba((char*)p, q - p, &vals[i].d.color))
                    vals[i].type = NECVT_COLOR;
            }
            else if (NBK_strncmp_nocase((char*)p, "url", 3) == 0) {
                css_parse_url((char*)p, q - p, &vals[i].d.url);
                if (vals[i].d.url)
                    vals[i].type = NECVT_URL;
            }
            else {
                t = *q;
                *q = 0;
                id = binary_search_id(css_getColorNames(), CC_LASTCOLOR, (char*)p);
                if (id != N_INVALID_ID) {
                    vals[i].d.color = css_getColorValues()[id];
                    vals[i].type = NECVT_COLOR;
                }
                else {
                    id = binary_search_id(css_getValueNames(), CSSVAL_LAST, (char*)p);
                    if (id != N_INVALID_ID) {
                        vals[i].d.value = id;
                        vals[i].type = NECVT_VALUE;
                    }
                }
                *q = t;
            }

            if (vals[i].type != NECVT_NONE)
                i++;
            p = str_skip_invisible_char(q, tooFar, &end);
        }
        else
            break;
    }
    
    return i;
}

coord css_calcBgPos(coord w, coord bw, nid type, coord x)
{
    coord n;
    
    if (type == CSS_POS_PERCENT)
        n = (w * x / 100) - (bw * x / 100);
    else
        n = x;
    
    return n;
}

//static int calc_precision(int decimal)
//{
//    int prec = 10;
//    while (decimal / prec)
//        prec *= 10;
//    return prec;
//}

coord css_calcValue(NCssValue val, int32 base, void* style, int32 defVal)
{
    coord c = defVal;
    int scale;
    
    if (val.type == NECVT_INT) {
        c = val.d.i32;
    }
    else if (val.type == NECVT_PERCENT) {
        if (base != 0) {
            scale = NFLOAT_EMU / 100;
            c = base * scale / 100 * val.d.perc / scale;
        }
    }
    else if (val.type == NECVT_EM) {
        base = ((NStyle*)style)->font_size;
        c = base * val.d.flo.i;
        c += base * val.d.flo.f / NFLOAT_EMU;
    }
    else if (val.type == NECVT_FLOAT) {
        c = base * val.d.flo.i;
        c += base * val.d.flo.f / NFLOAT_EMU;
    }
    
    return c;
}

coord calcHoriAlignDelta(coord maxw, coord w, nid ha)
{
    coord da = 0;
    
    if (ha == CSS_ALIGN_CENTER)
        da = (maxw - w) / 2;
    else if (ha == CSS_ALIGN_RIGHT)
        da = maxw - w;
    
    return da;
}

coord calcVertAlignDelta(coord maxb, coord b, nid va)
{
    coord da = 0;
    
    if (va == CSS_VALIGN_MIDDLE)
        da = (maxb - b) / 2;
    else if (va == CSS_VALIGN_BOTTOM)
        da = maxb - b;
    
    return da;
}

void css_boxAddSide(NCssBox* dst, const NCssBox* src)
{
    if (src->l != N_INVALID_COORD)
        dst->l = src->l;
    if (src->t != N_INVALID_COORD)
        dst->t = src->t;
    if (src->r != N_INVALID_COORD)
        dst->r = src->r;
    if (src->b != N_INVALID_COORD)
        dst->b = src->b;
}

//void css_boxNormalize(NCssBox* box)
//{
//    if (box->l == N_INVALID_COORD)
//        box->l = 0;
//    if (box->t == N_INVALID_COORD)
//        box->t = 0;
//    if (box->r == N_INVALID_COORD)
//        box->r = 0;
//    if (box->b == N_INVALID_COORD)
//        box->b = 0;
//}
