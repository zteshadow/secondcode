/*
 * cm_sum.h
 *
 *  Created on: 16 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_SUM_H
#define _CM_SUM_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif

//����������е�һ����ٷ���(N3)
S32 cm_max_sequence_sum_N3(const S32 *data, S32 size);

//����������е�һ���۰뷨(NLogN)
S32 cm_max_sequence_sum_NLogN(const S32 *data, S32 left, S32 right);

//����������е�һ�ַ���(N)
S32 cm_max_sequence_sum_N(const S32 *data, S32 size);

S32 cm_min_sequence_sum_N(const S32 *data, S32 size);

#ifdef __cplusplus
}
#endif


#endif //_CM_SUM_H

