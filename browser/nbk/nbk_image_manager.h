
#ifndef _NBK_IMAGE_MANAGER_H
#define _NBK_IMAGE_MANAGER_H


#include "cm_gdi.h"
#include "cm_facility.h"
#include "nbk_core.h"


typedef struct _nbk_cache_image
{
	S8 file_name[CM_MAX_PATH];

	NImage *nbk_image;
	CM_IMAGE *image_render;

}nbk_cache_image;


#ifdef __cplusplus
extern "C"
{
#endif

nbk_cache_image* nbk_cache_image_new(NImage *img);
void nbk_cache_image_free(nbk_cache_image *thiz);

#ifdef __cplusplus
}
#endif


#endif	//_NBK_IMAGE_MANAGER_H

