
#ifndef _NBK_SIM_CONNECTION_H
#define _NBK_SIM_CONNECTION_H


#include "cm_data_type.h"
#include "cm_time.h"
#include "cm_io.h"
#include "cm_zip_stream.h"
#include "nbk_connection.h"
#include "nbk_ffpkg_parser.h"


typedef enum _nbk_sim_type
{
	SIM_HISTORY_CACHE = 1,
	SIM_FILE_CACHE,
	SIM_NO_CACHE_ERROR

}nbk_sim_type;

typedef struct _nbk_sim_connection
{
	S32 phase;

	void *timer;

	CM_HANDLE file_strm;
	void *zip_strm;

	nbk_ffpkg_parser *ff_parser;

	S32 type;
	S8 *url;
	S32 data_type;
	S32 fscache_index;

	S8 buff[4096];

}nbk_sim_connection, *nbk_sim_connection_ptr;


#ifdef __cplusplus
extern "C"
{
#endif

nbk_connection_ptr nbk_sim_connection_new(nbk_web_view_ptr web_view, NRequest *req, S32 type);
void nbk_sim_connection_set_fscache_index(nbk_connection_ptr thiz, S32 index);

#ifdef __cplusplus
}
#endif


#endif //_NBK_SIM_CONNECTION_H

