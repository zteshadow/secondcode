
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
* This module contains some common API for debug.
*
*****************************************************************************/


#ifndef _CM_DEBUG_H
#define _CM_DEBUG_H


#include "cm_data_type.h"


#define CM_TRACE_LEVEL (5)

#define CM_ASSERT(x) cm_assert((x), __FILE__, __LINE__)

#if CM_TRACE_LEVEL >= 0
#define CM_TRACE_0 cm_trace
#else
#define CM_TRACE_0
#endif

#if CM_TRACE_LEVEL >= 1
#define CM_TRACE_1 cm_trace
#else
#define CM_TRACE_1
#endif

#if CM_TRACE_LEVEL >= 2
#define CM_TRACE_2 cm_trace
#else
#define CM_TRACE_2
#endif

#if CM_TRACE_LEVEL >= 3
#define CM_TRACE_3 cm_trace
#else
#define CM_TRACE_3
#endif

#if CM_TRACE_LEVEL >= 4
#define CM_TRACE_4 cm_trace
#else
#define CM_TRACE_4
#endif

#if CM_TRACE_LEVEL >= 5
#define CM_TRACE_5 cm_trace
#else
#define CM_TRACE_5
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Input:
* ------
*  1 'flag'     : assert message will be shown if flag is false
*  2 'message'  : user formatted message, just like 'printf'
*
* Description:
* ------------
* useful tools for debug, close the macro of 'CM_ASSERT' when release
*
*****************************************************************************/
void cm_assert(S32 flag, const S8 *file, U32 line);

/*****************************************************************************
* Input:
* ------
*  1 'message'  : user formatted message, just like 'printf'
*
* Description:
* ------------
* useful tools for debug, close the macro of 'CM_TRACE' when release
*
* ***********DO NOT OUTPUT MORE THAN 199 CHARACTERS*********
*
*****************************************************************************/
void cm_trace(const S8 *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif //_CM_DEBUG_H

