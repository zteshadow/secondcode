
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


#ifndef _BD_DEBUG_H
#define _BD_DEBUG_H

#import <Foundation/Foundation.h>

#define _BD_DEBUG_

#define BD_TRACE_LEVEL (0)

#ifdef _BD_DEBUG_
#define BD_ASSERT(x) bd_assert((x), __FILE__, __LINE__)
#else
#define BD_ASSERT(x)
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#define BD_SHOW_VIEW(x) bd_show_view_hierarchy((x), 0)
#else
#define BD_SHOW_VIEW(x)
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#define BD_SHOW_STACK bd_show_view_hierarchy()
#else
#define BD_SHOW_STACK
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#define BD_START_TIME() double tmpCurrentTime = bd_get_current_time();
#define BD_PRINT_TIME() bd_trace(@"time costed: %f", bd_get_current_time() - tmpCurrentTime);
#else
#define BD_START_TIME()
#define BD_PRINT_TIME()
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#define BD_SHOW_SIZE(size) bd_show_size((size))
#else
#define BD_SHOW_SIZE(size)
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#define BD_SHOW_RECT(rect) bd_show_rect((rect))
#else
#define BD_SHOW_RECT(rect)
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
#if BD_TRACE_LEVEL >= 0
#define BD_TRACE_0 bd_trace
#else
#define BD_TRACE_0 ((void *)0)
#endif

#if BD_TRACE_LEVEL >= 1
#define BD_TRACE_1 bd_trace
#else
#define BD_TRACE_1
#endif

#if BD_TRACE_LEVEL >= 2
#define BD_TRACE_2 bd_trace
#else
#define BD_TRACE_2
#endif

#if BD_TRACE_LEVEL >= 3
#define BD_TRACE_3 bd_trace
#else
#define BD_TRACE_3
#endif

#if BD_TRACE_LEVEL >= 4
#define BD_TRACE_4 bd_trace
#else
#define BD_TRACE_4
#endif

#if BD_TRACE_LEVEL >= 5
#define BD_TRACE_5 bd_trace
#else
#define BD_TRACE_5
#endif
#endif //_BD_DEBUG_

#ifdef _BD_DEBUG_
void bd_assert(int flag, const char *file, unsigned int line);
void bd_trace(NSString *fmt, ...);
void bd_show_view_hierarchy(UIView *view, NSInteger level);
void bd_show_stack(void);
double bd_get_current_time(void);
void bd_show_size(CGSize size);
void bd_show_rect(CGRect rect);
#endif //_BD_DEBUG_

#endif //_BD_DEBUG_H

