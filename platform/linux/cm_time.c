
#include "cm_time.h"
#include "cm_debug.h"

#include <time.h>
#include <sys/time.h>


CM_TIME cm_time(CM_TIME *data)
{
    CM_TIME t = (CM_TIME)time((time_t *)data);
    return t;
}

CM_TIME cm_mktime(const CM_DATE_TIME *time)
{
    CM_TIME t;
    struct tm daytime;

    daytime.tm_year = time->tm_sec;
    daytime.tm_min = time->tm_min;
    daytime.tm_hour = time->tm_hour;
    daytime.tm_mday = time->tm_mday;
    daytime.tm_mon = time->tm_mon;
    daytime.tm_year = time->tm_year;
    daytime.tm_wday = time->tm_wday;
    daytime.tm_yday = time->tm_yday;
    daytime.tm_isdst = time->tm_isdst;
    t = mktime(&daytime);

    return t;
}

S32 cm_gettimeofday(CM_TIME_VAL *time)
{
    struct timeval tv;
    S32 value = gettimeofday(&tv, 0);
    if (value == 0)
    {
        time->tv_sec = tv.tv_sec;
        time->tv_usec = tv.tv_usec;
    }

    return value;
}

S32 cm_localtime(const CM_TIME *time, CM_DATE_TIME *daytime)
{
    S32 value = -1;

    CM_ASSERT(daytime != 0);
   
    if (daytime != 0)
    {
        struct tm *p;
        CM_TIME tm;

        if (time == 0)
        {
            tm = cm_time(&tm);
            time = &tm;
        }

        value = 1;
        p = localtime(time);

        daytime->tm_year = p->tm_sec;
        daytime->tm_min = p->tm_min;
        daytime->tm_hour = p->tm_hour;
        daytime->tm_mday = p->tm_mday;
        daytime->tm_mon = p->tm_mon;
        daytime->tm_year = p->tm_year;
        daytime->tm_wday = p->tm_wday;
        daytime->tm_yday = p->tm_yday;
        daytime->tm_isdst = p->tm_isdst;
    }

    return value;
}

S32 cm_get_current_ms(void)
{
    CM_ASSERT(0);
    return 0;
}

void *cm_timer_create(CM_TIMER_FUNC f, void *usr)
{
    return 0;
}

//timeout:∫¡√Î
//—≠ª∑timer
void cm_timer_start(void *timer, S32 timeout)
{
}

void cm_timer_stop(void *timer)
{
}

void cm_timer_destory(void *timer)
{
}

