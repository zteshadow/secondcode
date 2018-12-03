
#include "nbk_web_view.h"

#include "nbk_core.h"
#include "nbk_connection.h"
#include "nbk_io_stream.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_facility.h"
#include "cm_platform_config.h"
#include "cm_debug.h"


#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define ARROW_STOP	0
#define ARROW_MOVE_STEP	12
#define NBK_USER_PAGE_ID 65000; //	special url
#define IS_4WAY_MODE() page_4ways(nbk_web_view_get_page(thiz))


typedef struct _bd_size
{
	S32 width;
	S32 height;

}bd_size;


static void nbk_web_view_move_left(nbk_web_view_ptr thiz);
static void nbk_web_view_move_right(nbk_web_view_ptr thiz);
static void nbk_web_view_move_up(nbk_web_view_ptr thiz);
static void nbk_web_view_move_down(nbk_web_view_ptr thiz);
static void nbk_web_view_update_screen(nbk_web_view_ptr thiz);

		
void NBK_fep_enable(void* platfom)
{
}

void NBK_fep_disable(void* platform)
{
}

void NBK_fep_updateCursor(void *pfd)
{
}

//custom paint test
bd_bool NBK_paintText(void* platfom, const NRect* rect, NECtrlState state)
{
	return N_FALSE;
}

bd_bool NBK_paintCheckbox(void* pfd, const NRect* rect, NECtrlState state, bd_bool checked)
{
	return N_FALSE;
}

bd_bool NBK_paintRadio(void* pfd, const NRect* rect, NECtrlState state, bd_bool checked)
{
	return N_FALSE;
}

bd_bool NBK_paintButton(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state)
{
	return N_FALSE;
}

bd_bool NBK_paintSelectNormal(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state)
{
	return N_FALSE;
}


bd_bool NBK_paintTextarea(void* pfd, const NRect* rect, NECtrlState state)
{
	return N_FALSE;
}

bd_bool NBK_paintBrowse(void* pfd, const NFontId fontId, const NRect* rect, const wchr* text, int len, NECtrlState state)
{
	return N_FALSE;
}

bd_bool NBK_paintSelectExpand(void* pfd, const wchr* items, int num, int* sel)
{
	return N_FALSE;
}

bd_bool NBK_paintFoldBackground(void* pfd, const NRect* rect, NECtrlState state)
{
	return N_FALSE;
}

// for editor
bd_bool NBK_editInput(void* pfd, NFontId fontId, NRect* rect, const wchr* text, int len)
{
	return N_FALSE;
}

bd_bool NBK_editTextarea(void* pfd, NFontId fontId, NRect* rect, const wchr* text, int len)
{
	return N_FALSE;
}

bd_bool NBK_browseFile(void* pfd, const char* oldFile, char** newFile)
{
	return N_FALSE;
}

bd_bool NBK_dlgAlert(void* pfd, const wchr* text, int len)
{
	return N_FALSE;
}
bd_bool NBK_dlgConfirm(void* pfd, const wchr* text, int len, int* ret)
{
	return N_FALSE;
}
bd_bool NBK_dlgPrompt(void* pfd, const wchr* text, int len, int* ret, char** input)
{
	return N_FALSE;
}

void NBK_helper_getViewableRect(void* pfd, NRect* view)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)pfd;
	NRect *r = &web_view->view_rect;

	view->l = r->l;
	view->t = r->t;
	view->r = r->r;
	view->b = r->b;
}

void NBK_helper_getViewableDocRect(void* pfd, NRect* rect)
{
	rect->l = rect->t = rect->r = rect->b = 0;
}

coord NBK_helper_getScreenCoord(void* pfd, coord x)
{
	return x;
}

coord NBK_helper_getDocumentCoord(void* pfd, coord x)
{
	return x;
}

bd_bool NBK_ext_isDisplayDocument(void* document)
{
	return N_TRUE;
}

