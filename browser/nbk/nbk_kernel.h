
#ifndef _NBK_KERNEL_H
#define _NBK_KERNEL_H


#include "cm_time.h"
#include "cm_gdi.h"
#include "cm_facility.h"
#include "bd_list.h"
#include "nbk_core.h"
#include "nbk_fscache.h"
#include "nbk_cookie_manager.h"


typedef struct _nbk_kernel nbk_kernel, *nbk_kernel_ptr;
typedef struct _nbk_wap_white_list nbk_wap_white_list, *nbk_wap_white_list_ptr;

struct _nbk_wap_white_list
{
	CM_BOOL updated;//0: not update 1: update
	S32 version;
	CM_DATE_TIME t_update;
};

struct _nbk_kernel
{
	//fonts
	CM_FONT font_list[16];

	//res conn list
	bd_list *res_conn_list;

	//image
	bd_list *image_list;

	//fs cache
	nbk_fscache *fs_cache;

	//cache path
	S8 cache_path[CM_MAX_PATH];

	nbk_cookie_info *cookie_info;

	nbk_wap_white_list white_list;

	S32 ref;
};


#ifdef __cplusplus
extern "C"
{
#endif

nbk_kernel_ptr nbk_kernel_new(void);
void nbk_kernel_free(nbk_kernel_ptr kernel);

void nbk_kernel_draw_screen(nbk_kernel_ptr kernel);
void nbk_kernel_update_screen(nbk_kernel_ptr kernel);
void nbk_kernel_update_view_rect(nbk_kernel_ptr kernel);

void nbk_kernel_load_url(nbk_kernel_ptr kernel, const S8 *url);
void nbk_kernel_clear(nbk_kernel_ptr kernel);

//image manager
void nbk_kernel_add_image(nbk_kernel_ptr kernel, NImage *image);
void nbk_kernel_delete_image(nbk_kernel_ptr kernel, NImage *image);

//void nbk_kernel_set_cache_path(nbk_kernel_ptr kernel, const S8 *path);
const S8 *nbk_kernel_get_cache_path(nbk_kernel_ptr kernel);

void nbk_kernel_clear_cache(nbk_kernel_ptr kernel, S32 page_id);

void nbk_kernel_delete_cache_cookie_ff(void);

//white list
nbk_wap_white_list_ptr nbk_kernel_get_white_list(nbk_kernel_ptr kernel);


#ifdef __cplusplus
}
#endif


#endif	//_NBK_KERNEL_H

