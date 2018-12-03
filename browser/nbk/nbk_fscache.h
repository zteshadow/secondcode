
#ifndef _NBK_FSCACHE_H
#define _NBK_FSCACHE_H


#include "cm_data_type.h"
#include "cm_time.h"
#include "cm_facility.h"


typedef enum _NECacheType 
{
	NECT_HTML,
	NECT_IMAGE,
	NECT_MISC

}NECacheType;

typedef struct _NFCDate
{
	U16  year;
	U8   month;
	U8   day;
	U8   hh;
	U8   mm;
}NFCDate;

typedef struct _NFCItem
{
	S8		    id[32];
	NFCDate		create;
	NFCDate		expire;
	S32		    size;
	U16	lru;
	U8	type;

} NFCItem;

typedef struct _nbk_fscache
{
	CM_BOOL loaded;
	S32 max_space;
	NFCItem *items;
	S32 max;
	S32 used;

	S8 dir[CM_MAX_PATH / 2];
	S8 path[CM_MAX_PATH / 2];

	S32 cur_index;
	S32 saved_bytes;

}nbk_fscache;


#ifdef __cplusplus
extern "C"
{
#endif


nbk_fscache* nbk_fscache_new(void);
void nbk_fscache_free(nbk_fscache *thiz);

S32 nbk_fscache_get_cache_index(nbk_fscache *thiz, const S8 *url);
void nbk_fscache_get_cache_header(nbk_fscache *thiz, S32 index, S32 *type, S32 *size);
S32 nbk_fscache_get_cache_data(nbk_fscache *thiz, S32 index, S8 *buff, S32 size, S32 offset);
void nbk_fscache_get_cache_data_over(nbk_fscache *thiz);


#ifdef __cplusplus
}
#endif


#endif	//_NBK_FSCACHE_H

