
#ifndef _NBK_HTTP_CONNECTION_H
#define _NBK_HTTP_CONNECTION_H


#include "cm_http.h"
#include "cm_zip_stream.h"
#include "nbk_connection.h"
#include "nbk_ffpkg_parser.h"


typedef struct _nbk_http_connection
{
	void *http_client;

	S8 *url_response;

	void *zstrm;
	CM_ZIP_TYPE zip_type;

	nbk_ffpkg_parser *ff_parser;

	S8 cache_file_name[CM_MAX_PATH];

}nbk_http_connection, *nbk_http_connection_ptr;


#ifdef __cplusplus
extern "C"
{
#endif

nbk_connection_ptr nbk_http_connection_new(nbk_web_view_ptr web_view, NRequest *req);


#ifdef __cplusplus
}
#endif


#endif	//_NBK_HTTP_CONNECTION_H

