//
//  BDBackgroundView.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDBackgroundView.h"

@implementation BDBackgroundView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.userInteractionEnabled = YES;
    }
    return self;
}

- (void)setBackgroundImage:(UIImage *)image
{
    [super setImage:image forState:UIControlStateNormal];
}

@end