bd_bool NBK_handleError(int error)
{
    return N_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

static void nbk_web_view_adjust_view_rect(nbk_web_view_ptr thiz, NRect *rect, bd_bool down)
{
	S16 w, h;
	bd_size size;
	NPage *page = 0;

	page = nbk_web_view_get_page(thiz);

	size.width = nbk_web_view_width(thiz);
	size.height = nbk_web_view_height(thiz);

	w = page_width(page);
	h = page_height(page);

	if (size.height >= h)
		return;

	if (down)
	{
		if (rect->b > thiz->view_rect.b)
		{
			thiz->view_rect.t = rect->t;
			thiz->view_rect.b = thiz->view_rect.t + size.height;
			if (thiz->view_rect.b >= h)
			{
				thiz->view_rect.b = h;
				thiz->view_rect.t = thiz->view_rect.b - size.height;
			}
		}
	}
	else
	{
		if (rect->t < thiz->view_rect.t)
		{
			thiz->view_rect.b = rect->b;
			thiz->view_rect.t = thiz->view_rect.b - size.height;
			if (thiz->view_rect.t <= 0)
			{
				thiz->view_rect.t = 0;
				thiz->view_rect.b = size.height;
			}
		}
	}

}

static void nbk_web_view_update_screen(nbk_web_view_ptr thiz)
{
#if 0
    CM_COLOR color;
    CM_RECT rect;

    cm_memset(&color, 0xFF, sizeof(color));
    cm_gdi_set_brush_color(thiz->graphic, &color);

    rect.x = thiz->x;
    rect.y = thiz->y;
    rect.w = thiz->width;
    rect.h = thiz->height;
    
    cm_gdi_fill_rect(thiz->graphic, &rect);
	nbk_web_view_render(thiz, thiz->graphic);
	cm_gdi_update_screen(thiz->graphic, &rect);
#endif
    CM_RECT rect;

    rect.x = thiz->x;
    rect.y = thiz->y;
    rect.w = thiz->width;
    rect.h = thiz->height;
    
    cm_gdi_update_screen(thiz->graphic, &rect);
}

static void nbk_web_view_on_page_event(S32 id, void* user, void* info)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)user;
	
	switch (id)
	{
	case NBK_EVENT_NEW_DOC:
	{
        CM_TRACE_5("new doc start ");
        //init view rect
        if (web_view->call_back)
        {
            web_view->call_back(web_view->user, id, info);
        }
    }
    break;

	case NBK_EVENT_NEW_DOC_BEGIN:
	{
        CM_TRACE_5("new event new doc begin");
		web_view->view_rect.l = 0;
		web_view->view_rect.t = 0;
		web_view->view_rect.r = web_view->width;
		web_view->view_rect.b = web_view->height;
		if (web_view->call_back)
		{
			web_view->call_back(web_view->user, id, info);
        }
		//nbk_web_view_update_screen(web_view);
    }
    break;

	case NBK_EVENT_UPDATE:
	{
        if (web_view->call_back)
        {
        	web_view->call_back(web_view->user, id, info);	
        }
        nbk_web_view_update_screen(web_view);
    }
    break;

	case NBK_EVENT_UPDATE_PIC:
	{
        if (web_view->call_back)
        	web_view->call_back(web_view->user, id, info);	
        nbk_web_view_update_screen(web_view);
    }
    break;

	case NBK_EVENT_LAYOUT_FINISH:
	{
		if (web_view->call_back)
		{
			web_view->call_back(web_view->user, id, info);
        }
		nbk_web_view_update_screen(web_view);
	}
    break;

	case NBK_EVENT_DEBUG1:
    break;

	case NBK_EVENT_GOT_DATA:
	{
		if (web_view->call_back)
			web_view->call_back(web_view->user, id, info);
	}
	break;

	//add for call kernel paint in order to editInput
	case NBK_EVENT_PAINT_CTRL:
    {
        break;
    }

	case NBK_EVENT_DOWNLOAD_IMAGE:
	{
	}
	break;

	case NBK_EVENT_DOWNLOAD_FILE:
	{
		if (web_view->call_back)
			web_view->call_back(web_view->user, id, info);
	}
    break;
		
	case NBK_EVENT_HISTORY_DOC:
    break;

	case NBK_EVENT_WAITING:
	case NBK_EVENT_GOT_RESPONSE:
	case NBK_EVENT_ENTER_MAINBODY:
	case NBK_EVENT_QUIT_MAINBODY:
	case NBK_EVENT_QUIT_MAINBODY_AFTER_CLICK:
	case NBK_EVENT_LOADING_ERROR_PAGE:
		{
		}
    break;

	case NBK_EVENT_REPOSITION:
	default:
	break;
	}

	web_view->iEventId = id;
}

static void nbk_web_view_get_white_list_file(S8 *file)
{
    S32 value;

    value = cm_facility_get_work_dir(file);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(file, "nbk_w1.dat");
    CM_ASSERT(value >= 0);    
}

static void nbk_web_view_get_white_list_t_file(S8 *file)
{
    S32 value;

    value = cm_facility_get_work_dir(file);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(file, "nbk_wl_t.dat");
    CM_ASSERT(value >= 0);    
}

