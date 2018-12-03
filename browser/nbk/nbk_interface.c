
#include "nbk_core.h"
#include "nbk_kernel.h"
#include "nbk_web_view.h"
#include "nbk_connection.h"
#include "nbk_file_connection.h"
#include "nbk_http_connection.h"
#include "nbk_sim_connection.h"

#include "cm_string.h"
#include "cm_facility.h"
#include "cm_md5.h"
#include "cm_debug.h"


static int8 s_stock_page_name[CM_MAX_PATH];


bd_bool NBK_resourceLoad(void *platform, NBK_Request *req)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	
	if (0 == req)
		return N_FALSE;

	CM_TRACE_5("req->via = %d", req->via);
	//CM_TRACE_5("req->cache = %d", req->cache);
	CM_TRACE_5("req->type = %d", req->type);
	CM_TRACE_5("req->method = %d", req->method);
	CM_TRACE_5("req->url = %s", req->url);

	{
		//these code is for debuging, reject the image request
		//if (req->via == NEREV_FF_MULTIPICS)
		//	return N_FALSE;

		//if (req->type == NEREQT_IMAGE)
		//	return N_FALSE;

		if (req->via == NEREV_FF_FLASH)
			return N_FALSE;
	}

	if (req->via == NEREV_FF_FULL ||
		req->via == NEREV_FF_MULTIPICS ||
		req->via == NEREV_UCK ||
		req->via == NEREV_FF_NARROW ||
		req->method == NEREM_HISTORY
		/*|| req->cache*/)
	{
		nbk_connection_ptr http_conn = 0;

		if (NEREM_HISTORY == req->method)
		{
			nbk_connection_ptr sim_conn = nbk_sim_connection_new(web_view, req, SIM_HISTORY_CACHE);
			if (0 == sim_conn)
				return N_FALSE;

			if (sim_conn->open(sim_conn) == 0)
			{
				//when false, nbk core will free this request, 
				//so we set request to 0 avoid nbk_connection_free() to free this request again.
				sim_conn->request = 0;
				nbk_connection_free(sim_conn);
				return N_FALSE;
			}

			//add to connection list
			nbk_web_view_add_connection(web_view, sim_conn);

			return N_TRUE;
		}

		if (NEREQT_IMAGE == req->type)
		{
			S32 index = 0;
			nbk_kernel_ptr k = nbk_web_view_get_kernel(web_view);

			index = nbk_fscache_get_cache_index(k->fs_cache, req->url);
			if (index >= 0)
			{
				nbk_connection_ptr sim_conn = nbk_sim_connection_new(web_view, req, SIM_FILE_CACHE);
				if (0 == sim_conn)
					return N_FALSE;

				nbk_sim_connection_set_fscache_index(sim_conn, index);

				if (sim_conn->open(sim_conn) == 0)
				{
					//when false, nbk core will free this request, 
					//so we set request to 0 avoid nbk_connection_free() to free this request again.
					sim_conn->request = 0;
					nbk_connection_free(sim_conn);
					return N_FALSE;
				}

				//add to connection list
				nbk_web_view_add_connection(web_view, sim_conn);

				return N_TRUE;
			}

			if (NEREM_JUST_CACHE == req->method)
			{
				loader_onError(req, NELERR_NOCACHE);
				return N_FALSE;
			}
		}

		//http connection
		http_conn = nbk_http_connection_new(web_view, req);
		if (0 == http_conn)
			return N_FALSE;

		if (http_conn->open(http_conn) == 0)
		{
			http_conn->request = 0;
			nbk_connection_free(http_conn);
			return N_FALSE;
		}

		//add to connection list
		nbk_web_view_add_connection(web_view, http_conn);
		
		return N_TRUE;
	}

	if (cm_strncasecmp(req->url, "http://", 7) == 0)
	{//direct
		nbk_connection_ptr http_conn = nbk_http_connection_new(web_view, req);

		if (0 == http_conn)
			return N_FALSE;

		if (http_conn->open(http_conn) == 0)
		{
			http_conn->request = 0;
			nbk_connection_free(http_conn);
			return N_FALSE;
		}

		//add to connection list
		nbk_web_view_add_connection(web_view, http_conn);
		
		return N_TRUE;
	}

	if (cm_strncasecmp(req->url, "file://", 7) == 0)
	{
		nbk_connection_ptr file_conn = nbk_file_connection_new(web_view, req);
		if (0 == file_conn)
			return N_FALSE;

		if (file_conn->open(file_conn) == 0)
		{
			file_conn->request = 0;
			nbk_connection_free(file_conn);
			return N_FALSE;
		}

		//add to connection list
		nbk_web_view_add_connection(web_view, file_conn);

		return N_TRUE;
	}
	
	return N_FALSE;
}

void NBK_resourceStopAll(void* pfd, nid pageId)
{
	nbk_web_view *web_view = (nbk_web_view*)pfd;

	CM_TRACE_5("stop all connection of page_id[%d] now", pageId);

	nbk_web_view_sever_connection_by_page_id(web_view, pageId);

	CM_TRACE_5("stop all connection of page_id[%d] successfully", pageId);
}

//history.h
void NBK_resourceClean(void* pfd, nid id)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)pfd;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
    nbk_kernel_clear_cache(kernel, id);
}

S8* NBK_getStockPage(S32 type)
{
    switch (type) 
	{
    case NESPAGE_ERROR:
        cm_facility_get_deposit_fname(CM_DEPOSIT_ERROR, s_stock_page_name);
    break;
    
    case NESPAGE_ERROR_404: //fall through
	default:
        cm_facility_get_deposit_fname(CM_DEPOSIT_404, s_stock_page_name);
    break;
    }

    return s_stock_page_name;
}

int NBK_freemem(void)
{
    return cm_facility_get_max_mem();
}

int NBK_conv_gbk2unicode(const char* mbs, int mbLen, wchr* wcs, int wcsLen)
{
    CM_ASSERT(0);
    return 0;
}

void NBK_md5(const char* s, int len, uint8* md5)
{
    cm_create_md5(s, len, md5);
}

int NBK_currentMilliSeconds(void)
{
    return cm_get_current_ms();
}
