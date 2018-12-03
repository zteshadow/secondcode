/*
 * cm_union_find.h
 *
 *  Created on: 17 April 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_UNION_FIND_H
#define _CM_UNION_FIND_H


#include "cm_data_type.h"


typedef S32 *CM_UF_SET;


#ifdef __cplusplus
extern "C"
{
#endif


void cm_union_find_init(CM_UF_SET set, S32 cnt);
void cm_union(CM_UF_SET set, S32 x, S32 y);
S32 cm_find(CM_UF_SET set, S32 x);
void cm_dump_union_find(CM_UF_SET set, S32 cnt);


#ifdef __cplusplus
}
#endif


#endif //_CM_UNION_FIND_H

