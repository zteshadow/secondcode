
#include "nbk_http_connection.h"
#include "nbk_fscache.h"
#include "nbk_kernel.h"

#include "cm_utility.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


#define NBK_FULL_URL				"http://cbs.baidu.com:80/"
//#define NBK_MULTIPICSSERVICE_URL	"http://f3.mi.baidu.com/folder_mod?tc-res=5"
#define NBK_MULTIPICSSERVICE_URL    "http://cbs.baidu.com:80/?tc-res=6"

#define HTTP_HEADER_USER_AGENT				"User-Agent"
#define HTTP_HEADER_CONNECTION				"Connection"
#define HTTP_HEADER_HOST					"Host"
#define HTTP_HEADER_CONTENT_LENGTH			"Content-Length"
#define HTTP_HEADER_CACHE_CONTROL			"Cache-Control"
#define HTTP_HEADER_ACCEPT					"Accept"
#define HTTP_HEADER_ACCEPT_ENCODING			"Accept-Encoding"
#define HTTP_HEADER_ACCEPT_CHARSET			"Accept-Charset"
#define HTTP_HEADER_CONTENT_TYPE			"Content-Type"
#define HTTP_HEADER_CONTENT_ENCODING		"Content-Encoding"
#define HTTP_HEADER_REFERER					"Referer"
#define HTTP_HEADER_TRANSFER_ENCODING		"Transfer-Encoding"
#define HTTP_HEADER_COOKIE					"Cookie"
#define HTTP_HEADER_SET_COOKIE				"Set-Cookie"

#define SUFFIX_HTML	".htm"
#define SUFFIX_GZIP	".gz"
#define SUFFIX_GIF	".gif"
#define SUFFIX_JPEG	".jpg"
#define SUFFIX_PNG	".png"


typedef struct _bd_http_url
{
	S8 host[128];
	S32 port;
	S8 *path;
	S8 *query;
}bd_http_url;


static S32 bd_file_create(const S8 *fname);
static S32 bd_get_content_length(void *http);

static void nbk_http_connection_on_receive_header(nbk_connection_ptr thiz);
static void nbk_http_connection_on_receive_body(nbk_connection_ptr thiz, void *data, S32 len);
static void nbk_http_connection_on_receive_end(nbk_connection_ptr thiz);
static void nbk_http_connection_on_receive_error(nbk_connection_ptr thiz);

static S32 nbk_on_http_connection_event(CM_HTTP_STATUS status, void *data, S32 len, void *user);
static CM_BOOL nbk_http_connection_open(nbk_connection_ptr thiz);
static void nbk_http_connection_close(nbk_connection_ptr thiz);
static void nbk_http_connection_free(nbk_connection_ptr thiz);

static void bd_http_url_init(bd_http_url *url);
static CM_IO_RESULT bd_http_parse_url(const S8 *str_url, bd_http_url *url);

static void nbk_http_connection_add_headers(nbk_connection_ptr thiz);
static void nbk_http_connection_set_value(nbk_connection_ptr thiz);
static void nbk_http_connection_save_cookie(nbk_connection_ptr thiz);

static void nbk_http_connection_cache_write(nbk_connection_ptr thiz, void *data, S32 len);


static S32 bd_file_create(const S8 *fname)
{
    S32 value = 0;

    if (fname)
    {
        CM_HANDLE fp = cm_fopen(fname, "w");
        if (fp)
        {
            value = 1;
            cm_fclose(fp);
        }
    }

    return value;
}

static S32 bd_get_content_length(void *http)
{
    S32 value = 0;

    if (http)
    {
        S8 *data = cm_http_get_header(http, HTTP_HEADER_CONTENT_LENGTH);
        if (data)
        {
            //"Content-Length:"
            data += 15;
            value = cm_atoi(data);
        }
    }

    return value;
}