static bd_bool nbk_web_view_wap_white_list_need_update(nbk_web_view_ptr thiz)
{
	//S8 disk[10] = {0};
	S8 path[CM_MAX_PATH] = {0};

	CM_HANDLE fp;
	//bd_sys_time st = {0};
	CM_DATE_TIME now = {0};
	nbk_wap_white_list *wl = 0;

	nbk_web_view_get_white_list_t_file(path);
	wl = &thiz->kernel->white_list;

	if (fp = cm_fopen(path, "rb"))
	{
		enum t_step
		{
			ts_year,
			ts_month,
			ts_day,
			ts_hour,
			ts_minute,
		};

		S8 line[256] = {0};
		S8 * str = 0;

		enum t_step step = ts_year;

		cm_fgets(line, 256, fp);
		str = cm_strtok(line, ":");

		while (str)
		{
			if (step == ts_year)
			{
				wl->t_update.tm_year = cm_atoi(str);
				step = ts_month;
				str = cm_strtok(0, ":");
				continue;
			}

			if (step == ts_month)
			{
				wl->t_update.tm_mon = cm_atoi(str);
				step = ts_day;
				str = cm_strtok(0, ":");
				continue;
			}

			if (step == ts_day)
			{
				wl->t_update.tm_mday = cm_atoi(str);
				step = ts_hour;
				str = cm_strtok(0, ":");
				continue;
			}

			if (step == ts_hour)
			{
				wl->t_update.tm_hour = cm_atoi(str);
				step = ts_minute;
				str = cm_strtok(0, ":");
				continue;
			}

			if (step == ts_minute)
			{
				wl->t_update.tm_min = cm_atoi(str);
				str = cm_strtok(0, ":");
				break;
			}
		}

		cm_fclose(fp);
	}

	cm_localtime(0, &now);

	if (wl->t_update.tm_year != now.tm_year)
		return 1;

	if (wl->t_update.tm_mon != now.tm_mon)
		return 1;

	if (now.tm_mday < wl->t_update.tm_mday || (wl->t_update.tm_mday - now.tm_mday) > 1)
		return 1;

	return 0;
}

static void nbk_web_view_wap_white_list_release(nbk_wap_white_list *white_list)
{
	white_list->version = 0;
	cm_memset(white_list, 0, sizeof(*white_list));
}

static void nbk_web_view_on_white_list_recieve(NEResponseType type, void* user, void* data, int length, int comprLen)
{
	static bd_output_stream os = {0};
    NRequest* req = (NRequest*)user;
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)req->platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
    
    switch (type) {
    case NEREST_HEADER:
    {
        //NRespHeader* hdr = (NRespHeader*)data;
		bd_output_stream_init(&os);
        break;
    }
    case NEREST_DATA:
    {
		//save data
		bd_os_write(&os, data, length);
        break;
    }
    case NEREST_COMPLETE:
    {
		if (os.pos >= (4+16))
		{
			CM_HANDLE fp;
			S8 wl_file[CM_MAX_PATH] = {0};

			nbk_web_view_get_white_list_file(wl_file);

			//save to file
			if (fp = cm_fopen(wl_file, "a+"))
			{
				cm_fwrite(os.buff, os.pos, 1, fp);
				cm_fclose(fp);
			}

			url_parseWapWhiteList(os.buff, os.pos);

			//update t
			cm_localtime(0, &kernel->white_list.t_update);
			
			//save t
			nbk_web_view_get_white_list_t_file(wl_file);
			if (fp = cm_fopen(wl_file, "a+"))
			{
				const S8 * format = "%d:%d:%d:%d:%d:%d";
				S8 str_t[256] = {0};
				cm_sprintf(str_t, format, kernel->white_list.t_update.tm_year, kernel->white_list.t_update.tm_mon, kernel->white_list.t_update.tm_mday, kernel->white_list.t_update.tm_hour, kernel->white_list.t_update.tm_min, kernel->white_list.t_update.tm_sec);
				cm_fwrite(str_t, cm_strlen(str_t), 1, fp);
				cm_fclose(fp);
			}
		}

		bd_output_stream_destroy(&os);

        break;
    }
    case NEREST_ERROR:
    case NEREST_CANCEL:
    {
		bd_output_stream_destroy(&os);
        break;
    }
    default:
        break;
    }
}

static void nbk_web_view_update_wap_white_list(nbk_web_view_ptr thiz)
{
	const S8 * url_white_list = "http://r2.mo.baidu.com/wap_items/check";
	const S8 * url_format = "%s/%d?uid=%s&ua=%s";
	nbk_kernel_ptr kernel = 0;
	S8 ua[32] = {0};
	S8 uid[32] = {0};
	S8 *url = 0;
	//S32 version = 0;
	NRequest* req = 0;

	if (0 == thiz)
		return;

	kernel = nbk_web_view_get_kernel(thiz);

	if (kernel->white_list.updated)
		return;

	kernel->white_list.updated = 1;

	nbk_web_view_get_baidu_uid(thiz, uid);
	nbk_web_view_get_baidu_ua(thiz, ua);

    url = (S8 *)CM_MALLOC(sizeof(S8) * 256);
    
	cm_sprintf(url, url_format, url_white_list, kernel->white_list.version, uid, ua);

    req = loader_createRequest();
    loader_setRequest(req, NEREQT_WAP_WL, N_NULL, N_NULL);
    loader_setRequestUrl(req, url, N_TRUE);
    req->via = NEREV_SEV_WAPWL;
    req->pageId = NBK_USER_PAGE_ID;
    req->responseCallback = nbk_web_view_on_white_list_recieve;
	req->platform = (void*)thiz;
    
    if (!NBK_resourceLoad(thiz, req)) {
        loader_deleteRequest(&req);
    }
}

