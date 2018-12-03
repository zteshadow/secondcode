
#include "nbk_ffpkg_parser.h"
#include "nbk_core.h"

#include "cm_string.h"
#include "cm_lib.h"
#include "cm_zip_stream.h"
#include "cm_debug.h"


#define MIN(a, b)   ((a) < (b) ? (a) : (b))


enum _ff_msgwin_stage
{
	FF_MSGWIN_READY,
	FF_MSGWIN_TYPE,
	FF_MSGWIN_LENGTH,
	FF_MSGWIN_MSG,
	FF_MSGWIN_OVER
};

enum _ff_msgwin_type
{
	MSGWIN_ALERT		= 0x1,
	MSGWIN_CONFIRM,
	MSGWIN_PROMPT
};

typedef enum _TClipSetp {
	TXRepeat,
	TYRepeat,
	TXIsPercent,
	TYIsPercent,
	TX,
	TY,
	TElemWidth,
	TElemHeight
}TClipSetp;

typedef enum _TBgPosSetp {
	   TPosX, TPosY
}TBgPosSetp;


static void nbk_ffpkg_packimg_parser_init(nbk_ffpkg_packimg_parser *parser);
static void nbk_ffpkg_inc_data_parser_init(nbk_ffpkg_inc_data_parser *parser);
static void nbk_html_content_handle(nbk_ffpkg_parser *parser, S8 *data, S32 len, CM_BOOL finish);
static void nbk_ffpkg_packimg_parse(nbk_connection_ptr conn, nbk_ffpkg_packimg_parser *parser, S8 *data, S32 size);
static NRequest* nbk_create_fake_image_request(NRequest *req, S8 *url, const S8 *str_clip, const S8 *refer_url);


static void nbk_ffpkg_msgwin_init(nbk_ffpkg_msgwin *msgwin)
{
	msgwin->stage = FF_MSGWIN_READY;
	msgwin->type = 0;
	msgwin->msg_length = 0;
	if (msgwin->msg)
		bd_string_free(&msgwin->msg);
}

static void nbk_ffpkg_msgwin_release(nbk_ffpkg_msgwin *msgwin)
{
	nbk_ffpkg_msgwin_init(msgwin);     
}

static CM_BOOL nbk_ffpkg_msgwin_parse(/*nbk_res_conn *res, */nbk_ffpkg_msgwin *win, bd_input_stream *rs)
{
	if (win->stage == FF_MSGWIN_READY)
	{
		win->stage = FF_MSGWIN_TYPE;
	}

	if (win->stage == FF_MSGWIN_TYPE)
	{
		S16 type = 0;
		if (CM_ERROR_SUCCESS != bd_is_read_short(rs, &type))
			return 0;

		win->type = type;
		win->stage = FF_MSGWIN_LENGTH;
	}

	if (win->stage == FF_MSGWIN_LENGTH)
	{
		S32 len = 0;
		if (CM_ERROR_SUCCESS != bd_is_read_int(rs, &len))
			return 0;

		win->msg_length = len;
		win->stage = FF_MSGWIN_MSG;
	}

	if (win->stage == FF_MSGWIN_MSG)
	{
		if (!bd_is_require_size(rs, win->msg_length))
			return 0;

		win->msg = bd_string_new(win->msg_length);
		bd_is_read_utf8(rs, win->msg, win->msg_length);

		win->stage = FF_MSGWIN_OVER;
	}

	if (win->stage == FF_MSGWIN_OVER)
		return 1;
    
    return 1;
}


enum _ff_inc_data_stage
{
	FF_INC_READY,
	FF_INC_URL_LEN,
	FF_INC_URL,
	FF_INC_CONTENT_LEN,
	FF_INC_CONTENT,
	FF_INC_END
};

enum _ff_op_stage
{
	FF_OP_READY,
	FF_OP_TYPE,
	FF_OP_XPATH_LEN,
	FF_OP_CONTENT_LEN,
	FF_OP_XPATH,
	FF_OP_CONTENT,
	FF_OP_END
};

enum _ff_op_type
{
	OP_DELETE = 0,
	OP_INSERT,
	OP_REPLACE
};

static void nbk_ffpkg_inc_data_parser_init(nbk_ffpkg_inc_data_parser *parser)
{
	if (parser->url)
	{
		bd_string_free(&parser->url);
	}

	bd_output_stream_init(&parser->ws);

	parser->url_len = 0;
	parser->content_len = 0;

	parser->stage = FF_INC_READY;
	parser->stage_op = FF_OP_READY;

	if (parser->op.xpath)
	{
		bd_string_free(&parser->op.xpath);
	}

    cm_memset(&parser->op, 0, sizeof(parser->op));
}

static void nbk_ffpkg_inc_data_parser_release(nbk_ffpkg_inc_data_parser *parser)
{
	if (parser->url)
	{
		bd_string_free(&parser->url);
	}

	if (parser->op.xpath)
	{
		bd_string_free(&parser->op.xpath);
	}

	bd_output_stream_destroy(&parser->ws);
}

