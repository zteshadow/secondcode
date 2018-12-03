
#ifndef _CM_PARSER_H
#define _CM_PARSER_H


#include "cm_data_type.h"


typedef enum
{
    CM_GET_TEXT,
    CM_GET_OPEN_TAG,
    CM_GET_CLOSE_TAG,
    CM_GET_P_NAME,
    CM_GET_P_VALUE,
    CM_PARSE_DONE,
    
    CM_GET_MAX
    
} CMOutputType;

typedef void (*PARSER_FUNC)(CMOutputType cmd, S8 *data, void *usr_data);


#ifdef __cplusplus
extern "C"
{
#endif


S32 cm_data_parser(S8 *data, PARSER_FUNC cb, void *usr_data);


#ifdef __cplusplus
}
#endif


#endif	//_CM_PARSER_H

