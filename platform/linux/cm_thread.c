
#include "cm_thread.h"
#include "cm_debug.h"


CM_HANDLE cm_create_thread(cm_thread_proc func, cm_thread_info *info)
{
    CM_ASSERT(func != NULL && info != NULL);
    return 0;
}

void cm_destory_thread(CM_HANDLE thread)
{
    CM_ASSERT(thread != NULL);
}