static void nbk_web_view_load_wap_white_list(nbk_web_view_ptr thiz)
{
	S8 wl_file[CM_MAX_PATH];
	CM_HANDLE fp;
	nbk_kernel_ptr kernel = 0;

	kernel = nbk_web_view_get_kernel(thiz);

	if (kernel->white_list.version > 0)
		return;

	nbk_web_view_get_white_list_file(wl_file);

	if (nbk_web_view_wap_white_list_need_update(thiz))
	{
		nbk_web_view_update_wap_white_list(thiz);
	}
	else
	{
		if (fp = cm_fopen(wl_file, "rb"))
		{
		    S32 fsize;
		    
		    cm_fseek(fp, 0, CM_SEEK_END);
		    fsize = cm_ftell(fp);

		    cm_fseek(fp, 0, CM_SEEK_SET);
			if (fsize >= (4 + 16))
			{
				S8 *data = (S8 *)CM_MALLOC(fsize);
				cm_fread(data, fsize, 1, fp);

				kernel->white_list.version = (data[0] & 0x000000ff) | ((data[1] << 8) & 0x0000ff00) | ((data[2] << 16) & 0x00ff0000) | ((data[3] << 24) & 0xff000000);

				url_parseWapWhiteList(data, fsize);

				CM_FREE(data);
			}
			else
			{
				nbk_web_view_wap_white_list_release(&kernel->white_list);
			}

			cm_fclose(fp);
		}
		else
			nbk_web_view_wap_white_list_release(&kernel->white_list);
	}
}

nbk_web_view_ptr nbk_web_view_create(const void *graphic, const CM_RECT *frame)
{
	//S32 x = frame->x, y = frame->y;
	S32 width = frame->w, height = frame->h;
	
	//nbk_web_view_ptr web_view = 0;
	NPage *page = 0;
	nbk_web_view_ptr thiz = (nbk_web_view_ptr)CM_MALLOC(sizeof(nbk_web_view));
	if (!thiz) return thiz;

    cm_memset(thiz, 0, sizeof(*thiz));

    thiz->graphic = graphic;

	thiz->kernel = nbk_kernel_new();
	if (0 == thiz->kernel)
	{
		CM_FREE(thiz);
		return thiz;
	}

	thiz->width = width;
	thiz->height = height;

	//setting
	thiz->settings.allowImage = 1;
	thiz->settings.allowIncreateMode = 1;
	thiz->settings.recognisePhoneSite = 0;

#if 1
	thiz->settings.initMode = NEREV_STANDARD;
	thiz->settings.mode = NEREV_FF_FULL;
#else
	thiz->settings.initMode = NEREV_FF_FULL;
	thiz->settings.mode = NEREV_FF_FULL;
#endif

	thiz->settings.mainFontSize = 16;
	thiz->settings.selfAdaptionLayout = 1;

	//page init
	thiz->history = history_create();
	page = nbk_web_view_get_page(thiz);

	history_setMainBodyWidth(thiz->history, (S16)width);
	//page_setPlatformData(page, thiz);
	history_setPlatformData(thiz->history, thiz);
	//page_enablePic(page, 1);
	history_enablePic(thiz->history, 1);
	//page->settings = &web_view->settings;
	history_setSettings(thiz->history, &thiz->settings);
	history_enablePic(thiz->history, thiz->settings.allowImage);
	page_setScreenWidth(page, (S16)width);
	page_setEventNotify(page, (NBK_CALLBACK)nbk_web_view_on_page_event, thiz);

	//nbk_web_view_load_wap_white_list(thiz);

	return thiz;
}

void nbk_web_view_destroy(nbk_web_view_ptr thiz)
{
	if (0 == thiz)
		return;

	nbk_kernel_free(thiz->kernel);
	nbk_web_view_sever_connection_by_page_id(thiz, -1);
	history_delete(&thiz->history);

	CM_FREE(thiz);
}

nbk_kernel_ptr nbk_web_view_get_kernel(nbk_web_view_ptr thiz)
{
	if (0 == thiz)
		return 0;

	return thiz->kernel;
}

S32 nbk_web_view_width(nbk_web_view_ptr thiz)
{
	return thiz?thiz->width:0;
}

