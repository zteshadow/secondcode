/* this file is generated automatically */

#ifndef __XML_TAGS_H__
#define __XML_TAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

enum TAGS {
	TAGID_A = 1,
	TAGID_ANCHOR = 2,
	TAGID_B = 3,
	TAGID_BASE = 4,
	TAGID_BIG = 5,
	TAGID_BODY = 6,
	TAGID_BR = 7,
	TAGID_BUTTON = 8,
	TAGID_CARD = 9,
	TAGID_DIV = 10,
	TAGID_FONT = 11,
	TAGID_FORM = 12,
	TAGID_FRAME = 13,
	TAGID_FRAMESET = 14,
	TAGID_GO = 15,
	TAGID_H1 = 16,
	TAGID_H2 = 17,
	TAGID_H3 = 18,
	TAGID_H4 = 19,
	TAGID_H5 = 20,
	TAGID_H6 = 21,
	TAGID_HEAD = 22,
	TAGID_HR = 23,
	TAGID_HTML = 24,
	TAGID_IMG = 25,
	TAGID_INPUT = 26,
	TAGID_LABEL = 27,
	TAGID_LI = 28,
	TAGID_LINK = 29,
	TAGID_META = 30,
	TAGID_OBJECT = 31,
	TAGID_OL = 32,
	TAGID_ONEVENT = 33,
	TAGID_OPTION = 34,
	TAGID_P = 35,
	TAGID_PARAM = 36,
	TAGID_POSTFIELD = 37,
	TAGID_SCRIPT = 38,
	TAGID_SELECT = 39,
	TAGID_SMALL = 40,
	TAGID_SPAN = 41,
	TAGID_STRONG = 42,
	TAGID_STYLE = 43,
	TAGID_TABLE = 44,
	TAGID_TC_ATTACHMENT = 45,
	TAGID_TD = 46,
	TAGID_TEXTAREA = 47,
	TAGID_TIMER = 48,
	TAGID_TITLE = 49,
	TAGID_TR = 50,
	TAGID_UL = 51,
	TAGID_WML = 52,
	TAGID_TEXT = 53,
	TAGID_LASTTAG = 53,
	TAGID_CLOSETAG = 32000
};

#define MAX_TAG_LEN	13

void xml_initTags(void);
void xml_delTags(void);
const char** xml_getTagNames(void);

#ifdef __cplusplus
}
#endif

#endif