static void nbk_ffpkg_inc_data_parse(nbk_connection_ptr conn, nbk_ffpkg_inc_data_parser *parser, S8 *data, S32 size)
{
	bd_output_stream_ptr ws = 0;
	bd_output_stream_ptr rs = 0;

	ws = &parser->ws;
	rs = &parser->rs;

	//init
	if (rs->buff && rs->pos < rs->size)
	{//last data left
		S32 len = rs->size - rs->pos;
		cm_memmove(ws->buff, &rs->buff[rs->pos], len);
		ws->pos = len;
	}
	else
		bd_output_stream_init(ws);

	bd_os_write(ws, data, size);
	bd_input_stream_init(rs, ws->buff, ws->pos);

	while (rs->pos < rs->size)
	{
		if (parser->stage == FF_INC_READY)
		{
			CM_TRACE_5("ready inc data parse..");
			parser->stage = FF_INC_URL_LEN;
		}

		if (parser->stage == FF_INC_URL_LEN)
		{
			S16 len = 0;
			if (CM_ERROR_SUCCESS != bd_is_read_short(rs, &len))
				break;

			parser->url_len = (U16)len;
			parser->stage = FF_INC_URL;

			CM_TRACE_5("FF_INC_URL_LEN: %u", len);
		}

		if (parser->stage == FF_INC_URL)
		{
			if (!bd_is_require_size(rs, parser->url_len))
				break;

			if (parser->url)
				bd_string_free(&parser->url);

			parser->url = bd_string_new(parser->url_len);
			if (!parser->url)
				break;
			//bd_strncpy(parser->url, &rs->buff[rs->pos], parser->url_len);
			bd_is_read_utf8(rs, parser->url, parser->url_len);

			//nbk_res_on_package(res, NEREST_INC_URL, parser->url, parser->url_len, parser->url_len);
			nbk_connection_on_package(conn, NEREST_INC_URL, parser->url, parser->url_len, parser->url_len);

			parser->stage = FF_INC_CONTENT_LEN;

			CM_TRACE_5("FF_INC_URL: %s", parser->url);
		}
		
		if (parser->stage == FF_INC_CONTENT_LEN)
		{
			S32 len = 0;
			if (CM_ERROR_SUCCESS != bd_is_read_int(rs, &len))
				break;

			parser->require_len = parser->content_len = len;
			parser->stage = FF_INC_CONTENT;

			CM_TRACE_5("FF_INC_CONTENT_LEN: %u", len);
		}

		if (parser->stage == FF_INC_CONTENT)
		{
			if (parser->stage_op == FF_OP_READY)
			{
				CM_TRACE_5("ready op parse...");
				parser->stage_op = FF_OP_TYPE;
			}

			if (parser->stage_op == FF_OP_TYPE)
			{
				S16 type = 0;
				if (CM_ERROR_SUCCESS != bd_is_read_short(rs, &type))
					break;

				parser->op.type = type;

				parser->stage_op = FF_OP_XPATH_LEN;

				CM_TRACE_5("FF_OP_TYPE: %u", type);
			}

			if (parser->stage_op == FF_OP_XPATH_LEN)
			{
				S16 len = 0;
				if (CM_ERROR_SUCCESS != bd_is_read_short(rs, &len))
					break;

				parser->op.xpath_len = (U16)len;

				if (parser->op.type == OP_DELETE)
					parser->stage_op = FF_OP_XPATH;
				else
					parser->stage_op = FF_OP_XPATH;

				CM_TRACE_5("FF_OP_XPATH_LEN: %u", len);
			}

			if (parser->stage_op == FF_OP_CONTENT_LEN)
			{
				S32 len = 0;
				if (CM_ERROR_SUCCESS != bd_is_read_int(rs, &len))
					break;

				parser->op.require_len = parser->op.content_len = len;
				parser->stage_op = FF_OP_CONTENT;

				CM_TRACE_5("FF_OP_CONTENT_LEN: %u", len);
			}

			if (parser->stage_op == FF_OP_XPATH)
			{
				NEResponseType respT = NEREST_INC_OP_REPLACE;
				if (!bd_is_require_size(rs, parser->op.xpath_len))
					break;

				if (parser->op.xpath)
					bd_string_free(&parser->op.xpath);

				parser->op.xpath = bd_string_new(parser->op.xpath_len);
				bd_is_read_utf8(rs, parser->op.xpath, parser->op.xpath_len);

				if (parser->op.type == OP_DELETE)
				{
					respT = NEREST_INC_OP_DELETE;
					parser->stage_op = FF_OP_END;
				}
				else
				{
					if (parser->op.type == OP_INSERT)
						respT = NEREST_INC_OP_INSERT;
					else
						respT = NEREST_INC_OP_REPLACE;

					parser->stage_op = FF_OP_CONTENT_LEN;
				}

				//nbk_res_on_package(res, respT, parser->op.xpath, parser->op.xpath_len, parser->op.xpath_len);
				nbk_connection_on_package(conn, respT, parser->op.xpath, parser->op.xpath_len, parser->op.xpath_len);

				CM_TRACE_5("FF_OP_XPATH: %s", parser->op.xpath);
			}

			if (parser->stage_op == FF_OP_CONTENT)
			{
				S32 len = MIN(parser->op.require_len, rs->size - rs->pos);

				parser->op.require_len -= len;

				if (parser->op.require_len <= 0)
				{//finish
					parser->stage_op = FF_OP_END;
				}

				//nbk_res_on_package(res, NEREST_INC_DATA, &rs->buff[rs->pos], len, len);
				nbk_connection_on_package(conn, NEREST_INC_DATA, &rs->buff[rs->pos], len, len);

				bd_is_skip(rs, len);
			}

			if (parser->stage_op == FF_OP_END)
			{
				CM_TRACE_5("Parse op block ok");

				//nbk_res_on_package(res, NEREST_INC_DATA_COMPLETE, 0, 0, 0);
				nbk_connection_on_package(conn, NEREST_INC_DATA_COMPLETE, 0, 0, 0);

				parser->require_len -= (2+2+4+parser->op.xpath_len+parser->op.content_len);
				
				if (parser->require_len <= 0)
				{
					parser->stage = FF_INC_END;
					CM_TRACE_5("all op over..");
				}
				else
				{
					parser->stage_op = FF_OP_READY;
					CM_TRACE_5("next op..");
				}
			}
		}

		if (parser->stage == FF_INC_END)
		{
			CM_TRACE_5("parse inc data ok");
		}
	}
}

