
#ifndef _CM_HTTP_H
#define _CM_HTTP_H


#include "cm_data_type.h"


typedef enum
{
    CM_HTTP_GET,
    CM_HTTP_POST,
    CM_HTTP_MAX

} CM_HTTP_REQ_TYPE;

typedef enum
{
    CM_HTTP_ON_HEADER,
    CM_HTTP_ON_BODY,
    CM_HTTP_ON_END,
    CM_HTTP_ON_ERROR,
    CM_HTTP_ON_MAX

} CM_HTTP_STATUS;

typedef S32 (*CM_HTTP_FUNC)(CM_HTTP_STATUS status, void *data, S32 len, void *usr);


#ifdef __cplusplus
extern "C"
{
#endif


void *cm_http_create(CM_HTTP_FUNC f, void *usr);
S32 cm_http_set_header(void *http, const S8 *name, const S8 *value);

S32 cm_http_set_data(void *http, void *data, S32 size);
S32 cm_http_send_request(void *http, CM_HTTP_REQ_TYPE type, const S8 *url);

S32 cm_http_get_code(void *http);
S8 *cm_http_get_response_url(void *http);
S8 *cm_http_get_header(void *http, const S8 *name);
S32 cm_http_get_header_cnt(void *http, const S8 *name);
//index: 0 to cnt - 1
S8 *cm_http_get_header_item(void *http, const S8 *name, S32 index);


void cm_http_destory(void *http);


#ifdef __cplusplus
}
#endif


#endif //_CM_HTTP_H

