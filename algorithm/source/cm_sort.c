
#include "cm_sort.h"

#include "cm_debug.h"
#include "cm_lib.h"
#include "cm_string.h"


#define CM_SIGNIFICANT_BIT(x, base) (((x) / base) % 10)
#define CM_LEFT_CHILD(i) (2 * (i) + 1)
#define _SWAP(a, i, j)\
{\
    S32 tmp = (a)[(i)];\
    (a)[(i)] = (a)[(j)];\
    (a)[(j)] = tmp;\
}


//为了删除方便，设计双向链表，且添加表头
typedef struct _node
{
    S32 value;
    struct _node *next;
    struct _node *pre;

} CM_BUCKET_NODE;


//为了追加方便，设计了tail
typedef struct
{
    CM_BUCKET_NODE *head;
    CM_BUCKET_NODE *tail;

} CM_BUCKET_STRUCT;


static CM_BUCKET_NODE *cm_make_bucket_node(S32 value);
static void cm_process_list(CM_BUCKET_STRUCT *bucket, S32 index, S32 base);
static void cm_append_bucket_node(CM_BUCKET_STRUCT *bucket, CM_BUCKET_NODE *node);

static void cm_init_bucket_list(CM_BUCKET_STRUCT *bucket, S32 size);
static void cm_print_bucket_list(CM_BUCKET_STRUCT *bucket, S32 size);
static void cm_deinit_bucket_list(const CM_BUCKET_STRUCT *bucket, S32 size);

static S32 cm_increment_sort(S32 *data, S32 size, S32 inc);

static void cm_heap_perc_down(S32 *data, S32 index, S32 size);
static S32 cm_merge_sort_ext(S32 *data, S32 *output, S32 begin, S32 end);
static void cm_merge(S32 *data, S32 *tmp, S32 begin, S32 mid, S32 end);

static S32 _find_mid(S32 *data, S32 begin, S32 end);
static void cm_quick_sort_ext(S32 *data, S32 begin, S32 end);


static CM_BUCKET_NODE *cm_make_bucket_node(S32 value)
{
    CM_BUCKET_NODE *node = (CM_BUCKET_NODE *)CM_MALLOC(sizeof(CM_BUCKET_NODE));
    if (node)
    {
        node->value = value;
        node->next = 0;
        node->pre = 0;
    }

    return node;
}

static void cm_process_list(CM_BUCKET_STRUCT *bucket, S32 index, S32 base)
{
    CM_ASSERT(bucket != 0 && index >= 0 && base > 0);

    if (bucket != 0 && index >= 0 && base > 0)
    {
        CM_BUCKET_STRUCT *list = bucket + index;
        CM_BUCKET_NODE *p = list->head->next, *q;

        while (p)
        {
			S32 value = CM_SIGNIFICANT_BIT(p->value, base);
            q = p->next;

            if (value != index) //remove and append to list[value]
            {
                //remove
                p->pre->next = q;

                if (q)
                {
                    q->pre = p->pre;
                }
                else    //remove tail
                {
                    list->tail = p->pre;
                }

                //append
                cm_append_bucket_node(bucket + value, p);
            }
            p = q;
        }
    }
}

static void cm_append_bucket_node(CM_BUCKET_STRUCT *bucket, CM_BUCKET_NODE *node)
{
    CM_ASSERT(bucket != 0 && node != 0);

    if (bucket != 0 && node != 0)
    {
        CM_BUCKET_NODE *tail = bucket->tail;

        node->next = 0;
        node->pre = tail;

        tail->next = node;
        bucket->tail = node;
    }
}

static void cm_init_bucket_list(CM_BUCKET_STRUCT *bucket, S32 size)
{
	S32 i;

    CM_ASSERT(bucket != 0);

    if (bucket != 0)
    {
        for (i = 0; i < size; i++)
        {
            CM_BUCKET_NODE *node = cm_make_bucket_node(-1);
            bucket[i].head = node;
            bucket[i].tail = node;
        }
    }
}

static void cm_print_bucket_list(CM_BUCKET_STRUCT *bucket, S32 size)
{
    CM_ASSERT(bucket != 0);

    if (bucket != 0)
    {
		S32 i;

        for (i = 0; i < size; i++)
        {
            CM_BUCKET_NODE *p = bucket[i].head->next;
			CM_TRACE_5("[%d]: ", i);

            while (p)
            {
                CM_BUCKET_NODE *q = p->next;
                CM_TRACE_0("%d ", p->value);
                p = q;
            }
            CM_TRACE_5("\n");
        }
    }
}

static void cm_deinit_bucket_list(const CM_BUCKET_STRUCT *bucket, S32 size)
{
    CM_ASSERT(bucket != 0);

    if (bucket != 0)
    {
		S32 i;

        for (i = 0; i < size; i++)
        {
            CM_BUCKET_NODE *p = bucket[i].head;
            while (p)
            {
                CM_BUCKET_NODE *q = p->next;
                CM_FREE(p);
                p = q;
            }        
        }
    }
}

static S32 cm_increment_sort(S32 *data, S32 size, S32 inc)
{
    S32 i, j;

    for (i = inc; i < size; i += inc)
    {
        S32 tmp = data[i];

        for (j = i; j > 0 && tmp < data[j - inc]; j -= inc)
        {
            data[j] = data[j - inc];
        }

        data[j] = tmp;
    }
    
    return 0;
}

