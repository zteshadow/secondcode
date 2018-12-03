
/*
 * cm_mem_check.h
 *
 *  Created on: 21 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_MEM_CHECK_H
#define _CM_MEM_CHECK_H


#include "cm_data_type.h"


typedef void *CM_POINTER;


#ifdef __cplusplus
extern "C"
{
#endif


void cm_mem_check_init(U32 max);
void cm_mem_check(void);
void cm_mem_check_deinit(void);

U32 cm_mem_check_size(U32 size);

//此时的size是加过pad之后的size
void cm_mem_check_append(CM_POINTER *p, U32 size, const S8 *fname, U32 line);
void cm_mem_check_delete(CM_POINTER *p);


#if 0 //demo
void* baidu_malloc(U32 size, const S8 *file, S32 line)
{
    void *p = 0;

    if (size > 0)
    {
    #if defined(MEM_TEST)
        size = cm_mem_check_size(size);
    #endif
    
        p = malloc(size);
        BD_ASSERT(p != 0);

    #if defined(MEM_TEST)
        cm_mem_check_append(&p, size, file, line);
    #endif
    }
    
    return p;
}

void baidu_free(void *p)
{
    if (p != 0)
    {
    #if defined(MEM_TEST)
	    cm_mem_check_delete(&p);
    #endif

	    free(p);
	}
}
#endif //demo

#ifdef __cplusplus
}
#endif


#endif //_CM_MEM_CHECK_H

