
#include "cm_thread.h"
#include "cm_debug.h"
#include <process.h>
#include <windows.h>


CM_HANDLE cm_create_thread(cm_thread_proc func, cm_thread_info *info)
{
    CM_ASSERT(func != NULL && info != NULL);

    if (info->auto_delete)
    {
        return _beginthread(func, 0, info->param);  //here is a truncation
    }
    else
    {
	    DWORD id;

	    CM_HANDLE t = CreateThread(0, 0,(LPTHREAD_START_ROUTINE)func, 
	                            (void *)info->param, CREATE_SUSPENDED, &id);

	    SetThreadPriority(t, THREAD_PRIORITY_NORMAL);
	    ResumeThread(t);
	    
	    return t;        
    }    
}

void cm_destory_thread(CM_HANDLE thread)
{
    CM_ASSERT(thread != NULL);
    
    TerminateThread(thread, 0);
}

