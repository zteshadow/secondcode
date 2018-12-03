
#include "nbk_io_stream.h"
#include "cm_lib.h"
#include "cm_io.h"
#include "cm_string.h"
#include "cm_debug.h"


#define OS_BLOCK_SIZE	1024


CM_IO_RESULT bd_output_stream_init(bd_output_stream_ptr stream)
{
	if (!stream) return CM_ERROR_INVALID_PARAM;

	if (stream->buff)
	{
		cm_memset(stream->buff, 0, stream->size);
		stream->pos = 0;
		return CM_ERROR_SUCCESS;
	}

	cm_memset(stream, 0, sizeof(bd_output_stream));

	stream->buff = (S8 *)CM_MALLOC(OS_BLOCK_SIZE);
	if (stream->buff == NULL)
		return CM_ERROR_OUT_OF_MEMORY;//oom

	cm_memset(stream->buff, 0, OS_BLOCK_SIZE);

	stream->size = OS_BLOCK_SIZE;
	stream->pos = 0;

	return CM_ERROR_SUCCESS;
}

void bd_output_stream_destroy(bd_output_stream_ptr stream)
{
	if (!stream || !stream->buff) return;

	CM_FREE(stream->buff);
	stream->size = 0;
	stream->pos = 0;
}

CM_IO_RESULT bd_os_expand_size(bd_output_stream_ptr stream, S32 ex_size)
{
	S32 new_size = 0;
	S8 *new_buff = NULL;

	if (!stream || ex_size < 0) return CM_ERROR_INVALID_PARAM;

	new_size = stream->size + ex_size;
	new_buff = (S8 *)CM_MALLOC(new_size);
	
	if (new_buff == NULL)
		return CM_ERROR_OUT_OF_MEMORY;//oom

	cm_memcpy(new_buff, stream->buff, stream->size);
	CM_FREE(stream->buff);
	stream->buff = new_buff;
	stream->size = new_size;
	return CM_ERROR_SUCCESS;
}

static CM_IO_RESULT bd_os_assure_buff_size(bd_output_stream_ptr stream, S32 need_size)
{
	S32 size = 0;

	if (!stream || need_size < 0) return CM_ERROR_INVALID_PARAM;

	size = stream->size - stream->pos;
	if (need_size < size)  return CM_ERROR_SUCCESS;//

	size = 0;
	while (size <= need_size) size += OS_BLOCK_SIZE;

	return bd_os_expand_size(stream, size);
}


