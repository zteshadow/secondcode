
#ifndef _BIBLE_IOS_PLATFORM_H
#define _BIBLE_IOS_PLATFORM_H


#include "cm_data_type.h"
#import <UIKit/UIView.h>


#ifdef __cplusplus
extern "C"
{
#endif

    
void *bible_ios_graphic_create(CGContextRef context, UIView *view, S32 x, S32 y, S32 w, S32 h);
void bible_ios_graphic_set_context(void *graphic, CGContextRef context);
void bible_ios_graphic_destroy(void *graphic);

void bible_ios_timer_init(id control, SEL func);
void bible_ios_on_timer(id info);

#ifdef __cplusplus
}
#endif

#endif //_BIBLE_IOS_PLATFORM_H
