
/*****************************************************************************
* Copyright Statement:
* --------------------
* This software is protected by Copyright and the information contained
* herein is confidential. The software may not be copied and the information
* contained herein may not be used or disclosed except with the written
* permission of XXX Inc. (C) 200X-20XX
*
* Version:
* --------
* Ver: 1.0              Author: Samuel.Song             Date: 2008-10-06
*
* Description:
* ------------
* This module contains some common API wrapper for I/O.
*
*****************************************************************************/


#ifndef _CM_IO_H
#define _CM_IO_H


#include "cm_data_type.h"
#include "stdarg.h"


typedef enum
{
    CM_SEEK_SET,    //from begin
    CM_SEEK_CUR,    //from current
    CM_SEEK_END,    //from end
    CM_SEEK_MAX
    
} cm_seek_enum;


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Input:
* ------
*  1 'path' : path and name for the file to be opened
*  2 'mode' : open method:
*      1 "r"    : read only (file must exist)
*      2 "r+"   : r/w (file must exist)
*      3 "w"    : write only, clear file data if exist, or create a new file
*                 if not exist.
*      4 "w+"   : r/w, the same as "w"
*      5 "a"    : write only, append data to the end if exist, or create a new
*                 file if not exist.
*      6 "a+"   : r/w, the same as "a"
*
* Return:
* ------------
* ERR: NULL(if not enough memory)
* OK : file handle
*
* Description:
* ------------
* open or create a file for read/write
*
*****************************************************************************/
CM_HANDLE cm_fopen(const S8 *path, const S8 *mode);

/*****************************************************************************
* Input:
* ------
*  1 'p'    : memory to save the read data
*  2 'size' : size of every block
*  3 'n'    : blocks to read, 1 if just one block
*  4 'file' : handle for the file to be read
*
* Return:
* ------------
* number have been read, if less than 'n', error
*
*****************************************************************************/
CM_SIZE cm_fread(void *p, CM_SIZE size, CM_SIZE n, CM_HANDLE file);

/*****************************************************************************
* Input:
* ------
*  1 'p'    : data write into file
*  2 'size' : size of every block
*  3 'n'    : blocks to write, 1 if just one block
*  4 'file' : handle for the file to be read
*
* Return:
* ------------
* number have been written, if less than 'n', error
*
*****************************************************************************/
CM_SIZE cm_fwrite(const void *p, CM_SIZE size, CM_SIZE n, CM_HANDLE file);

/*****************************************************************************
* Input:
* ------
*  1 'file'     : handle for the file to be read
*  2 'offset'   : if 'whence' is 'CM_SEEK_CUR' or 'CM_SEEK_END', can be
*                 negative
*  3 'whence'   : CM_SEEK_SET, ...
*
* Return:
* ------------
* ERR: negative
* OK : 0
*
*****************************************************************************/
S32 cm_fseek(CM_HANDLE file, S32 offset, S32 whence);

/*****************************************************************************
* Input:
* ------
*  1 'file'     : handle for the file to be read
*
* Return:
* ------------
* current read/write position of this file
*
*****************************************************************************/
S32 cm_ftell(CM_HANDLE file);

/*****************************************************************************
* Input:
* ------
*  1: file: file handle created at by fopen
*
* Return:
* ------------
* ERR: negative
* OK : 0
*
*****************************************************************************/
S32 cm_fclose(CM_HANDLE file);

//从文件读取数据，直到出现换行符或者读了size - 1个字符，或者文件结尾
//在buffer尾部加0组成字符串返回
S8 *cm_fgets(S8 *buffer, S32 size, CM_HANDLE fp);

/*****************************************************************************
* Input:
* ------
*  1: str: buffer for formatted data
*  2: format: format
*  3: ...: variable format and data
*
* Return:
* ------------
* ERR: negative
* OK : 0
*
*****************************************************************************/
S32 cm_sprintf(S8 *str, const S8 *format, ...);

/*****************************************************************************
* Input:
* ------
*  1: str: buffer for formatted data
*  2: format: format
*  3: va_list: variable format and data
*
* Return:
* ------------
* ERR: negative
* OK : 0
*
*****************************************************************************/
S32 cm_vsprintf(S8 *str, const S8 *format, va_list ap);

S32 cm_dir_exist(const S8 *dir);
S32 cm_mkdir(const S8 *dir);
S32 cm_rmdir(const S8 *dir);
S32 cm_file_delete(const S8 *fname);


#ifdef __cplusplus
}
#endif


#endif //_CM_IO_H

