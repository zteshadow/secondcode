
#include "cm_lib.h"
#include "cm_debug.h"
#include "cm_string.h"
#include "cm_mem_check.h"

#include <stdlib.h>


#if (CM_MEMORY_CHECK_SUPPORT)

void *cm_malloc_ext(const S8 *file, S32 line, CM_SIZE size)
{
    void *p = 0;

    if (size > 0)
    {
        size = cm_mem_check_size(size);
        p = cm_malloc(size);

        if (p == 0)
        {
            CM_TRACE_0("Memory Error(%d):%s@%d", size, file, line);
            CM_ASSERT(0);
        }
        cm_mem_check_append(&p, size, file, line);
    }
    
    return p;
}

void *cm_realloc_ext(const S8 *file, S32 line, void *ptr, CM_SIZE size)
{
	void *p;

    CM_ASSERT(size > 0);

    p = cm_malloc_ext(file, line, size);
    if (p)
    {
        if (ptr)
        {
            cm_memcpy(p, ptr, size);
        }
    }

    if (ptr)
    {
        cm_free_ext(ptr);
    }
    
    return p;
}

void cm_free_ext(void *p)
{
    CM_ASSERT(p != 0);

    cm_mem_check_delete(&p);
    cm_free(p);
}
#endif

void *cm_malloc(CM_SIZE size)
{
    CM_ASSERT(size > 0);
    
    return malloc(size);
}

void *cm_realloc(void *p, CM_SIZE new_size)
{
    CM_ASSERT(new_size > 0);
    return realloc(p, new_size);
}

void cm_free(void *p)
{
    CM_ASSERT(p != 0);

    free(p);
}

S32 cm_atoi(const S8 *data)
{
    S32 a = 0;
#if 0
    int negative = 0;
    
    CM_ASSERT(data != NULL);

    while (cm_is_blank(*data))              /*scape heading space*/
    {
        data ++;
    }

    if (*data == '-')                       /*operator process*/
    {
        negative = 1;
        data ++;
    }
    else if (*data == '+')
    {
        data ++;
    }
    
    while (cm_is_digit(*data))              /*is digital*/
    {
        a = (a * 10) + (*data - '0');
        
        data ++;
    }

    if (negative)
    {
        a = - a;
    }
#else
    a = atoi(data);
#endif
    return a;
}

ULong cm_strtoul(const S8 *data, S8 **end_ptr, S32 base)
{
    ULong a = 0;

    a = strtoul(data, end_ptr, base);
    return a;
}

void cm_srand(U32 seed)
{
    srand(seed);
}

S32 cm_rand(void)
{
    return rand();
}

S32 cm_rand_max(void)
{
    return RAND_MAX;
}

