
#include "cm_algorithm_utility.h"

#include "cm_lib.h"
#include "cm_time.h"
#include "cm_string.h"
#include "cm_io.h"

#include "cm_debug.h"


//0 ~ 100, percent
static S32 cm_is_probability(S32 prob);


static S32 cm_is_probability(S32 prob)
{
    S32 max = cm_rand_max();
    S32 value = (S32)((cm_rand()/(max + 1.0)) * 100);

    if (value <= prob)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

S32 *cm_data_create(S32 size, S32 max, CM_DATA_TYPE type)
{
    S32 *data, i;

	CM_ASSERT(size > 0 && max > 0);

	data = (S32 *)CM_MALLOC(sizeof(S32) * size);
    if (data)
    {
        S32 max_rand = cm_rand_max();
        S32 coefficient = (CM_DATA_NEGATIVE == type) ? -1 : 1;

        cm_srand((U32)cm_time(0));
        for (i = 0; i < size; i++)
        {
            data[i] = (S32)(coefficient * max * (cm_rand() / (1.0 + max_rand)));
        }

        if (CM_DATA_MIXED == type)          //40%
        {
            S32 negative_cnt = (S32)(size * 4.0 / 10.0);
            for (i = 0; i < negative_cnt; i++)
            {
                S32 index = (S32)(size * (cm_rand() / (1.0 + max_rand)));
                data[index] = -data[index]; //maby 0
            }
        }
    }

    return data;
}

S32 *cm_data_copy(const S32 *src, S32 cnt)
{
    void *p = CM_MALLOC(sizeof(S32) * cnt);

    if (p != 0)
    {
        cm_memcpy(p, src, sizeof(S32) * cnt);
    }

    return (S32 *)p;
}

void cm_data_dump(const S32 *src, S32 cnt, const S8 *fname)
{
    S32 i;
    CM_HANDLE fp = cm_fopen(fname, "wb");

    if (fp != 0)
    {
        for (i = 0; i < cnt; i++)
        {
            S8 str_value[20];
            cm_sprintf(str_value, "%d\n", src[i]);
            cm_fwrite((const void *)str_value, cm_strlen(str_value), 1, fp);
        }

        cm_fclose(fp);
    }
}

void cm_data_destory(S32 *h)
{
    CM_ASSERT(h != 0);
    if (h)
    {
        CM_FREE(h);
    }
}

S32 cm_data_print(const S32 *data, S32 size)
{
	S32 i;

    CM_ASSERT(data && size > 0);

    CM_TRACE_0("\n");

    for (i = 0; i < size; i++)
    {
        CM_TRACE_0("data[%d] = %d\n", i, data[i]);
    }

    return 0;
}

