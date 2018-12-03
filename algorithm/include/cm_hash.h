/*
 * cm_hash.h
 *
 *  Created on: 7 Feb 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_HASH_H
#define _CM_HASH_H


#include "cm_data_type.h"


typedef S32 HASH_POSITION;

struct _HASH_TABLE;
typedef struct _HASH_TABLE* HASH_TABLE;


#ifdef __cplusplus
extern "C"
{
#endif


HASH_TABLE cm_hash_init(S32 size);
void cm_hash_destroy(HASH_TABLE h);

//return collision count
S32 cm_hash_linear_insert(HASH_TABLE h, void *data);

S32 cm_hash_square_insert(HASH_TABLE h, void *data);
S32 cm_hash_double_insert(HASH_TABLE h, void *data);

//HASH_POSITION cm_hash_find(HASH_TABLE h, void *data);

void cm_hash_delete(HASH_TABLE h, void *data);

//for test
void cm_hash_print(HASH_TABLE h);
void cm_hash_test(const S8 *fname);


#ifdef __cplusplus
}
#endif


#endif //_CM_HASH_H

