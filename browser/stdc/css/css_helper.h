/*
 * css_helper.h
 *
 *  Created on: 2011-1-4
 *      Author: wuyulun
 */

#ifndef CSS_HELPER_H_
#define CSS_HELPER_H_

#include "../inc/config.h"
#include "../inc/nbk_gdi.h"
#include "../css/cssSelector.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NCssStr {
    char*   s;
    int16   l;
} NCssStr;

typedef struct _NCssDecl {
    NCssStr p;
    NCssStr v;
} NCssDecl;

bd_bool css_parseColor(const char* colorStr, int len, NColor* color);
//BOOL css_parseBox(const char* valStr, int len, NCssBox* box);
//BOOL css_parseFontSize(const char* valStr, int len, int8* size);

//uint8 css_parseFloat(const char* str, int len);
uint8 css_parseAlign(const char* str, int len);

//void css_parseBackground(const char* str, int len, NCssStyle* cs);
void css_parseNumber(const char* str, int len, NCssValue* val);

nid css_getPropertyId(const char* prop, int len);
int css_parseDecl(const char* style, int len, NCssDecl decl[], int max);
int css_parseValues(const char* str, int len, NCssValue vals[], int max, void* user);

void css_addStyle(NCssStyle* s1, const NCssStyle* s2);

coord css_calcBgPos(coord w, coord bw, nid type, coord x);
coord css_calcValue(NCssValue val, int32 base, void* style, int32 defVal);

coord calcHoriAlignDelta(coord maxw, coord w, nid ha);
coord calcVertAlignDelta(coord maxb, coord b, nid va);

void css_boxAddSide(NCssBox* dst, const NCssBox* src);
//void css_boxNormalize(NCssBox* box);

#ifdef __cplusplus
}
#endif

#endif /* CSS_HELPER_H_ */
