/*
 * cm_bin_packing.c
 *
 *  Created on: 17 May 2012
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_bin_packing.h"
#include "cm_debug.h"

typedef struct
{
    double size;
    S32 box;    //start from 1
    
} CM_BOX_ITEM;


void cm_bin_packing_next_fit(void)
{
    S32 box = 1;
    double total = 0.0;
    CM_BOX_ITEM data[] = {{0.2, 0}, {0.5, 0}, {0.4, 0}, {0.7, 0}, {0.1, 0}, {0.3, 0}, {0.8, 0}};
    S32 i, j, cnt = sizeof(data) / sizeof(CM_BOX_ITEM);

    for (i = 0; i < cnt; i++)
    {
        total += data[i].size;

        if (total <= 1.0)
        {
            data[i].box = box;
        }
        else
        {
            data[i].box = ++box;
            total = data[i].size;
        }
    }

    CM_TRACE_0("%d box in total", box);
    for (i = 1, j = 0; i <= box; i++)
    {
        CM_TRACE_0("\n%d:\t", i);
        while (data[j].box == i)
        {
            CM_TRACE_0(", %f", data[j++].size);
        }
    }
    CM_TRACE_0("\n");
}

