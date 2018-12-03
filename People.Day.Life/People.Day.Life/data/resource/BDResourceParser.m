//
//  BDResourceParser.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceTheme+Private.h"
#import "BDResourceParser.h"

@implementation BDResourceParser

- (id)initWithDecoder:(id)decoder
{
    self = [super init];
    if (self)
    {
        decoder_ = [decoder retain]; 
    }
    return self;
}

- (void)dealloc
{
    [decoder_ release];
    [super    dealloc];
}

- (id)parse:(NSString *)file
{
    BDResourceTheme *styles = nil;
    if ([decoder_ decodeBeginWithFile:file])
    {
        styles = [[BDResourceTheme alloc] init];
        styles.toolBarStyle = [decoder_ decodeToolBarStyle];
        styles.titleBarStyle = [decoder_ decodeTitleBarStyle];
        styles.textFieldStyle = [decoder_ decodeTextFieldStyle];
        styles.editTitleBarStyle = [decoder_ decodeEditTitleBarStyle];
        styles.shortcutMenuStyle = [decoder_ decodeShortcutMenuStyle];
    }
    [decoder_ decodeEndWithFile:file];
    return [styles autorelease];
}

@end
