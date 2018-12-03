
#ifndef __CONFIG_H__
#define __CONFIG_H__


// nbk version
#define NBK_VER_MAJOR   0
#define NBK_VER_MINOR   9
#define NBK_VER_BUILD   1
#define NBK_VER_REV     0

// features
//#define NBK_USE_BIGENDIAN

#define NBK_EXT_SHORTCUT_MENU       1
#define NBK_EXT_MAINBODY_MODE       1

#define NBK_DISABLE_ZOOM            1

#define NBK_REQ_IMG_ALONE           1 // request images with multi-pics

//#define NBK_MEM_TEST
//#define _BD_MEM_DEBUG_


#ifdef __cplusplus
extern "C" {
#endif

/*
 * basic data types
 */

typedef char				int8;
typedef unsigned char		uint8;
typedef uint8               bd_bool;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;

typedef unsigned int		bd_size_t;
typedef unsigned short      wchr;


typedef int32               coord;
typedef uint16              nid;

typedef struct _NFloat {
    int16   i;
    int16   f;
} NFloat;

#define N_TRUE  1
#define N_FALSE 0
#define N_NULL  0L
#define N_ASSERT(e) NBK_Assert((int32)e)
#define N_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define N_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define N_ABS(n) ((n >= 0) ? n : -n)
#define N_INVALID_ID    0xffff
#define N_MAX_UINT      0xffffffff
#define N_INVALID_COORD 12321
#define N_MAX_COORD     100000

#define N_KILL_ME() {char* crash = N_NULL; *crash = 88;}

#define NFLOAT_EMU   10000

#ifdef NBK_USE_BIGENDIAN
#define N_SWAP_UINT16(n)    ((n >> 8 & 0xff) | (n << 8 & 0xff00))
#define N_SWAP_UINT32(n)    ((n >> 24 & 0xff) | (n >> 8 & 0xff00) | \
                             (n << 8 & 0xff0000) | (n << 24 & 0xff000000)) 
#else
#define N_SWAP_UINT16(n)    (n)
#define N_SWAP_UINT32(n)    (n)
#endif

int32 nfloat_imul(int32 src, NFloat factor);
int32 nfloat_idiv(int32 src, NFloat factor);

/*
 * memory interface
 */

#if defined(_BD_MEM_DEBUG_)
#define NBK_malloc(size)        nbk_malloc_ext((__FILE__), (__LINE__), (size))
#define NBK_malloc0(size)       nbk_malloc0_ext((__FILE__), (__LINE__), (size))
#define NBK_realloc(p, size)    nbk_realloc_ext((__FILE__), (__LINE__), (p), (size))
#define NBK_free(p)             nbk_free_ext((p))
#else
#define NBK_malloc(size)        nbk_malloc((size))
#define NBK_malloc0(size)       nbk_malloc0((size))
#define NBK_realloc(p, size)    nbk_realloc((p), (size))
#define NBK_free(p)             nbk_free((p))
#endif //_BD_MEM_DEBUG_

void NBK_Assert(int32 value);
void NBK_memcpy(void* dst, void* src, bd_size_t size);
void NBK_memset(void* dst, int8 value, bd_size_t size);
void NBK_memmove(void* dst, void* src, bd_size_t size);

//#define NBK_sprintf cm_sprintf
int32 NBK_sprintf(int8 *str, const int8 *format, ...);

int NBK_currentMilliSeconds(void);
int NBK_freemem(void); // total free memory in bytes

void NBK_fep_enable(void* pfd);
void NBK_fep_disable(void* pfd);
void NBK_fep_updateCursor(void* pfd);

int NBK_conv_gbk2unicode(const char* mbs, int mbLen, wchr* wcs, int wcsLen);

bd_bool NBK_ext_isDisplayDocument(void* document);


/*for memory*/
void* nbk_malloc(bd_size_t size);
void* nbk_malloc0(bd_size_t size);
void* nbk_realloc(void* ptr, bd_size_t size);
void nbk_free(void* p);

#if defined(_BD_MEM_DEBUG_)
void* nbk_malloc_ext(const int8 *fname, int32 line, bd_size_t size);
void* nbk_malloc0_ext(const int8 *fname, int32 line, size_t size);
void* nbk_realloc_ext(const int8 *fname, int32 line, void* ptr, bd_size_t size);
void nbk_free_ext(void* p);
#endif //_BD_MEM_DEBUG_


#ifdef __cplusplus
}
#endif


#endif // __CONFIG_H__

