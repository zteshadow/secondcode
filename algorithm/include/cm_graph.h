/*
 * cm_graph..h
 *
 *  Created on: 23 April 2012
 *      Author: samuel.song.bc@gmail.com
 */

#ifndef _CM_GRAPH_H
#define _CM_GRAPH_H


#include "cm_data_type.h"


struct CM_GRAPH_STRUCT_EXT;
typedef struct CM_GRAPH_STRUCT_EXT *CM_GRAPH;


#ifdef __cplusplus
extern "C"
{
#endif


CM_GRAPH cm_graph_init(S32 cnt);
void cm_graph_deinit(CM_GRAPH g);

void cm_graph_connect(CM_GRAPH g, S32 i, S32 j);
void cm_graph_connect_weight(CM_GRAPH g, S32 i, S32 j, S32 w);

void cm_graph_sort(CM_GRAPH g);
S32 cm_graph_is_no_cycle(CM_GRAPH g);

//Un-Weighted Path Length
void cm_graph_find_UWPL(CM_GRAPH g, S32 i);
void cm_graph_find_UWPL_quick(CM_GRAPH g, S32 i);

//Weighted Path Length
void cm_graph_find_WPL_Dijkstra(CM_GRAPH g, S32 i);
void cm_graph_find_WPL_Dijkstra_no_cycle(CM_GRAPH g, S32 i);

void cm_graph_find_WPL_negative(CM_GRAPH g, S32 i);

//for test
void cm_graph_print(CM_GRAPH g);


#ifdef __cplusplus
}
#endif


#endif //_CM_GRAPH_H

