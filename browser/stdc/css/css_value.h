/* this file is generated automatically */

#ifndef __CSS_VALUE_H__
#define __CSS_VALUE_H__

#ifdef __cplusplus
extern "C" {
#endif
    
enum NCSSVALUE {
	CSSVAL_BOLD = 1,
	CSSVAL_BOTH = 2,
	CSSVAL_BOTTOM = 3,
	CSSVAL_CENTER = 4,
	CSSVAL_DASHED = 5,
	CSSVAL_DOTTED = 6,
	CSSVAL_FIXED = 7,
	CSSVAL_HIDDEN = 8,
	CSSVAL_INLINE_BLOCK = 9,
	CSSVAL_LEFT = 10,
	CSSVAL_MIDDLE = 11,
	CSSVAL_NO_REPEAT = 12,
	CSSVAL_NONE = 13,
	CSSVAL_REPEAT_X = 14,
	CSSVAL_REPEAT_Y = 15,
	CSSVAL_RIGHT = 16,
	CSSVAL_SOLID = 17,
	CSSVAL_TOP = 18,
	CSSVAL_UNDERLINE = 19,
	CSSVAL_LAST = 19
};

void css_initVals(void);
void css_delVals(void);
const char** css_getValueNames();

#ifdef __cplusplus
}
#endif

#endif
