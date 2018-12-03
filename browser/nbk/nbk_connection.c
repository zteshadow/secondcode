
#include "nbk_connection.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


nbk_connection_ptr nbk_connection_new(nbk_web_view_ptr web_view, NRequest *req)
{
	nbk_connection_ptr thiz = 0;

	thiz = (nbk_connection_ptr)CM_MALLOC(sizeof(*thiz));
	if (0 != thiz)
	{
	    cm_memset(thiz, 0, sizeof(nbk_connection_ptr));
		thiz->request = req;
		thiz->web_view = web_view;
		thiz->closed = 0;
	}

	return thiz;
}

void nbk_connection_close(nbk_connection_ptr thiz)
{
	if (0 == thiz)
		return;

	thiz->closed = 1;

	if (thiz->close)
		thiz->close(thiz);
}

void nbk_connection_free(nbk_connection_ptr thiz)
{
	if (0 == thiz)
		return;

	if (thiz->free)
		thiz->free(thiz);

	if (0 != thiz->request)
	{
		loader_deleteRequest(&thiz->request);
	}

	CM_FREE(thiz);
}


void nbk_connection_on_receive_header(nbk_connection_ptr thiz, NRespHeader *header)
{
	if (0 == thiz)
		return;

	loader_onReceiveHeader(thiz->request, header);
}

void nbk_connection_on_receive_data(nbk_connection_ptr thiz, S8 *data, S32 length, S32 comprLen)
{
	if (0 == thiz)
		return;
	if( 0 == length )
		return;
	loader_onReceiveData(thiz->request, data, length, comprLen);
}

void nbk_connection_on_package(nbk_connection_ptr thiz, NEResponseType type, void* data, S32 v1, S32 v2)
{
	if (0 == thiz)
		return;

	loader_onPackage(thiz->request, type, data, v1, v2);
}

void nbk_connection_on_package_over(nbk_connection_ptr thiz)
{
	if (0 == thiz)
		return;

	page_onFFPackageOver(thiz->request);
	imagePlayer_onFFPackageOver(thiz->request);
}

void nbk_connection_on_complete(nbk_connection_ptr thiz)
{
	if (0 == thiz)
		return;

	loader_onComplete(thiz->request);
}

void nbk_connection_on_cancel(nbk_connection_ptr thiz)
{
	if (0 == thiz)
		return;

	loader_onCancel(thiz->request);
}

void nbk_connection_on_error(nbk_connection_ptr thiz, S32 err)
{
	if (0 == thiz)
		return;

	loader_onError(thiz->request, err);
}

