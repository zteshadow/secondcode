/*
 * cm_sort.h
 *
 *  Created on: 12 Jan 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_ALGORITHM_MISC_H
#define _CM_ALGORITHM_MISC_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif

//{}, [], ()
S32 cm_algorithm_check_symbol_pair(const S8 *fname);
S32 cm_algorithm_gcd(S32 a, S32 b);

S32 cm_algorithm_postfix_value(const S8 *data);
S32 cm_algorithm_infix_to_postfix(const S8 *infix, S8 *postfix);


#ifdef __cplusplus
}
#endif


#endif //_CM_ALGORITHM_MISC_H