static void cm_heap_perc_down(S32 *data, S32 index, S32 size)
{
    S32 i, child;
    S32 value = data[index];

    for (i = index; CM_LEFT_CHILD(i) <= size; i = child)
    {
        child = CM_LEFT_CHILD(i);

        if (child < size && data[child] < data[child + 1])
        {
            child++;
        }

        if (value < data[child])
        {
            data[i] = data[child];
        }
        else
        {
            break;
        }
    }

    data[i] = value;
}

static S32 cm_merge_sort_ext(S32 *data, S32 *output, S32 begin, S32 end)
{
    if (begin < end)
    {
        S32 mid = (begin + end) / 2;

        cm_merge_sort_ext(data, output, begin, mid);
        cm_merge_sort_ext(data, output, mid + 1, end);

        //merge
        cm_merge(data, output, begin, mid, end);
    }
    
    return 0;
}

static void cm_merge(S32 *data, S32 *tmp, S32 begin, S32 mid, S32 end)
{
    S32 l_pos = begin;
    S32 r_pos = mid + 1;
    S32 p = 0;

    while (l_pos <= mid && r_pos <= end)
    {
        if (data[l_pos] < data[r_pos])
        {
            tmp[p++] = data[l_pos++];
        }
        else
        {
            tmp[p++] = data[r_pos++];
        }
    }

    while (l_pos <= mid)
    {
        tmp[p++] = data[l_pos++];
    }

    while (r_pos <= end)
    {
        tmp[p++] = data[r_pos++];
    }

    for (mid = 0; mid < p; mid++)
    {
        data[begin + mid] = tmp[mid]; //***start from begin
    }
}

static S32 _find_mid(S32 *data, S32 begin, S32 end)
{
    S32 mid = (begin + end) / 2;

    if (data[begin] > data[mid])
    {
        _SWAP(data, begin, mid);
    }

    if (data[begin] > data[end])
    {
        _SWAP(data, begin, end);
    }

    if (data[mid] > data[end])
    {
        _SWAP(data, mid, end);
    }

    _SWAP(data, mid, end - 1);
    
    return data[end - 1];    
}

static void cm_quick_sort_ext(S32 *data, S32 begin, S32 end)
{
    S32 cnt = end - begin + 1;

    if (cnt > 2)
    {
        S32 pivot = _find_mid(data, begin, end);

        S32 i = begin;
        S32 j = end - 1;

        while (1)
        {
            while (data[++i] < pivot) {}
            while (data[--j] > pivot) {}

            if (i < j)
            {
                _SWAP(data, i, j);
            }
            else
            {
                break;
            }
        }

        _SWAP(data, i, end - 1);

        cm_quick_sort_ext(data, begin, i - 1);
        cm_quick_sort_ext(data, i + 1, end);
    }
    else
    {
        if (cnt == 2)
        {
            if (data[begin] > data[end])
            {
                _SWAP(data, begin, end);
            }
        }
    }
}


S32 cm_bucket_sort(const S32 *data, S32 size, S32 power)
{
	S32 i, j, base;
    CM_BUCKET_STRUCT bucket[10];

	CM_ASSERT(data != 0 && size > 0 && power > 0);
    cm_init_bucket_list(bucket, 10);

    //into bucket
    for (i = 0; i < size; i++)
    {
        S32 value = CM_SIGNIFICANT_BIT(data[i], 1);
        CM_BUCKET_NODE *node = cm_make_bucket_node(data[i]);

        if (node)
        {
            CM_BUCKET_STRUCT *p = bucket + value;
            cm_append_bucket_node(p, node);
        }
    }
    CM_TRACE_5("\nsort:\n");
    cm_print_bucket_list(bucket, 10);
    CM_TRACE_5("\n");

    //pass through
    base = 1;
    for (i = 1; i < power; i++)
    {
        base *= 10;
        for (j = 0; j < 10; j++)
        {
            cm_process_list(bucket, j, base);
        }
        
        CM_TRACE_5("\nsort:\n");
        cm_print_bucket_list(bucket, 10);
        CM_TRACE_5("\n");
    }

    CM_TRACE_5("\nsort:\n");
    cm_print_bucket_list(bucket, 10);
    CM_TRACE_5("\n");

    //free
    cm_deinit_bucket_list(bucket, 10);

    return 0;
}

S32 cm_insert_sort(S32 *data, S32 size)
{
    cm_increment_sort(data, size, 1);

    return 0;
}

S32 cm_shell_sort(S32 *data, S32 size)
{
    //sedgewick
    S32 i, table[] = {1, 5, 19, 41, 109};

    for (i = 0; i < 5; i++)
    {
        cm_increment_sort(data, size, table[i]);
    }
    
    return 0;
}

S32 cm_heap_sort(S32 *data, S32 size)
{
    S32 i;

    //build heap
    for (i = size / 2; i >= 0; i--)
    {
        cm_heap_perc_down(data, i, size - 1);
    }

    for (i = size - 1; i > 0; i--)
    {
        S32 tmp = data[i];

        data[i] = data[0];
        data[0] = tmp;

        cm_heap_perc_down(data, 0, i - 1);
    }

	return 0;
}

S32 cm_merge_sort(S32 *data, S32 size)
{
    S32 *tmp = (S32 *)CM_MALLOC(sizeof(S32) * size);

    if (tmp != 0)
    {
        cm_merge_sort_ext(data, tmp, 0, size - 1);
        CM_FREE(tmp);
    }

    return 0;
}

S32 cm_quick_sort(S32 *data, S32 size)
{
    cm_quick_sort_ext(data, 0, size - 1);

    return 0;
}

