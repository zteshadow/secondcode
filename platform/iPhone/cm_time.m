
#include "cm_time.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "bible_ios_platform.h"
#import <Foundation/NSTimer.h>
#import <UIKit/UIView.h>


typedef struct
{
    id control;
    SEL selector;
    NSTimer *timer;
    
    void *user;
    CM_TIMER_FUNC func;
    
} CM_TIMER_STRUCT;


@interface CMTimerInfo : NSObject
{
    CM_TIMER_STRUCT *data;
}

@property CM_TIMER_STRUCT *data;

- (id)initWithTimerInfo:(CM_TIMER_STRUCT *)d;

@end

@implementation CMTimerInfo

@synthesize data;

- (id)initWithTimerInfo:(CM_TIMER_STRUCT *)d
{
    if (self = [super init])
    {
        data = d;
    }
    
    return self;
}
@end


static id s_timer_control;
static SEL s_timer_selector;


void bible_ios_timer_init(id control, SEL func)
{
    CM_ASSERT(control != 0 && func != 0);
    
    s_timer_control = control;
    s_timer_selector = func;
}

void bible_ios_on_timer(id info)
{
    CM_TIMER_STRUCT *p;
    CMTimerInfo *data = (CMTimerInfo *)info;
    
    CM_ASSERT(data != 0);
    if (data)
    {
        p = data.data;
        CM_TIMER_FUNC f = p->func;
        f(p->user);
    }
}

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
    CM_TIME_VAL time;
    cm_gettimeofday(&time);
    
    return time.tv_usec / 1000;
}

void *cm_timer_create(CM_TIMER_FUNC f, void *usr)
{
    CM_TIMER_STRUCT *p;
    
    CM_ASSERT(f != 0);
    
    p = (CM_TIMER_STRUCT *)CM_MALLOC(sizeof(CM_TIMER_STRUCT));
    if (p)
    {
        cm_memset(p, 0, sizeof(CM_TIMER_STRUCT));
        p->func = f;
        p->user = usr;
        p->control = s_timer_control;
        p->selector = s_timer_selector;
    }
    
    return p;
}

//timeout:∫¡√Î
//—≠ª∑timer
void cm_timer_start(void *timer, S32 timeout)
{
    CM_TIMER_STRUCT *p = (CM_TIMER_STRUCT *)timer;
    
    CM_ASSERT(timer != 0 && timeout > 0);
    if (p)
    {
        CMTimerInfo *info = [[CMTimerInfo alloc] initWithTimerInfo:p];
        NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:timeout / 1000.0 target:p->control selector:p->selector userInfo:info repeats: YES];
        p->timer = timer;
        
        [info release];
    }
}

void cm_timer_stop(void *timer)
{
    CM_TIMER_STRUCT *p = (CM_TIMER_STRUCT *)timer;
    
    CM_ASSERT(timer != 0);
    if (p)
    {
        NSTimer *timer = p->timer;
        [timer invalidate];
        p->timer = 0;
    }
}

void cm_timer_destory(void *timer)
{
    CM_TIMER_STRUCT *p = (CM_TIMER_STRUCT *)timer;
    
    CM_ASSERT(timer != 0);
    if (p)
    {
        if (p->timer != 0)
        {
            NSTimer *timer = p->timer;
            [timer invalidate];            
        }
        
        CM_FREE(p);
    }
}

