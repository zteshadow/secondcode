/*
 * limit.h
 *
 *  Created on: 2010-12-27
 *      Author: wuyulun
 */

#ifndef LIMIT_H_
#define LIMIT_H_

#define MAX_TREE_DEPTH  32

#define MAX_CSS_DECL    16
#define MAX_CSS_VALUE   8

#define STR_POOL_SIZE   500

#define MAX_SELECTOR_NUM    16

#define MAX_DES_SEL_USE     8 // max descendant selectors kept at sametime
#define MAX_CLASS_NAME      4 // max name used at sametime

#define MAX_FILE_PATH   256

#if defined(MAX_URL_LEN) 
#undef MAX_URL_LEN
#endif 

#define MAX_URL_LEN     4096

#define MAX_EDITOR_VERTICAL_OFFSET 30

#define MAX_EXT_CSS_NUM     8

#define MAX_ZIDX_PANEL          30
#define MAX_ZIDX_CELL_IN_PANEL  50

#define FOCUS_RING_BOLD     2

#define MAX_IMAGE_CLIP_STR_LEN  36

#define MINI_MEMORY_FOR_PIC_DECODE      4000000

#endif /* LIMIT_H_ */