///////////////////////////////////////////////////////
//packimage parser
enum _ff_packimg_parse_stage
{
	FF_PPS_READY,           //0
	FF_PPS_FILE_NAME_LEN,   //1
	FF_PPS_FILE_NAME,       //2
	FF_PPS_CTRL_LEN,        //3
	FF_PPS_CTRL,            //4
	FF_PPS_CONTENT_LEN,     //5
	FF_PPS_CONTENT,         //6
	FF_PPS_END              //7
};


static NRequest* nbk_create_fake_image_request(NRequest *req, S8 *url, const S8 *str_clip, const S8 *refer_url)
{
	S8 m1[] = "./webkit-image/";
	S8 m2[] = "./webkit-bg-image/url(";

    NBK_Request* fakeReq = 0;
    S32 len = 0;
    S8 *p = 0;
    S8 *imgurl = 0;

    //TODO
    CM_ASSERT(0);

	imgurl = bd_string_new(cm_strlen(url) + 4);

    url_str_to_hex(imgurl, url);
    p = imgurl;
    len = cm_strlen(m1);
    if (cm_strncmp(imgurl, m1, len) == 0)
        p += len;
    else 
	{
        len = cm_strlen(m2);
        if (cm_strncmp(imgurl, m2, len) == 0)
		{
            S8* q;
            p += len;
            q = p;
            while (*q && *q != ')')
                q++;
            *q = 0;
        }
    }
    	//bd_lock(&mutex);
	CM_TRACE_5("conn request =  %s ",p);
	fakeReq = imagePlayer_getFakeRequestForFF(req, p,str_clip, refer_url);
	//bd_unlock(&mutex);
    
    bd_string_free(&imgurl);
    
    return fakeReq;
}

static void nbk_req_on_package_image(NBK_Request *req, NEResponseType type, void *data, S32 len)
{
	if (!req)
		return;

	switch(type)
	{
	case NEREST_HEADER:
		loader_onReceiveHeader(req, (NRespHeader*)data);
		break;
	case NEREST_DATA:
		loader_onReceiveData(req, (S8*)data, len, len);
		break;
	case NEREST_COMPLETE:
		loader_onComplete(req);
		break;
	}
}

static void nbk_ffpkg_packimg_parser_init(nbk_ffpkg_packimg_parser *parser)
{
	if (parser->url)
	{
		bd_string_free(&parser->url);
	}

	if (parser->fake_req)
	{
		loader_deleteRequest(&parser->fake_req);
	}

	bd_output_stream_init(&parser->ws);
	bd_output_stream_init(&parser->strm_data);

	parser->url_len = parser->content_len = parser->ctrl_len = parser->require_len = 0;
	parser->stage = FF_PPS_READY;
}

static void nbk_ffpkg_packimg_parser_release(nbk_ffpkg_packimg_parser *parser)
{
	if (parser->url)
	{
		bd_string_free(&parser->url);
	}

	if (parser->fake_req)
	{
		loader_deleteRequest(&parser->fake_req);
	}

	bd_output_stream_destroy(&parser->ws);
	bd_output_stream_destroy(&parser->strm_data);
}

