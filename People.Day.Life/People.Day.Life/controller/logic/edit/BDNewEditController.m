//
//  BDNewEditController.m
//  People.Day.Life
//
//  Created by samuel on 13-2-14.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDNewEditController.h"
#import "NSDateAdditions.h"

#define BDTitleBarHeight (30.0)

@interface BDNewEditController ()

@end

@implementation BDNewEditController

- (void)dealloc
{
    [timeLabel_ release];
    [countLabel_ release];
    [titleBar_ release];
    
    [super dealloc];
}

- (void)loadView
{
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 460.0)];
    
    //title bar
    titleBar_ = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, BDTitleBarHeight)];
    UIImage *titleImage = [UIImage imageNamed:@"resource/titlebar/titlebar_bg.png"];
    titleImage = [titleImage stretchableImageWithLeftCapWidth:titleImage.size.width / 2.0 topCapHeight:titleImage.size.height / 2.0];
    titleBar_.image = titleImage;
    
    //time label
    timeLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(0.0, 0.0, 160.0, BDTitleBarHeight)];
    timeLabel_.textColor = [UIColor whiteColor];
    timeLabel_.text = [NSDate dateString];
    timeLabel_.font = [UIFont boldSystemFontOfSize:12];
    timeLabel_.backgroundColor = [UIColor clearColor];
    timeLabel_.textAlignment = NSTextAlignmentRight;
    [titleBar_ addSubview:timeLabel_];
    
    //count label
    countLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(180.0, 0.0, 140.0, BDTitleBarHeight)];
    countLabel_.textColor = [UIColor whiteColor];
    countLabel_.text = @"0字";
    countLabel_.font = [UIFont boldSystemFontOfSize:12];
    countLabel_.backgroundColor = [UIColor clearColor];
    countLabel_.textAlignment = NSTextAlignmentLeft;
    [titleBar_ addSubview:countLabel_];
    
    [view addSubview:titleBar_];
    
    self.view = view;
    [view release];
}

- (void)adjustHeight:(CGFloat)height
{
    
}

- (void)dismiss
{
    [self removeKeyboardObserver];
    [textView_ resignFirstResponder];
    [super dismiss];
}

- (void)showInView:(UIView *)view
{
    [self addKeyboardObserver];
    [super showInView:view];
    
    [textView_ becomeFirstResponder];
}

@end
