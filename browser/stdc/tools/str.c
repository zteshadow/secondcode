/*
 * str.c
 *
 *  Created on: 2010-12-26
 *      Author: wuyulun
 */

#include "../dom/char_ref.h"
#include "../dom/xml_helper.h"
#include "str.h"

#define MI_MAX_REF_LEN  8

int NBK_strlen(const char* s)
{
    int i = 0;
    if (s) {
        while (s[i]) i++;
    }
    return i;
}

int NBK_strcmp(const char* s1, const char* s2)
{
    int i = 0;
    while (s1[i] && s2[i]) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        i++;
    }
    if (s1[i] == 0 && s2[i] == 0)
        return 0;
    else if (s1[i] == 0)
        return -1;
    else
        return 1;
}

int NBK_strncmp(const char* s1, const char* s2, int len)
{
    int i = 0;
    while (s1[i] && s2[i] && i < len) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        i++;
    }
    if (i == len)
        return 0;
    if (s1[i] == 0 && s2[i] == 0)
        return 0;
    else if (s1[i] == 0)
        return -1;
    else
        return 1;
}

int NBK_strncmp_nocase(const char* s1, const char* s2, int len)
{
    int i = 0;
    char a, b;
    
    while (s1[i] && s2[i] && i < len) {
        
        a = s1[i];
        b = s2[i];
        if (a >= 'A' && a <= 'Z') a += 32;
        if (b >= 'A' && b <= 'Z') b += 32;
        
        if (a < b)
            return -1;
        else if (a > b)
            return 1;
        i++;
    }
    
    if (i == len)
        return 0;
    if (s1[i] == 0 && s2[i] == 0)
        return 0;
    else if (s1[i] == 0)
        return -1;
    else
        return 1;
}

char* NBK_strcpy(char* dst, const char* src)
{
    int i = 0;
    if (src) {
    while (src[i]) {
        dst[i] = src[i];
        i++;
    }
    }
    dst[i] = 0;
    return dst;
}

char* NBK_strncpy(char* dst, const char* src, int len)
{
    int i;
    for (i=0; i < len; i++)
        dst[i] = src[i];
    dst[i] = 0;
    return dst;
}

int nbk_strfind(const char* string, const char* sub)
{
    char* p = (char*)string;
    char* q = (char*)sub;
    int s;
    
    while (*p && *q) {
        if (*q == *p) {
            p++;
            q++;
        }
        else {
            s = q - sub;
            q = (char*)sub;
            p -= s;
            p++;
        }
    }
    
    if (*q)
        return -1;
    else {
        s = NBK_strlen(sub);
        return p - string - s;
    }
}

int nbk_strfind_nocase(const char* string, const char* sub)
{
    return nbk_strnfind_nocase(string, sub, -1);
}

int nbk_strnfind_nocase(const char* str, const char* sub, int len)
{
    char* p = (char*)str;
    char* q = (char*)sub;
    int s;
    char a, b;
    char* toofar = (len == -1) ? (char*)N_MAX_UINT : p + len;
    
    while (*p && *q && p < toofar) {
        a = *p;
        b = *q;
        
        if (a >= 'A' && a <= 'Z')
            a += 32;
        if (b >= 'A' && b <= 'Z')
            b += 32;
             
        if (a == b) {
            p++;
            q++;
        }
        else {
            s = q - sub;
            q = (char*)sub;
            p -= s;
            p++;
        }
    }

    if (*q)
        return -1;
    else {
        s = NBK_strlen(sub);
        return p - str - s;
    }
}

int NBK_wcslen(const wchr* s)
{
    int i=0;
    while (s[i] != 0)
        i++;
    return i;
}

int8 NBK_wcscmp(const wchr* s1, const wchr* s2)
{
    int i = 0;
    while (s1[i] && s2[i]) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        i++;
    }
    if (s1[i] == 0 && s2[i] == 0)
        return 0;
    else if (s1[i] == 0)
        return -1;
    else
        return 1;
}

wchr* NBK_wcscpy(wchr* dst, const wchr* src)
{
    int i;
    for (i=0; src[i] > 0; i++)
        dst[i] = src[i];
    dst[i] = 0;
    return dst;
}

wchr* NBK_wcsncpy(wchr* dst, const wchr* src, int len)
{
    int i;
    for (i=0; i < len; i++)
        dst[i] = src[i];
    dst[i] = 0;
    return dst;
}

int NBK_wcsfind(const wchr* str, const wchr* sub)
{
    wchr* p = (wchr*) str;
    wchr* q = (wchr*) sub;
    int s;

    while (*p && *q) {
        if (*q == *p) {
            p++;
            q++;
        }
        else {
            s = q - sub;
            q = (wchr*) sub;
            p -= s;
            p++;
        }
    }

    if (*q)
        return -1;
    else {
        s = NBK_wcslen(sub);
        return p - str - s;
    }
}

/*
void NBK_memset(void* dst, int8 value, size_t size)
{
    uint8* p = (uint8*)dst;
    size_t i = 0;
    for (; i < size; i++)
        p[i] = value;
}
*/

// return: 1 ok; 0 reach end
uint8* str_skip_invisible_char(const uint8* s, const uint8* e, bd_bool* end)
{
    uint8* p = (uint8*)s;
    while (*p && p < e && *p <= 0x20)
        p++;
    *end = (*p == 0 || p == e) ? N_TRUE : N_FALSE;
    return p;
}

void str_toLower(char* s, int len)
{
    int l = (len == -1) ? NBK_strlen(s) : len;
    int i;
    for (i=0; i < l; i++) {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] += 32;
    }
}

void nbk_unescape(char* dst, char* src)
{
    char* p = (char*)src;
    char* q;
    int i;
    nid id;
    
    while (*p) {
        if (*p == '&') {
            i = 0;
            q = p;
            while (*q && i < MI_MAX_REF_LEN && *q != ';') {
                q++;
                i++;
            }
            if (i < MI_MAX_REF_LEN && *q == ';') {
                *q = 0;
                id = binary_search_id(xml_getCharrefNames(), CHARREF_TOTAL, p+1);
                *q = ';';
                if (id != N_INVALID_ID) {
                    NBK_strcpy(dst, xml_getEntityNames()[id]);
                    dst += NBK_strlen(xml_getEntityNames()[id]);
                    p = q + 1;
                    continue;
                }
            }
        }
        *dst++ = *p++;
    }
    *dst = 0;
}
