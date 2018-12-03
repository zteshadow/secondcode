
#include "nbk_sim_connection.h"
#include "nbk_kernel.h"

#include "cm_io.h"
#include "cm_lib.h"
#include "cm_facility.h"
#include "cm_string.h"
#include "cm_utility.h"
#include "cm_debug.h"


#define BUFFER_SIZE	4096

#define SUFFIX_HTML	".htm"
#define SUFFIX_GZIP	".gz"
#define SUFFIX_GIF	".gif"
#define SUFFIX_JPEG	".jpg"
#define SUFFIX_PNG	".png"


enum _sim_phase
{
	SP_READY,
	SP_READ_DATA,
	SP_OVER
};

enum _sim_data_type
{
	SD_HTML,
	SD_ZIP,
	SD_FF_PKG,
};


static void nbk_sim_connection_on_header(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;
	//const S8 *url = 0;
	NRespHeader hdr = {0};

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;


	hdr.mime = NEMIME_UNKNOWN;
	hdr.content_length = bd_fs_file_size(&sim->file_strm);

	if (sim->url)
		loader_setRequestUrl(thiz->request, sim->url, 1);

	if (SD_FF_PKG != sim->data_type)
	{
		nbk_connection_on_receive_header(thiz, &hdr);
		if (SD_ZIP == sim->data_type)
		{
		    sim->zip_strm = cm_zip_create(CM_ZIP_GZIP);
        }

		return;
	}

	//ff_pkg
	sim->ff_parser = nbk_ffpkg_parser_new(thiz);
}

static void nbk_sim_connection_on_body(nbk_connection_ptr thiz, S8 *body, S32 len)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	if (0 == body || len <= 0)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	if (SD_FF_PKG == sim->data_type)
	{
		nbk_ffpkg_parse(sim->ff_parser, body, len);
		return;
	}

	if (SD_ZIP == sim->data_type)
	{
		S8 *dest = 0;
		S32 dest_len = 0;
		dest_len = cm_zip_decompress(sim->zip_strm, body, len, &dest);

		if (dest)
		{
			nbk_connection_on_receive_data(thiz, dest, dest_len, len);
			CM_FREE(dest);
		}

		return;
	}

	nbk_connection_on_receive_data(thiz, body, len, len);
}

static void nbk_sim_connection_on_complete(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	if(SD_FF_PKG == sim->data_type)
	{
		nbk_connection_on_package_over(thiz);
	}
	else
	{
		nbk_connection_on_complete(thiz);
	}
}

static bd_bool nbk_sim_connection_open_cache_file(nbk_connection_ptr thiz)
{
	bd_bool found = 0;
	const S8 *cache_path = 0;
	S8 file_name[CM_MAX_PATH], name[CM_MAX_PATH];
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return 0;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	cache_path = nbk_kernel_get_cache_path(nbk_web_view_get_kernel(thiz->web_view));

	switch(thiz->request->type)
	{
	case NEREQT_MAINDOC:
		{
			CM_HANDLE fp;
		    cm_strcpy(file_name, cache_path);

            cm_facility_add_sub_dir(file_name, name);
            cm_facility_add_sub_dir(file_name, "uri");

			if (fp = cm_fopen(file_name, "rb"))
			{
				S32 size = 0;
				size = bd_fs_file_size(fp);

                if (0 == sim->url)
                {
					sim->url = bd_string_new(size);
                }
                else if (cm_strlen(sim->url) < size)
				{
					bd_string_free(&sim->url);
					sim->url = bd_string_new(size);
				}

				cm_fread((S8*)sim->url, size, 1, fp);
				sim->url[size] = 0;

				cm_fclose(fp);
			}

            cm_strcpy(file_name, cache_path);
    		cm_sprintf(name, "doc%s", SUFFIX_GZIP);
            cm_facility_add_sub_dir(file_name, name);
            
			if (sim->file_strm = cm_fopen(file_name, "rb"))
			{//open "xxx.gz"
				found = 1;
				sim->data_type = SD_ZIP;
				break;
			}

            cm_strcpy(file_name, cache_path);
    		cm_sprintf(name, "doc%s", SUFFIX_HTML);
            cm_facility_add_sub_dir(file_name, name);

			if (sim->file_strm = cm_fopen(file_name, "rb"))
			{//open "xxx.htm"
				found = 1;
				sim->data_type = SD_HTML;
				break;
			}

            cm_strcpy(file_name, cache_path);
            cm_facility_add_sub_dir(file_name, "ff.pkg");

			if (sim->file_strm = cm_fopen(file_name, "rb"))
			{//open "xxx.pkg"
				found = 1;
				sim->data_type = SD_FF_PKG;
				break;
			}
		}
		break;
	case NEREQT_IMAGE:
		{
			S8 md5[33] = {0};

			cm_create_md5_string(thiz->request->url, cm_strlen(thiz->request->url), md5);
            cm_strcpy(file_name, cache_path);
            cm_facility_add_sub_dir(file_name, md5);

			if (sim->file_strm = cm_fopen(file_name, "rb"))
			{
				sim->data_type = SD_HTML;
				CM_TRACE_5("open image cache[%s] ok", file_name);
				found = 1;
			}
			else
			{
				CM_TRACE_1("open image cache[%s] failed", file_name);
			}
		}
		break;
	case NEREQT_IMAGE_PACK:
		{
		//TODO
		CM_ASSERT(0);
		#if 0
			S8 path[CM_MAX_PATH] = {0};

//			bd_sprintf(path, FMT_CACHE_RES_PKG, cache_path, thiz->request->pageId);

			if (BD_ERROR_SUCCESS == bd_file_stream_open(&sim->file_strm, path, mode_open, access_read))
			{
				CM_TRACE_5("open image pack cache[%s] ok", path);
				found = 1;
				sim->data_type = SD_FF_PKG;
			}
			else
			{
				CM_TRACE_1("open image pack cache[%s] failed", path);
			}
        #endif
		}
		break;
	default:
		break;
	}

	return found;
}

