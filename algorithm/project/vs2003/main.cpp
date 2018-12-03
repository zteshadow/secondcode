
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cm_poly.h"
#include "cm_string.h"
#include "cm_sort.h"
#include "cm_algorithm_misc.h"
#include "cm_queue.h"
#include "cm_tree.h"
#include "cm_lib.h"
#include "cm_debug.h"
#include "cm_binary_tree.h"
#include "cm_algorithm_utility.h"
#include "cm_mem_check.h"
#include "cm_hash.h"
#include "cm_bin_queue.h"
#include "cm_union_find.h"
#include "cm_graph.h"
#include "cm_bin_packing.h"


#define CM_IS_BLANK(x) ((x) == ' ' || (x) == '\t')


static void *s_dir_root;
static void *s_dir_current;


extern void cm_sum_test(void);


static void poly_test(char *p1, char *p2);
static void cm_general_tree_test(void);
static void cm_list_directory(void *root);
static void cm_list_dir(void *node, int depth);
static void cm_tree_free_node(void *data);

//return 1 if equal
static S32 cm_tree_compare_node(void *data1, void *data2);

static void cm_binary_tree_test(void);


static void poly_test(char *p1, char *p2)
{
    printf("\np1: %s\np2: %s\n", p1, p2);
    void *poly1 = cm_poly_create_by_string(p1);
    void *poly2 = cm_poly_create_by_string(p2);

    cm_poly_print(poly1);
    cm_poly_print(poly2);
#if 0
    void *sum = cm_poly_add(poly1, poly2);
    cm_poly_print(sum);

    void *multi = cm_poly_multi(poly1, poly2);
    cm_poly_print(multi);
    cm_poly_destory(sum);
    cm_poly_destory(multi);
#endif

    cm_poly_destory(poly1);
    cm_poly_destory(poly2);
}

static void cm_general_tree_test(void)
{
    void *node;
    S8 line[200];

    s_dir_root = cm_tree_create_node(0);
    s_dir_current = s_dir_root;

    printf("welcome to tree test:\n");
    while (1)
    {
        memset(line, 0, sizeof(line));
        fgets(line, 200 - 1, stdin);

        if (strncmp(line, "mkdir", 5) == 0)
        {
            S8 *p = line + 5;
            
            while (CM_IS_BLANK(*p))
            {
                p++;
            }

            //printf("mkdir: %s\n", p);

            node = cm_tree_create_node((void *)(cm_strdup(p)));
            cm_tree_append(s_dir_current, node);
        }
        else if (strncmp(line, "cd", 2) == 0)
        {
            S8 *p = line + 2;
            while (CM_IS_BLANK(*p))
            {
                p++;
            }

            //printf("cd: %s\n", p);

            if (strncmp(p, "..", 2) == 0)
            {
                node = cm_tree_get_parent(s_dir_current);
                if (node)
                {
                    s_dir_current = node;
                }
            }
            else
            {
                node = cm_tree_get_child_by_value(s_dir_current, (void *)p, cm_tree_compare_node);
                //CM_ASSERT(node != 0);

				if (node != 0)
				{
                    //if (!cm_tree_node_is_leaf(node))
                    {
                        s_dir_current = node;
                    }
				}
            }
        }
        else if (strncmp(line, "ls", 2) == 0)
        {
            if (s_dir_current != 0)
            {
                cm_list_directory(s_dir_current);
            }
        }
        else if (strncmp(line, "exit", 4) == 0)
        {
            printf("good bye\n");
            break;
        }
    }

    cm_tree_destory(s_dir_root, cm_tree_free_node);
    s_dir_root = 0;
    s_dir_current = 0;
}

static void cm_list_directory(void *root)
{
    cm_list_dir(root, 0);
}

static void cm_list_dir(void *node, int depth)
{
    void *p;
    void *data;
    int i;

    //draw 'tab'
    for (i = 0; i < depth; i++)
    {
        printf("    ");
    }

    //draw node
    data = cm_tree_get_node_value(node);
    if (data)
    {
        printf("%s", (char *)data);
    }
    
    cm_tree_start_get_child(node);
    while ((p = cm_tree_get_next_child(node)) != 0)
    {
        cm_list_dir(p, depth + 1);
    }
}

static void cm_tree_free_node(void *data)
{
    CM_FREE(data);
}

static S32 cm_tree_compare_node(void *data1, void *data2)
{
    S32 value = 0;

    if (data1 && data2)
    {
        if (cm_strcmp((S8 *)data1, (S8 *)data2) == 0)
        {
            value = 1;
        }
    }
    
    return value;
}

