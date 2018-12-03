//
//  BibleView.h
//  Bible
//
//  Created by samuel on 12-1-18.
//  Copyright 2012 Baidu ,. Ltd. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface BibleView : UIView
{
    void *usrContext;
    void *nbkWebView;
}

@property void *usrContext;
@property void *nbkWebView;

- (void)test;
- (void)timerProcess:(NSTimer *)timer;

@end
