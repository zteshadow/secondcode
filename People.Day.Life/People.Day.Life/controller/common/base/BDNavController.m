//
//  BDNavController.m
//  People.Day.Life
//
//  Created by samuel on 13-2-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDNavController.h"

@interface BDNavController ()

@end

@implementation BDNavController

- (void)dismiss
{
    if (self.view.superview != nil)
    {
        [self.view removeFromSuperview];
    }
}

- (void)showInView:(UIView *)view
{
    [view addSubview:self.view];
}

@end
