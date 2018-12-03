/*
 * nbk_timer.cpp
 *
 *  Created on: 2012-1-4
 *      Author: samuel.song.bc@gmail.com
 */

#include "nbk_timer.h"
#include "cm_time.h"
#include "cm_debug.h"
#include "nbk_image_manager.h"


void NBK_timerCreate(NTimer* timer)
{
    void *id = 0;

    CM_ASSERT(timer != 0);

    if (timer != 0)
    {
        id = cm_timer_create(timer->func, timer->user);
        timer->id = (S32)id;
    }
}

void NBK_timerDelete(NTimer* timer)
{
    void *id;

    CM_ASSERT(timer != 0);

    if (timer != 0)
    {
        id = (void *)(timer->id);
        cm_timer_destory(id);
    }
}

void NBK_timerStart(NTimer* timer, int delay, int interval)
{
    void *id;

    CM_ASSERT(timer != 0);

    if (timer != 0)
    {
        id = (void *)(timer->id);
        delay = (delay / 1000) > 0 ? (delay / 1000) : 1; //set to 1 ms if too short.
        cm_timer_start(id, delay);
    }
}

void NBK_timerStop(NTimer* timer)
{
    void *id;

    CM_ASSERT(timer != 0);

    if (timer != 0)
    {
        id = (void *)(timer->id);
        cm_timer_stop(id);
    }
}


