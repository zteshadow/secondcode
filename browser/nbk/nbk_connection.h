
#ifndef _NBK_CONNECTION_H
#define _NBK_CONNECTION_H


#include "nbk_web_view.h"


typedef struct _nbk_connection nbk_connection, *nbk_connection_ptr;

typedef CM_BOOL (*fn_connection_open)(nbk_connection *);
typedef void (*fn_connection_close)(nbk_connection *);
typedef void (*fn_connection_free)(nbk_connection *);

struct _nbk_connection
{
	NRequest *request;
	nbk_web_view_ptr web_view;

	S32 state;
	CM_BOOL closed;

	fn_connection_open open;
	fn_connection_close close;
	fn_connection_free free;

	void *sub;
};


#ifdef __cplusplus
extern "C"
{
#endif

nbk_connection_ptr nbk_connection_new(nbk_web_view_ptr web_view, NRequest *req);
void nbk_connection_close(nbk_connection_ptr thiz);
void nbk_connection_free(nbk_connection_ptr thiz);

void nbk_connection_on_receive_header(nbk_connection_ptr thiz, NRespHeader *header);
void nbk_connection_on_receive_data(nbk_connection_ptr thiz, S8 *data, S32 length, S32 comprLen);
void nbk_connection_on_package(nbk_connection_ptr thiz, NEResponseType type, void* data, S32 v1, S32 v2);
void nbk_connection_on_package_over(nbk_connection_ptr thiz);
void nbk_connection_on_complete(nbk_connection_ptr thiz);
void nbk_connection_on_cancel(nbk_connection_ptr thiz);
void nbk_connection_on_error(nbk_connection_ptr thiz, S32 err);

#ifdef __cplusplus
}
#endif


#endif	//_NBK_CONNECTION_H

