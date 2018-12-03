
#ifndef _NBK_FFPKG_PARSER_H
#define _NBK_FFPKG_PARSER_H


#include "nbk_connection.h"
#include "loader/loader.h"
#include "nbk_io_stream.h"
#include "cm_io.h"
#include "cm_zip_stream.h"


#define DATA_TYPE_COUNT	16


typedef enum _ff_parse_stage
{
	FF_PRS_STG_TYPE,
	FF_PRS_STG_LENGTH,
	FF_PRS_STG_CONTENT_READY,
	FF_PRS_STG_CONTENT,
	FF_PRS_STG_END

}ff_parse_stage;

typedef enum _ff_data_type
{
	FF_NONE		= -1,
	FF_HTML,        //0
	FF_IMG,         //1
	FF_BGIMG,       //2
	FF_COOKIE,      //3
	FF_INC_DATA,    //4
	FF_PARAM,       //5
	FF_PACKIMG,     //6
	FF_MSGWIN,      //7
	FF_HTML_NEW     //8

}ff_data_type;

typedef struct _nbk_ffpkg_msgwin nbk_ffpkg_msgwin;
typedef struct _nbk_ffpkg_packimg_parser    nbk_ffpkg_packimg_parser;
typedef struct _nbk_ffpkg_inc_data_parser   nbk_ffpkg_inc_data_parser;

struct _nbk_ffpkg_msgwin
{
	S32 stage;
	S16 type;
	S32 msg_length;
	S8 *msg;
};

struct _nbk_ffpkg_packimg_parser
{
	U16 url_len;
	U16 ctrl_len;
	S32 content_len;

	S8 *url;
	bd_output_stream strm_data;

	S32 require_len;
	S32 stage;

	bd_input_stream rs;
	bd_output_stream ws;

	S32 parse_type;
	NBK_Request *fake_req;
};

typedef struct _inc_data_op
{
	U16 type;
	U16 xpath_len;
	S32 content_len;	// while type == 1, 2

	S8 *xpath;

	S32 require_len;
	//content? while type == 1, 2

}inc_data_op;

struct _nbk_ffpkg_inc_data_parser
{
	U16 url_len;
	U16 content_len;

	S8 *url;

	S32 require_len;

	S32 stage;

	inc_data_op op;
	S32 stage_op;

	bd_input_stream rs;
	bd_output_stream ws;
};

typedef struct _nbk_ffpkg_parser
{
	bd_output_stream ws;	//stream data
	bd_input_stream rs;		//stream parse

	ff_parse_stage stage;	//parse stage
	ff_data_type type;	    //data type	

	S32 require_len;	    //require length of last block data
	S32 content_len;

	S32 data_no[DATA_TYPE_COUNT];

	nbk_ffpkg_packimg_parser packimg_parser;
	nbk_ffpkg_inc_data_parser inc_data_parser;

	nbk_ffpkg_msgwin msg_win;

	CM_HANDLE fs_cache;
	void *zstrm;
	
	nbk_connection_ptr conn;

}nbk_ffpkg_parser;


#ifdef __cplusplus
extern "C"
{
#endif

nbk_ffpkg_parser* nbk_ffpkg_parser_new(nbk_connection_ptr conn);
void nbk_ffpkg_parser_free(nbk_ffpkg_parser *parser);
void nbk_ffpkg_parse(nbk_ffpkg_parser *parser, S8 *data, S32 len);

//work_dir/cache/type_num.ext
void nbk_get_cache_file_name(ff_data_type type, S32 no, S8 *file_name);

#ifdef __cplusplus
}
#endif


#endif	//_NBK_FFPKG_PARSER_H

