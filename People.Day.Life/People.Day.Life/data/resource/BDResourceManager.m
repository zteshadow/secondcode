//
//  BDResourceManager.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceManager.h"
#import "BDResourceParser.h"
#import "UIColorAdditions.h"
#import "BDFontStyle.h"

SYNTHESIZE_SINGLETON_FOR_CLASS_PROTOTYPE(BDResourceManager);

@implementation BDResourceManager

@synthesize orientation = orientation_;

- (id)init
{
    self = [super init];
    if (self)
    {
        orientation_  = UIDeviceOrientationUnknown;
        cachedThemes_ = [[NSMutableDictionary alloc] initWithCapacity:2];
    }
    return self;
}

- (void)dealloc
{
    [cachedThemes_ release];
    [super dealloc];
}

- (UIImage *)imageNamed:(NSString *)name
{
    return [UIImage imageNamed:name];
}

- (UIImage *)noCacheImageWithPath:(NSString *)path
{
    return [[[UIImage alloc] initWithContentsOfFile:path] autorelease];
}

- (UIFont *)fontWithFontStyle:(BDFontStyle *)style
{
    UIFont * resultFont = nil;
    switch (style.fontType) {
        case BDFontTypeStandard:
            resultFont = [UIFont systemFontOfSize:style.fontSize];
            break;
        case BDFontTypeBold:
            resultFont = [UIFont boldSystemFontOfSize:style.fontSize];
            break;
        case BDFontTypeItalic:
            resultFont = [UIFont italicSystemFontOfSize:style.fontSize];
            break;
        default:
            
            break;
    }
    return resultFont;
}// fontWithFontStyle:

- (UIColor *)colorWithHex:(NSUInteger)hex
{
    return [UIColor colorWithHex:hex];
}

- (NSString *)localizedStringForKey:(NSString *)key
{
    return NSLocalizedString(key, nil);
}

#pragma mark - Load Resource

- (void)reloadWithInterfaceOrientation:(UIInterfaceOrientation)orientation
{
    orientation_ = orientation;
    
    BOOL landscape = orientation == UIInterfaceOrientationLandscapeLeft || 
                    orientation == UIInterfaceOrientationLandscapeRight;
    NSString *fileName = landscape ? kHardCodeResourceDecoderFileLand : kHardCodeResourceDecoderFilePort;
    
    //Set Current Theme
    currentTheme_ = [cachedThemes_ objectForKey:fileName];
    if (nil == currentTheme_)
    {
        BDResourceDecoderHardCode *decoder = [[BDResourceDecoderHardCode alloc] init];
        BDResourceParser *parser = [[BDResourceParser alloc] initWithDecoder:decoder];
        currentTheme_ = [parser parse:fileName];
        [cachedThemes_ setObject:currentTheme_ forKey:fileName];
        [decoder release];
        [parser  release];
    }
    
    NSAssert(nil != currentTheme_, @"No Style Manager Loaded!!!!!");
}

#pragma mark - Property

- (BDToolBarStyle *)toolBarStyle
{
    return currentTheme_.toolBarStyle;
}

- (BDToolBarStyle *)titleBarStyle
{
    return currentTheme_.titleBarStyle;
}

- (BDTextFieldStyle *)textFieldStyle
{
    return currentTheme_.textFieldStyle;
}

- (BDTitleBarStyle *)editTitleBarStyle
{
    return currentTheme_.editTitleBarStyle;
}

- (BDShortcutMenuStyle *)shortcutMenuStyle
{
    return currentTheme_.shortcutMenuStyle;
}

#pragma mark - Singleton

SYNTHESIZE_SINGLETON_FOR_CLASS(BDResourceManager);

@end
