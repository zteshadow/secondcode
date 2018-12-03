
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
* This module contains some common API wrapper for string and memory process.
*
*****************************************************************************/


#ifndef _CM_STRING_H
#define _CM_STRING_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Input:
* ------
*  1 'd'    : destinatino
*  2 's'    : source
*  3 'n'    : bytes of data to copy
*
* Return:
* ------------
* 'd'
*
* Description:
* ------------
* copy 'n' bytes of data from 's' into 'd' no matter '\0'
* be aware of the overflow
*
*****************************************************************************/
void *cm_memcpy(void *d, const void *s, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 'd'    : destinatino
*  2 's'    : source
*  3 'n'    : bytes of data to copy
*
* Return:
* ------------
* 'd'
*
* Description:
* ------------
* copy 'n' bytes of data from 's' into 'd' no matter '\0'
* be aware of the overflow
*
*****************************************************************************/
void *cm_memmove(void *d, const void *s, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 's1'    : memory to be compared
*  2 's2'    : 
*
* Return:
* ------------
* 0 equal
*
* Description:
* ------------
* compare byte by byte
*
*****************************************************************************/
S32 cm_memcmp(const void *s1, const void *s2, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 's'    : memory to be set
*  2 'c'    : data set to memory
*  3 'n'    : size to set
*
* Return:
* ------------
* 's'
*
* Description:
* ------------
* set 'n' bytes of data to be 'c' from 's'
* be aware of the overflow
*
*****************************************************************************/
void *cm_memset(void *s, S32 c, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 's1'   : first string to be compared
*  2 's2'   : second string to be compared
*
* Return:
* ------------
* 0 : if equal
*
*****************************************************************************/
S32 cm_strcmp(const S8 *s1, const S8 *s2);

/*****************************************************************************
* Input:
* ------
*  1 's1'   : first string to be compared
*  2 's2'   : second string to be compared
*  3 'n'    : count of char to be compared
*
* Return:
* ------------
* 0 : if equal
*
*****************************************************************************/
S32 cm_strncmp(const S8 *s1, const S8 *s2, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 's1'   : first string to be compared
*  2 's2'   : second string to be compared
*
* Return:
* ------------
* 0 : if equal(ignore case)
*
*****************************************************************************/
S32 cm_strcasecmp(const S8 *s1, const S8 *s2);

/*****************************************************************************
* Input:
* ------
*  1 's1'   : first string to be compared
*  2 's2'   : second string to be compared
*  3 'n'    : count of char to be compared
*
* Return:
* ------------
* 0 : if equal(ignore case)
*
*****************************************************************************/
S32 cm_strncasecmp(const S8 *s1, const S8 *s2, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 's' : string to be measured
*
* Return:
* ------------
* length of the 's'
*
*****************************************************************************/
CM_SIZE cm_strlen(const S8 *s);

/*****************************************************************************
* Input:
* ------
*  1 'd'    : memory to write data
*  2 's'    : source of data
*  3 'n'    : length to be copy
*
* Return:
* ------------
* 'dest'
*
* Description:
* ------------
* copy 'n' bytes of data into 'd' from 's'
*
*****************************************************************************/
S8 *cm_strncpy(S8 *d, const S8 *s, CM_SIZE n);

/*****************************************************************************
* Input:
* ------
*  1 'd'    : memory to write data
*  2 's'    : source of data
*
* Return:
* ------------
* 'dest'
*
* Description:
* ------------
* copy into 'd' from 's'
*
*****************************************************************************/
S8 *cm_strcpy(S8 *d, const S8 *s);

S8 *cm_strcat(S8 *d, const S8 *s);

/*****************************************************************************
* Input:
* ------
*  1 's'    : source of data
*
* Return:
* ------------
* 'dest'
*
* Description:
* ------------
* copy string of 's' to new string, return
*
*****************************************************************************/
S8 *cm_strdup(const S8 *s);

/*****************************************************************************
* Input:
* ------
*  1 'source'   : 
*  2 'pattern'  :
*
* Return:
* ------------
* 'match'
*
* Description:
* ------------
* pointer if 'pattern' is in 'source', 0 otherwise
*
*****************************************************************************/
S8 *cm_strstr(const S8 *source, const S8 *pattern);

/*****************************************************************************
* Input:
* ------
*  1 'source'   : 
*  2 'c'        : should be a char
*
* Return:
* ------------
* 'match'
*
* Description:
* ------------
* pointer to the latest 'c' in 'source
*
*****************************************************************************/
S8 *cm_strrchr(const S8 *source, S32 c);

/*****************************************************************************
* Input:
* ------
*  1 'source'   : 
*  2 'c'        : should be a char
*
* Return:
* ------------
* 'match'
*
* Description:
* ------------
* pointer to the first 'c' in 'source
*
*****************************************************************************/
S8 *cm_strchr(const S8 *source, S32 c);

S8 *cm_strtok(S8 *src, const S8 *delim);


/*****************************************************************************
* Description:
* ------------
* unicode version of string process
*
*****************************************************************************/
S32 cm_ustrcmp(const U16 *s1, const U16 *s2);
CM_SIZE cm_ustrlen(const U16* s);
U16 *cm_ustrncpy(U16 *d, const U16 *s, CM_SIZE n);


#ifdef __cplusplus
}
#endif


#endif //_CM_STRING_H

