
#include "cm_hash.h"
#include "cm_list.h"

#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"


typedef enum
{
    NODE_EMPTY,
    NODE_USED,
    NODE_DELETED

} HASH_NODE_STATE;

typedef struct
{
    HASH_NODE_STATE state;
    void *data;

} HASH_NODE;

struct _HASH_TABLE
{
    S32 size;
    HASH_NODE *table;
};


static S32 cm_is_prime(S32 d);
static S32 cm_nex_prime(S32 d);
static S32 cm_pre_prime(S32 d);
static U32 cm_BKD_hash(const S8 *data);

static HASH_POSITION cm_hash_linear_find_ext(HASH_TABLE h, void *data, S32 *collision);
static HASH_POSITION cm_hash_square_find_ext(HASH_TABLE h, void *data, S32 *collision);
static HASH_POSITION cm_hash_double_find_ext(HASH_TABLE h, void *data, S32 *collision);


static S32 cm_is_prime(S32 d)
{
    S32 i, end = d >> 1;

    CM_ASSERT(d > 0);

    for (i = 2; i <= end; i++)
    {
        if ((d % i) == 0)
        {
            return 0;
        }
    }

    return 1;
}

static S32 cm_nex_prime(S32 d)
{
    CM_ASSERT(d > 0);

    while (++d)
    {
        if (cm_is_prime(d))
        {
            return d;
        }
    }

    CM_ASSERT(0);
}

static S32 cm_pre_prime(S32 d)
{
    CM_ASSERT(d > 0);

    while (--d)
    {
        if (cm_is_prime(d))
        {
            return d;
        }
    }

    CM_ASSERT(0);
}

static U32 cm_BKD_hash(const S8 *data)
{
    U32 seed = 131;//31, 131, 1313, 13131, 
    U32 hash = 0;

    while (*data != 0)
    {
        hash = hash * seed + (*data++);
    }

    return hash & 0x7FFFFFFF;
}

static HASH_POSITION cm_hash_linear_find_ext(HASH_TABLE h, void *data, S32 *collision)
{
    S32 index = (S32)data % h->size, cnt = 0;
    HASH_NODE *node = &(h->table[index]);

    while (node->state != NODE_EMPTY && node->data != data)
    {
        cnt++;
        index = (index + 1) % h->size;
        node = &(h->table[index]);
    }

    *collision = cnt;
    return index;
}

static HASH_POSITION cm_hash_square_find_ext(HASH_TABLE h, void *data, S32 *collision)
{
    S32 index = (S32)data % h->size, cnt = 0;
    HASH_NODE *node = &(h->table[index]);

    while (node->state != NODE_EMPTY && node->data != data)
    {
        cnt++;
        index = (index + (cnt << 1) - 1) % h->size;
        node = &(h->table[index]);
    }

    *collision = cnt;
    return index;
}

static HASH_POSITION cm_hash_double_find_ext(HASH_TABLE h, void *data, S32 *collision)
{
    S32 index = (S32)data % h->size, cnt = 0;
    HASH_NODE *node = &(h->table[index]);

    if (node->state == NODE_EMPTY || node->data == data)
    {
        //find
    }
    else
    {
        S32 size = cm_pre_prime(h->size);

        while (node->state != NODE_EMPTY && node->data != data)
        {
            cnt++;
            index = (index + (size - (S32)data % size)) % h->size;
            node = &(h->table[index]);
        }
    }

    *collision = cnt;
    return index;
}

HASH_TABLE cm_hash_init(S32 size)
{
    HASH_TABLE p;

    CM_ASSERT(size > 0);

    p = (HASH_TABLE)CM_MALLOC(sizeof(*p));
    if (p != 0)
    {
        size = cm_nex_prime(size * 2);  //at least size * 2
        p->size = size;
        p->table = (HASH_NODE *)CM_MALLOC(sizeof(HASH_NODE) * size);

        CM_ASSERT(p->table != 0);
        if (p->table != 0)
        {
            cm_memset(p->table, 0, sizeof(HASH_NODE) * size);
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
        CM_FREE(h->table);
        CM_FREE(h);
    }
}

//x mod size
//(x + i) mode size
S32 cm_hash_linear_insert(HASH_TABLE h, void *data)
{
    S32 collision;
    S32 index = cm_hash_linear_find_ext(h, data, &collision);

    //empty or deleted
    if (h->table[index].state != NODE_USED)
    {
        h->table[index].data = data;
        h->table[index].state = NODE_USED;
    }

    return collision;
}

//x mod size
//(x + i^2) mode size
S32 cm_hash_square_insert(HASH_TABLE h, void *data)
{
    S32 collision;
    S32 index = cm_hash_square_find_ext(h, data, &collision);

    //empty or deleted
    if (h->table[index].state != NODE_USED)
    {
        h->table[index].data = data;
        h->table[index].state = NODE_USED;
    }

    return collision;
}

//x mod size
//(x + i) mode size
S32 cm_hash_double_insert(HASH_TABLE h, void *data)
{
    S32 collision;
    S32 index = cm_hash_double_find_ext(h, data, &collision);

    //empty or deleted
    if (h->table[index].state != NODE_USED)
    {
        h->table[index].data = data;
        h->table[index].state = NODE_USED;
    }

    return collision;
}

void cm_hash_delete(HASH_TABLE h, void *data)
{
    return;
}

void cm_hash_print(HASH_TABLE h)
{
    S32 i;

    CM_ASSERT(h != 0);

    CM_TRACE_5("hash table:\n");
    for (i = 0; i < h->size; i++)
    {
        CM_TRACE_5("%d: ", i);

        if (h->table[i].state == NODE_USED)
        {
            CM_TRACE_5("%d\n", h->table[i].data);
        }
        else if (h->table[i].state == NODE_DELETED)
        {
            CM_TRACE_5("(D)\n");
        }
        else
        {
            CM_TRACE_5("\n");
        }
    }
}

