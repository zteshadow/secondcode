
#include "cm_bin_queue.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_io.h"
#include "cm_debug.h"


typedef struct _CM_BIN_NODE
{
    S32 value;

    struct _CM_BIN_NODE *left_child;
    struct _CM_BIN_NODE *right_sibling;
    
} CM_BIN_NODE, *CM_BIN_TREE;

struct CMBinQueueStruct
{
    S32 capacity;
    S32 size;

    CM_BIN_TREE *table;
};


static void cm_bin_queue_free_tree(CM_BIN_TREE t);
static CM_BIN_TREE cm_bin_queue_merge_tree(CM_BIN_TREE t1,
                                           CM_BIN_TREE t2,
                                           S32 *more);
static CM_BIN_TREE cm_bin_queue_create_tree(S32 x);
static void cm_bin_queue_add_tree(CMBinQueue h, CM_BIN_TREE t, S32 i);


static void cm_bin_queue_free_tree(CM_BIN_TREE t)
{
    if (t != 0)
    {
        CM_BIN_TREE p = t->left_child;

        //free all child
        while (p != 0)
        {
            CM_BIN_TREE q = p->right_sibling;
            cm_bin_queue_free_tree(p);
            p = q;
        }

        //free root
        CM_FREE(t);
    }
}

static CM_BIN_TREE cm_bin_queue_merge_tree(CM_BIN_TREE t1,
                                           CM_BIN_TREE t2,
                                           S32 *more)
{
    CM_BIN_TREE p;

    CM_ASSERT(more != 0);

    *more = 0;
    if (t1 == 0)
    {
        p = t2;
    }
    else if (t2 == 0)
    {
        p = t1;
    }
    else
    {
        CM_BIN_TREE q;
        
        *more = 1;
        if (t1->value < t2->value)
        {
            q = t1->left_child;

            t1->left_child = t2;
            t2->right_sibling = q;

			p = t1;
        }
        else
        {
            p = cm_bin_queue_merge_tree(t2, t1, more);
        }
    }

    return p;
}

static CM_BIN_TREE cm_bin_queue_create_tree(S32 x)
{
    CM_BIN_TREE p = (CM_BIN_TREE)CM_MALLOC(sizeof(*p));

    if (p)
    {
        p->value = x;
        p->left_child = p->right_sibling = 0;
    }

    return p;
}

static void cm_bin_queue_add_tree(CMBinQueue h, CM_BIN_TREE t, S32 i)
{
    S32 j, more;

    CM_ASSERT(i < h->capacity);
    
    h->table[i] = cm_bin_queue_merge_tree(h->table[i],
                                          t,
                                          &more);

    if (more)
    {
    	for (j = i + 1; more && j < h->capacity; j++)
        {
            h->table[j] = cm_bin_queue_merge_tree(h->table[j],
                                                  h->table[j - 1],
                                                  &more);

            h->table[j - 1] = 0;
        }

        h->size = j + 1 > h->size ? j + 1 : h->size;
    }
    else
    {
        h->size = i + 1 > h->size ? i + 1 : h->size;
    }
}

CMBinQueue cm_bin_queue_create(S32 max)
{
    CMBinQueue q = (CMBinQueue)CM_MALLOC(sizeof(*q));

    if (q != 0)
    {
        q->table = (CM_BIN_TREE *)CM_MALLOC(sizeof(CM_BIN_TREE) * max);

        if (q->table != 0)
        {
            cm_memset(q->table, 0, sizeof(CM_BIN_TREE) * max);
            q->capacity = max;
            q->size = 0;
        }
        else
        {
            CM_FREE(q);
            q = 0;
        }
    }
    
    return q;
}

void cm_bin_queue_destory(CMBinQueue q)
{
    CM_ASSERT(q != 0);

    if (q != 0)
    {
        S32 i;

        for (i = 0; i < q->size; i++)
        {
            cm_bin_queue_free_tree(q->table[i]);
        }

        CM_FREE(q->table);
        CM_FREE(q);
    }
}

CMBinQueue cm_bin_queue_merge(CMBinQueue h1, CMBinQueue h2)
{
    CM_ASSERT(h1 != 0);
    
    if (h2 == 0)
    {
        return h1;
    }
    else
    {
        S32 i, j, more;

        for (i = 0; i < h2->size; i++)
        {
            cm_bin_queue_add_tree(h1, h2->table[i], i);
            h2->table[i] = 0;
        }
    }
    
    return h1;
}

CMBinQueue cm_bin_queue_insert(CMBinQueue h, S32 x)
{
    CM_BIN_TREE t;

    CM_ASSERT(h != 0);

    t = cm_bin_queue_create_tree(x);

    cm_bin_queue_add_tree(h, t, 0);

	return h;
}

CMBinQueue cm_bin_queue_delete_min(CMBinQueue h, S32 *min)
{
    return 0;
}

CM_POINT cm_bin_queue_print(CMBinQueue q, S32 x, S32 y, 
                                  S32 left, S32 *r,
                                  void *hdc)
{
    S32 i;
    CM_POINT p = {0};

    for (i = 0; i < q->size; i++)
    {
        CM_TRACE_5("[%d]: ", i);
        if (q->table[i] != 0)
        {
            CM_TRACE_5("%d\n", q->table[i]->value);
        }
        else
        {
            CM_TRACE_5("empty\n");
        }
    }
    
    return p;
}

