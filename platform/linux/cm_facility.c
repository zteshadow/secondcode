
#include "cm_facility.h"
#include "cm_debug.h"
#include "cm_string.h"


#define CM_MAX_MEMORY_SIZE  (10 * 1024 * 1024)


CM_SIZE cm_facility_get_max_mem(void)
{
    return CM_MAX_MEMORY_SIZE;
}

S32 cm_facility_get_deposit_fname(CM_DEPOSIT_FILE type, S8 *fname)
{
	S32 value = -1;

    CM_ASSERT(0);
    
    return value;
}

//e:\nbk (without the last '\' or '/')
S32 cm_facility_get_work_dir(S8 *dir)
{
    CM_ASSERT(dir != 0);

    if (dir)
    {
        cm_strcpy(dir, "e:\\tmp");
        return 1;
    }
    
    return -1;
}

//用dir返回全路径，主要是由平台决定dir/sub or dir\sub
//dir/sub or dir\sub
S32 cm_facility_add_sub_dir(S8 *dir, const S8 *sub)
{
    CM_ASSERT(dir != 0 && sub != 0);

    if (dir != 0 && sub != 0)
    {
        cm_strcat(dir, "\\");
        cm_strcat(dir, sub);
        return 1;
    }
    
    return -1;
}

S32 cm_get_imei(S8 *imei)
{
    return -1;
}
