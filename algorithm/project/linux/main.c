
#include <stdio.h>

#include "cm_hash.h"
#include "cm_time.h"
#include "cm_algorithm_utility.h"


#define WAN_TEST_CNT (100000)


int main(int argc, char **argv)
{
    int wait;
    CM_TIME_VAL start, end;
    
    S32 *data1 = cm_data_create(WAN_TEST_CNT, 100 * WAN_TEST_CNT, CM_DATA_POSITIVE);
    S32 *data2 = cm_data_copy(data1, WAN_TEST_CNT);
    S32 *data3 = cm_data_copy(data1, WAN_TEST_CNT);
    S32 *data4 = cm_data_copy(data1, WAN_TEST_CNT);
    S32 *data5 = cm_data_copy(data1, WAN_TEST_CNT);

    cm_gettimeofday(&start);
    cm_insert_sort(data1, WAN_TEST_CNT);
    cm_gettimeofday(&end);
    //cm_data_print(data1, WAN_TEST_CNT);
    cm_data_dump(data1, WAN_TEST_CNT, "./1.txt");
    printf("insert time: %d(%d-%d)\n", end.tv_usec - start.tv_usec, end.tv_sec, start.tv_sec);

    cm_gettimeofday(&start);
    cm_shell_sort(data2, WAN_TEST_CNT);
    cm_gettimeofday(&end);
    //cm_data_print(data1, WAN_TEST_CNT);
    cm_data_dump(data2, WAN_TEST_CNT, "./2.txt");
    printf("shell time: %d(%d-%d)\n", end.tv_usec - start.tv_usec, end.tv_sec, start.tv_sec);

    cm_gettimeofday(&start);
    cm_heap_sort(data3, WAN_TEST_CNT);
    cm_gettimeofday(&end);
    //cm_data_print(data1, WAN_TEST_CNT);
    cm_data_dump(data3, WAN_TEST_CNT, "./3.txt");
    printf("heap time: %d(%d-%d)\n", end.tv_usec - start.tv_usec, end.tv_sec, start.tv_sec);

    cm_gettimeofday(&start);
    cm_merge_sort(data4, WAN_TEST_CNT);
    cm_gettimeofday(&end);
    //cm_data_print(data1, WAN_TEST_CNT);
    cm_data_dump(data4, WAN_TEST_CNT, "./4.txt");
    printf("merge time: %d(%d-%d)\n", end.tv_usec - start.tv_usec, end.tv_sec, start.tv_sec);

    cm_gettimeofday(&start);
    cm_quick_sort(data5, WAN_TEST_CNT);
    cm_gettimeofday(&end);
    //cm_data_print(data1, WAN_TEST_CNT);
    cm_data_dump(data5, WAN_TEST_CNT, "./5.txt");
    printf("quick time: %d(%d-%d)\n", end.tv_usec - start.tv_usec, end.tv_sec, start.tv_sec);

    cm_data_destory(data1);
    cm_data_destory(data2);
    cm_data_destory(data3);
    cm_data_destory(data4);
    cm_data_destory(data5);

    scanf("%d", &wait);
    return 0;
}