//add for image clip
static void parse_image_ctl_clip(S8* data, S32 len, NImageClipInfo* ci)
{
	S8* p = data;
    //S8* v = 0;
    S32 nCnt = 0, start = 0, value;
    
    TClipSetp step = TXRepeat;
    
    while (len && nCnt <= len) {
        if (*(p + nCnt) == '_' || nCnt == len) {
            
//            v = p + start + (nCnt - start);
//            *v = 0;
            value = NBK_atoi(p + start);
            //*v = '_';
            
            switch (step)
            {
            case TXRepeat:
                ci->x_repeat = value;
                break;
            case TYRepeat:
                ci->y_repeat = value;
                break;
            case TXIsPercent:
                ci->x_t = value;
                break;
            case TYIsPercent:
                ci->y_t = value;
                break;
            case TX:
                ci->x = value;
                break;
            case TY:
                ci->y = value;
                break;
            case TElemWidth:
                ci->w = value;
                break;
            case TElemHeight:
                ci->h = value;
                break;
            }                
            step = (TClipSetp) (step + 1);
            start = nCnt + 1;
        }
        nCnt++;
    }
}

static void parse_image_ctl_bgpos(S8* data, S32 len, NImageClipInfo* ci)
{
    S8* p = data;
    //S8* v;
    S32 nCnt = 0, start = 0, value;
    
    TBgPosSetp step1 = TPosX;
    
    while (len && nCnt <= len) {
        if (*(p + nCnt) == ',' || nCnt == len) {
            
//            v = p + start + (nCnt - start);
//            *v = 0;
            value = NBK_atoi(p + start);
            //*v = ',';

            switch (step1) {
            case TPosX:
                ci->new_x = value;
                break;
            case TPosY:
                ci->new_y = value;
                break;
            }
            step1 = (TBgPosSetp) (step1 + 1);
            start = nCnt + 1;
        }
        nCnt++;
    }
}

static void ParseImageExCtlInfo(void* data, S32 len, S8 * referUrl, NImageClipInfo* clipInfo)
{
	enum TFoundRet {
        FOUND_NOT,
        FOUND_DATE,
        FOUND_EXPIRE,
        FOUND_AGE,
        FOUND_CACHECONTROL,
        FOUND_CLIP,
        FOUND_BGPOS,
        FOUND_PIMAGE
    }; 
    const S8* toofar = (S8*)data + len;
    S8* p = (S8*)data;
    S8* q;
    S32 stage = FOUND_NOT;
	//while(data){
		//data++;
	//}
	if(len == 0)
		return; 
    while (p < toofar) {
        
        q = p;
        while (*p != ':' && p < toofar)
            p++;
        
        if (*p == ':') {
            if (NBK_strncmp(q, "date", p - q) == 0)
                stage = FOUND_DATE;
            else if (NBK_strncmp(q, "expire", p - q) == 0)
                stage = FOUND_EXPIRE;
            else if (NBK_strncmp(q, "age", p - q) == 0)
                stage = FOUND_AGE;
            else if (NBK_strncmp(q, "cache-control", p - q) == 0)
                stage = FOUND_CACHECONTROL;
            else if (NBK_strncmp(q, "clip", p - q) == 0)
                stage = FOUND_CLIP;
            else if (NBK_strncmp(q, "bgposition", p - q) == 0)
                stage = FOUND_BGPOS;
            else if (NBK_strncmp(q, "pimg", p - q) == 0)
                stage = FOUND_PIMAGE;
        }
        else
            break;
        
        q = ++p;
        while (*p != '\r' && *p != ';' && p < toofar)
            p++;
        
        if (*p == '\r' || *p == ';') {
            if (stage == FOUND_DATE) {
                //create.Set((S8 *)q, p - q);
            }
            else if (stage == FOUND_EXPIRE) {
				
                //expire.Set((S8 *)q, p - q);
            }
            else if (stage == FOUND_AGE) {
            }
            else if (stage == FOUND_PIMAGE) {
				//unfinished
				cm_strncpy(referUrl,q,p-q);
            }
            else if (stage == FOUND_CACHECONTROL) {                
                //iCache =;
            }
            else if (stage == FOUND_CLIP) {
                parse_image_ctl_clip(q, p - q, clipInfo);
            }
            else if (stage == FOUND_BGPOS) {
                parse_image_ctl_bgpos(q, p - q, clipInfo);
            }
        }
        else
            break;
        
        if (*p == '\r')
            p += 2; // skip \r\n
        else
            p++;
    }
}