S32 nbk_web_view_height(nbk_web_view_ptr thiz)
{
	return thiz?thiz->height:0;
}

NPage* nbk_web_view_get_page(nbk_web_view_ptr thiz)
{
	return history_curr(thiz->history);
}

S32 nbk_web_view_get_page_width(nbk_web_view_ptr thiz)
{
	return page_width(nbk_web_view_get_page(thiz));
}

S32 nbk_web_view_get_page_height(nbk_web_view_ptr thiz)
{
	return page_height(nbk_web_view_get_page(thiz));
}


void nbk_web_view_set_mode(nbk_web_view* thiz, nbk_wv_mode mode)
{
	NPage *page = 0;

	if (!thiz)
		return;

	//web_view->mode = mode;
	page = nbk_web_view_get_page(thiz);

	if (mode == WV_MODE_DIRECT)
	{
		thiz->settings.initMode = NEREV_STANDARD;
	}
	else if (mode == WV_MODE_FF_FULL)
	{
		thiz->settings.initMode = NEREV_FF_FULL;
		thiz->settings.mode = NEREV_FF_FULL;
	}
	else if (mode == WV_MODE_FF_NARROW)
	{
		thiz->settings.initMode = NEREV_FF_NARROW;
		thiz->settings.mode = NEREV_FF_NARROW;
	}
	else if (mode == WV_MODE_UCK)
	{
		thiz->settings.initMode = NEREV_UCK;
		thiz->settings.mode = NEREV_UCK;
	}
}

void nbk_web_view_load_url(nbk_web_view_ptr thiz, const S8 * url)
{
	page_loadUrl(nbk_web_view_get_page(thiz), url, 0, N_NULL, N_NULL, N_FALSE);
}

void nbk_web_view_load_file(nbk_web_view_ptr thiz, const S8 * path)
{
    S32 len;
    S8 *url;

    CM_ASSERT(thiz != 0 && path != 0 && path[0] != 0);
    len = cm_strlen(path);

    url = (S8 *)CM_MALLOC(len + 8);    //add "file://"
    CM_ASSERT(url != 0);

    if (url)
    {
        cm_strcpy(url, "file://");
        cm_strcat(url, path);
        nbk_web_view_load_url(thiz, url);
		cm_free(url);
    }
}

void nbk_web_view_load_data(nbk_web_view_ptr thiz, const S8 * path, S8 *data, S32 len)
{
	NPage *page = 0;

	CM_TRACE_5("begin to load data: %d", len);

	page = nbk_web_view_get_page(thiz);
	page_loadData(page, path, data, len);

	CM_TRACE_5("load data ok");
}

void nbk_web_view_set_view_rect(nbk_web_view_ptr thiz, const CM_RECT *rect)
{
	thiz->view_rect.l = rect->x;
	thiz->view_rect.t = rect->y;
	thiz->view_rect.r = rect->x + rect->w;
	thiz->view_rect.b = rect->y + rect->h;
}

void nbk_web_view_render(nbk_web_view_ptr thiz, void * g)
{
	page_paint(nbk_web_view_get_page(thiz), &thiz->view_rect);
}

void nbk_web_view_update_area(nbk_web_view_ptr thiz, CM_RECT *update_disp_rect, bd_bool update)
{
#if 0
	NRect view_rect;
	CM_RECT rc, update_rect;
	CM_COLOR color;

	rc.x = thiz->x;
	rc.y = thiz->y;
	rc.w = thiz->width;
	rc.h = thiz->height;

	if( 0 == update_disp_rect ) 
	{
		update_rect = rc;
	}
	else if( rc.x > update_disp_rect->x || rc.y > update_disp_rect->y 
	      || rc.x + rc.w < update_disp_rect->x + update_disp_rect->w 
	      || rc.y + rc.h < update_disp_rect->y + update_disp_rect->h)
	{
		//return;
	}
	else
	{
		//update_rect = *update_disp_rect;
	}	

	view_rect = thiz->view_rect;
	
	//TODO
#if 0
	thiz->view_rect.l  += (update_rect.x - rc.x);
	thiz->view_rect.r   =  thiz->view_rect.l + (update_rect.right- update_rect.x);
	thiz->view_rect.t  +=  (update_rect.top - rc.top); 
	thiz->view_rect.b = thiz->view_rect.t + (update_rect.bottom - update_rect.top);	
#endif

    cm_memset(&color, 0xFF, sizeof(color));
	cm_gdi_set_brush_color(thiz->graphic, &color);
	cm_gdi_fill_rect(thiz->graphic, &update_rect);
	nbk_web_view_render(thiz, thiz->graphic);	
	
	thiz->view_rect = view_rect;
	if( update )
	{
		//cm_gdi_update_screen(thiz->graphic, &update_rect);
	}
#endif
    CM_COLOR color;
    CM_RECT rect;
    NRect view_rect;

    cm_memset(&color, 0xFF, sizeof(color));
    cm_gdi_set_brush_color(thiz->graphic, &color);
    
    thiz->y = update_disp_rect->y;
    thiz->height = update_disp_rect->h;
    
    rect.x = thiz->x;
    rect.y = thiz->y;
    rect.w = thiz->width;
    rect.h = thiz->height;
    
    cm_gdi_fill_rect(thiz->graphic, &rect);
    
    view_rect.l = update_disp_rect->x;
    view_rect.t = update_disp_rect->y;
    view_rect.r = update_disp_rect->x + update_disp_rect->w;
    view_rect.b = view_rect.t + update_disp_rect->h;
    
    thiz->view_rect = view_rect;
	nbk_web_view_render(thiz, thiz->graphic);
}