static bd_bool nbk_sim_connection_read_cache_file(nbk_connection_ptr thiz)
{
	S32 len = 0;
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return 0;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	len = cm_fread(sim->buff, BUFFER_SIZE, 1, sim->file_strm);

	CM_TRACE_5("read len: %d", len);

	if (len < 0)
		return 0;

	nbk_sim_connection_on_body(thiz, sim->buff, len);

	if (len < BUFFER_SIZE)
	{
		cm_fclose(sim->file_strm);
		nbk_sim_connection_on_complete(thiz);
		return 0;
	}

	return 1;
}

static void nbk_sim_connection_fscache_read_header(nbk_connection_ptr thiz)
{
	S32 type = 0;
	S32 size = 0;
	nbk_kernel_ptr k = nbk_web_view_get_kernel(thiz->web_view);
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	nbk_fscache_get_cache_header(k->fs_cache, sim->fscache_index, &type, &size);

	nbk_sim_connection_on_header(thiz);
}

static bd_bool nbk_sim_connection_fscache_read_data(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;
	nbk_kernel_ptr k = 0;
	S32 len = 0;

	if (0 == thiz || 0 == thiz->sub)
		return 0;

	sim = (nbk_sim_connection_ptr)thiz->sub;
	k = nbk_web_view_get_kernel(thiz->web_view);
	len = nbk_fscache_get_cache_data(k->fs_cache, sim->fscache_index, sim->buff, BUFFER_SIZE, 0);

	if (len < 0)
		return 0;

	nbk_sim_connection_on_body(thiz, sim->buff, len);

	if (len < BUFFER_SIZE)
	{
		nbk_fscache_get_cache_data_over(k->fs_cache);
		nbk_sim_connection_on_complete(thiz);
		return 0;
	}

	return 1;
}

static bd_bool nbk_sim_connection_open(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return 0;

	sim = (nbk_sim_connection_ptr)thiz->sub;


	sim->phase = SP_READY;
	cm_timer_start(sim->timer, 200);

	return 1;
}

static void nbk_sim_connection_close(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	cm_zip_destory(sim->zip_strm);
	cm_timer_stop(sim->timer);
}

static void nbk_sim_connection_free(nbk_connection_ptr thiz)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	cm_timer_destory(sim->timer);

	if (sim->ff_parser)
	{
		nbk_ffpkg_parser_free(sim->ff_parser);
	}

	CM_FREE(sim);
}

static void nbk_sim_connection_run(void *user)
{
	nbk_connection_ptr thiz = (nbk_connection_ptr)user;
	nbk_sim_connection_ptr sim = 0;

	if (0 == user || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;

	thiz->state = 1;//running

	switch(sim->phase)
	{
	case SP_READY:
		{
			bd_bool success = 0;
			switch(sim->type)
			{
			case SIM_HISTORY_CACHE:
				{
					success = nbk_sim_connection_open_cache_file(thiz);
				}
				break;
			case SIM_FILE_CACHE:
				{
					nbk_sim_connection_fscache_read_header(thiz);
					success = 1;
				}
				break;
			case SIM_NO_CACHE_ERROR:
				{
					nbk_connection_on_error(thiz, NELERR_NOCACHE);
					success = 0;
				}
				break;
			default:
				success = 0;
				break;
			}

			if (success)
			{
				nbk_sim_connection_on_header(thiz);
				sim->phase = SP_READ_DATA;
			}
			else
			{
				sim->phase = SP_OVER;
			}
		}
		break;
	case SP_READ_DATA:
		{
			bd_bool success = 0;
			switch(sim->type)
			{
			case SIM_HISTORY_CACHE:
				{
					success = nbk_sim_connection_read_cache_file(thiz);
				}
				break;
			case SIM_FILE_CACHE:
				{
					success = nbk_sim_connection_fscache_read_data(thiz);
				}
				break;
			default:
				break;
			}

			if (success == 0)
			{
				sim->phase = SP_OVER;
			}
		}
		break;
	}

	if (SP_OVER == sim->phase)
		cm_timer_stop(sim->timer);

	thiz->state = 0;//running over
}

nbk_connection_ptr nbk_sim_connection_new(nbk_web_view_ptr web_view, NRequest *req, S32 type)
{
	nbk_connection_ptr thiz = 0;
	nbk_sim_connection_ptr sub = 0;

	thiz = nbk_connection_new(web_view, req);

	if (0 == thiz)
		return 0;

	sub = (nbk_sim_connection_ptr)CM_MALLOC(sizeof(*sub));
	if (0 == sub)
	{
		CM_FREE(thiz);
		return 0;
	}
    cm_memset(sub, 0, sizeof(*sub));
    
	sub->url = 0;
	sub->timer = cm_timer_create(nbk_sim_connection_run, thiz);
	sub->ff_parser = 0;
	sub->type = type;


	thiz->sub = sub;
	thiz->open = nbk_sim_connection_open;
	thiz->close = nbk_sim_connection_close;
	thiz->free = nbk_sim_connection_free;

	return thiz;
}

void nbk_sim_connection_set_fscache_index(nbk_connection_ptr thiz, S32 index)
{
	nbk_sim_connection_ptr sim = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	sim = (nbk_sim_connection_ptr)thiz->sub;
	sim->fscache_index = index;
}