static S32 nbk_on_http_connection_event(CM_HTTP_STATUS status, void *data, S32 len, void *user)
{
	nbk_connection_ptr thiz = (nbk_connection_ptr)user;

	switch(status)
	{
	case CM_HTTP_ON_HEADER:
		nbk_http_connection_on_receive_header(thiz);
    break;

	case CM_HTTP_ON_BODY:
		nbk_http_connection_on_receive_body(thiz, data, len);
	break;

	case CM_HTTP_ON_END:
        nbk_http_connection_on_receive_end(thiz);
    break;

	default:
		nbk_http_connection_on_receive_error(thiz);
	break;
	}

	return 1;
}

static CM_BOOL nbk_http_connection_open(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;
	//S32 width = 0;
	//S32 height = 0;
	//S8 * misc = "00000";

	//width = thiz->web_view->width;//bd_get_screen_width();
	//height = thiz->web_view->height;//();
	
	if (0 == thiz || 0 == thiz->sub)
		return 0;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	nbk_http_connection_add_headers(thiz);

	if (NEREV_FF_FULL == thiz->request->via || NEREV_FF_NARROW == thiz->request->via)
	{
		const S8 * url_format = "%s?uid=%s&ua=%s";
        S8 nbk_full_url[256] = {0};
		S8 ua[32] = {0};
		S8 uid[32] = {0};
 		CM_TRACE_5("Start ff full request");

		nbk_web_view_get_baidu_uid(thiz->web_view, uid);
		nbk_web_view_get_baidu_ua(thiz->web_view, ua);

		cm_sprintf(nbk_full_url, url_format, NBK_FULL_URL, uid, ua);

    	cm_http_set_data(http_conn->http_client, thiz->request->upcmd->buf, thiz->request->upcmd->cur);
    	cm_http_send_request(http_conn->http_client, CM_HTTP_POST, nbk_full_url);
		return 1;
	}

	if (NEREV_UCK == thiz->request->via)
	{
		return 0;
	}

	if (NEREV_FF_MULTIPICS == thiz->request->via)
	{
		CM_TRACE_5("Start ff multipics request");

    	cm_http_set_data(http_conn->http_client, thiz->request->upcmd->buf, thiz->request->upcmd->cur);
    	cm_http_send_request(http_conn->http_client, CM_HTTP_POST, NBK_MULTIPICSSERVICE_URL);
		return 1;
	}

	if (0 != thiz->request->body)
	{
		S32 len = cm_strlen(thiz->request->body);
		CM_TRACE_5("Start direct post request");
    	cm_http_set_data(http_conn->http_client, thiz->request->body, len);
    	cm_http_send_request(http_conn->http_client, CM_HTTP_POST, thiz->request->url);
		return 1;
	}

	CM_TRACE_5("Start direct get request");
	cm_http_send_request(http_conn->http_client, CM_HTTP_GET, thiz->request->url);

	return 1;
}

static void nbk_http_connection_close(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;
    if(http_conn->zip_type != CM_ZIP_NONE )
    {
		cm_zip_destory(http_conn->zstrm);
		http_conn->zstrm = 0;
    }

    cm_http_destory(http_conn->http_client);
    http_conn->http_client = 0;
}

static void nbk_http_connection_free(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http = 0;

	if (0 == thiz)
		return;

	http = (nbk_http_connection_ptr)thiz->sub;

	if (0 == http)
		return;

	if (http->url_response)
		bd_string_free(&http->url_response);

	nbk_http_connection_set_value(0);

	nbk_ffpkg_parser_free(http->ff_parser);
	CM_FREE(http);
}

static void bd_http_url_init(bd_http_url *url)
{
	if (NULL == url)
		return;

	cm_memset(url->host, 0, sizeof(url->host));
	url->port = 0;
	if (url->path)
		bd_string_free(&url->path);
	if (url->query)
		bd_string_free(&url->query);
}