static void nbk_web_view_update_node(nbk_web_view_ptr thiz, NNode *node, bd_bool update)
{
	CM_RECT rc, update_rect;
	NPage *page = nbk_web_view_get_page(thiz);

	if( page->doc  && node )
	{
		NRect node_rect = { 0} ;
		
		doc_getFocusNodeRect(page->doc, node, &node_rect);
		node_rect.l -=2;
		node_rect.t -=2;
		node_rect.r +=2;
		node_rect.b +=2;

    	rc.x = thiz->x;
    	rc.y = thiz->y;
    	rc.w = thiz->width;
    	rc.h = thiz->height;

		update_rect.x = rc.x + (node_rect.l - thiz->view_rect.l);
		update_rect.w = update_rect.w + (node_rect.r - node_rect.l);
		update_rect.y = rc.y + (node_rect.t - thiz->view_rect.t);
		update_rect.h = update_rect.h + (node_rect.b - node_rect.t);		

		if( update_rect.x >= rc.x + rc.w || update_rect.x + update_rect.w <= rc.x 
		 || update_rect.y >= rc.y + rc.h || update_rect.y + update_rect.h <= rc.y )
		{
			return;
		}

		if( update_rect.x < rc.x )
		{
			update_rect.x = rc.x;
		}

//TODO
#if 0
		if( update_rect.x + update_rect.w > rc.x + rc.w)
		{
			update_rect.w = rc.right;
		}

		if( update_rect.y< rc.y )
		{
			update_rect.y = rc.y;
		}
		if( update_rect.bottom > rc.bottom)
		{
			update_rect.bottom = rc.bottom;
		}		
		nbk_web_view_update_area(thiz, &update_rect, update);	
#endif
	}
}

static bd_bool nbk_web_view_control_on_mouse_event(nbk_web_view_ptr thiz, S32 x, S32 y, bd_bool mouse_up)
{
	NEvent event;
    x += thiz->view_rect.l;
    y += thiz->view_rect.t;

	cm_memset(&event, 0, sizeof(NEvent));
    
    event.type = NEEVENT_PEN;
    event.page = nbk_web_view_get_page(thiz);
    event.d.penEvent.pos.x = x;
    event.d.penEvent.pos.y = y;

	if (mouse_up)
		event.d.penEvent.type = NEPEN_UP;
	else
		event.d.penEvent.type = NEPEN_DOWN;

    return (bd_bool)doc_processPen(&event);	
}

void nbk_web_view_on_mouse_down(nbk_web_view_ptr thiz, S32 x, S32 y)
{
	NPage *page = nbk_web_view_get_page(thiz);
	NNode *cur_node_focus;
	
	if (!page) return;

	if (nbk_web_view_control_on_mouse_event(thiz, x, y, 0))
		return;

	x += thiz->view_rect.l;
	y += thiz->view_rect.t;
	cur_node_focus = view_getFocusNode(page->view);	
	thiz->node_focus = doc_getFocusNodeByPos(page->doc, (S16)x, (S16)y);	

	if( cur_node_focus != thiz->node_focus )
	{
		view_setFocusNode(page->view, thiz->node_focus);	
#if 0		
		if( cur_node_focus )
		{
			nbk_web_view_update_node(thiz, cur_node_focus, TRUE);
		}

		if( thiz->node_focus  )
		{
			nbk_web_view_update_node(thiz, thiz->node_focus , TRUE);
		}
#endif 		
		nbk_web_view_update_screen(thiz);
	}
}

