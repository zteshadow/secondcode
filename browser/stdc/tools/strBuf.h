/*
 * strBuf.h
 *
 *  Created on: 2011-8-2
 *      Author: migr
 */

#ifndef STRBUF_H_
#define STRBUF_H_

#include "../inc/config.h"
#include "slist.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct _NStrBufSeg {
    char*   data;
    int     len;
} NStrBufSeg;

typedef struct _NStrBuf {
    NSList* lst;
} NStrBuf;

NStrBuf* strBuf_create(void);
void strBuf_delete(NStrBuf** strBuf);
void strBuf_reset(NStrBuf* strBuf);

void strBuf_appendStr(NStrBuf* strBuf, const char* str, int length);
bd_bool strBuf_getStr(NStrBuf* strBuf, char** str, int* len, bd_bool begin);
void strBuf_joinAllStr(NStrBuf* strBuf);

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H_ */