static CM_IO_RESULT bd_http_parse_url(const S8 *str_url, bd_http_url *url)
{
	enum url_part
	{
		up_host,
		up_port,
		up_path,
		up_query
	};

	CM_IO_RESULT result = CM_ERROR_SUCCESS;

	S32 part = up_host;
	const S8 *p = str_url;
	const S8 *q = 0;
	const S8 *t = 0;

	bd_http_url_init(url);

	t = p + cm_strlen(str_url);

	if (cm_strncasecmp(str_url, "http://", 7) == 0)
	{
		p = &p[7];
	}
	//scheme://domain:port/path?query_string#fragment_id

	q = p;

	while (*p && (part != up_query))//wuli debug for scheme://domain:port/path?quary&src=http://www.baidu.com
	{
		if (*p == ':')
		{
			if (part != up_host)
				return CM_ERROR_FAILED;

			cm_strncpy(url->host, q, p - q);
			q = p+1;
			part = up_port;
		}

		if (*p == '/')
		{
			if (part == up_host)
			{
				url->port = 80;

				cm_strncpy(url->host, q, p - q);
				q = p;
				part = up_path;
				continue;
			}

			if (part == up_port)
			{
				S8 port[10] = {0};
				cm_strncpy(port, q, p - q);
				url->port = cm_atoi(port);

				q = p;
				part = up_path;
				continue;
			}
		}

		if (*p == '?')
		{
			if (part == up_host)
			{
				cm_strncpy(url->host, q, p - q);
				q = p+1;

				url->port = 80;
				//bd_strncpy(url->query, p, t - p);

				part = up_query;
				continue;
			}

			if (part == up_port)
			{
				S8 port[10] = {0};
				cm_strncpy(port, q, p - q);
				url->port = cm_atoi(port);

				q = p;
				part = up_query;
				continue;
			}

			if (part == up_path)
			{
				url->path = bd_string_new(p - q);
				cm_strncpy(url->path, q, p - q);
				q = p+1;

				//bd_strncpy(url->query, p, t - p);

				part = up_query;
				continue;
			}
		}

		p++;
	}

	if (part == up_host)
	{
		cm_strncpy(url->host, q, p - q);
		q = p+1;
	}

	if (part == up_port)
	{
		S8 port[10] = {0};
		cm_strncpy(port, q, p - q);
		url->port = cm_atoi(port);

		q = p;
	}

	if (part == up_path)
	{
		url->path = bd_string_new(p - q);
		cm_strncpy(url->path, q, p - q);
		url->path[p-q] = 0;
		q = p+1;
	}

	if (part == up_query)
	{
		p += (cm_strlen(q)+1);
		url->query = bd_string_new(p - q);
		cm_strncpy(url->query, q, p - q);
		url->query[p-q] = 0;
		q = p+1;
	}

	if (cm_strlen(url->host) == 0)
		return CM_ERROR_FAILED;

	if (url->path == 0)
	{
		url->path = bd_string_new(1);
		url->path[0] = '/';
		url->path[1] = 0;
	}

	if (url->port == 0)
		url->port = 80;

	return result;
}

S8 *bd_http_url_to_str(bd_http_url *url)
{
	S8 *str_url = NULL;
	S32 len = 0;

	len += 7; //"http://"
	len += cm_strlen(url->host);
	if (url->port != 80)
	{
		len += 1; //":"
		len += 5; //max 65535
	}

	if (url->path)
	{
		if (url->path[0] != '/')
			len += 1; // '/'

		len += cm_strlen(url->path);
	}

	if (url->query)
	{
		len += 1; //'?'
		len += cm_strlen(url->query);
	}

	str_url = bd_string_new(len);

	cm_strcat(str_url, "http://");
	cm_strcat(str_url, url->host);

	if (url->port != 80)
	{
		S8 sp[10] = {0};
		cm_strcat(str_url, ":");

        cm_sprintf(sp, "%d", url->port);
		cm_strcat(str_url, sp);
	}

	if (url->path)
	{
		if (url->path[0] != '/')
		{
			cm_strcat(str_url, "/");
		}

		cm_strcat(str_url, url->path);
	}

	if (url->query)
	{
		cm_strcat(str_url, "?");
		cm_strcat(str_url, url->query);
	}

	return str_url;
}

