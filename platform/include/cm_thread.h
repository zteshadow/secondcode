
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
* Ver: 1.0              Author: Samuel.Song             Date: 2009-11-07
*
* Description:
* ------------
* This module contains some socket API wrapper for thread process
*
*****************************************************************************/


#ifndef _CM_THREAD_H
#define _CM_THREAD_H


#include "cm_data_type.h"


typedef struct
{
	void    *param;
	S32     priority;
	S32     auto_delete;
	CM_SIZE stack_size;
	
} cm_thread_info;

#ifdef WIN32
typedef void (*cm_thread_proc)(void *);
#else
typedef void* (*cm_thread_proc)(void *);
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Return:
* ------------
* ERR: < 0(if network can not be well initialized or other error)
* OK : 0
*
* Description:
* ------------
* This is for embeded system such as MTK feature phone, network must be
* initialized before use and well released after use.
*
*****************************************************************************/
CM_HANDLE cm_create_thread(cm_thread_proc func, cm_thread_info *info);

/*****************************************************************************
* Description:
* ------------
* This is for embeded system such as MTK feature phone, network must be
* initialized before use and well released after use.
*
*****************************************************************************/
void cm_destory_thread(CM_HANDLE thread);

#ifdef __cplusplus
}
#endif


#endif //_CM_THREAD_H

