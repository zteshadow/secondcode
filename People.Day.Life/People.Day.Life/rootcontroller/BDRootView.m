//
//  BDRootView.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-7-26.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDRootView.h"

@implementation BDRootView

- (void)willRemoveSubview:(UIView *)subview
{
    if ([subview isKindOfClass:[UIControl class]]) 
    {
        return;
    }    
}

@end
