//
//  BDToolBarController.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDBaseController.h"

@implementation BDBaseController

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
