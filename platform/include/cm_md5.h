
#ifndef _CM_MD5_H
#define _CM_MD5_H


#include "cm_data_type.h"


#define CM_MAX_MD5      (16)


#ifdef __cplusplus
extern "C"
{
#endif


//返回长度，成功16字节
//md5返回16个字节的结果
S32 cm_create_md5(const void *data, CM_SIZE len, S8 *md5);


#ifdef __cplusplus
}
#endif


#endif //_CM_MD5_H