static void nbk_ffpkg_packimg_parse(nbk_connection_ptr conn, nbk_ffpkg_packimg_parser *parser, S8 *data, S32 size)
{
	bd_output_stream_ptr ws = 0;
	bd_output_stream_ptr rs = 0;
	//NRespHeader header;
    
	ws = &parser->ws;
	rs = &parser->rs;

	//init
	if (rs->buff && rs->pos < rs->size)
	{//last data left
		S32 len = rs->size - rs->pos;
		cm_memmove(ws->buff, &rs->buff[rs->pos], len);
		ws->pos = len;
	}
	else
		bd_output_stream_init(ws);

	bd_os_write(ws, data, size);
	bd_input_stream_init(rs, ws->buff, ws->pos);

	while (rs->pos < rs->size)
	{
		if (parser->stage == FF_PPS_READY)
		{
			parser->stage = FF_PPS_FILE_NAME_LEN;
		}

		if (parser->stage == FF_PPS_FILE_NAME_LEN)
		{
			S16 len = 0;
			if (bd_is_read_short(rs, &len) != CM_ERROR_SUCCESS)
				break;

			parser->url_len = (U16)len;
			parser->stage = FF_PPS_FILE_NAME;
			CM_TRACE_5("url_len: %d", parser->url_len);
		}

		if (parser->stage == FF_PPS_FILE_NAME)
		{
			CM_TRACE_5("stage file name");
			CM_TRACE_5("left size: %d", rs->size - rs->pos);
			if (!bd_is_require_size(rs, parser->url_len))
				break;
			CM_TRACE_5("stage file name go");
			parser->url = bd_string_new(parser->url_len);
			bd_is_read_utf8(rs, parser->url, parser->url_len);

			CM_TRACE_5("file name: %s", parser->url);

			//parser->fake_req = nbk_res_get_fake_image_request(res, parser->url);
			//parser->require_len = parser->ctrl_len;
			parser->stage = FF_PPS_CTRL_LEN;
		}
		
		if (parser->stage == FF_PPS_CTRL_LEN)
		{
			S16 len = 0;
			if (bd_is_read_short(rs, &len) != CM_ERROR_SUCCESS)
				break;

			//sometimes this 'len' will be 0, but it doesn't matter
			//the next 'FF_PPS_CTRL' will process it
			parser->ctrl_len = (U16)len;
			parser->stage = FF_PPS_CTRL;
			parser->require_len = parser->ctrl_len;
			CM_TRACE_5("ctrl_len: %d", parser->ctrl_len);
		}

		if (parser->stage == FF_PPS_CTRL)
		{
			S32 len = MIN(parser->require_len, rs->size - rs->pos);
			//if (iData == 0) {
			//	iData = (uint8*)NBK_malloc(parser->ctlLen + 1);
			//}

			//bd_os_write(&parser->strm_data, &rs->buff[rs->pos], len);
			parser->require_len -= len;

			if (parser->require_len <= 0) 
			{//finish
				//write cache
				//next stage init
				NImageClipInfo clipInfo;
				S8 *referUrl = 0;
				NRespHeader header;
                
				referUrl = bd_string_new(128);
                //rs->buff[rs->pos] = 0;
				//bd_output_stream_init(&parser->strm_data);
				//sys
				//NBK_memcpy(iData + iDataPos, iCurPos, len);
				//iDataPos += iDataLeave;
				//iData[iDataPos] = 0;
				//end
                NBK_memset(&clipInfo,0,sizeof(NImageClipInfo));
                ParseImageExCtlInfo(&rs->buff[rs->pos], parser->ctrl_len, referUrl,&clipInfo);
				//end 
				//parser->fake_req = nbk_res_get_fake_image_request(res, parser->url);

				header.mime = NEMIME_FF_IMAGE;
				header.content_length = parser->content_len;
				header.user = 0;
				//add
				if (clipInfo.w && clipInfo.h) {
					S8 clipStr[36];
	                header.user = &clipInfo;
	                //clipBgPos.SetXY(clipInfo.new_x, clipInfo.new_y);              
	                cm_sprintf(clipStr, "_%d_%d_%d_%d_%d_%d_%d_%d", clipInfo.x_repeat,
	                    clipInfo.y_repeat, clipInfo.x_t, clipInfo.y_t, clipInfo.x, clipInfo.y,
	                    clipInfo.w, clipInfo.h);
					parser->fake_req = nbk_create_fake_image_request(conn->request, parser->url,(const S8*) clipStr, referUrl);					
				//parser->fake_req = nbk_res_get_fake_image_request(res, parser->url,(const S8*) clipStr, refer);
				}else {
            		parser->fake_req = nbk_create_fake_image_request(conn->request, parser->url,0,0);
					//parser->fake_req = nbk_res_get_fake_image_request(res, parser->url,0,0);
				}
				bd_string_free(&referUrl);
	            if (parser->fake_req) {
	                nbk_req_on_package_image(parser->fake_req, NEREST_HEADER, &header, 0);
	            }
				//end
				CM_TRACE_5("header : %d,%d", &header,header.content_length);
				//nbk_res_on_package_image(parser->fake_req, NEREST_HEADER, &header, 0);
				//nbk_connection_on_package_image(parser->fake_req, NEREST_HEADER, &header, 0);
				//nbk_req_on_package_image(parser->fake_req, NEREST_HEADER, &header, 0);
			//if (parser->fake_req->via == NEREV_FF_MULTIPICS&&!cm_strlen(referUrl)) 
                    //iCacheIndex = CFsCache::GetPtr()->SaveHeader(url, cr, ex, clipBgPos,CFsCache::NECT_IMAGE);
				//parser->require_len = parser->content_len;
				//nbk_res_on_package_image(parser->fake_req, (parser->content_len)?NEREST_COMPLETE:NEREST_ERROR, 0, 0);			
				parser->stage = FF_PPS_CONTENT_LEN;
			}
			bd_is_skip(rs, len);
		}

		if (parser->stage == FF_PPS_CONTENT_LEN)
		{
			if (bd_is_read_int(rs, &parser->content_len) != CM_ERROR_SUCCESS)
				break;

			CM_TRACE_5("content_len: %d", parser->content_len);
			parser->require_len = parser->content_len;
			parser->stage = FF_PPS_CONTENT;
		}
		
		if (parser->stage == FF_PPS_CONTENT)
		{
			S32 len = MIN(parser->require_len, rs->size - rs->pos);

			parser->require_len -= len;

			//nbk_res_on_package_image(parser->fake_req, NEREST_DATA, &rs->buff[rs->pos], len);
			nbk_req_on_package_image(parser->fake_req, NEREST_DATA, &rs->buff[rs->pos], len);

			if (parser->require_len <= 0)
			{//finish
				//nbk_res_on_package_image(parser->fake_req, (parser->content_len)?NEREST_COMPLETE:NEREST_ERROR, 0, 0);
				nbk_req_on_package_image(parser->fake_req, (parser->content_len)?NEREST_COMPLETE:NEREST_ERROR, 0, 0);

				parser->stage = FF_PPS_END;
			}
			bd_is_skip(rs, len);
		}
		
		if (parser->stage == FF_PPS_END)
		{
			CM_TRACE_5("ffpkg image ok: rs->pos[%d], rs->size[%d]", rs->pos, rs->size);
			parser->stage = FF_PPS_READY;

			if (rs->pos != rs->size)
			{
				//bd_warning("Image data error!!!");
				CM_ASSERT(0);
			}

			CM_ASSERT(rs->pos == rs->size);
		}
	}
}


