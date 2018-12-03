
/*
 * cm_mem_check.h
 *
 *  Created on: 30 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_UTILITY_H
#define _CM_UTILITY_H


#include "cm_data_type.h"


#define CM_MAX_MD5_STRING   (33)


#ifdef __cplusplus
extern "C"
{
#endif


//refer cm_strrchr
void* cm_memrchr(const void* s, S32 c, CM_SIZE n);

S8* cm_strtok_r(S8 *s, const S8 *delim, S8 **last);

//·µ»Ømd5×Ö·û´®,md5ÖÁÉÙCM_MAX_MD5_STRING
S32 cm_create_md5_string(const void *data, CM_SIZE len, S8 *md5);

CM_SIZE cm_create_hex_string(const void *buff, CM_SIZE len, S8 *output);

//DO NOT forget to free data
S8 *cm_read_file(const S8 *fname, CM_SIZE *len);

S8 *cm_read_file_string(const S8 *fname);


#ifdef __cplusplus
}
#endif


#endif //_CM_UTILITY_H

