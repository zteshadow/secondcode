/*
 * cm_base64.h
 *
 *  Created on: 20 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_BASE64_H
#define _CM_BASE64_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


S8 *cm_base64_encode(const S8 *data, S32 size);
S8 *cm_base64_decode(const S8 *data, S32 size, S32 *result_len);


#ifdef __cplusplus
}
#endif


#endif //_CM_BASE64_H

