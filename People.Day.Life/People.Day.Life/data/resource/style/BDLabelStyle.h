//
//  BDLabelStyle.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDFontStyle.h"

@interface BDLabelStyle : NSObject
{
    CGRect       viewFrame_;
    NSInteger    titleColor_;
    NSInteger    colorDisabled_;
    BDFontStyle *titleFont_;
}

@property (nonatomic, readonly, assign) CGRect       viewFrame;
@property (nonatomic, readonly, assign) NSInteger    titleColor;
@property (nonatomic, readonly, assign) NSInteger    colorDisabled;

@property (nonatomic, readonly, retain) BDFontStyle *titleFont;

@end
