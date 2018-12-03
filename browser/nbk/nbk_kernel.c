
#include "nbk_kernel.h"
#include "nbk_image_manager.h"
#include "nbk_ffpkg_parser.h"

#include "cm_lib.h"
#include "cm_debug.h"
#include "cm_io.h"
#include "cm_string.h"
#include "cm_facility.h"


static void nbk_kernel_set_cache_path(nbk_kernel_ptr kernel, const S8 *path);


static nbk_kernel_ptr sg_nbk_kernel = 0;	//global single kernel instance


static void nbk_kernel_release_wap_white_list(nbk_wap_white_list *white_list)
{
	white_list->version = -1;
}

static void nbk_kernel_get_cookie_file(S8 *file)
{
    S32 value;

    value = cm_facility_get_work_dir(file);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(file, "cookie.dat");
    CM_ASSERT(value >= 0);
}

nbk_kernel_ptr nbk_kernel_new(void)
{
	nbk_kernel_ptr nbk = 0;
	S8 cookie_file[CM_MAX_PATH] = {0};

	if (0 != sg_nbk_kernel)
	{
		sg_nbk_kernel->ref++;
		return sg_nbk_kernel;
	}

	nbk = (nbk_kernel_ptr)CM_MALLOC(sizeof(*nbk));

	if (!nbk) return nbk;

	cm_memset(nbk, 0, sizeof(*nbk));

	nbk->fs_cache = nbk_fscache_new();

	if (0 == nbk->fs_cache)
	{
		CM_FREE(nbk);
		return nbk;
	}

	nbk_kernel_get_cookie_file(cookie_file);
	nbk->cookie_info = nbk_cookie_init(cookie_file, 0);
	if (0 == nbk->cookie_info)
	{
		CM_FREE(nbk->fs_cache);
		CM_FREE(nbk);
		return nbk;
	}

	nbk->white_list.version = 0;

	//nbk_kernel_load_wap_while_list(nbk);

	sg_nbk_kernel = nbk;
	sg_nbk_kernel->ref++;

	return nbk;
}
 
void nbk_kernel_free(nbk_kernel_ptr kernel)
{
	S8 cookie_file[CM_MAX_PATH] = {0};
	if (!kernel) return;

	kernel->ref--;
	if (kernel->ref > 0)
		return;

	//nbk_timer_manager_delete_all();

	nbk_fscache_free(kernel->fs_cache);

	nbk_kernel_get_cookie_file(cookie_file);
	nbk_cookie_output(kernel->cookie_info, (const S8 *)cookie_file);
	nbk_cookie_deinit(kernel->cookie_info);

	CM_FREE(kernel);
}

//image manager
void nbk_kernel_add_image(nbk_kernel_ptr kernel, NImage *image)
{
	nbk_cache_image *cache_image = 0;

	cache_image = nbk_cache_image_new(image);
	if (!cache_image) return;

	kernel->image_list = bd_list_append(kernel->image_list, cache_image);
}

void nbk_kernel_delete_image(nbk_kernel_ptr kernel, NImage *image)
{
	bd_list *node, *list;

	list = kernel->image_list;

	bd_list_foreach(node, list)
	{
		nbk_cache_image *cache_image = node->data;
		if (cache_image->nbk_image == image)
		{
			kernel->image_list = bd_list_delete_link(list, node);
			nbk_cache_image_free(cache_image);
			break;
		}
	}
}

static void nbk_kernel_set_cache_path(nbk_kernel_ptr kernel, const S8 *path)
{
    S32 value;
    
	if (!kernel || !path)
		return;

    value = cm_facility_get_work_dir(kernel->cache_path);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(kernel->cache_path, path);
    CM_ASSERT(value >= 0);
}

const S8 *nbk_kernel_get_cache_path(nbk_kernel_ptr kernel)
{
	if (!kernel)
		return 0;

	if (0 == kernel->cache_path[0])
		nbk_kernel_set_cache_path(kernel, "cache");

	return (const S8 *)kernel->cache_path;
}

void nbk_kernel_clear_cache(nbk_kernel_ptr kernel, S32 page_id)
{
    S32 value;
	CM_IO_RESULT ret = CM_ERROR_SUCCESS;
	S8 path[CM_MAX_PATH] = {0};
	S8 sub_dir[CM_MAX_PATH] = {0};
	const S8 *cache_path;

	if (!kernel)
		return;

	cache_path = nbk_kernel_get_cache_path(kernel);

    cm_strcpy(path, cache_path);
    
	cm_sprintf(sub_dir, "cp%05d", page_id);
    value = cm_facility_add_sub_dir(path, sub_dir);
    CM_ASSERT(value >= 0);

	ret = cm_rmdir(path);
	if (ret != CM_ERROR_SUCCESS)
	{
		CM_TRACE_1("clean cached page failed[%d]: page_id[%d], path[%s]", ret, page_id, path);
	}
}

void nbk_kernel_delete_cache_cookie_ff()
{
	S32 count = 1;
	S8 path[CM_MAX_PATH] = {0};

	nbk_get_cache_file_name(FF_COOKIE, count, path);

	while (cm_file_delete(path) == CM_ERROR_SUCCESS)
	{
		count++;
		nbk_get_cache_file_name(FF_COOKIE, count, path);
	}

	CM_TRACE_5("delete cache failed: %s", path);
}


/////////////////////////////////////////////////////////////////////////////////////
//white list

#define WHITE_LIST_WAITING	0
#define WHITE_LIST_UPDATED	1

nbk_wap_white_list_ptr nbk_kernel_get_white_list(nbk_kernel_ptr kernel)
{
	if (0 == kernel)
		return 0;

	return &kernel->white_list;
}

