/*
 * cm_utility.cpp
 *
 *  Created on: 30 Dec 2011
 *      Author: samuel.song.bc@gmail.com
 */

#include "cm_utility.h"
#include "cm_md5.h"
#include "cm_lib.h"
#include "cm_io.h"
#include "cm_debug.h"


void* cm_memrchr(const void* s, S32 c, CM_SIZE n)
{
	const U8 *p = (const U8 *)s;
    const U8 *q = (const U8 *)s;
    
    CM_ASSERT(s != 0 && n > 0);

	p += n - 1;

	while (p >= q)
	{
		if (*p == (unsigned char)c)
		{
			return (void *)p;
        }
		p--;
	}

	return 0;
}

S8* cm_strtok_r(S8 *s, const S8 *delim, S8 **last)
{
	S8 *spanp;
	S32 c, sc;
	S8 *tok;

	if (s == NULL && (s = *last) == NULL)
		return 0;

	/*
	* Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	*/
cont:
	c = *s++;
	for (spanp = (S8 *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {           /* no non-delimiter characters */
		*last = 0;
		return (0);
	}
	tok = s - 1;

	/*
	* Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	* Note that delim must have one NUL; we stop if we see that, too.
	*/
	for (;;) {
		c = *s++;
		spanp = (S8 *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = 0;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

S32 cm_create_md5_string(const void *data, CM_SIZE len, S8 *md5)
{
    S8 md5_value[CM_MAX_MD5];
    S32 size;
    
    size = cm_create_md5(data, len, md5_value);
    if (size > 0)
    {
        size = cm_create_hex_string((const void *)md5_value, size, md5);
    }

    return size;
}

CM_SIZE cm_create_hex_string(const void *buff, CM_SIZE len, S8 *output)
{
	S8 hex_char[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	CM_SIZE i;
	U8 *p = (U8 *)buff;

	for (i = 0; i < len; i++)
	{
		output[i << 1] = hex_char[(p[i] >> 4) & 0x0F];
		output[(i << 1) + 1] = hex_char[p[i] & 0x0F];
	}
	
	output[i << 1] = '\0';

	return i << 1;
}

S8 *cm_read_file(const S8 *fname, CM_SIZE *len)
{
    CM_HANDLE fp;
    S8 *data = 0;
    
    CM_ASSERT(fname != 0 && len != 0);
    
    *len = 0;
    fp = cm_fopen(fname, "rb");

    if (fp)
    {
        S32 size;
        cm_fseek(fp, 0, CM_SEEK_END);
        size = cm_ftell(fp);
        cm_fseek(fp, 0, CM_SEEK_SET);

        data = (S8 *)CM_MALLOC(size);
        if (data)
        {
            cm_fread(data, size, 1, fp);
            *len = size;
        }

        cm_fclose(fp);
    }
    
    return data;
}

S8 *cm_read_file_string(const S8 *fname)
{
    CM_HANDLE fp;
    S8 *data = 0;
    
    CM_ASSERT(fname != 0);
    
    fp = cm_fopen(fname, "rb");

    if (fp)
    {
        S32 size;
        cm_fseek(fp, 0, CM_SEEK_END);
        size = cm_ftell(fp);
        cm_fseek(fp, 0, CM_SEEK_SET);

        data = (S8 *)CM_MALLOC(size + 1);
        if (data)
        {
            cm_fread(data, size, 1, fp);
            *(data + size) = '\0';
        }

        cm_fclose(fp);
    }
    
    return data;
}
