
#ifndef _CM_ZIP_STREAM_H
#define _CM_ZIP_STREAM_H


#include "cm_data_type.h"


typedef enum
{
    CM_ZIP_NONE,
    CM_ZIP_GZIP,
    CM_ZIP_DEFLATE,
    CM_ZIP_MAX

} CM_ZIP_TYPE;


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_zip_create(CM_ZIP_TYPE type);

//return compressed 'length'
//output: compressed data
S32 cm_zip_decompress(void *zip, S8 *src, S32 len, S8 **output);
void cm_zip_destory(void *zip);


#ifdef __cplusplus
}
#endif


#endif //_CM_ZIP_STREAM_H

