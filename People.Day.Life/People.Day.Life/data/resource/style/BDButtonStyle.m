//
//  BDButtonStyle.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDButtonStyle.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceManager.h"
#import "UIColorAdditions.h"

@implementation BDButtonStyle

@synthesize titleFont            = titleFont_;
@synthesize titleColor           = titleColor_;
@synthesize buttonType           = buttonType_;
@synthesize buttonFrame          = buttonFrame_;
@synthesize imageNormal          = imageNormal_;
@synthesize imageDisabled        = imageDisabled_;
@synthesize imageHighlighted     = imageHighlighted_;
@synthesize imageBackNormal      = imageBackNormal_;
@synthesize imageBackHighlighted = imageBackHighlighted_;
@synthesize colorDisabled        = colorDisabled_;
@synthesize colorHighlighted     = colorHighlighted_;
@synthesize titleString = titleString_;
@synthesize alignType = alighType_;

+ (UIButton *)buttonFromSytle:(BDButtonStyle *)style
{
    UIButtonType buttonType = style.buttonType;
    buttonType = MAX(UIButtonTypeCustom, buttonType);
    buttonType = MIN(UIButtonTypeContactAdd, buttonType);
    
    UIButton * button = [UIButton buttonWithType:buttonType];
    
    [button setFrame:style.buttonFrame];
    // set font
    if (style.titleFont) {
        CGFloat size = style.titleFont.fontSize;
        if (size <= 0) {
            size = [UIFont buttonFontSize];
        }
        UIFont * font = nil;
        if (style.titleFont.fontName) {

            font = [UIFont fontWithName:style.titleFont.fontName 
                                            size:size];
        }
        else {
            font = [UIFont systemFontOfSize:size];
        }
        [button.titleLabel setFont:font];
    }
    // set images
    if (style.imageNormal) {
        [button setImage:[UIImage imageNamed:style.imageNormal]
                forState:UIControlStateNormal];
    }
    if (style.imageDisabled) {
        [button setImage:[UIImage imageNamed:style.imageDisabled]
                forState:UIControlStateDisabled];
    }
    if (style.imageHighlighted) {
        [button setImage:[UIImage imageNamed:style.imageHighlighted]
                forState:UIControlStateHighlighted];
    }
    if (style.imageBackNormal) {
        UIImage * normalImage = [UIImage imageNamed:style.imageBackNormal];
        normalImage = [normalImage stretchableImageWithLeftCapWidth:normalImage.size.width  / 2
                                                       topCapHeight:normalImage.size.height / 2];
        [button setBackgroundImage:normalImage
                          forState:UIControlStateNormal];
    }
    if (style.imageBackHighlighted) {
        UIImage * highlightImage = [UIImage imageNamed:style.imageBackHighlighted];
        highlightImage = [highlightImage stretchableImageWithLeftCapWidth:highlightImage.size.width / 2
                                                             topCapHeight:highlightImage.size.height / 2];
        [button setBackgroundImage:highlightImage
                          forState:UIControlStateHighlighted];
    }
    // title color
    if (style.titleColor) {
        UIColor * color = [UIColor colorWithHex:style.titleColor];
        [button setTitleColor:color forState:UIControlStateNormal];
    }
    
    return button;
}// buttonFromStyle:

+ (BDButtonStyle *) buttonStyleFromStyle:(BDButtonStyle *)style
{
    if (style == nil)
    {
        return nil;
    }
    //
    // generate a new button style according to style
    BDButtonStyle * newStyle = [[[BDButtonStyle alloc] init] autorelease];
    newStyle.buttonFrame = style.buttonFrame;
    newStyle.buttonType = style.buttonType;
    newStyle.titleColor = style.titleColor;
    newStyle.imageNormal = style.imageNormal;
    newStyle.imageDisabled = style.imageDisabled;
    newStyle.imageHighlighted = style.imageHighlighted;
    newStyle.imageBackNormal = style.imageBackNormal;
    newStyle.imageBackHighlighted = style.imageBackHighlighted;
    newStyle.titleFont = [BDFontStyle fontStyleFromStyle:style.titleFont];
    newStyle.titleString = style.titleString;
    
    return newStyle;
}// buttonStyleFromStyle:

- (void)dealloc
{
    [titleFont_   release];
    [imageNormal_ release];
    [imageDisabled_ release];
    [imageBackNormal_ release];
    [imageHighlighted_ release];
    [imageBackHighlighted_ release];
    [titleString_ release];
    
    [super dealloc];
}

@end
