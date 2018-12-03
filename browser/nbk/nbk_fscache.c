
#include "nbk_fscache.h"
#include "nbk_io_stream.h"

#include "cm_lib.h"
#include "cm_io.h"
#include "cm_string.h"
#include "cm_utility.h"
#include "cm_debug.h"


#define DEFAULT_CACHE_DIR       "cache"
#define CACHE_INDEX_FILE_NAME	"fscache.idx"
#define ITEM_CHUNK_NUM	        (100)


static void nbk_fscache_alloc_index_table(nbk_fscache *thiz, S32 n);
static void nbk_fscache_get_current_date(NFCDate *date);
static CM_BOOL nbk_fscache_get_cache_file_path(nbk_fscache *thiz, 
                                               S32 index, S8 *path);


static void nbk_fscache_alloc_index_table(nbk_fscache *thiz, S32 n)
{
	S32 num = 0;
	NFCItem* items = NULL;
	if (!thiz) return;

	if (thiz->max >= thiz->used + n)
		return;//overflow

	while (num < n)
		num += ITEM_CHUNK_NUM;

	items = thiz->items;

	thiz->items = (NFCItem *)CM_MALLOC(sizeof(NFCItem) * num);
	if (!thiz->items)
	{
		thiz->items = items;
		return;
	}

	if (items)
	{
	    if (thiz->used > 0)
	    {
    		cm_memcpy(thiz->items, items, sizeof(NFCItem) * thiz->used);
    		CM_FREE(items);
		}
	}

	thiz->max = num;
}

static void nbk_fscache_get_current_date(NFCDate *date)
{
    CM_DATE_TIME t;
    
	cm_localtime(0, &t);
	date->day = t.tm_mday;
	date->hh = t.tm_hour;
	date->mm = t.tm_min;
	date->month = t.tm_mon;
	date->year = t.tm_year + 1900;
}

CM_BOOL nbk_fscache_later_than_now(NFCDate* now, NFCDate* date)
{
    if (date->year > now->year)
        return 1;
    else if (date->year < now->year)
        return 0;
    
    if (date->month > now->month)
        return 1;
    else if (date->month < now->month)
        return 0;
    
    if (date->day > now->day)
        return 1;
    else if (date->day < now->day)
        return 0;
    
    if (date->hh > now->hh)
        return 1;
    else if (date->hh < now->hh)
        return 0;
    
    if (date->mm > now->mm)
        return 1;
    else if (date->mm < now->mm)
        return 0;
    
    return 1;
}

static CM_BOOL nbk_fscache_get_cache_file_path(nbk_fscache *thiz, S32 index, S8 *path)
{
    S32 value;
    S8 fname[CM_MAX_PATH];
	S8 sub_dir[2] = {0};

	sub_dir[0] = thiz->items[index].id[0];
	sub_dir[1] = 0;

	if (index < 0 || index >= thiz->used)
		return 0;

    value = cm_facility_get_work_dir(path);
    CM_ASSERT(value >= 0);

    cm_sprintf(fname, "%s%s", DEFAULT_CACHE_DIR, sub_dir);

    value = cm_facility_add_sub_dir(path, fname);
    CM_ASSERT(value >= 0);
    value = cm_facility_add_sub_dir(path, thiz->items[index].id);
    CM_ASSERT(value >= 0);

	return 1;
}

nbk_fscache* nbk_fscache_new(void)
{
	nbk_fscache *thiz;
	thiz = (nbk_fscache*)CM_MALLOC(sizeof(nbk_fscache));
	if (thiz)
	{
	    cm_memset(thiz, 0, sizeof(nbk_fscache));
	    cm_facility_get_work_dir(thiz->dir);
	    cm_facility_add_sub_dir(thiz->dir, DEFAULT_CACHE_DIR);

    	nbk_fscache_alloc_index_table(thiz, 1);
	}

	return thiz;
}

void nbk_fscache_free(nbk_fscache *thiz)
{
    CM_ASSERT(thiz != 0);

	if (thiz->items)
	{
		CM_FREE(thiz->items);
	}

	CM_FREE(thiz);
}

S32 nbk_fscache_get_cache_index(nbk_fscache *thiz, const S8 *url)
{
	S8 id[CM_MAX_MD5_STRING] = {0};
	S32 index = 0;
	S32 len, i = 0;

    len = cm_create_md5_string((const void *)url, cm_strlen(url), id);
    CM_ASSERT(len > 0);

	for (i = 0; i < thiz->used; i++)
	{
		if (cm_strncmp(id, thiz->items[i].id, 32) == 0)
		{
			NFCDate now = {0};
			nbk_fscache_get_current_date(&now);

			if (nbk_fscache_later_than_now(&now, &thiz->items[i].expire))
			{
				thiz->items[i].lru++;
				return i;
			}

			return -1;
		}
	}

	return -1;
}

void nbk_fscache_get_cache_header(nbk_fscache *thiz, S32 index, S32 *type, S32 *size)
{
	S8 path[CM_MAX_PATH] = {0};
	if (!thiz)
		return;

	if (index < 0 || index > thiz->used)
		return;

	thiz->cur_index = index;
	*type = thiz->items[index].type;

	nbk_fscache_get_cache_file_path(thiz, index, path);
	*size = bd_file_length(path);
}

S32 nbk_fscache_get_cache_data(nbk_fscache *thiz, S32 index, S8 *buff, S32 size, S32 offset)
{
    CM_HANDLE fp;
	S8 path[CM_MAX_PATH] = {0};
	S32 len = 0;

	if (index < 0 || index >= thiz->used)
		return len;

	nbk_fscache_get_cache_file_path(thiz, index, path);

	if (fp = cm_fopen(path, "rb"))
	{
		cm_fseek(fp, offset, CM_SEEK_SET);
		len = cm_fread(buff, size, 1, fp);
		cm_fclose(fp);
	}

	return len;
}

void nbk_fscache_get_cache_data_over(nbk_fscache *thiz)
{
	return;
}

