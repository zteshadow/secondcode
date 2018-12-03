//
//  BDResourceTheme+Private.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceTheme.h"
#import "BDButtonStyle.h"
#import "BDLabelStyle.h"
#import "BDToolBarStyle.h"
#import "BDTextFieldStyle.h"
#import "BDShortcutMenuStyle.h"
#import "BDTitleBarStyle.h"

@interface BDResourceTheme ()
@property (nonatomic, readwrite, retain) BDToolBarStyle *toolBarStyle;
@property (nonatomic, readwrite, retain) BDToolBarStyle *titleBarStyle;
@property (nonatomic, readwrite, retain) BDTextFieldStyle *textFieldStyle;
@property (nonatomic, readwrite, retain) BDTitleBarStyle *editTitleBarStyle;
@property (nonatomic, readwrite, retain) BDShortcutMenuStyle *shortcutMenuStyle;
@end

@interface BDToolBarStyle()
@property (nonatomic, readwrite, assign) CGRect barFrame;
@property (nonatomic, readwrite, retain) NSString *backgroundImage;
@property (nonatomic, readwrite, retain) NSMutableArray *barButtonStyles;
@end

@interface BDTitleBarStyle()
@property (nonatomic, readwrite, retain) BDLabelStyle *titleLabelStyle;
@end

@interface BDTextFieldStyle()
@property (nonatomic, readwrite, assign) CGRect viewFrame;
@property (nonatomic, readwrite, assign) CGRect textFieldFrame;
@property (nonatomic, readwrite, assign) CGFloat gap;
@property (nonatomic, readwrite, retain) BDLabelStyle *labelStyle;
@property (nonatomic, readwrite, retain) NSMutableArray *barButtonStyles;
@end

@interface BDFontStyle()
@property (nonatomic, readwrite, assign) CGFloat   fontSize;
@property (nonatomic, readwrite, retain) NSString *fontFamily;
@property (nonatomic, readwrite, retain) NSString *fontName;
@property (nonatomic, readwrite, assign) BDFontType fontType;
@end

@interface BDButtonStyle()
@property (nonatomic, readwrite, assign) CGRect buttonFrame;
@property (nonatomic, readwrite, assign) NSInteger buttonType;
@property (nonatomic, readwrite, assign) BDButtonAlighType alignType;
@property (nonatomic, readwrite, assign) NSInteger titleColor;
@property (nonatomic, readwrite, assign) NSInteger colorDisabled;
@property (nonatomic, readwrite, assign) NSInteger colorHighlighted;
@property (nonatomic, readwrite, retain) NSString *imageNormal;
@property (nonatomic, readwrite, retain) NSString *imageDisabled;
@property (nonatomic, readwrite, retain) NSString *imageHighlighted;
@property (nonatomic, readwrite, retain) NSString *imageBackNormal;
@property (nonatomic, readwrite, retain) NSString *imageBackHighlighted;
@property (nonatomic, readwrite, retain) BDFontStyle *titleFont;
@property (nonatomic, readwrite, retain) NSString *titleString;
@end

@interface BDLabelStyle()
@property (nonatomic, readwrite, assign) CGRect viewFrame;
@property (nonatomic, readwrite, assign) NSInteger titleColor;
@property (nonatomic, readwrite, assign) NSInteger colorDisabled;
@property (nonatomic, readwrite, retain) BDFontStyle *titleFont;
@end

@interface BDShortcutMenuStyle()
@property (nonatomic, readwrite, assign) CGFloat top;
@property (nonatomic, readwrite, assign) CGFloat bottom;
@property (nonatomic, readwrite, assign) CGFloat right;
@property (nonatomic, readwrite, assign) CGFloat radius;
@property (nonatomic, readwrite, assign) CGFloat changeSideOffset;
@property (nonatomic, readwrite, assign) CGRect viewFrameOpen;
@property (nonatomic, readwrite, assign) CGRect viewFrameClose;
@property (nonatomic, readwrite, retain) BDButtonStyle *mainButtonStyle;
@property (nonatomic, readwrite, retain) NSMutableArray *menuButtonStyles;
@end