void nbk_web_view_on_mouse_up(nbk_web_view_ptr thiz, S32 x, S32 y)
{
	NPage *page = 0;
	NNode *node = 0;

	page = nbk_web_view_get_page(thiz);

	if (nbk_web_view_control_on_mouse_event(thiz, x, y, 1))
		return;

	x += thiz->view_rect.l;
	y += thiz->view_rect.t;

	node = doc_getFocusNodeByPos(page->doc, (short)x, (short)y);
	//jstring jurl = nbk_web_view_get_url(env,page,node);
	//jboolean flag = (*env)->CallBooleanMethod(env, obj, nbk_judge_url_method,jurl);
	if (node != 0 && node == thiz->node_focus)
	{
		doc_clickFocusNode(page->doc, thiz->node_focus);
		nbk_web_view_update_screen(thiz);
	}
}

U32 nbk_web_view_set_cur_pos(nbk_web_view_ptr thiz, S32  cur_pos)
{
//	NRect view_rect = thiz->view_rect;
	//S32 height = page_height(nbk_web_view_get_page(thiz));
	S32 temp = 0;
	
	if(thiz->view_rect.t == cur_pos)
	{
		return 0;
	}
	temp = thiz->view_rect.t -cur_pos;
	thiz->view_rect.t = cur_pos;
	thiz->view_rect.b -= temp;
	nbk_web_view_update_screen(thiz);
	return 0;
}

//offset < 0: move to right
//offset > 0: move to left
U32 nbk_web_view_horizontal_move(nbk_web_view_ptr thiz, S32 x, S32 width, S32 offset)
{
	S32 pwidth, move = 0;

    pwidth = page_width(nbk_web_view_get_page(thiz));

    if (offset > 0) //left
    {
        if (thiz->view_rect.l <= pwidth)
        {
            move= 1;
        }
    }
    else            //right
    {
        if (thiz->view_rect.l + (width) >= x)
        {
            move= 1;
        }
    }

    if (move)
    {
    	thiz->view_rect.l += offset;
    	thiz->view_rect.r += offset;
    	nbk_web_view_update_screen(thiz);
    }

    return 0;
}

void nbk_web_view_move_to_prev_focus(nbk_web_view_ptr thiz)
{
    NBK_Page* page = nbk_web_view_get_page(thiz);
    NNode* focus = (NNode*) view_getFocusNode(page->view);
    NRect r;

    if (focus == 0)
	{
        focus = doc_getFocusNode(page->doc, (NRect*) &thiz->view_rect);
    }
    else
	{
        doc_getFocusNodeRect(page->doc, focus, &r);
        if (rect_isIntersect((NRect*) &thiz->view_rect, &r))
            focus = doc_getPrevFocusNode(page->doc, focus);
        else
            focus = doc_getFocusNode(page->doc, (NRect*) &thiz->view_rect);
    }

    if (focus)
	{
        doc_getFocusNodeRect(page->doc, focus, &r);
        nbk_web_view_adjust_view_rect(thiz, (NRect*) &r, 0);
        view_setFocusNode(page->view, focus);
        nbk_web_view_update_screen(thiz);
    }
    else
		nbk_web_view_page_up(thiz, thiz->height);
}

void nbk_web_view_move_to_next_focus(nbk_web_view_ptr thiz)
{
    NBK_Page* page = nbk_web_view_get_page(thiz);
    NNode* focus = (NNode*) view_getFocusNode(page->view);
    NRect r;

    if (focus == 0)
	{
        focus = doc_getFocusNode(page->doc, (NRect*) &thiz->view_rect);
    }
    else
	{
        doc_getFocusNodeRect(page->doc, focus, &r);
        if (rect_isIntersect((NRect*) &thiz->view_rect, &r))
            focus = doc_getNextFocusNode(page->doc, focus);
        else
            focus = doc_getFocusNode(page->doc, (NRect*) &thiz->view_rect);
    }

    if (focus)
	{
        doc_getFocusNodeRect(page->doc, focus, &r);
        nbk_web_view_adjust_view_rect(thiz, (NRect*) &r, 1);
        view_setFocusNode(page->view, focus);
        nbk_web_view_update_screen(thiz);
    }
    else
        nbk_web_view_page_down(thiz, thiz->height);
}

void nbk_web_view_click_focus(nbk_web_view_ptr thiz)
{
	NPage *page = 0;
	NNode *node = 0;
	if (0 == thiz)
		return;

	page = nbk_web_view_get_page(thiz);

	node = view_getFocusNode(page->view);
	doc_clickFocusNode(page->doc, node);
}

U32 nbk_web_view_move_offset(nbk_web_view_ptr thiz, S32 offset)
{
	NRect view_rect = thiz->view_rect;
	S32 height = page_height(nbk_web_view_get_page(thiz));

	if( offset > 0 )
	{
		offset = MIN(offset, height - view_rect.b);	
	}
	else 
	{
		if( offset + view_rect.t < 0)
		{
			offset = -view_rect.t;
		}
	}
	view_rect.t += offset;
	view_rect.b += offset;		

	if( thiz->view_rect.t != view_rect.t  || thiz->view_rect.b != view_rect.b )
	{
		thiz->view_rect.t = view_rect.t;
		thiz->view_rect.b = view_rect.b;
		nbk_web_view_update_screen(thiz);
		return 1;
	}
	return 0;
}

