
#include "cm_union_find.h"
#include "cm_debug.h"


void cm_union_find_init(CM_UF_SET set, S32 cnt)
{
    S32 i;
    
    CM_ASSERT(set != 0 && cnt > 0);

    if (set != 0 && cnt > 0)
    {
        for (i = 1; i <= cnt; i++)
        {
            set[i] = -1;
        }
    }
}

void cm_union(CM_UF_SET set, S32 y, S32 x)
{
    S32 x1, y1;
    
    CM_ASSERT(set != 0 && x > 0 && y > 0);

    x1 = cm_find(set, x);
    y1 = cm_find(set, y);
    
    if (x1 != y1)
    {
        if (set[x1] < set[y1])
        {
            set[y1] = x1;
        }
        else
        {
            if (set[x1] == set[y1])
            {
                set[y1]--;
            }
            
            set[x1] = y1;
        }
    }
}

S32 cm_find(CM_UF_SET set, S32 x)
{
    CM_ASSERT(set != 0 && x > 0);

    while (set[x] > 0)
    {
        x = set[x];
    }

    return x;
}

void cm_dump_union_find(CM_UF_SET set, S32 cnt)
{
    S32 i;

    CM_ASSERT(set != 0 && cnt > 0);

    CM_TRACE_0("set:\t");
    
    if (set != 0 && cnt > 0)
    {
        for (i = 1; i <= cnt; i++)
        {
            CM_TRACE_0("%d ", set[i]);
        }

        CM_TRACE_0("\n");
    }
}
