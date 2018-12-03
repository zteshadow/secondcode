/*
 * nbk_stdc.cpp
 *
 *  Created on: 2010-12-19
 *      Author: samuel.song.bc@gmail.com
 */

#include "config.h"
#include "str.h"

#include "cm_lib.h"
#include "cm_io.h"
#include "cm_string.h"
#include "cm_debug.h"


void NBK_Assert(int32 value)
{
    CM_ASSERT(value);
}

void* nbk_malloc(bd_size_t size)
{
    CM_ASSERT(size > 0);
    return cm_malloc(size);
}

void* nbk_malloc0(bd_size_t size)
{
    void *p;

    CM_ASSERT(size > 0);
    p = cm_malloc(size);
    if (p)
    {
        cm_memset(p, 0, size);
    }

    return p;
}

void* nbk_realloc(void* ptr, bd_size_t size)
{
    void *p;

    CM_ASSERT(size > 0);
    p = cm_malloc(size);
    if (p)
    {
        if (ptr)
        {
            cm_memcpy(p, ptr, size);
        }
    }

    if (ptr)
    {
        cm_free(ptr);
    }

    return p;
}

void nbk_free(void* p)
{
    CM_ASSERT(p != 0);
    cm_free(p);
}

int32 NBK_sprintf(int8 *str, const int8 *format, ...)
{
    va_list ap;
    S32 value;

    CM_ASSERT(str != 0);
	
	va_start(ap, format);
	value = cm_vsprintf(str, format, ap);
	va_end(ap);

	return value;
}

#if defined(_BD_MEM_DEBUG_)
void* nbk_malloc_ext(const int8 *fname, int32 line, bd_size_t size)
{
    CM_ASSERT(size > 0);
    return cm_malloc_ext(fname, line, size);
}

void* nbk_malloc0_ext(const int8 *fname, int32 line, bd_size_t size)
{
    void *p;
    
    CM_ASSERT(size > 0);

    p = nbk_malloc_ext(fname, line, size);
    if (p)
    {
        cm_memset(p, 0, size);
    }

    return p;
}

void* nbk_realloc_ext(const int8 *fname, int32 line, void* ptr, bd_size_t size)
{
    void *p;
    
    CM_ASSERT(size > 0);
    p = cm_malloc_ext(fname, line, size);
    if (p)
    {
        if (ptr)
        {
            cm_memcpy(p, ptr, size);
        }
    }

    if (ptr)
    {
        cm_free_ext(ptr);
    }

    return p;
}

void nbk_free_ext(void* p)
{
    CM_ASSERT(p != 0);
    cm_free_ext(p);
}

#endif //_BD_MEM_DEBUG_

void NBK_memcpy(void* dst, void* src, bd_size_t size)
{
    cm_memcpy(dst, src, size);
}

void NBK_memset(void* dst, int8 value, bd_size_t size)
{
    cm_memset(dst, value, size);
}

void NBK_memmove(void* dst, void* src, bd_size_t size)
{
    cm_memmove(dst, src, size);
}

int NBK_atoi(const char* s)
{
    return cm_atoi(s);
}

uint32 NBK_htoi(const char* s)
{
    return cm_strtoul(s, 0, 0);
}

#define MAX_NBK_FLOAT_BIT   32

NFloat NBK_atof(const char *s)
{
	NFloat f = {0};
	S8 *p;
	S8 num[MAX_NBK_FLOAT_BIT] = {0};

    CM_ASSERT(s != 0);
    if (s)
    {
    	p = cm_strchr(s, '.');
    	if (p)
    	{
    	    CM_ASSERT(p - s < MAX_NBK_FLOAT_BIT);
    	    cm_strncpy(num, s, (p - s));
        }
    	else
    	{
    	    CM_ASSERT(cm_strlen(s) < MAX_NBK_FLOAT_BIT);
    		cm_strcpy(num, s);
        }

    	f.i = cm_atoi(num);

    	if (p)
    	{
    	    cm_strcpy(num, &p[1]);
    	    f.f = cm_atoi(num);
    	}
    	else
    	{
    		f.f = 0;
        }
    }
    
	return f;
}

