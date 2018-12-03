
#include "nbk_file_connection.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_debug.h"


enum _file_conn_stage
{
	FCS_READY,
	FCS_READ_DATA,
	FCS_OVER
};


static void nbk_file_connection_get_path(nbk_connection_ptr thiz, S8 *path);
static CM_BOOL nbk_file_connection_open(nbk_connection_ptr thiz);
static void nbk_file_connection_close(nbk_connection_ptr thiz);
static void nbk_file_connection_free(nbk_connection_ptr thiz);


static void nbk_file_connection_get_path(nbk_connection_ptr thiz, S8 *path)
{
	nbk_file_connection_ptr file_conn = NULL;
	char *p = NULL;

	if (NULL == thiz)
	{
		return;
    }

	file_conn = (nbk_file_connection_ptr)thiz->sub;
	p = thiz->request->url + 7; //"file://"

	cm_strncpy(path, p, CM_MAX_PATH);
}

static CM_BOOL nbk_file_connection_open(nbk_connection_ptr thiz)
{
	nbk_file_connection_ptr file_conn = NULL;

	if (NULL == thiz)
		return 0;

	file_conn = (nbk_file_connection_ptr)thiz->sub;

	if (NULL == file_conn)
		return 0;

	if (NULL == file_conn->timer)
		return 0;

	cm_timer_start(file_conn->timer, 200);

	return 1;
}

static void nbk_file_connection_close(nbk_connection_ptr thiz)
{
	nbk_file_connection_ptr file_conn = NULL;

	if (NULL == thiz)
		return;

	file_conn = (nbk_file_connection_ptr)thiz->sub;

	cm_timer_stop(file_conn->timer);
	cm_fclose(file_conn->fp);
}

static void nbk_file_connection_free(nbk_connection_ptr thiz)
{
	nbk_file_connection_ptr file_conn = NULL;

	if (NULL == thiz)
		return;

	file_conn = (nbk_file_connection_ptr)thiz->sub;

	cm_timer_destory(file_conn->timer);
	file_conn->timer = NULL;

	CM_FREE(file_conn);
}

static void nbk_file_connection_run(void *user)
{
	nbk_connection_ptr thiz = NULL;
	nbk_file_connection_ptr file_conn = NULL;

	thiz = (nbk_connection_ptr)user;
	file_conn = (nbk_file_connection_ptr)thiz->sub;

	thiz->state = 1;

	switch(file_conn->phase)
	{
	case FCS_READY:
	{
	    S8 path[CM_MAX_PATH];

		nbk_file_connection_get_path(thiz, path);

		if ((file_conn->fp = cm_fopen(path, "rb")))
		{
			cm_memset(file_conn->buffer, 0, sizeof(file_conn->buffer));
			file_conn->phase = FCS_READ_DATA;
			break;
		}

		file_conn->phase = FCS_OVER;
	}
    break;

	case FCS_READ_DATA:
	{
		S32 len = 0;

		if ((len = cm_fread(file_conn->buffer, BUFFER_SIZE, 1, file_conn->fp)) > 0)
		{
			nbk_connection_on_receive_data(thiz, file_conn->buffer, len, len);
			break;
		}

		nbk_connection_on_complete(thiz);
		file_conn->phase = FCS_OVER;
	}
    break;

	case FCS_OVER:
    {
    	cm_fclose(file_conn->fp);
    	cm_timer_stop(file_conn->timer);
    }
    break;

    default:
        CM_ASSERT(0);
    break;
	}

	thiz->state = 0;
}

nbk_connection_ptr nbk_file_connection_new(nbk_web_view_ptr web_view, NRequest *req)
{
	nbk_connection_ptr thiz = NULL;
	nbk_file_connection_ptr sub = NULL;

	thiz = nbk_connection_new(web_view, req);

	if (NULL != thiz)
	{
		sub = (nbk_file_connection_ptr)CM_MALLOC(sizeof(nbk_file_connection));
		if (NULL != sub)
		{
		    cm_memset(sub, 0, sizeof(nbk_file_connection));
			sub->phase = FCS_READY;
			sub->timer = cm_timer_create(nbk_file_connection_run, thiz);

            thiz->state = 0;
			thiz->sub = sub;
			thiz->open = nbk_file_connection_open;
			thiz->close = nbk_file_connection_close;
			thiz->free = nbk_file_connection_free;
		}
		else
		{
			CM_FREE(thiz);
		}
	}

	return thiz;
}