static void cm_binary_tree_test(void)
{
    SearchTree t = 0;

#if 0
    S32 *array = cm_data_create(10, 100, CM_DATA_POSITIVE);

    if (array)
    {
        for (S32 i = 0; i < 10; i++)
        {
            t = cm_binary_tree_insert(t, array[i]);
        }
        
        cm_data_destory(array);
    }
#endif

#if 1
    t = cm_binary_tree_insert(t, 4);
	t = cm_binary_tree_insert(t, 3);
	t = cm_binary_tree_insert(t, 2);
	t = cm_binary_tree_insert(t, 1);
	//t = cm_binary_tree_delete(t, 1);
	//t = cm_binary_tree_insert(t, 5);
	//st = cm_binary_tree_insert(t, 6);
#endif

    cm_binary_tree_print(t);
	cm_binary_tree_free(t);
}

#define _MAX_TEST_CNT 10000
static void cm_main_hash_test(void)
{
    S32 i, cnt = 0;
    S32 *data = cm_data_create(_MAX_TEST_CNT, 10 * _MAX_TEST_CNT, CM_DATA_POSITIVE);

    {
        HASH_TABLE h = cm_hash_init(_MAX_TEST_CNT);

#if 1
        for (i = 0; i < _MAX_TEST_CNT; i++)
        {
            cnt += cm_hash_linear_insert(h, (void *)(data[i]));
        }
#else
        cnt += cm_hash_double_insert(h, (void *)20);
        cnt += cm_hash_double_insert(h, (void *)43);
        cnt += cm_hash_double_insert(h, (void *)66);
        cnt += cm_hash_double_insert(h, (void *)89);
        cnt += cm_hash_double_insert(h, (void *)69);
        cm_hash_print(h);
#endif

        printf("%d collisions in total\n", cnt);
        
        cm_hash_destroy(h);
    }
    {
        HASH_TABLE h = cm_hash_init(_MAX_TEST_CNT);

        for (i = 0; i < _MAX_TEST_CNT; i++)
        {
            cnt += cm_hash_square_insert(h, (void *)(data[i]));
        }

        printf("%d collisions in total\n", cnt);
        
        cm_hash_destroy(h);
    }
    {
        HASH_TABLE h = cm_hash_init(_MAX_TEST_CNT);

        for (i = 0; i < _MAX_TEST_CNT; i++)
        {
            cnt += cm_hash_double_insert(h, (void *)(data[i]));
        }

        printf("%d collisions in total\n", cnt);
        
        cm_hash_destroy(h);
    }
    cm_data_destory(data);
}

#define MAX_SORT_CNT 1000

static cm_graph_test_1(void)
{
    CM_GRAPH g = cm_graph_init(7);

    cm_graph_connect_weight(g, 1, 4, 1);
    cm_graph_connect_weight(g, 1, 2, 2);

    cm_graph_connect_weight(g, 2, 4, 3);
    cm_graph_connect_weight(g, 2, 5, 10);

    cm_graph_connect_weight(g, 3, 1, 4);
    cm_graph_connect_weight(g, 3, 6, 5);

    cm_graph_connect_weight(g, 4, 3, 2);
    cm_graph_connect_weight(g, 4, 6, 8);
    cm_graph_connect_weight(g, 4, 7, 4);
    cm_graph_connect_weight(g, 4, 5, 2);

    cm_graph_connect_weight(g, 5, 7, 6);

    cm_graph_connect_weight(g, 7, 6, 1);

    cm_graph_find_WPL_Dijkstra(g, 1);
    
    cm_graph_print(g);
    cm_graph_deinit(g);
}

static cm_graph_test_2(void)
{
    CM_GRAPH g = cm_graph_init(7);

    cm_graph_connect_weight(g, 1, 4, 1);
    cm_graph_connect_weight(g, 1, 2, 2);

    cm_graph_connect_weight(g, 2, 4, 3);
    cm_graph_connect_weight(g, 2, 5, -10);

    cm_graph_connect_weight(g, 3, 1, 4);
    cm_graph_connect_weight(g, 3, 6, 5);

    //cm_graph_connect_weight(g, 4, 3, 2);
    cm_graph_connect_weight(g, 4, 6, 8);
    cm_graph_connect_weight(g, 4, 7, 4);
    cm_graph_connect_weight(g, 4, 5, 2);

    cm_graph_connect_weight(g, 5, 7, 6);

    cm_graph_connect_weight(g, 7, 6, 1);

    cm_graph_find_WPL_negative(g, 1);
    
    cm_graph_print(g);
    cm_graph_deinit(g);
}

