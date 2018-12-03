//
//  BDShortcutMenuStyle.h
//  People.Day.Life
//
//  Created by samuel on 12-12-30.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BDButtonStyle;

@interface BDShortcutMenuStyle : NSObject
{
    CGFloat top_, bottom_, right_;
    CGFloat changeSideOffset_, radius_;
    CGRect viewFrameClose_, viewFrameOpen_;
    BDButtonStyle *mainButtonStyle_;
    NSMutableArray *menuButtonStyles_;
}

@property (nonatomic, readonly, assign) CGFloat top;
@property (nonatomic, readonly, assign) CGFloat bottom;
@property (nonatomic, readonly, assign) CGFloat right;
@property (nonatomic, readonly, assign) CGFloat radius;
@property (nonatomic, readonly, assign) CGFloat changeSideOffset;
@property (nonatomic, readonly, assign) CGRect viewFrameOpen;
@property (nonatomic, readonly, assign) CGRect viewFrameClose;
@property (nonatomic, readonly, retain) BDButtonStyle *mainButtonStyle;
@property (nonatomic, readonly, retain) NSMutableArray *menuButtonStyles;

@end
