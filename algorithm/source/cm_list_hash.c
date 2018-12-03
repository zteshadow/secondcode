
#include "cm_hash.h"
#include "cm_list.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_time.h"
#include "cm_debug.h"

#include "cm_binary_tree.h"


typedef void *HASH_NODE;

struct _HASH_TABLE
{
    S32 size;
    HASH_NODE *table;
};


static S32 cm_nex_prime(S32 d);
static S32 cm_hash(S32 d, S32 size);
static U32 cm_BKDHash(const S8 *data);


static S32 cm_nex_prime(S32 d)
{
    //next prime
    return d;
}

static S32 cm_hash(S32 d, S32 size)
{
    return d % size;
}

static U32 cm_BKDHash(const S8 *data)
{
    U32 seed = 131; //31, 131, 1313
    U32 hash = 0;

    while (*data != 0)
    {
        //hash = hash * seed + (*data++);

        //131 = 128 + 2 + 1
        hash = (hash << 7) + (hash << 1) + hash + (*data++);
    }

    return hash & 0x7fffffff;
}

HASH_TABLE cm_hash_init(S32 size)
{
    HASH_TABLE p;

    CM_ASSERT(size > 0);

    p = (HASH_TABLE)CM_MALLOC(sizeof(*p));
    if (p != 0)
    {
        size = cm_nex_prime(size);
        p->size = size;
        p->table = (HASH_NODE *)CM_MALLOC(sizeof(HASH_NODE) * size);

        CM_ASSERT(p->table != 0);
        if (p->table != 0)
        {
            S32 i;
            for (i = 0; i < size; i++)
            {
                void *q = cm_list_create();
                CM_ASSERT(q != 0);
                p->table[i] = q;
            }
        }
        else
        {
            CM_FREE(p);
            p = 0;
        }
    }

    return p;
}

void cm_hash_destroy(HASH_TABLE h)
{
    CM_ASSERT(h != 0);
    if (h != 0)
    {
        S32 size = h->size, i;
        for (i = 0; i < size; i++)
        {
            cm_list_destory(h->table[i]);
        }
        CM_FREE(h->table);
        CM_FREE(h);
    }
}

void cm_hash_insert(HASH_TABLE h, void *data)
{
    S32 d = (S32)data;
    S32 i = cm_hash(d, h->size);

    void *list = h->table[i];
    cm_list_ahead_item(list, data);
}

HASH_POSITION cm_hash_find(HASH_TABLE h, void *data)
{
    S32 d = (S32)data;
    S32 i = cm_hash(d, h->size);

    void *list = h->table[i];
    if (cm_list_find_item(list, data))
    {
        return i;
    }
    else
    {
        return -1;
    }
}

void cm_hash_delete(HASH_TABLE h, void *data)
{
    S32 d = (S32)data;
    S32 i = cm_hash(d, h->size);

    void *list = h->table[i];
    cm_list_delete_item(list, data);
}

void cm_hash_print(HASH_TABLE h)
{
    S32 i;

    CM_ASSERT(h != 0);

    CM_TRACE_5("hash table:\n");
    for (i = 0; i < h->size; i++)
    {
        CM_TRACE_5("%d: ", i);
        cm_list_print(h->table[i]);
        CM_TRACE_5("\n");
    }
}

void cm_hash_test(const S8 *fname)
{
    SLong total = 0;
    S32 cnt = 0, collision = 0;
    
    CM_HANDLE *fp = cm_fopen(fname, "rb");
    if (fp != 0)
    {
        U32 code;
        S8 url[5000];
        SearchTree tree = 0;
        
        while (cm_fgets(url, 5000, fp) != 0)
        {
            CM_TRACE_0("data(%d): %s", cnt, url);
            
            code = cm_BKDHash((const S8 *)url);

            if (cm_binary_tree_find(tree, (S32)code))
            {
                CM_TRACE_0("collision: %d\n", collision);
				collision++;
            }
            else
            {
                tree = cm_binary_tree_insert(tree, (S32)code);
            }
		
            cnt++;
        }

        cm_binary_tree_free(tree);
    }

    CM_TRACE_0("total time: %d(%d)", total, cnt);
}