///////////////////////////////////////////////////////


static void nbk_html_content_handle(nbk_ffpkg_parser *parser, S8 *data, S32 len, CM_BOOL finish)
{
	S8 *dest = 0;
	S32 dest_len = 0;

    cm_fwrite(data, len, 1, parser->fs_cache);
    
	dest_len = cm_zip_decompress(parser->zstrm, data, len, &dest);

#ifdef _BD_DEBUG_
	//bd_fs_write(&http_conn->file_dest, dest, dest_len);
#endif
	//nbk_res_on_receive_data(parser->res, (S8*)dest, dest_len, dest_len);

	//nbk_res_on_package(parser->res, NEREST_PKG_DATA, (void*)dest, dest_len, len);
	nbk_connection_on_package(parser->conn, NEREST_PKG_DATA, (void*)dest, dest_len, len);

	if (finish)
	{
		//nbk_res_on_package(parser->res, NEREST_PKG_DATA_COMPLETE, 0, 0, 0);
		nbk_connection_on_package(parser->conn, NEREST_PKG_DATA_COMPLETE, 0, 0, 0);
	}

	CM_FREE(dest);
	return;
}

static void nbk_param_content_handle(nbk_ffpkg_parser *parser, S8 *data, S32 len)
{
	S8 **kvs = 0;

	S32 count = 0;
	S32 i = 0;
	S32 ki = -1;
	S32 vi = -1;

	//get count
	for (i = 0; i < len; i++)
	{
		if ((S8)data[i] == '\r')
			count++;
	}

	if (count <= 0)
		return;

	kvs = (S8 **)CM_MALLOC(sizeof(S8 *) * (count * 2 + 1));
	if (0 == kvs)
		return;

	count = 0;

	for (i = 0; i < len; i++)
	{
		S8 c = (S8)data[i];

		if (ki == -1)
			ki = i;
		if (vi == -1)
			vi = i;

		if (c == '=')
		{
			data[i] = 0;
			kvs[count++] = &data[ki];

			vi = -1;
			continue;
		}

		if (c == '\r')
		{
			if (data[vi] == '\r')
			{//no value
				count--;
				kvs[count] = 0;
			}
			else
			{
				kvs[count++] = &data[vi];
			}

			ki = vi = -1;
			data[i] = 0;
			continue;
		}

		if (c == '\n')
		{
			ki = vi = -1;
			data[i] = 0;
			continue;
		}
	}

	kvs[count] = 0;

	//nbk_res_on_package(parser->res, NEREST_PARAM, (void*)kvs, 0, 0);
	nbk_connection_on_package(parser->conn, NEREST_PARAM, (void*)kvs, 0, 0);

	CM_FREE(kvs);
}

