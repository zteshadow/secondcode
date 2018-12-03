/*
 * cm_sum.cpp
 *
 *  Created on: 16 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_sum.h"
#include "cm_debug.h"


#define CM_MAX_OF_2(a, b)       ((a) > (b) ? (a) : (b))
#define CM_MAX_OF_3(a, b, c)    (CM_MAX_OF_2(CM_MAX_OF_2((a), (b)), (c)))


S32 cm_max_mid_left_sequence(const S32 *data, S32 left, S32 mid);
S32 cm_max_mid_right_sequence(const S32 *data, S32 mid, S32 right);


S32 cm_max_mid_left_sequence(const S32 *data, S32 left, S32 mid)
{
    S32 sum = 0, max = 0;
    CM_ASSERT(data != 0 && left >= 0 && mid >= left);

    for (S32 i = mid; i >= left; i--)
    {
        sum += data[i];

        if (sum > max)
        {
            max = sum;
        }
    }

    return max;
}

S32 cm_max_mid_right_sequence(const S32 *data, S32 mid, S32 right)
{
    S32 sum = 0, max = 0;
    CM_ASSERT(data != 0 && mid >= 0 && right >= mid);

    for (S32 i = mid; i <= right; i++)
    {
        sum += data[i];

        if (sum > max)
        {
            max = sum;
        }
    }

    return max;
}


//Çî¾Ù
S32 cm_max_sequence_sum_N3(const S32 *data, S32 size)
{
    CM_ASSERT(data != 0 && size > 0);
    S32 sum = 0, max = 0;

    for (S32 i = 0; i < size; i++)
    {
        for (S32 j = i; j < size; j++)
        {
            sum = 0;

            for (S32 k = i; k <= j; k++)
            {
                sum += data[k];
                if (sum > max)
                {
                    max = sum;
                }
            }
        }
    }

    return max;
}

S32 cm_max_sequence_sum_NLogN(const S32 *data, S32 left, S32 right)
{
    S32 max, left_max, right_max;

    CM_ASSERT(data != 0 && left >= 0 && right >= 0);

    if (left == right)
    {
        max = data[left];
    }
    else
    {
        S32 mid = (left + right) >> 1;  //(left + right)/2
        left_max = cm_max_sequence_sum_NLogN(data, left, mid);
        right_max = cm_max_sequence_sum_NLogN(data, mid + 1, right);

        S32 mid_left_max = cm_max_mid_left_sequence(data, left, mid);
        S32 mid_right_max = cm_max_mid_right_sequence(data, mid + 1, right);
        S32 mid_max = mid_left_max + mid_right_max;

        max = CM_MAX_OF_3(left_max, right_max, mid_max);
    }

    return max;
}

S32 cm_max_sequence_sum_N(const S32 *data, S32 size)
{
    CM_ASSERT(data != 0 && size > 0);
    S32 sum = 0, max = 0;

    for (S32 i = 0; i < size; i++)
    {
        sum += data[i];

        if (sum > max)
        {
            max = sum;
        }
        else if (sum < 0)
        {
            sum = 0;
        }
    }

    return max;
}

S32 cm_min_sequence_sum_N(const S32 *data, S32 size)
{
    CM_ASSERT(data != 0 && size > 0);

    S32 sum = 0, min = 0;

    for (S32 i = 0; i < size; i++)
    {
        sum += data[i];

        if (sum < min)
        {
            min = sum;
        }
        else if (sum > 0)
        {
            sum = 0;
        }
    }

    return min;
}

