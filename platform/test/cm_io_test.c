
#include "cm_io.h"
#include "cm_time.h"
#include "cm_facility.h"
#include "cm_string.h"
#include "cm_debug.h"


void cm_io_test(void)
{
    S32 value;
    S8 home[CM_MAX_PATH];
    
    value = cm_facility_get_work_dir(home);
    if (value >= 0)
    {
        value = cm_facility_add_sub_dir(home, "bible");
        if (value >= 0)
        {
            if (!cm_dir_exist(home))
            {
                cm_mkdir(home);
                
                value = cm_facility_add_sub_dir(home, "cache.dat");
                if (value >= 0)
                {
                    CM_HANDLE fp = cm_fopen(home, "a+");
                    if (fp)
                    {
                        cm_fwrite("hello world", cm_strlen("hello world"), 1, fp);
                        cm_fclose(fp);
                    }
                }
            }
        }
    }
}


static S32 s_timeout_cnt;
static void *s_timer;


static void cm_timer_callback(void *usr)
{
    S32 id = (S32)usr;
    CM_TRACE_0("cm_timer_callback: %d-%d\n", s_timeout_cnt++, id);

    if (s_timeout_cnt >5)
    {
        cm_timer_stop(s_timer);
        cm_timer_destory(s_timer);
    }
}

void cm_timer_test(void)
{
    s_timer = cm_timer_create(cm_timer_callback, (void *)5);

    cm_timer_start(s_timer, 100);
}

