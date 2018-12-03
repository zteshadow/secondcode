/*
 * cm_sort.h
 *
 *  Created on: 31 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_SORT_H
#define _CM_SORT_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


S32 cm_bucket_sort(const S32 *data, S32 size, S32 power);
S32 cm_insert_sort(S32 *data, S32 size);
S32 cm_shell_sort(S32 *data, S32 size);
S32 cm_heap_sort(S32 *data, S32 size);
S32 cm_merge_sort(S32 *data, S32 size);
S32 cm_quick_sort(S32 *data, S32 size);


#ifdef __cplusplus
}
#endif


#endif //_CM_SORT_H

