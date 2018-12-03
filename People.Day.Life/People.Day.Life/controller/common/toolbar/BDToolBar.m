//
//  BDToolBar.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBar.h"
#import "UIViewAdditions.h"

@implementation BDToolBar

- (void)setItems:(NSArray *)items animated:(BOOL)animated
{
    [self removeAllSubviews];
    
    for (UIView * view in items)
    {
        [self addSubview:view];
    }
}

@end