static void nbk_http_connection_set_cookie_direct(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;
	///bd_http_request *req;
	nbk_cookie *co = 0;
	S32 pathlen = 0;
	bd_output_stream strm = {0};
	CM_IO_RESULT result = CM_ERROR_SUCCESS;
	bd_http_url url = {0};

	if (!thiz) return;

	http_conn = thiz->sub;

	bd_http_parse_url(thiz->request->url, &url);

	pathlen = url.path ? cm_strlen(url.path) : 0;

	co = nbk_cookie_get_list(nbk_web_view_get_kernel(thiz->web_view)->cookie_info, url.host, pathlen?url.path:"/", 0);

	if (!co)
	{
		CM_TRACE_1("get co list failed");
	}

	if (co)
	{
		result = bd_output_stream_init(&strm);
		if (result == CM_ERROR_SUCCESS)
		{
		    nbk_cookie *p = co;
			while (p)
			{
				bd_os_write_utf8(&strm, p->name);
				bd_os_write_utf8(&strm, "=");
				bd_os_write_utf8(&strm, p->value);
				bd_os_write_utf8(&strm, "; ");
				p = p->next;
			}

			strm.buff[strm.pos] = 0;

			CM_TRACE_5("cookie: %s", (S8 *)strm.buff);
			cm_http_set_header(http_conn->http_client, HTTP_HEADER_COOKIE, (const S8 *)strm.buff);

			bd_output_stream_destroy(&strm);
		}

		nbk_cookie_free_list(co, 0);
	}

	if (url.path)
	{
	    bd_string_free(&(url.path));
	}

	if (url.query)
	{
	    bd_string_free(&(url.query));
	}
}

static void nbk_http_connection_set_cookie(nbk_connection_ptr thiz)
{
	if (thiz->request->via == NEREV_STANDARD)
	{
		nbk_http_connection_set_cookie_direct(thiz);
		return;
	}
}

static void nbk_http_connection_add_headers(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	cm_http_set_header(http_conn->http_client, HTTP_HEADER_ACCEPT, "*/*");
	cm_http_set_header(http_conn->http_client, HTTP_HEADER_ACCEPT_CHARSET, "utf-8, iso-8859-1;");
	//cm_http_set_header(http_conn->http_client, HTTP_HEADER_USER_AGENT, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.2; Android/1.6; NBK)");
	cm_http_set_header(http_conn->http_client, HTTP_HEADER_USER_AGENT, "Baidu-NBK/0.9 (zhanxunOS/9.2; Series60/3.2; zhanxun/X6-00) 3gpp-gba");
	cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONNECTION, "Keep-Alive");
	cm_http_set_header(http_conn->http_client, HTTP_HEADER_CACHE_CONTROL, "no-cache");

	nbk_http_connection_set_cookie(thiz);

	if (thiz->request->type != NEREQT_IMAGE)
		cm_http_set_header(http_conn->http_client, HTTP_HEADER_ACCEPT_ENCODING, "gzip, deflate");

	if (NEREV_FF_FULL == thiz->request->via ||
		NEREV_UCK == thiz->request->via ||
		NEREV_FF_NARROW == thiz->request->via ||
		NEREV_FF_MULTIPICS == thiz->request->via)
	{
		S8 strLen[32] = {0};
		cm_sprintf(strLen, "%d", thiz->request->upcmd->cur);
		cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONTENT_LENGTH, strLen);
		cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONTENT_TYPE, "application/octet-stream");
	}
	else if (thiz->request->body)
	{
		S8 strLen[32] = {0};
		cm_sprintf(strLen, "%d", cm_strlen(thiz->request->body));
		cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONTENT_LENGTH, strLen);

		if (NEENCT_URLENCODED == thiz->request->enc)
		{
			cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
		}
		else if (NEENCT_MULTIPART == thiz->request->enc)
		{
			cm_http_set_header(http_conn->http_client, HTTP_HEADER_CONTENT_TYPE, "multipart/form-data");
		}
	}
}

