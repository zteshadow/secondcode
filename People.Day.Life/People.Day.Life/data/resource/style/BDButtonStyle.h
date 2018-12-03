//
//  BDButtonStyle.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDFontStyle.h"

typedef enum
{
    BDButtonAlignNone = 0,
    BDButtonAlignTop,
    BDButtonAlignBottom,
    BDButtonAlighMax
    
} BDButtonAlighType;

@interface BDButtonStyle : NSObject
{
    CGRect       buttonFrame_;
    NSInteger    buttonType_;
    
    BDButtonAlighType alighType_;
    
    NSString    *imageNormal_;
    NSString    *imageDisabled_;
    NSString    *imageHighlighted_;
    NSString    *imageBackNormal_;
    NSString    *imageBackHighlighted_;

    NSInteger    titleColor_;
    NSInteger    colorDisabled_;
    NSInteger    colorHighlighted_;
    BDFontStyle *titleFont_;
    NSString *titleString_;
}

@property (nonatomic, readonly, assign) CGRect       buttonFrame;
@property (nonatomic, readonly, assign) NSInteger    buttonType;
@property (nonatomic, readonly, assign) BDButtonAlighType alignType;
@property (nonatomic, readonly, assign) NSInteger    titleColor;
@property (nonatomic, readonly, assign) NSInteger    colorDisabled;
@property (nonatomic, readonly, assign) NSInteger    colorHighlighted;
@property (nonatomic, readonly, retain) NSString    *imageNormal;
@property (nonatomic, readonly, retain) NSString    *imageDisabled;
@property (nonatomic, readonly, retain) NSString    *imageHighlighted;
@property (nonatomic, readonly, retain) NSString    *imageBackNormal;
@property (nonatomic, readonly, retain) NSString    *imageBackHighlighted;
@property (nonatomic, readonly, retain) BDFontStyle *titleFont;
@property (nonatomic, readonly, retain) NSString *titleString;
// Jubal
+ (UIButton *) buttonFromSytle:(BDButtonStyle *)style;
+ (BDButtonStyle *) buttonStyleFromStyle:(BDButtonStyle *)style;

@end
