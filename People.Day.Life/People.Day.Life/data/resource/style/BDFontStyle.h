//
//  BDFontStyle.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-5-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum
{
    BDFontTypeStandard,
    BDFontTypeBold,
    BDFontTypeItalic,
}BDFontType;

@interface BDFontStyle : NSObject
{
    CGFloat   fontSize_;
    NSString *fontFamily_;
    NSString *fontName_;
    BDFontType fontType_;
}

@property (nonatomic, readonly, assign) CGFloat   fontSize;
@property (nonatomic, readonly, retain) NSString *fontFamily;
@property (nonatomic, readonly, retain) NSString *fontName;
@property (nonatomic, readonly, assign) BDFontType fontType; 

// Jubal
+ (BDFontStyle *) fontStyleFromStyle:(BDFontStyle *)style;

@end