static S32 nbk_http_connection_get_mime(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;
	S8 * header = 0;
	S32 mime = NEMIME_UNKNOWN;

	if (0 == thiz || 0 == thiz->sub)
		return NEMIME_UNKNOWN;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	header = cm_http_get_header(http_conn->http_client, HTTP_HEADER_CONTENT_TYPE);

	if (0 == header)
		return mime;

	if (cm_strstr(header, "html") != 0)
	{
		mime = NEMIME_DOC_HTML;
	}
	else if (cm_strstr(header, "wap") != 0)
	{
		mime = NEMIME_DOC_WML;
	}
	else if (cm_strstr(header, "xhtml") != 0)
	{
		mime = NEMIME_DOC_XHTML;
	}
	else if (cm_strstr(header, "gif") != 0)
	{
		mime = NEMIME_IMAGE_GIF;
	}
	else if (cm_strstr(header, "jpeg") != 0)
	{
		mime = NEMIME_IMAGE_JPEG;
	}
	else if (cm_strstr(header, "png") != 0)
	{
		mime = NEMIME_IMAGE_PNG;
	}
	else if (cm_strstr(header, "css") != 0)
	{
		mime = NEMIME_DOC_CSS;
	}

	return mime;
}

static CM_ZIP_TYPE nbk_http_connection_zip_type(nbk_connection_ptr thiz)
{
	S8 *header = 0;
	CM_ZIP_TYPE type = CM_ZIP_NONE;
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return type;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	header = cm_http_get_header(http_conn->http_client, HTTP_HEADER_CONTENT_ENCODING);

	if (0 == header)
		return CM_ZIP_NONE;

	if (cm_strstr(header, "gzip") != 0)
		type = CM_ZIP_GZIP;
	else if (cm_strstr(header, "deflate") != 0)
		type = CM_ZIP_DEFLATE;

	return type;
}

static void nbk_http_connection_save_cookie(nbk_connection_ptr thiz)
{
    S32 cnt, i;
    bd_http_url url = {0};
	nbk_http_connection_ptr http_conn = 0;

	nbk_kernel_ptr k = nbk_web_view_get_kernel(thiz->web_view);
	//bd_list *node = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;
    cnt = cm_http_get_header_cnt(http_conn->http_client, HTTP_HEADER_SET_COOKIE);
    bd_http_parse_url(cm_http_get_response_url(http_conn->http_client), &url);

    for (i = 0; i < cnt; i++)
	{
		S8 *co = cm_http_get_header_item(http_conn->http_client, HTTP_HEADER_SET_COOKIE, i);

        //TODO with Set-Cookie or not
		CM_ASSERT(0);
		nbk_cookie_add(k->cookie_info, 1, co, url.host, 0);
	}

	if (url.path)
	{
	    bd_string_free(&(url.path));
	}

	if (url.query)
	{
	    bd_string_free(&(url.query));
	}
}

