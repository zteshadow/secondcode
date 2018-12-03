//
//  BDTitleBarController.m
//  People.Day.Life
//
//  Created by samuel on 13-1-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDTitleBarController.h"
#import "BDToolBarController+Private.h"
#import "BDTitleBarStyle.h"
#import "BDLabelStyle.h"
#import "BDToolBar.h"
#import "BDResourceManager.h"

@implementation BDTitleBarController

- (id)initWithStyle:(BDTitleBarStyle *)style
{
    self = [super initWithStyle:style];
    return self;
}

- (void)dealloc
{
    [titleLabel_ release];
    
    [super dealloc];
}

- (NSString *)titleWithCurrentTime
{
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
    
    NSInteger unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit |
    NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit;
    
    NSDateComponents *comps  = [calendar components:unitFlags fromDate:[NSDate date]];
    
    NSInteger year = [comps year];
    NSInteger month = [comps month];
    NSInteger day = [comps day];
    NSString *title = [NSString stringWithFormat:@"%d年%d月%d日", year, month, day];
    [calendar release];
    
    return title;
}

- (void)reloadToolBar
{
    [super reloadToolBar];
    
    BDTitleBarStyle *style = (BDTitleBarStyle *)toolBarStyle_;
    BDLabelStyle *labelStyle = style.titleLabelStyle;
    
    BDResourceManager *resourceManager = [BDResourceManager sharedInstance];
    titleLabel_ = [[UILabel alloc] initWithFrame:labelStyle.viewFrame];
    titleLabel_.textColor = [resourceManager colorWithHex:labelStyle.titleColor];
    titleLabel_.text = [self titleWithCurrentTime];
    titleLabel_.font = [resourceManager fontWithFontStyle:labelStyle.titleFont];
    titleLabel_.backgroundColor = [UIColor clearColor];
    titleLabel_.textAlignment = NSTextAlignmentCenter;
    [toolBar_ addSubview:titleLabel_];
}

@end
