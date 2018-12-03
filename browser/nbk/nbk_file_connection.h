
#ifndef _NBK_FILE_CONNECTION_H
#define _NBK_FILE_CONNECTION_H


#include "nbk_connection.h"
#include "cm_facility.h"
#include "cm_io.h"


#define BUFFER_SIZE		1024

typedef struct _nbk_file_connection
{
	S32 phase;

	void *timer;

	CM_HANDLE fp;
	S8 buffer[BUFFER_SIZE];

}nbk_file_connection, *nbk_file_connection_ptr;


#ifdef __cplusplus
extern "C"
{
#endif

nbk_connection_ptr nbk_file_connection_new(nbk_web_view_ptr web_view, NRequest *req);

#ifdef __cplusplus
}
#endif


#endif	//_NBK_FILE_CONNECTION_H

