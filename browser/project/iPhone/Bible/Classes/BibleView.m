//
//  BibleView.m
//  Bible
//
//  Created by samuel on 12-1-18.
//  Copyright 2012 Baidu ,. Ltd. All rights reserved.
//

#import "BibleView.h"
#include "bible_ios_platform.h"
#include "nbk_web_view.h"


@implementation BibleView

@synthesize usrContext;
@synthesize nbkWebView;

- (void)test
{
    NSString * path = [[NSBundle mainBundle] pathForResource:@"example" ofType:@"htm"];
    nbk_web_view_load_file((nbk_web_view_ptr)nbkWebView, [path UTF8String]);
}

- (void)timerProcess:(NSTimer *)timer
{
    id data = [timer userInfo];
    bible_ios_on_timer(data);
}

- (id)initWithCoder:(NSCoder *)coder
{
    if ((self = [super initWithCoder:coder]))
    {
        bible_ios_timer_init(self, @selector(timerProcess:));
        
        CGContextRef context = UIGraphicsGetCurrentContext();
        self.usrContext = bible_ios_graphic_create(context, self,
                                                 self.frame.origin.x, self.frame.origin.y,
                                                 self.frame.size.width, self.frame.size.height);
        
        CM_RECT viewSize;
        viewSize.x = self.frame.origin.x;
        viewSize.y = self.frame.origin.y;
        viewSize.w = self.frame.size.width;
        viewSize.h = self.frame.size.height;
        
        self.nbkWebView = nbk_web_view_create(self.usrContext, &viewSize);
    }
    
    return self;
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
    bible_ios_graphic_set_context(self.usrContext, context);
    
    CM_RECT viewSize;
    viewSize.x = self.frame.origin.x;
    viewSize.y = self.frame.origin.y - 20;
    viewSize.w = self.frame.size.width;
    viewSize.h = self.frame.size.height;
    
    nbk_web_view_update_area(self.nbkWebView, &viewSize, 1);    
}


- (void)dealloc {
    
    bible_ios_graphic_destroy(self.usrContext);
    nbk_web_view_destroy(self.nbkWebView);
    [super dealloc];
}


@end