nbk_ffpkg_parser* nbk_ffpkg_parser_new(nbk_connection_ptr conn)
{
	nbk_ffpkg_parser *parser = 0;

	parser = (nbk_ffpkg_parser *)CM_MALLOC(sizeof(*parser));

	if (!parser)
		return 0;
    else
    {
        cm_memset(parser, 0, sizeof(nbk_ffpkg_parser));
    }

	if (bd_output_stream_init(&parser->ws) != CM_ERROR_SUCCESS)
	{
		CM_FREE(parser);
		return 0;
	}

	parser->conn = conn;
	parser->type = FF_NONE;

	return parser;
}

void nbk_ffpkg_parser_free(nbk_ffpkg_parser *parser)
{
	if (!parser)
		return;

	bd_output_stream_destroy(&parser->ws);

	if (parser->zstrm != 0)
	{
	    cm_zip_destory(parser->zstrm);
	}

	nbk_ffpkg_packimg_parser_release(&parser->packimg_parser);
	nbk_ffpkg_inc_data_parser_release(&parser->inc_data_parser);

	CM_FREE(parser);
}

void nbk_ffpkg_parse(nbk_ffpkg_parser *parser, S8 *data, S32 size)
{
	bd_output_stream_ptr ws = 0;
	bd_output_stream_ptr rs = 0;
	NRespHeader header;

	S16 type = 0;

	if (0 == parser)
		return;

	ws = &parser->ws;
	rs = &parser->rs;

	//init
	if (rs->buff && rs->pos < rs->size)
	{//last data left
		S32 len = rs->size - rs->pos;
		/*S8* buf = rs->buff;
        if (parser->res && parser->res->request) {
            if (parser->res->request->via != NEREV_FF_MULTIPICS) {
            	CM_TRACE_5("request != NEREV_FF_MULTIPICS");
                len -= 4;
                buf += 4;
            }
        }*/
		cm_memmove(ws->buff, &rs->buff[rs->pos], len);
		ws->pos = len;
	}
	else
		bd_output_stream_init(ws);


	bd_os_write(ws, data, size);
	bd_input_stream_init(rs, ws->buff, ws->pos);

	while (rs->pos < rs->size)
	{
		if (parser->stage == FF_PRS_STG_TYPE)
		{
			//read type
			if (bd_is_read_short(rs, (S16*)&type) != CM_ERROR_SUCCESS)
			{//not enough
				break;
			}

			parser->type = type;
			parser->data_no[parser->type]++;
			//parser->res->request->idx = parser->data_no[parser->type];
			parser->conn->request->idx = parser->data_no[parser->type];
			parser->stage = FF_PRS_STG_LENGTH;

			CM_TRACE_5("FF_PRS_STG_TYPE: type[%d]", parser->type);
		}

		if (parser->stage == FF_PRS_STG_LENGTH)
		{
			S32 length = 0;
			if (bd_is_read_int(rs, &length) != CM_ERROR_SUCCESS)
				break;

			parser->require_len = parser->content_len = length;
			parser->stage = FF_PRS_STG_CONTENT_READY;
			CM_TRACE_5("FF_PRS_STG_LENGTH: require length[%d]", length);
		}

		if (parser->stage == FF_PRS_STG_CONTENT_READY)
		{
			S8 path[128] = {0};

			//init something
			switch(parser->type)
			{
			case FF_HTML:
			case FF_HTML_NEW:
				{
					CM_TRACE_5("html pkg init now");
					header.mime = NEMIME_FF_XML;
					header.content_length = parser->content_len;///header.content_length = len?
					//nbk_res_on_package(parser->res, NEREST_PKG_DATA_HEADER, &header, 0, 0);
					nbk_connection_on_package(parser->conn, NEREST_PKG_DATA_HEADER, &header, 0, 0);

					CM_TRACE_5("res on package ok");
					parser->zstrm = cm_zip_create(CM_ZIP_GZIP);
					CM_TRACE_5("html pkg init ok");
				}
				break;

			case FF_IMG:
			case FF_BGIMG:
			case FF_PACKIMG:
				{
					nbk_ffpkg_packimg_parser_init(&parser->packimg_parser);
				}
				break;
			case FF_INC_DATA:
				{
					nbk_ffpkg_inc_data_parser_init(&parser->inc_data_parser);
					parser->zstrm = cm_zip_create(CM_ZIP_GZIP);
				}
				break;

			case FF_MSGWIN:
				{
					//nbk_ffpkg_msgwin_init(&parser->msg_win);
				}
				break;

			default:
				break;
			}

			nbk_get_cache_file_name(parser->type, parser->data_no[parser->type], path);

            parser->fs_cache = cm_fopen(path, "a+");
			parser->stage = FF_PRS_STG_CONTENT;

			CM_TRACE_5("FF_PRS_STG_CONTENT: type[%d]", parser->type);
		}
		//CM_TRACE_5("parser->stage = %d,%d",parser->stage, MIN(parser->require_len, rs->size - rs->pos));
		//CM_TRACE_5("parser->stage1 = %d,%d",parser->type, parser->require_len);
		if (parser->stage == FF_PRS_STG_CONTENT)
		{
			switch(parser->type)
			{
			case FF_HTML:
			case FF_HTML_NEW:
				{
					if (parser->require_len > 0)
					{
						CM_BOOL finish = 0;
						//S32 len = rs->size - rs->pos;
						S32 len = MIN(parser->require_len, rs->size - rs->pos);
						parser->require_len -= len;

						if (parser->require_len <= 0)
							finish = 1;

						nbk_html_content_handle(parser, &rs->buff[rs->pos], len, finish);

						bd_is_skip(rs, len);
						CM_TRACE_5("FF_HTML content: status[%d]", finish);
						CM_TRACE_5("FF_HTML content: parser->request_len[%d]", parser->require_len);

						if (finish)
						{
							parser->stage = FF_PRS_STG_END;
						}
					}
					else
					{
						parser->stage = FF_PRS_STG_END;
					}
				}
				break;

			case FF_IMG:
			case FF_BGIMG:
			case FF_PACKIMG:
				{
					S32 len = MIN(parser->require_len, rs->size - rs->pos);
					parser->require_len -= len;

                    cm_fwrite(&rs->buff[rs->pos], len, 1, parser->fs_cache);
					nbk_ffpkg_packimg_parse(parser->conn, &parser->packimg_parser, &rs->buff[rs->pos], len);

					bd_is_skip(rs, len);

					if (parser->require_len <= 0)
					{
						CM_TRACE_5("parse image end: %d", parser->require_len);
						parser->stage = FF_PRS_STG_END;
					}
				}
				break;

			case FF_INC_DATA:
				{
					if (parser->require_len > 0)
					{
						S8 *dest = 0;
						S32 dest_len = 0;
						CM_BOOL finish = 0;
						//S32 len = rs->size - rs->pos;
						S32 len = MIN(parser->require_len, rs->size - rs->pos);
						parser->require_len -= len;

						if (parser->require_len <= 0)
							finish = 1;

						cm_fwrite(data, len, 1, parser->fs_cache);
						dest_len = cm_zip_decompress(parser->zstrm, &rs->buff[rs->pos], len, &dest);
						
						//nbk_ffpkg_inc_data_parse(parser->res, &parser->inc_data_parser, dest, dest_len);
						nbk_ffpkg_inc_data_parse(parser->conn, &parser->inc_data_parser, dest, dest_len);

						bd_is_skip(rs, len);
						CM_FREE(dest);

						if (finish)
						{
							parser->stage = FF_PRS_STG_END;
						}
					}
					else
					{
						parser->stage = FF_PRS_STG_END;
					}
				}
				break;
			
			case FF_COOKIE:
			default:
				{
					CM_BOOL finish = 0;
					//S32 len = rs->size - rs->pos;
					S32 len = MIN(parser->require_len, rs->size - rs->pos);
					parser->require_len -= len;

					if (parser->require_len <= 0)
						finish = 1;

                    cm_fwrite(&rs->buff[rs->pos], len, 1, parser->fs_cache);

					bd_is_skip(rs, len);

					if (parser->require_len == 0)
					{
						parser->stage = FF_PRS_STG_END;
					}
				}
				break;
			}
		}

		if (parser->stage == FF_PRS_STG_END)
		{
			//close file
			cm_fclose(parser->fs_cache);
			parser->type = FF_NONE;
			parser->stage = FF_PRS_STG_TYPE;
		}
	}
}

void nbk_get_cache_file_name(ff_data_type type, S32 no, S8 *file_name)
{
    S32 value;
    S8 fname[CM_MAX_PATH];
    
	S8 type_str[32] = {0};

    value = cm_facility_get_work_dir(file_name);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(file_name, "cache");
    CM_ASSERT(value >= 0);

	switch(type)
	{
	case FF_HTML:
	case FF_HTML_NEW:
		cm_strcpy(type_str, "html");
	break;

	case FF_IMG:
		cm_strcpy(type_str, "img");
	break;

	case FF_BGIMG:
		cm_strcpy(type_str, "bgimg");
	break;

	case FF_PACKIMG:
		cm_strcpy(type_str, "packimg");
	break;

	case FF_COOKIE:
		cm_strcpy(type_str, "cookie");
	break;

	case FF_INC_DATA:
		cm_strcpy(type_str, "incdata");
	break;

	case FF_PARAM:
		cm_strcpy(type_str, "param");
    break;
    
	default:
		cm_strcpy(type_str, "unknown[%d]");
	break;
	}

    cm_sprintf(fname, "%s_%d.bin", type_str, no);

    value = cm_facility_add_sub_dir(file_name, fname);
    CM_ASSERT(value >= 0);
}

