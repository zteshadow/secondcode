//
//  BDTitleBarStyle.m
//  People.Day.Life
//
//  Created by samuel on 13-1-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDTitleBarStyle.h"
#import "BDResourceTheme+Private.h"

@implementation BDTitleBarStyle

@synthesize titleLabelStyle = titleLableStyle_;

- (void)dealloc
{
    [titleLableStyle_ release];
    
    [super dealloc];
}

@end
