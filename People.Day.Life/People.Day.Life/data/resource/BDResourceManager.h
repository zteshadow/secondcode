//
//  BDResourceManager.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDSingletonSynthesizer.h"
#import "BDResourceTheme.h"
#import "BDResourceConst.h"
#import "BDFontStyle.h"

@interface BDResourceManager : NSObject
{
    UIDeviceOrientation    orientation_;
    NSMutableDictionary   *cachedThemes_;
    BDResourceTheme       *currentTheme_;
}

@property (nonatomic, readonly) UIDeviceOrientation orientation;
@property (nonatomic, readonly) BDToolBarStyle *toolBarStyle;
@property (nonatomic, readonly) BDToolBarStyle *titleBarStyle;
@property (nonatomic, readonly) BDTextFieldStyle *textFieldStyle;
@property (nonatomic, readonly) BDTitleBarStyle *editTitleBarStyle;
@property (nonatomic, readonly) BDShortcutMenuStyle *shortcutMenuStyle;

- (UIImage *)imageNamed:(NSString *)name;

- (UIFont *)fontWithFontStyle:(BDFontStyle *)style;

- (UIColor *)colorWithHex:(NSUInteger)hex;

- (NSString *)localizedStringForKey:(NSString *)key;

- (void)reloadWithInterfaceOrientation:(UIInterfaceOrientation)orientation;

SYNTHESIZE_SINGLETON_FOR_CLASS_HEADER(BDResourceManager);

@end
