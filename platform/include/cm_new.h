
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
* This module is imply the 'new' and 'delete' method if platform does not
* support
*
*****************************************************************************/


#ifndef _CM_NEW_H
#define _CM_NEW_H


#include "cm_data_type.h"


/*****************************************************************************
* Input:
* ------
*  1: size: size of heap memory wanted(SIZE_T is for compatible of 32/64)
*
* Output:
* --------
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : address of the memory 
*
* Description:
* ------------
* constructor will be called after this.
*
*****************************************************************************/
void *operator new(CM_SIZE size);

/*****************************************************************************
* Input:
* ------
*  1: size: size of heap memory wanted(SIZE_T is for compatible of 32/64)
*
* Output:
* --------
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : address of the memory 
*
* Description:
* ------------
* constructor will be called after this.
*
*****************************************************************************/
void *operator new[](CM_SIZE size);

/*****************************************************************************
* Input:
* ------
*  1: p: pointer
*
* Output:
* --------
*
* Return:
* ------------
*
* Description:
* ------------
* de-constructor will be called before.
*
*****************************************************************************/
void operator delete(void*);

/*****************************************************************************
* Input:
* ------
*  1: p: pointer
*
* Output:
* --------
*
* Return:
* ------------
*
* Description:
* ------------
* de-constructor will be called before.
*
*****************************************************************************/
void operator delete[](void*);


#endif //_CM_NEW_H