static void nbk_http_connection_cache_init(nbk_connection_ptr thiz, S32 mime)
{
    S32 value;
	CM_IO_RESULT result = CM_ERROR_SUCCESS;
	nbk_kernel_ptr k = nbk_web_view_get_kernel(thiz->web_view);
	S8 dir[CM_MAX_PATH] = {0};
	S8 sub_dir[CM_MAX_PATH] = {0};
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	cm_memset(http_conn->cache_file_name, 0, sizeof(http_conn->cache_file_name));

	CM_TRACE_5("create cache file now...");

	if (NEREQT_MAINDOC == thiz->request->type)
	{//clear 
		CM_TRACE_5("it is main doc, so beginning to clean all old cached pages");

		nbk_kernel_delete_cache_cookie_ff();
		nbk_kernel_clear_cache(k, thiz->request->pageId);
	}

    value = cm_facility_get_work_dir(dir);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(dir, nbk_kernel_get_cache_path(k));
    CM_ASSERT(value >= 0);

	cm_sprintf(sub_dir, "cp%05d", thiz->request->pageId);
    value = cm_facility_add_sub_dir(dir, sub_dir);
    CM_ASSERT(value >= 0);

	if (!cm_dir_exist(dir))
		result = cm_mkdir(dir);

	if (CM_ERROR_SUCCESS != result)
	{
		CM_TRACE_1("make cache dir[%s] failed: %d", dir, result);
		return;
	}

	CM_TRACE_5("make cache dir ok: %s", dir);

	if (NEREV_FF_FULL != thiz->request->type && NEREV_FF_NARROW != thiz->request->type && (NEREQT_MAINDOC == thiz->request->type) && http_conn->url_response)
	{//write uri file
		S8 file_name[CM_MAX_PATH] = {0};
		CM_HANDLE fp = {0};

        cm_strcpy(file_name, dir);
        value = cm_facility_add_sub_dir(file_name, "uri");
        CM_ASSERT(value >= 0);

		CM_TRACE_5("create uri cache now: %s", file_name);

		if (fp = cm_fopen(file_name, "a+"))
		{
			cm_fwrite((void *)http_conn->url_response, cm_strlen(http_conn->url_response), 1, fp);
			cm_fclose(fp);
		}
	}

	if (NEREQT_MAINDOC == thiz->request->type && 
		(mime == NEMIME_DOC_HTML || mime == NEMIME_DOC_XHTML || mime == NEMIME_DOC_WML))
	{//write doc
		cm_strcpy(http_conn->cache_file_name, dir);	
		cm_sprintf(sub_dir, "doc%s", (http_conn->zip_type != CM_ZIP_NONE) ? SUFFIX_GZIP : SUFFIX_HTML);
        value = cm_facility_add_sub_dir(http_conn->cache_file_name, sub_dir);
        CM_ASSERT(value >= 0);

		CM_TRACE_5("create doc cache now: %s", http_conn->cache_file_name);

		if (!bd_file_create(http_conn->cache_file_name))
			http_conn->cache_file_name[0] = 0;

		return;
	}
	
	if (NEMIME_IMAGE_GIF == mime || NEMIME_IMAGE_JPEG == mime || NEMIME_IMAGE_PNG == mime)
	{//write image
		S8 md5[33] = {0};
		cm_create_md5_string(thiz->request->url, cm_strlen(thiz->request->url), md5);

		cm_strcpy(http_conn->cache_file_name, dir);
        value = cm_facility_add_sub_dir(http_conn->cache_file_name, md5);
        CM_ASSERT(value >= 0);

		CM_TRACE_5("create image cache[%s] now", http_conn->cache_file_name);

		if (!bd_file_create(http_conn->cache_file_name))
		{
			CM_TRACE_1("create image cache[%s] failed.", http_conn->cache_file_name);
			http_conn->cache_file_name[0] = 0;
		}
	
		return;
	}

	if (thiz->request->via == NEREV_FF_FULL || thiz->request->via == NEREV_FF_NARROW)
	{//write ff doc
		cm_strcpy(http_conn->cache_file_name, dir);
        value = cm_facility_add_sub_dir(http_conn->cache_file_name, "ff.pkg");
        CM_ASSERT(value >= 0);

		CM_TRACE_5("create ffpkg cache[%s] now.request via [%d]", http_conn->cache_file_name,thiz->request->via);

		if (!bd_file_create(http_conn->cache_file_name))
		{
			CM_TRACE_1("create ffpkg cache[%s] failed.", http_conn->cache_file_name);
			http_conn->cache_file_name[0] = 0;
		}

		return;
	}
	
	if(thiz->request->via ==NEREV_FF_MULTIPICS)
	{//write ff multipics
		cm_strcpy(http_conn->cache_file_name, dir);
        value = cm_facility_add_sub_dir(http_conn->cache_file_name, "res.pkg");
        CM_ASSERT(value >= 0);
	
		CM_TRACE_5("create ffpkg cache NEREV_FF_MULTIPICS[%s] now.request via [%d]", http_conn->cache_file_name,thiz->request->via );

		if (!bd_file_create(http_conn->cache_file_name))
		{
			CM_TRACE_1("create ffpkg cache[%s] failed.", http_conn->cache_file_name);
			http_conn->cache_file_name[0] = 0;
		}

		return;
	}
}

