//
//  BDToolBarStyle.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBarStyle.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceManager.h"

@implementation BDToolBarStyle

@synthesize barFrame         = barFrame_;
@synthesize backgroundImage  = backgroundImage_;
@synthesize barButtonStyles  = barButtonStyles_;

- (void)dealloc
{
    [backgroundImage_  release];
    [barButtonStyles_  release];

    [super dealloc];
}

@end
