
/*****************************************************************************/
/*** samuel.song.bc@gmail.com ***/
/*****************************************************************************/


#ifndef _CM_DATA_TYPE_H
#define _CM_DATA_TYPE_H


#ifndef NULL
#define NULL ((void *)0)
#endif


typedef char			S8;
typedef short			S16;
typedef int				S32;
typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;
typedef unsigned char	CM_BOOL;
typedef long            SLong;      //��С��int��
typedef unsigned long   ULong;      //��С��int��

typedef unsigned int    CM_SIZE;    //for 32/64 compatible
typedef void *          CM_HANDLE;
typedef long            CM_TIME;
typedef S32             SCE_HANDLE;


#endif //_CM_DATA_TYPE_H

