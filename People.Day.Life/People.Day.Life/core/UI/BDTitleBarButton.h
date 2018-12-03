//
//  BDTitleBarButton.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDToolBarButton.h"

@interface BDTitleBarButton : BDToolBarButton
{
    NSString *title_;
    UIColor *color_;
    UIFont *font_;
}

@property (nonatomic, retain) NSString *title;
@property (nonatomic, retain) UIColor *color;
@property (nonatomic, retain) UIFont *font;

+ (BDTitleBarButton *)titleBarButtonWithStyle:(BDButtonStyle *)style;

@end