static void nbk_http_connection_cache_write(nbk_connection_ptr thiz, void *data, S32 len)
{
	CM_HANDLE fp;
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	if (http_conn->cache_file_name[0] == 0)
		return;

	if (fp = cm_fopen(http_conn->cache_file_name, "a+"))
	{
		cm_fwrite(data, len, 1, fp);
		cm_fclose(fp);
	}
	else
	{
		CM_TRACE_1("write cache file failed!");
	}
}

static void nbk_http_connection_completed(nbk_connection_ptr thiz, S32 error)
{
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	if (http_conn->ff_parser)
	{
		nbk_connection_on_package_over(thiz);
	}

	if (NEREQT_IMAGE == thiz->request->type)
	{
		//nbk_fscache_save_data_over(0, 0);
	}

	if (0 != error)
	{
		nbk_connection_on_error(thiz, error);
		return;
	}

	nbk_connection_on_complete(thiz);
}

static void nbk_http_connection_set_value(nbk_connection_ptr thiz)
{
#ifdef __ANDROID__
	nbk_http_connection_ptr http_conn = 0;
	JNIEnv* env = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;

	if (0 == http_conn->http_client || 0 == http_conn->http_client->socket)
		return;

	env = getJNIEnv();
	(*env)->SetIntField(env, (jobject)http_conn->http_client->socket, http_conn_id, (jint)BD_POINTER_TO_INT(thiz));
	jni_release_env();
#endif
}

nbk_connection_ptr nbk_http_connection_new(nbk_web_view_ptr web_view, NRequest *req)
{
	nbk_connection_ptr thiz = nbk_connection_new(web_view, req);

	if (0 != thiz)
	{
		nbk_http_connection_ptr sub = 0;

		sub = (nbk_http_connection_ptr)CM_MALLOC(sizeof(*sub));

		if (0 != sub)
		{
		    cm_memset(sub, 0, sizeof(*sub));

            sub->http_client = cm_http_create(nbk_on_http_connection_event, thiz);

			if (0 != sub->http_client)
			{
				sub->url_response = cm_strdup(req->url);
				sub->ff_parser = 0;
				thiz->sub = sub;

				if (NEREV_FF_FULL == req->via ||
					NEREV_FF_NARROW == req->via ||
					NEREV_FF_MULTIPICS == req->via)
				{
					sub->ff_parser = nbk_ffpkg_parser_new(thiz);
				}

				thiz->open = nbk_http_connection_open;
				thiz->close = nbk_http_connection_close;
				thiz->free = nbk_http_connection_free;

				nbk_http_connection_set_value(thiz);
			}
			else
			{
				CM_FREE(sub);
				CM_FREE(thiz);
			}
		}
		else
		{
			CM_FREE(thiz);
		}
	}

	return thiz;
}

