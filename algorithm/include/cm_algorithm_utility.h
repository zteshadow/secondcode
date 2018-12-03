/*
 * cm_utility.h
 *
 *  Created on: 16 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_ALGORITHM_UTILITY_H
#define _CM_ALGORITHM_UTILITY_H


#include "cm_data_type.h"


typedef enum
{
    CM_DATA_POSITIVE,   //+
    CM_DATA_NEGATIVE,   //-
    CM_DATA_MIXED,
    CM_DATA_MAX

} CM_DATA_TYPE;


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Input:
* ------
*  1 'size' : total number to be generated, the memory will be created from
              heap by malloc.
*  2 'max'  : max value of data
*  3 'type' : all positive, all negative, or mixed
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : handle
*
* Description:
* ------------
* USE cm_algorithm_data_destory to release memory
*
*****************************************************************************/
S32 *cm_data_create(S32 size, S32 max, CM_DATA_TYPE type);

S32 *cm_data_copy(const S32 *src, S32 cnt);
void cm_data_dump(const S32 *src, S32 cnt, const S8 *fname);
void cm_data_destory(S32 *h);

S32 cm_data_print(const S32 *data, S32 size);


#ifdef __cplusplus
}
#endif


#endif //_CM_ALGORITHM_UTILITY_H

