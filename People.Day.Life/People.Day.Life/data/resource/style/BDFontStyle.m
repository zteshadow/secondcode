//
//  BDFontStyle.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-5-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDFontStyle.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceManager.h"

@implementation BDFontStyle

@synthesize fontSize   = fontSize_;
@synthesize fontName   = fontName_;
@synthesize fontFamily = fontFamily_;
@synthesize fontType = fontType_;

- (id) init
{
    if ( (self = [super init]) )
    {
        fontSize_ = 12.0f;
        fontName_ = nil;
        fontFamily_ = nil;
        fontType_ = BDFontTypeStandard;
    }
    return self;
}// init

+ (BDFontStyle *) fontStyleFromStyle:(BDFontStyle *)style
{
    if (style == nil)
    {
        return nil;
    }
    
    BDFontStyle * newStyle = [[[BDFontStyle alloc] init] autorelease];
    newStyle.fontSize = style.fontSize;
    newStyle.fontFamily = style.fontFamily;
    newStyle.fontName = style.fontName;
    newStyle.fontType = style.fontType;
    return newStyle;
}

- (void)dealloc
{
    [fontName_ release];
    [fontFamily_ release];
    [super dealloc];
}

@end
