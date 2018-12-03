/*
 * cm_poly.h
 *
 *  Created on: 26 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_POLY_H
#define _CM_POLY_H


#include "cm_data_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


//will modify string of 'p' for convienient
void *cm_poly_create_by_string(S8 *p);
void *cm_poly_add(void *p1, void *p2);
void *cm_poly_multi(void *p1, void *p2);
void cm_poly_print(void *poly);
void cm_poly_destory(void *poly);


#ifdef __cplusplus
}
#endif


#endif //_CM_POLY_H

