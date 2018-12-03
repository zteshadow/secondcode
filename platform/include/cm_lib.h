
/*****************************************************************************
* Copyright Statement:
* --------------------
* This software is protected by Copyright and the information contained
* herein is confidential. The software may not be copied and the information
* contained herein may not be used or disclosed except with the written
* permission of XXX Inc. (C) 200X-20XX
*
* Version:
* --------
* Ver: 1.0              Author: Samuel.Song             Date: 2008-10-06
*
* Description:
* ------------
* This module contains some common API wrapper such as malloc, free, etc.
*
*****************************************************************************/


#ifndef _CM_LIB_H
#define _CM_LIB_H


#include "cm_data_type.h"


#define CM_MEMORY_CHECK_SUPPORT (0)

#if (CM_MEMORY_CHECK_SUPPORT)
#define CM_MALLOC(size)     cm_malloc_ext((__FILE__), (__LINE__), (size))
#define CM_REALLOC(p, size) cm_realloc_ext((__FILE__), (__LINE__), (p), (size))
#define CM_FREE(p)          cm_free_ext((p))
#else
#define CM_MALLOC(size)     cm_malloc((size))
#define CM_REALLOC(p, size) cm_realloc((p), (size))
#define CM_FREE(p)          cm_free((p))
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Input:
* ------
*  1: size: size of heap memory wanted(SIZE_T is for compatible of 32/64)
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : address of the memory 
*
* Description:
* ------------
* be carefull of the memory leak
*
*****************************************************************************/
void *cm_malloc(CM_SIZE size);

/*****************************************************************************
* Input:
* ------
*  1: p: old pointer
*  2: size: size of heap memory wanted(SIZE_T is for compatible of 32/64)
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : address of the memory 
*
* Description:
* ------------
* malloc new buffer of 'new_size', copy data from 'p' to new buffer, return
*
*****************************************************************************/
void *cm_realloc(void *p, CM_SIZE new_size);

/*****************************************************************************
* Input:
* ------
*  1: p: pointer
*
* Description:
* ------------
* be carefull of the memory leak
*
*****************************************************************************/
void cm_free(void *p);

#define cm_isblank(x)  ((x) == ' ' || (x) == '\t')

#define cm_isdigit(x)  ((x) >= '0' && (x) <= '9')
#define cm_isalpha(x)   (((x) >= 'A' && (x) <= 'Z') || ((x) >= 'a' && (x) <= 'z'))
#define cm_isalnum(x)   (cm_isdigit((x)) || cm_isalpha((x)))


/*****************************************************************************
* Input:
* ------
*  1: data: pointer of the data string
*
* Return:
* ------------
*  int value of the string
*
* Description:
* ------------
*  cm_atoi will scape the heading space and translate from the first '+' or
*  '-' or digital untill non-digital or '\0'
*
*****************************************************************************/
S32 cm_atoi(const S8 *data);

/*****************************************************************************
* Input:
* ------
*  1: data: pointer of the data string
*  2: end_ptr: 如果不为0，则指向在data中遇到的不合格字符
*  3: base: 编码0和10代表10进制，16代表16进制
*
* Return:
* ------------
*  int value of the string
*
* Description:
* ------------
*  cm_atoi will scape the heading space and translate from the first '+' or
*  '-' or digital untill non-digital or '\0'
*
*****************************************************************************/
ULong cm_strtoul(const S8 *data, S8 **end_ptr, S32 base);

/*****************************************************************************
* Input:
* ------
*  1: seed: seed for rand, usually time(0) or getpid
*
* Description:
* ------------
*  must set seed before 'rand'
*
*****************************************************************************/
void cm_srand(U32 seed);

/*****************************************************************************
* Return:
* ------------
*  int value from 0 to RAND_MAX, because RAND_MAX is macro, we design a func
*  'cm_rand_max() to get it
*
* Description:
* ------------
*  need use 'cm_srand' to setup seed first, if not the seed will be 1.
*
*****************************************************************************/
S32 cm_rand(void);

/*****************************************************************************
* Return:
* ------------
*  return RAND_MAX(used to be 0x7FFF).
*
* Description:
* ------------
*  just designed for platform independent
*
*****************************************************************************/
S32 cm_rand_max(void);

#if (CM_MEMORY_CHECK_SUPPORT)
/*****************************************************************************
* Description:
* ------------
* memory check version of 'malloc' and 'free', you can check memory leak by
* 'cm_memory_check'
*
*****************************************************************************/
void *cm_malloc_ext(const S8 *file, S32 line, CM_SIZE size);
void *cm_realloc_ext(const S8 *file, S32 line, void *p, CM_SIZE new_size);
void cm_free_ext(void *p);
#endif


#ifdef __cplusplus
}
#endif


#endif //_CM_LIB_H