static cm_graph_test_3(void)
{
    CM_GRAPH g = cm_graph_init(7);

    cm_graph_connect_weight(g, 1, 4, 1);
    cm_graph_connect_weight(g, 1, 2, 2);

    cm_graph_connect_weight(g, 2, 4, 3);
    cm_graph_connect_weight(g, 2, 5, -10);

    cm_graph_connect_weight(g, 3, 1, 4);
    cm_graph_connect_weight(g, 3, 6, 5);

    cm_graph_connect_weight(g, 4, 3, 2);    //cycle
    cm_graph_connect_weight(g, 4, 6, 8);
    cm_graph_connect_weight(g, 4, 7, 4);
    cm_graph_connect_weight(g, 4, 5, 2);

    cm_graph_connect_weight(g, 5, 7, 6);

    cm_graph_connect_weight(g, 7, 6, 1);

    if (cm_graph_is_no_cycle(g))
    {
        CM_TRACE_0("no cycle\n");
    }
    else
    {
        CM_TRACE_0("with cycle\n");
    }
    
    //cm_graph_print(g);
    cm_graph_deinit(g);
}

static void cm_bin_packing_test()
{
    cm_bin_packing_next_fit();
}

int main(int argc, char **argv)
{
	int value;
    int a[4];

    cm_mem_check_init(1000);
    
#if 0
    cm_sum_test();

    char p1[] = "10x1000+x+1";
    char p2[] = "5x999-45x34+2x";
    poly_test(p1, p2);

    char p3[] = "x2-x-1";
    char p4[] = "x999-45x34-x";
    poly_test(p4, p3);


    int data[] = {64, 8, 216, 512, 27, 729, 0, 1, 343, 125};
    cm_bucket_sort(data, sizeof(data) / sizeof(int), 3);
#endif

/*
    cm_algorithm_check_symbol_pair("f:\\test.c");
*/

#if 0
{
    char *p = "6523+8*+3+*";
    value = cm_algorithm_postfix_value(p);
    printf("value of %s is %d\n", p, value);

    char *infix = "1+2*3+(4*5+6)*7";
    char postfix[16];
    value = cm_algorithm_infix_to_postfix(infix, postfix);
    postfix[value] = 0;
    printf("infix: %s\npostfix: %s(%d)\n", infix, postfix, cm_algorithm_postfix_value(postfix));   
}

{
    void *q = cm_queue_create((void *)51);
    if (q)
    {
        for (int i = 0; i < 50; i++)
        {
            cm_queue_enqueue(q, (void *)i);
        }

        while (!cm_queue_is_empty(q))
        {
            S32 value = (S32)cm_queue_dequeue(q);
            printf("%d ", value);
        }
        cm_queue_destory(q);
    }
}
#endif

    //cm_general_tree_test();

    //cm_binary_tree_test();

    //cm_main_hash_test();

#if 0
    {
        S32 r;

        CMBinQueue h = cm_bin_queue_create(20);

        cm_bin_queue_insert(h, 1);
		cm_bin_queue_insert(h, 2);
		cm_bin_queue_insert(h, 3);
		cm_bin_queue_insert(h, 4);
		cm_bin_queue_insert(h, 5);

        cm_bin_queue_print(h, 0, 0, 0, &r, 0);
        cm_bin_queue_destory(h);

        /*
        cm_bin_queue_insert(&h1, 1);


        cm_bin_queue_destory(&h1);
        */
    }
#endif

    //cm_hash_test("C:\\samuel\\freemind\\source\\algorithm\\project\\test.txt");

#if 0
    //sort test
    {
        S32 *data = cm_data_create(MAX_SORT_CNT, 100000, CM_DATA_POSITIVE);

        cm_data_print(data, MAX_SORT_CNT);
        //cm_insert_sort(data, 10);
        //cm_shell_sort(data, 10);
        //cm_heap_sort(data, 10);
        //cm_merge_sort(data, 10);
		cm_quick_sort(data, MAX_SORT_CNT);
        cm_data_print(data, MAX_SORT_CNT);
        cm_data_destory(data);
    }
#endif

#if 0
    //union find
    {
        S32 set[9];

        cm_union_find_init(set, 8);

        cm_union(set, 5, 6);
        cm_union(set, 7, 8);
        cm_union(set, 5, 7);
        cm_union(set, 4, 5);

        cm_dump_union_find(set, 8);
    }
#endif

    //
    //cm_graph_test_3();

    cm_bin_packing_test();
    
	cm_mem_check();
	cm_mem_check_deinit();

    printf("\nhello, world!\n");
	scanf("%d", &value);

    return 0;
}

