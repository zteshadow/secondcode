/*
 * cm_base64.cpp
 *
 *  Created on: 20 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_base64.h"
#include "cm_debug.h"
#include "cm_lib.h"


//size is 65(with 0)
static const S8 s_base_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                              "abcdefghijklmnopqrstuvwxyz" \
                              "0123456789" \
                              "+/";

//'z' + 1 = 123
static const S8 s_base_map[] =
{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

//size of data is 123('z' + 1)
void cm_base64_map_create(S8 *data)
{
    const S8 *p = s_base_64;
    S8 i, j, size = sizeof(s_base_64) - 1;    //remove 0 of tail
	CM_ASSERT(data != 0);

    for (i = 0; i <= 'z'; i++)
    {
        for (j = 0; j < size; j++)
        {
            if (i == p[j])
            {
                break;
            }
        }

        data[i] = j;
    }
}

S8 *cm_base64_encode(const S8 *buffer, S32 len)
{
    S32 i = 0;
    S8 *p, *result = 0;
    const S8 *table = s_base_64;

    CM_ASSERT(buffer != 0 && len > 0);

    if ((buffer == 0) || (len <= 0))
    {
        return 0;
    }

    /* Malloc result buffer */
    result = (S8 *)CM_MALLOC(((len + 2) / 3 * 4) + 1/*for '0'*/);
    CM_ASSERT(result != 0);

    if (result == 0)
    {
        return 0;
    }

    p = result;
    for (i = 0; i < len - 2/*at least 3*/; i += 3)
    {
        *p++ = table[(buffer[i] >> 2) & 0x3F];
        *p++ = table[((buffer[i] & 0x3) << 4) |
                        ((S32) (buffer[i + 1] & 0xF0) >> 4)];
        *p++ = table[((buffer[i + 1] & 0xF) << 2) |
                        ((S32) (buffer[i + 2] & 0xC0) >> 6)];
        *p++ = table[buffer[i + 2] & 0x3F];
    }

    if (i < len)
    {
        *p++ = table[(buffer[i] >> 2) & 0x3F];
        if (i == (len - 1)) //1 byte left
        {
            *p++ = table[((buffer[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else                //2 byte left
        {
            *p++ = table[((buffer[i] & 0x3) << 4) |
                            ((S32) (buffer[i + 1] & 0xF0) >> 4)];
            *p++ = table[((buffer[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *p++ = '\0';

    return result;
}

S8 *cm_base64_decode(const S8 *buffer, S32 len, S32 *result_len)
{
	S8 *data, *out;
	const S8 *p;
    S32 size, decoded_len;

    CM_ASSERT(buffer != 0 && len > 0 && result_len != 0);

	p = buffer + len - 1; //last item

	CM_ASSERT( *p <= 'z');
    while (*p == '=')
    {
		p--;
		CM_ASSERT(*p <= 'z');
    }

	size = p - buffer + 1;
    decoded_len = ((size + 3) / 4) * 3;
    data = (S8*)CM_MALLOC(decoded_len + 1);
    CM_ASSERT(data != 0);

    if (data == 0)
    {
        return 0;
    }

    out = data;
    p = buffer;

    while (size > 4)
    {
        *(out++) = (S8) (s_base_map[p[0]] << 2 | s_base_map[p[1]] >> 4);
        *(out++) = (S8) (s_base_map[p[1]] << 4 | s_base_map[p[2]] >> 2);
        *(out++) = (S8) (s_base_map[p[2]] << 6 | s_base_map[p[3]]);
        p += 4;
        size -= 4;
    }

    CM_ASSERT(size > 1);
    if (size > 1)       //2 '='
    {
        *(out++) = (S8) (s_base_map[p[0]] << 2 | s_base_map[p[1]] >> 4);
    }
    
    if (size > 2)       //1 '='
    {
        *(out++) = (S8) (s_base_map[p[1]] << 4 | s_base_map[p[2]] >> 2);
    }

    if (size > 3)       //no '='
    {
        *(out++) = (S8) (s_base_map[p[2]] << 6 | s_base_map[p[3]]);
    }

    decoded_len -= (4 - size) & 3;

    *result_len = decoded_len;
    return data;
}

