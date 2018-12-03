//
//  BDShortcutMenuStyle.m
//  People.Day.Life
//
//  Created by samuel on 12-12-30.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDShortcutMenuStyle.h"
#import "BDResourceTheme+Private.h"

@implementation BDShortcutMenuStyle

@synthesize mainButtonStyle = mainButtonStyle_;
@synthesize menuButtonStyles = menuButtonStyles_;
@synthesize viewFrameOpen = viewFrameOpen_;
@synthesize viewFrameClose = viewFrameClose_;
@synthesize top = top_;
@synthesize bottom = bottom_;
@synthesize right = right_;
@synthesize radius = radius_;
@synthesize changeSideOffset = changeSideOffset_;

- (void)dealloc
{
    [mainButtonStyle_ release];
    [menuButtonStyles_ release];
    
    [super dealloc];
}

@end
