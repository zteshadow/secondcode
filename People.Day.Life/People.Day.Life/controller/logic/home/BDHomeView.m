//
//  BDHomeView.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-7-26.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDHomeView.h"

@implementation BDHomeView

- (void)willRemoveSubview:(UIView *)subview
{
    if ([subview isKindOfClass:[UIControl class]]) 
    {
        return;
    }    
}

@end
