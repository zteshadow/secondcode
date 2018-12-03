
#ifndef _NBK_IO_STREAM_H	
#define _NBK_IO_STREAM_H


#include "cm_data_type.h"


typedef enum
{
	CM_ERROR_SUCCESS,
	CM_ERROR_OUT_OF_MEMORY,
	CM_ERROR_NULL_REF_POINTER,
	CM_ERROR_INVALID_PARAM,

	CM_ERROR_QUEUE_FULL,
	CM_ERROR_QUEUE_EMPTY,

	CM_ERROR_CREATE_TIMER_FAILED,
	
	//stream
	CM_ERROR_STREAM_OVERFLOW,
    CM_ERROR_READ_FILE_FAILED,
    
	CM_ERROR_FAILED = 0x9000

} CM_IO_RESULT;

typedef struct _tag_bd_output_stream
{
	S8* buff;
	S32 size;
	S32 pos;

}bd_stream, *bd_stream_ptr;

typedef bd_stream		bd_output_stream;
typedef bd_stream_ptr   bd_output_stream_ptr;

typedef bd_stream		bd_input_stream;
typedef bd_stream_ptr	bd_input_stream_ptr;


#ifdef __cplusplus
extern "C"
{
#endif


CM_IO_RESULT bd_output_stream_init(bd_output_stream_ptr stream);
void bd_output_stream_destroy(bd_output_stream_ptr stream);

CM_IO_RESULT bd_os_write(bd_output_stream_ptr stream, S8* buff, S32 len);
CM_IO_RESULT bd_os_write_byte(bd_output_stream_ptr stream, S8 b);
CM_IO_RESULT bd_os_write_short(bd_output_stream_ptr stream, S16 s);
CM_IO_RESULT bd_os_write_int(bd_output_stream_ptr stream, S32 i);

CM_IO_RESULT bd_os_write_ucs(bd_output_stream_ptr stream, const U16 *str);
CM_IO_RESULT bd_os_write_utf8(bd_output_stream_ptr stream, const S8 *str);

CM_IO_RESULT bd_os_to_byte_array(bd_output_stream_ptr stream, S8 **arr, S32 *len);

CM_IO_RESULT bd_os_write_to_file(bd_output_stream_ptr stream, const S8 *fname);

CM_IO_RESULT bd_os_expand_size(bd_output_stream_ptr stream, S32 ex_size);

CM_IO_RESULT bd_input_stream_init(bd_input_stream_ptr stream, S8 *buff, CM_SIZE size);

CM_IO_RESULT bd_is_read(bd_input_stream_ptr stream, S8* buff, S32 buff_size, S32 *read_size);
CM_IO_RESULT bd_is_read_byte(bd_input_stream_ptr stream, S8* byte);
CM_IO_RESULT bd_is_read_short(bd_input_stream_ptr stream, S16* s);
CM_IO_RESULT bd_is_read_int(bd_input_stream_ptr stream, S32* i);

S32 bd_is_read_from_file(bd_input_stream_ptr stream, const S8 *fname);
CM_IO_RESULT bd_is_read_ucs(bd_input_stream_ptr stream, U16 *str, S32 len);
CM_IO_RESULT bd_is_read_utf8(bd_input_stream_ptr stream, S8 *str, S32 len);

S32 bd_is_skip(bd_input_stream_ptr stream, S32 len);
CM_BOOL bd_is_require_size(bd_input_stream_ptr stream, S32 size);

S8 *bd_string_new(CM_SIZE len);
void bd_string_free(S8 **str);

//do not forget to free the result
CM_IO_RESULT bd_file_read_all_bytes(const S8 *path, S8 **buff, CM_SIZE *len);

S32 bd_fs_file_size(CM_HANDLE fp);
S32 bd_file_length(const S8 *fname);


#ifdef __cplusplus
}
#endif


#endif //_NBK_IO_STREAM_H

