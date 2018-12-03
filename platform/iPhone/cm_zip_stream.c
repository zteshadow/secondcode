
#include "cm_zip_stream.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"

#include "zlib.h"


#define CM_CHUNK (1024)


void *cm_zip_create(CM_ZIP_TYPE type)
{
    z_stream *p;

    p = (z_stream *)CM_MALLOC(sizeof(z_stream));
    if (p != 0)
    {
    	S32 ret = Z_OK;

        cm_memset(p, 0, sizeof(z_stream));
    	if (type == CM_ZIP_GZIP)
    	{
    		ret = inflateInit2(p, 47);
    	}
    	else
    	{
    		ret = inflateInit(p);
    	}

    	if (ret != Z_OK)
    	{
    	    CM_FREE(p);
    	    p = 0;
    	}
	}

	return (void *)p;
}

//return compressed 'length'
//output: compressed data
S32 cm_zip_decompress(void *zip, S8 *src, S32 len, S8 **output)
{
	S32 ret, dest_len;
	U32 have; 
	S8 out[CM_CHUNK]; 
    z_stream *p = (z_stream *)zip;

    CM_ASSERT(zip != 0);

    //add protection
    dest_len = 0;
    *output = 0;

    if (p)
    {
    	p->avail_in = len;
    	p->next_in = (Bytef*)src;

	    do 
    	{ 
    		p->avail_out = CM_CHUNK; 
    		p->next_out = (Bytef *)out; 
    		ret = inflate(p, Z_NO_FLUSH); 
    		//assert(ret != Z_STREAM_ERROR); /* state not clobbered */

    		switch (ret) 
    		{ 
    		case Z_NEED_DICT: 
    			ret = Z_DATA_ERROR; /* and fall through */ 
    		case Z_DATA_ERROR: 
    		case Z_MEM_ERROR: 
    			CM_TRACE_0("zip error: %d", ret);
    			inflateEnd(p); 
    			return 0;
    		}
    		
    		have = CM_CHUNK - p->avail_out; 

    		dest_len += have;

    		*output = CM_REALLOC(*output, dest_len); 
    		cm_memcpy(*output + dest_len - have, out, have); 

    	} while (p->avail_out == 0); 

    	if (ret == Z_STREAM_END)
    	{
    		inflateEnd(p);
    	}
	}

	return dest_len;
}

void cm_zip_destory(void *zip)
{
    z_stream *p = (z_stream *)zip;

    CM_ASSERT(zip != 0);
    if (p)
    {
        inflateEnd(p);
        CM_FREE(p);
	}
}

