//
//  BDHomeController+Private.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDHomeController+Private.h"
#import "BDToolBarController.h"
#import "BDToolBarButton.h"
#import "BDShortcutMenuController.h"

@implementation BDHomeController(Private)

- (void)setupComponents
{
    [mainView_ addSubview:self.titleController.view];
    [mainView_ addSubview:self.shortcutMenuController.view];
}

- (void)animateMainViewToRect:(CGRect)rect
{
    [UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.3];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
    mainView_.frame = rect;
	[UIView commitAnimations];
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    //
    // do something to record the app's state
#if 0
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    [defaults synchronize];
#endif
}

@end
