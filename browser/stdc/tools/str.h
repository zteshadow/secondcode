/*
 * str.h
 *
 *  Created on: 2010-12-26
 *      Author: wuyulun
 */

#ifndef STR_H_
#define STR_H_

#include "../inc/config.h"

#ifdef __cplusplus
extern "C" {
#endif

// for ascii
int NBK_strlen(const char* s);
int NBK_strcmp(const char* s1, const char* s2);
int NBK_strncmp(const char* s1, const char* s2, int len);
int NBK_strncmp_nocase(const char* s1, const char* s2, int len);
char* NBK_strcpy(char* dst, const char* src);
char* NBK_strncpy(char* dst, const char* src, int len);
int nbk_strfind(const char* str, const char* sub);
int nbk_strfind_nocase(const char* str, const char* sub);
int nbk_strnfind_nocase(const char* str, const char* sub, int len);

void str_toLower(char* s, int len);

int NBK_atoi(const char* s);
uint32 NBK_htoi(const char* s);
NFloat NBK_atof(const char* s);
void NBK_md5(const char* s, int len, uint8* md5);

//#define NBK_sprintf(s, f, args ...) sprintf(s, f, args)

// for unicode
int NBK_wcslen(const wchr* s);
int8 NBK_wcscmp(const wchr* s1, const wchr* s2);
wchr* NBK_wcscpy(wchr* dst, const wchr* src);
wchr* NBK_wcsncpy(wchr* dst, const wchr* src, int len);
int NBK_wcsfind(const wchr* str, const wchr* sub);

// helper
uint8* str_skip_invisible_char(const uint8* s, const uint8* e, bd_bool* end);
void nbk_unescape(char* dst, char* src);
    
#ifdef __cplusplus
}
#endif

#endif /* STR_H_ */