CM_IO_RESULT bd_os_write(bd_output_stream_ptr stream, S8* buff, S32 len)
{
	if (!stream || !buff || len < 0) return CM_ERROR_INVALID_PARAM;

	if (bd_os_assure_buff_size(stream, len) != CM_ERROR_SUCCESS)
		return CM_ERROR_OUT_OF_MEMORY;//no size

	cm_memcpy(&(stream->buff[stream->pos]), buff, len);
	stream->pos += len;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_os_write_byte(bd_output_stream_ptr stream, S8 b)
{
	if (!stream) return CM_ERROR_INVALID_PARAM;

	if (bd_os_assure_buff_size(stream, 1) != CM_ERROR_SUCCESS) return CM_ERROR_OUT_OF_MEMORY;

	stream->buff[stream->pos] = b;
	stream->pos++;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_os_write_short(bd_output_stream_ptr stream, S16 s)
{
	if (!stream) return CM_ERROR_INVALID_PARAM;

	if (bd_os_assure_buff_size(stream, 2) != CM_ERROR_SUCCESS) return CM_ERROR_OUT_OF_MEMORY;


	stream->buff[stream->pos] = (S8)(s & 0x00ff);
	stream->buff[stream->pos + 1] = (S8)((s & 0xff00) >> 8);

	stream->pos += 2;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_os_write_int(bd_output_stream_ptr stream, S32 i)
{
	if (!stream) return CM_ERROR_INVALID_PARAM;

	if (bd_os_assure_buff_size(stream, 4) != CM_ERROR_SUCCESS) return CM_ERROR_OUT_OF_MEMORY;

	stream->buff[stream->pos] = (S8)(i & 0x000000ff);
	stream->buff[stream->pos + 1] = (S8)((i & 0x0000ff00) >> 8);
	stream->buff[stream->pos + 2] = (S8)((i & 0x00ff0000) >> 16);
	stream->buff[stream->pos + 3] = (S8)((i & 0xff000000) >> 24);

	stream->pos += 4;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_os_write_ucs(bd_output_stream_ptr stream, const U16 *str)
{
	S32 len = 0;

	if (!stream || !str) return CM_ERROR_INVALID_PARAM;

	len = cm_ustrlen(str);
	len <<= 1; // byte size

	return bd_os_write(stream, (S8 *)str, len);
}

CM_IO_RESULT bd_os_write_utf8(bd_output_stream_ptr stream, const S8 *str)
{
	if (!str) return CM_ERROR_INVALID_PARAM;
	return bd_os_write(stream, (S8 *)str, cm_strlen(str));
}

CM_IO_RESULT bd_os_to_byte_array(bd_output_stream_ptr stream, S8 **arr, S32 *len)
{
	if (!stream || !arr || !len) return CM_ERROR_INVALID_PARAM;

	*len = stream->pos;

	CM_ASSERT(*arr == NULL);
	*arr = (S8 *)CM_MALLOC(*len);
	if (!*arr) return CM_ERROR_OUT_OF_MEMORY;

	cm_memcpy(*arr, stream->buff, *len);

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_input_stream_init(bd_input_stream_ptr stream, S8 *buff, CM_SIZE size)
{
	if (!stream || !buff) return CM_ERROR_INVALID_PARAM;

	stream->buff = buff;
	stream->size = size;
	stream->pos = 0;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_is_read(bd_input_stream_ptr stream, S8* buff, S32 buff_size, S32 *read_size)
{
	S32 real_size = 0;

	if (!stream || !buff || buff_size < 0) return CM_ERROR_INVALID_PARAM;

	if (stream->pos == stream->size)
	{
		if (read_size) *read_size = -1;
		return CM_ERROR_SUCCESS;
	}

	real_size = buff_size;

	if (real_size + stream->pos >= stream->size)
		real_size = stream->size - stream->pos;

	if (real_size < 0)
		return CM_ERROR_STREAM_OVERFLOW;

	cm_memcpy(buff, &stream->buff[stream->pos], real_size);

	stream->pos += real_size;

	if (read_size) *read_size = real_size;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_is_read_byte(bd_input_stream_ptr stream, S8* byte)
{
	if (!stream || !byte) return CM_ERROR_INVALID_PARAM;

	if (stream->size - stream->pos < sizeof(S8))
		return CM_ERROR_STREAM_OVERFLOW;

	*byte = stream->buff[stream->pos];
	stream->pos++;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_is_read_short(bd_input_stream_ptr stream, S16* s)
{
	if (!stream || !s) return CM_ERROR_INVALID_PARAM;

	if (stream->size - stream->pos < sizeof(S16))
		return CM_ERROR_STREAM_OVERFLOW;

	*s = (((stream->buff[stream->pos + 1] << 8)) | (stream->buff[stream->pos] & 0X00ff)) & 0x0000fffff;
	stream->pos += 2;

	return CM_ERROR_SUCCESS;
}

CM_IO_RESULT bd_is_read_int(bd_input_stream_ptr stream, S32* i)
{
	S8 b0, b1, b2, b3;

	if (!stream || !i) return CM_ERROR_INVALID_PARAM;

	if (stream->size - stream->pos < sizeof(S32))
		return CM_ERROR_STREAM_OVERFLOW;

	b0 = stream->buff[stream->pos];
	b1 = stream->buff[stream->pos+1];
	b2 = stream->buff[stream->pos+2];
	b3 = stream->buff[stream->pos+3];

	*i = (b0 & 0x000000ff) | ((b1 << 8) & 0x0000ff00) | ((b2 << 16) & 0x00ff0000) | ((b3 << 24) & 0xff000000);

	stream->pos += 4;

	return CM_ERROR_SUCCESS;
}


CM_IO_RESULT bd_is_read_ucs(bd_input_stream_ptr stream, U16 *str, S32 len)
{
	S32 read_len = 0;

	if (!stream || !str || len < 0) return CM_ERROR_INVALID_PARAM;

	if (stream->size - stream->pos < len)
		return CM_ERROR_STREAM_OVERFLOW;

	return bd_is_read(stream, (S8 *)str, len, &read_len);
}

CM_IO_RESULT bd_is_read_utf8(bd_input_stream_ptr stream, S8 *str, S32 len)
{
	S32 read_len = 0;
	if (!stream || !str || len < 0) return CM_ERROR_INVALID_PARAM;

	if (stream->size - stream->pos < len)
		return CM_ERROR_STREAM_OVERFLOW;

	return bd_is_read(stream, (S8 *)str, len, &read_len);
}

S32 bd_is_skip(bd_input_stream_ptr stream, S32 len)
{
	S32 real_len = len;
	if (stream->size - stream->pos < len)
		real_len = stream->size - stream->pos;

	stream->pos += real_len;

	return real_len;
}

CM_BOOL bd_is_require_size(bd_input_stream_ptr stream, S32 size)
{
	return (stream->size - stream->pos > size);
}

S8 *bd_string_new(CM_SIZE len)
{
	S8 *str = 0; 
	CM_SIZE size = sizeof(S8) * (len + 1);

	str = (S8 *)CM_MALLOC(size);
	if (str)
	{
	    cm_memset(str, 0, size);
	}
	
	return str;
}

void bd_string_free(S8 **str)
{
    CM_ASSERT(str != 0 && *str != 0);

    if (str)
    {
        S8 *p = *str;
    	if (p) 
    	    CM_FREE(p);
    	*str = 0;
	}
}

CM_IO_RESULT bd_file_read_all_bytes(const S8 *path, S8 **buff, CM_SIZE *len)
{
	CM_HANDLE fp = 0;

	if (fp = cm_fopen(path, "rb"))
	{
	    CM_SIZE size;
	    
	    cm_fseek(fp, 0, CM_SEEK_END);
        size = cm_ftell(fp);

		*len = size;
		*buff = (S8 *)CM_MALLOC(size);

		cm_fread(*buff, size, 1, fp);
		cm_fclose(fp);

		return CM_ERROR_SUCCESS;
	}

	return CM_ERROR_READ_FILE_FAILED;
}

S32 bd_fs_file_size(CM_HANDLE fp)
{
    S32 size, old;

    old = cm_ftell(fp);
    cm_fseek(fp, 0, CM_SEEK_END);
    size = cm_ftell(fp);
    cm_fseek(fp, old, CM_SEEK_SET);

    return size;
}

S32 bd_file_length(const S8 *fname)
{
    S32 size = 0;
    CM_HANDLE fp = cm_fopen(fname, "rb");
    if (fp)
    {
        cm_fseek(fp, 0, CM_SEEK_END);
        size = cm_ftell(fp);
        cm_fclose(fp);
    }
    
    return size;
}
