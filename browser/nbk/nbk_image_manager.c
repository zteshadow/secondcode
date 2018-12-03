
#include "nbk_image_manager.h"
#include "imagePlayer.h"

#include "nbk_kernel.h"
#include "nbk_web_view.h"
#include "nbk_io_stream.h"

#include "cm_lib.h"
#include "cm_debug.h"
#include "cm_io.h"
#include "cm_string.h"


static void nbk_get_cache_image_file_name(NImage *img, S8 *name);
static CM_IMAGE* bd_image_decode_by_file(const S8 *filename);


static nbk_cache_image* nbk_get_cache_image(nbk_kernel_ptr kernel, NImage *image)
{
	bd_list *node, *list;

	list = kernel->image_list;

	bd_list_foreach(node, list)
	{
		nbk_cache_image *cache_image = node->data;
		if (cache_image->nbk_image == image)
			return cache_image;
	}
	return 0;
}

static CM_IMAGE* bd_image_decode_by_file(const S8 *filename)
{
	CM_IMAGE *image = 0;
	
	CM_IO_RESULT result = CM_ERROR_SUCCESS;
	S8 *raw = 0;
	CM_SIZE len = 0;

	result = bd_file_read_all_bytes(filename, &raw, &len); 
	if (result != CM_ERROR_SUCCESS)
		return 0;

	image = cm_image_decode(raw, len);

	CM_FREE(raw);

	return image;
}

void NBK_imageCreate(void* platform, NImagePlayer* player, NImage* image)
{
	//add to list
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_kernel_add_image(kernel, image);
}

void NBK_imageDelete(void* platform, NImagePlayer* player, NImage* image)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_kernel_delete_image(kernel, image);
}

void NBK_imageDecode(void* platform, NImagePlayer* player, NImage* image)
{
	CM_IMAGE *img = 0;
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_cache_image *cache_image = nbk_get_cache_image(kernel, image);

	if (!cache_image)
	{
		imagePlayer_onDecodeOver(player, image->id, 0);
		return;
	}

	img = bd_image_decode_by_file(cache_image->file_name);
	if (!img)
	{
		imagePlayer_onDecodeOver(player, image->id, 0);
		return;
	}

	image->total = 1;
	image->size.w = img->width;
	image->size.h = img->height;

	cache_image->image_render = img;

	imagePlayer_onDecodeOver(player, image->id, 1);
}

void NBK_imageDrawBg(void* platform, NImagePlayer* player, NImage* image, const NRect* rect, int rx, int ry)
{
	NBK_imageDraw(platform, player, image, rect);
}

void NBK_imageDraw(void* platform, NImagePlayer* player, NImage* image, const NRect* rect)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_cache_image *cache_image = nbk_get_cache_image(kernel, image);
	void *g = web_view->graphic;

	if (!cache_image)
		return;

	cm_image_draw(g, rect->l, rect->t, cache_image->image_render);
}

void NBK_onImageData(void* platform, NImage* image, void* data, S32 length)
{
	//CM_IO_RESULT result = CM_ERROR_SUCCESS;
	CM_HANDLE fp;

	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_cache_image *cache_image = nbk_get_cache_image(kernel, image);

	if (!cache_image)
		return;

	//write cache file
	fp = cm_fopen(cache_image->file_name, "a+");
	if (fp != 0)
	{
	    cm_fseek(fp, 0, CM_SEEK_END);
	    cm_fwrite(data, length, 1, fp);
	    cm_fclose(fp);
	}
}

void NBK_onImageDataEnd(void* platform, NImage* image, bd_bool succeed)
{
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	nbk_kernel_ptr kernel = nbk_web_view_get_kernel(web_view);
	nbk_cache_image *cache_image = nbk_get_cache_image(kernel, image);

	if (!cache_image) return;

	if (!succeed)
	{//delete cache file
		cm_file_delete(cache_image->file_name);
	}
}

//not implement
void NBK_imageStopAll(void* pfd)
{
#ifdef __ANDROID__
	android_imageStopAll(pfd);
#endif
}

bd_bool NBK_isImageCached(const S8* url)
{
	return 0;
}

void NBK_drawAdPlaceHolder(void *platform, NRect *rect)
{
    CM_COLOR color = {0};
    CM_RECT rt = {0};
    
	nbk_web_view_ptr web_view = (nbk_web_view_ptr)platform;
	void *g = web_view->graphic;

    color.r = 0xE6;
    color.g = 0xEE;
    color.b = 0xF7;
    cm_gdi_set_brush_color(g, &color);

    rt.x = rect->l;
    rt.y = rect->t;
    rt.w = rect->r - rect->l;
    rt.h = rect->b - rect->t;
    
    cm_gdi_fill_rect(g, &rt);
}

static void nbk_get_cache_image_file_name(NImage *img, S8 *name)
{
    S32 value;
	S8 file_name[20] = {0};

    value = cm_facility_get_work_dir(name);
    CM_ASSERT(value >= 0);

    value = cm_facility_add_sub_dir(name, "cache");
    CM_ASSERT(value >= 0);

    cm_sprintf(name, "%x", img);
    value = cm_facility_add_sub_dir(name, file_name);
    CM_ASSERT(value >= 0);
}

nbk_cache_image* nbk_cache_image_new(NImage *img)
{
	nbk_cache_image *thiz = 0;

	thiz = CM_MALLOC(sizeof(*thiz));
	if (!thiz)
		return 0;

    cm_memset(thiz, 0, sizeof(*thiz));
	//create file name
	nbk_get_cache_image_file_name(img, thiz->file_name);

	thiz->nbk_image = img;
	return thiz;
}

void nbk_cache_image_free(nbk_cache_image *thiz)
{
	if (!thiz) return;

    if (thiz->image_render)
	    cm_image_free(thiz->image_render);

	CM_FREE(thiz);
}

