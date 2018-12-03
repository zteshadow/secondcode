/*
 * cm_test.cpp
 *
 *  Created on: 16 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_sum.h"
#include "cm_algorithm_utility.h"
#include "cm_debug.h"
#include "cm_time.h"


//if 10000, N3 will take 999 seconds
//3000, N3 will take 27 seconds
#define CM_BASE_COUNT (7)


void cm_sum_test(void)
{
    S32 *data = cm_data_create(CM_BASE_COUNT, 10, CM_DATA_MIXED);
    cm_data_print((const S32 *)data, CM_BASE_COUNT);

    CM_TIME s1 = cm_time(0);
    S32 max_sum1 = cm_max_sequence_sum_N3((const S32 *)data, CM_BASE_COUNT);
    CM_TIME e1 = cm_time(0);
    CM_TRACE_0("N3: %ld - %ld = %ld", e1, s1, e1 - s1);
    CM_TRACE_0("N3: %d\n", max_sum1);

    s1 = cm_time(0);
    max_sum1 = cm_max_sequence_sum_NLogN((const S32 *)data, 0, CM_BASE_COUNT - 1);
    e1 = cm_time(0);
    CM_TRACE_0("NLogN: %ld - %ld = %ld", e1, s1, e1 - s1);
    CM_TRACE_0("NLogN: %d\n", max_sum1);

    s1 = cm_time(0);
    max_sum1 = cm_max_sequence_sum_N((const S32 *)data, CM_BASE_COUNT);
    e1 = cm_time(0);
    CM_TRACE_0("N: %ld - %ld = %ld", e1, s1, e1 - s1);    
    CM_TRACE_0("N: %d\n", max_sum1);

    S32 min = cm_min_sequence_sum_N(data, CM_BASE_COUNT);
    CM_TRACE_0("Min: %d\n", min);
}