static void nbk_http_connection_on_receive_header(nbk_connection_ptr thiz)
{
    S32 code;
	nbk_http_connection_ptr http_conn = 0;
	NRespHeader header = {0};

	if (0 == thiz || 0 == thiz->sub)
		return;

    code = cm_http_get_code(http_conn->http_client);
	http_conn = (nbk_http_connection_ptr)thiz->sub;

	if (503 == code && (NEREV_FF_FULL == thiz->request->via || NEREV_FF_NARROW == thiz->request->via))
	{
		CM_TRACE_5("http status code: %d", code);
		nbk_http_connection_completed(thiz, NELERR_FF_NOT_SUPPORT);
		return;
	}

	if (code >= 400)
	{
		CM_TRACE_5("http status code: %d", code);
		nbk_http_connection_completed(thiz, code);
		return;
	}

#ifdef __SPR_WRE__
	if ( (code == 301) || (code == 302) )
	{
		S8 * url = 0;
		S32 url_len=0;
		S8 port[10]={0};

		url = cm_http_get_response_url(http_conn->http_client);

		if(thiz->request->url)
			bd_string_free(&thiz->request->url);
		thiz->request->url = cm_strdup(url);
		if (http_conn->url_response)
			bd_string_free(&http_conn->url_response);
		http_conn->url_response = cm_strdup(url);
		return;
	}
#endif
	if (NEREV_FF_FULL != thiz->request->via &&
		NEREV_FF_MULTIPICS != thiz->request->via &&
		NEREV_UCK != thiz->request->via &&
		NEREV_FF_NARROW != thiz->request->via &&
		NEREQT_MAINDOC == thiz->request->type)
	{//direct doc

	}

	header.mime = nbk_http_connection_get_mime(thiz);
	header.content_length = bd_get_content_length(http_conn->http_client);

	nbk_http_connection_save_cookie(thiz);

	http_conn->zip_type = nbk_http_connection_zip_type(thiz);
	if (http_conn->zip_type != CM_ZIP_NONE)
	{
		http_conn->zstrm = cm_zip_create(http_conn->zip_type);
	}

	if (NEREV_FF_FULL != thiz->request->via &&
		NEREV_FF_MULTIPICS != thiz->request->via &&
		NEREV_FF_NARROW != thiz->request->via)
	{
		nbk_connection_on_receive_header(thiz, &header);
	}

	nbk_http_connection_cache_init(thiz, header.mime);
}

static void nbk_http_connection_on_receive_body(nbk_connection_ptr thiz, void *data, S32 len)
{
	nbk_http_connection_ptr http_conn = 0;

	if (0 == thiz || 0 == thiz->sub)
		return;

	if (thiz->closed)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;
	nbk_http_connection_cache_write(thiz, data, len);

	if (http_conn->zip_type != CM_ZIP_NONE)
	{
		S8 *dest = 0;
		S32 dest_len = 0;
		dest_len = cm_zip_decompress(http_conn->zstrm, data, len, &dest);

		nbk_connection_on_receive_data(thiz, dest, dest_len, len);

		CM_FREE(dest);
	}
	else if (NEREV_FF_FULL == thiz->request->via ||
		NEREV_FF_MULTIPICS == thiz->request->via ||
		NEREV_FF_NARROW == thiz->request->via)
	{
		nbk_ffpkg_parse(http_conn->ff_parser, data, len);
	}
	else
	{
		nbk_connection_on_receive_data(thiz, data, len, len);
	}
}

static void nbk_http_connection_on_receive_end(nbk_connection_ptr thiz)
{
	nbk_http_connection_ptr http_conn = 0;
	//NRespHeader header;

	if (0 == thiz || 0 == thiz->sub)
		return;
	http_conn = (nbk_http_connection_ptr)thiz->sub;

	nbk_http_connection_completed(thiz, 0);
#ifdef __SPR_WRE__
	nbk_web_view_remove_connection(thiz->web_view, thiz);
	nbk_connection_close(thiz);
	nbk_connection_free(thiz);
#endif /* __SPR_WRE__ */
}

static void nbk_http_connection_on_receive_error(nbk_connection_ptr thiz)
{
	S32 status_code = 0;
	nbk_http_connection_ptr http_conn = 0;
	//NRespHeader header;

	if (0 == thiz || 0 == thiz->sub)
		return;

	http_conn = (nbk_http_connection_ptr)thiz->sub;	
	status_code = cm_http_get_code(http_conn->http_client);

	nbk_http_connection_completed(thiz, -99999);
#ifdef __SPR_WRE__
	nbk_web_view_remove_connection(thiz->web_view, thiz);
	nbk_connection_close(thiz);
	nbk_connection_free(thiz);
#endif /* __SPR_WRE__ */	
}

