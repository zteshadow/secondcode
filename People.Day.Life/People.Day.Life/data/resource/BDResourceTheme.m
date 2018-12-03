//
//  BDResourceTheme.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceTheme+Private.h"

@implementation BDResourceTheme

@synthesize toolBarStyle = toolBarStyle_;
@synthesize titleBarStyle = titleBarStyle_;
@synthesize textFieldStyle = textFieldStyle_;
@synthesize editTitleBarStyle = editTitleBarStyle_;
@synthesize shortcutMenuStyle = shortcutMenuStyle_;

- (void)dealloc
{
    [toolBarStyle_ release];
    [titleBarStyle_ release];

    [textFieldStyle_ release];
    [editTitleBarStyle_ release];
    
    [shortcutMenuStyle_ release];
    
    [super dealloc];
}

@end
