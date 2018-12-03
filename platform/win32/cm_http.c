
#include "cm_http.h"
#include "cm_debug.h"
#include "cm_lib.h"


void *cm_http_create(CM_HTTP_FUNC f, void *usr)
{
    return 0;
}

S32 cm_http_set_header(void *http, const S8 *name, const S8 *value)
{
    return 0;
}

S8 *cm_http_get_header(void *http, const S8 *name)
{
    return 0;
}

S32 cm_http_get_code(void *http)
{
    return 0;
}

S8 *cm_http_get_response_url(void *http)
{
    return 0;
}

S32 cm_http_get_header_cnt(void *http, const S8 *name)
{
    return 0;
}

S8 *cm_http_get_header_item(void *http, const S8 *name, S32 index)
{
    return 0;
}

S32 cm_http_set_data(void *http, void *data, S32 size)
{
    return 0;
}

S32 cm_http_send_request(void *http, CM_HTTP_REQ_TYPE type, const S8 *url)
{
    return 0;
}

void cm_http_destory(void *http)
{
}

