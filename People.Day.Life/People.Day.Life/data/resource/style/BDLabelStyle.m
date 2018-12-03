//
//  BDLabelStyle.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDLabelStyle.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceManager.h"
#import "UIColorAdditions.h"

@implementation BDLabelStyle

@synthesize titleFont           = titleFont_;
@synthesize titleColor          = titleColor_;
@synthesize viewFrame           = viewFrame_;
@synthesize colorDisabled       = colorDisabled_;

- (void)dealloc
{
    [titleFont_   release];
    [super dealloc];
}

@end