void nbk_web_view_page_up(nbk_web_view_ptr thiz, S32 d)
{
	d = MIN(d, thiz->view_rect.t);

	thiz->view_rect.t -= d;
	thiz->view_rect.b -= d;

	nbk_web_view_update_screen(thiz);
}

void nbk_web_view_page_down(nbk_web_view_ptr thiz, S32 d)
{
	S32 height = 0;

	height = page_height(nbk_web_view_get_page(thiz));

	d = MIN(d, height - thiz->view_rect.b);

	thiz->view_rect.t += d;
	thiz->view_rect.b += d;

	nbk_web_view_update_screen(thiz);
}


bd_bool nbk_web_view_forward(nbk_web_view_ptr thiz)
{
	return (bd_bool)history_next(thiz->history);
}

bd_bool nbk_web_view_backward(nbk_web_view_ptr thiz)
{
	return (bd_bool)history_prev(thiz->history);
}

bd_bool nbk_web_view_refresh(nbk_web_view_ptr thiz)
{
	page_refresh(nbk_web_view_get_page(thiz));
	return 1;
}

bd_bool nbk_web_view_stop(nbk_web_view_ptr thiz)
{
	page_stop(nbk_web_view_get_page(thiz));
	return 1;
}

void nbk_web_view_set_call_back(nbk_web_view_ptr thiz, on_nbk_event cb, void *user)
{
	if (0 == thiz)
		return;

	thiz->call_back = cb;
	thiz->user = user;
}

const S8 * nbk_web_view_get_page_url(nbk_web_view_ptr thiz)
{
	NPage *page = 0;

	page = nbk_web_view_get_page(thiz);
	if (0 == page)
		return 0;

	return doc_getUrl(page->doc);
}

const U16 *nbk_web_view_get_page_title(nbk_web_view_ptr thiz)
{
	NPage *page = 0;

	page = nbk_web_view_get_page(thiz);
	if (0 == page)
		return 0;
	return (const U16 *)doc_getTitle(page->doc);
}

void nbk_web_view_clear_cookies(nbk_web_view_ptr thiz)
{
	if (0 == thiz)
		return;

	nbk_cookie_deinit(thiz->kernel->cookie_info);
}

void nbk_web_view_get_baidu_uid(nbk_web_view_ptr thiz, S8 *uid)
{
	const S8 *uid_format = "bd_%s";
	S8 imei[32] = {0};
	S32 len = 0;
	S32 i = 0;

	if (cm_get_imei(imei) < 0)
	{
		cm_strcpy(imei, "0123456789ABCDEF");
	}

	//reverse imei
	len = cm_strlen(imei);

	for (i = 0; (len - i - 1) >= i; i++)
	{
		S8 c = imei[i];
		imei[i] = imei[len - i - 1];
		imei[len - i - 1] = c;
	}

	cm_sprintf(uid, uid_format, imei);
}

void nbk_web_view_get_baidu_ua(nbk_web_view_ptr thiz, S8 * ua)
{
	const S8 *ua_format = "nbk_%d_%d_%s_%d-%d-%d-%d_%s";

	if (0 == thiz || 0 == ua)
		return;

	cm_sprintf(ua, ua_format, thiz->width, thiz->height, cm_platform_get_name(), 
	               NBK_VER_MAJOR, NBK_VER_MINOR, NBK_VER_BUILD, 
	               NBK_VER_REV, cm_platform_get_code());
}

void nbk_web_view_add_connection(nbk_web_view_ptr thiz, nbk_connection_ptr conn)
{
	nbk_web_view *web_view = thiz;
	web_view->conn_list = bd_list_append(web_view->conn_list, conn);
}

void nbk_web_view_remove_connection(nbk_web_view_ptr thiz, nbk_connection_ptr conn)
{
	nbk_web_view *web_view = thiz;
	web_view->conn_list = bd_list_remove(web_view->conn_list, conn);
}

void nbk_web_view_sever_connection_by_page_id(nbk_web_view_ptr thiz, S32 pid)
{
	bd_list *node = 0;	

	node = thiz->conn_list;	
	while (node)
	{
		nbk_connection_ptr conn = (nbk_connection_ptr)node->data;
		if (pid == -1 || conn->request->pageId == pid)
		{
			bd_list *next = node->next;

			//conn->close(conn);
			nbk_connection_close(conn);
			nbk_connection_free(conn);

			node->data = conn = 0;

			thiz->conn_list = bd_list_delete_link(thiz->conn_list, node);
			node = next;
			continue;
		}
		node = node->next;
	}
}

