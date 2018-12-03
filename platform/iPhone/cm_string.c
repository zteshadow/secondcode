
#include "cm_string.h"
#include "cm_debug.h"
#include "cm_lib.h"

#include <string.h>


#define CM_FABS(a) ((a) > 0 ? (a) : -(a))


void *cm_memcpy(void *d, const void *s, U32 n)
{
    CM_ASSERT(d != NULL && s != NULL && n > 0);
    
	return memcpy(d, s, n);
}

void *cm_memmove(void *d, const void *s, U32 n)
{
    CM_ASSERT(d != NULL && s != NULL && n > 0);
    
	return memmove(d, s, n);		
}

S32 cm_memcmp(const void *s1, const void *s2, CM_SIZE n)
{
    CM_ASSERT(s1 != 0 && s2 != 0 && n > 0);

    return memcmp(s1, s2, n);
}

void *cm_memset(void *s, S32 c, CM_SIZE n)
{
    CM_ASSERT(s != NULL && n > 0);

	return memset(s, c, n);
}

S32 cm_strcmp(const S8 *s1, const S8 *s2)
{
    CM_ASSERT(s1 != NULL && s2 != NULL);
    
    return strcmp(s1, s2);
}

S32 cm_strncmp(const S8 *s1, const S8 *s2, CM_SIZE n)
{
    CM_ASSERT(s1 != NULL && s2 != NULL && n > 0);

    return strncmp(s1, s2, n);
}

S32 cm_strcasecmp(const S8 *s1, const S8 *s2)
{
    S8 diff;

    CM_ASSERT(s1 != NULL && s2 != NULL);

    while (*s1 != 0 && *s2 != 0)
    {
        diff = *s1 - *s2;

        if (diff != 0)
        {
            if (CM_FABS(diff) != 32)
            {
                break;
            }
        }

        s1++;
        s2++;
    }

    return diff;
}

S32 cm_strncasecmp(const S8 *s1, const S8 *s2, CM_SIZE n)
{
    S8 diff;

    CM_ASSERT(s1 != NULL && s2 != NULL);

    while (*s1 != 0 && *s2 != 0 && (n > 0))
    {
        diff = *s1 - *s2;

        if (diff != 0)
        {
            if (CM_FABS(diff) != 32)
            {
                break;
            }
        }

        s1++;
        s2++;
        n--;
    }

    return diff;
}

CM_SIZE cm_strlen(const S8 *s)
{
    CM_ASSERT(s != NULL);
    
    return strlen(s);
}

S8 *cm_strncpy(S8 *d, const S8 *s, CM_SIZE n)
{
    CM_ASSERT(d != NULL && s != NULL && n > 0);
    
    return strncpy(d, s, n);
}

S8 *cm_strcpy(S8 *d, const S8 *s)
{
    CM_ASSERT(d != NULL && s != NULL);
    
    return strcpy(d, s);
}

S8 *cm_strcat(S8 *d, const S8 *s)
{
    CM_ASSERT(d != NULL && s != NULL);
    
    return strcat(d, s);
}

S8 *cm_strdup(const S8 *s)
{
    S8 *data = 0;
    
    CM_ASSERT(s != 0);

    if (s)
    {
        S32 len = cm_strlen(s);
        if (len > 0)
        {
            data = (S8 *)CM_MALLOC(len + 1);
            CM_ASSERT(data != 0);

            if (data)
            {
                cm_strncpy(data, s, len + 1);
            }
        }
    }

    return data;
}

S8 *cm_strstr(const S8 *source, const S8 *pattern)
{
    CM_ASSERT(source != 0 && pattern != 0);

    return strstr(source, pattern);
}

S8 *cm_strrchr(const S8 *source, S32 c)
{
    CM_ASSERT(source != 0);

    return strrchr(source, c);
}

S8 *cm_strchr(const S8 *source, S32 c)
{
    CM_ASSERT(source != 0);

    return strchr(source, c);
}

S8 *cm_strtok(S8 *src, const S8 *delim)
{
    CM_ASSERT(delim != 0);
    return strtok(src, delim);
}

S32 cm_ustrcmp(const U16 *s1, const U16 *s2)
{
    CM_ASSERT(s1 != NULL && s2 != NULL);

    while (*s1 == *s2 && *s1 != 0x0000)
    {
        s1++;
        s2++;
    }
    
    return *s1 - *s2;
}

CM_SIZE cm_ustrlen(const U16* s)
{
    CM_SIZE i = 0;
    CM_ASSERT(s != NULL);

    while (*s++ != 0x0000)
    {
        i++;
    }
    
    return i;
}

U16 *cm_ustrncpy(U16 *d, const U16 *s, CM_SIZE n)
{
    CM_ASSERT(d != NULL && s != NULL && n > 0);

    while (*s != 0x0000 && (n-- > 0))
    {
        *d++ = *s++;        
    }

    *d = 0x0000;
    
    return d;
}

