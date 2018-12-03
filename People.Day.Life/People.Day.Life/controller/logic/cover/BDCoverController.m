//
//  BDCoverController.m
//  People.Day.Life
//
//  Created by samuel on 13-1-25.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoverController.h"
#import <QuartzCore/QuartzCore.h>

#import "UIImageAdditions.h"
#import "BDDataPeople.h"
#import "BDDataManager.h"
#import "NSDateAdditions.h"

#import "BD_Debug.h"

#define kBDBottonOffset     (50.0)
#define kBDLabelX           (100)
#define kBDLabelWidth       (200.0)
#define kBDLabelHeight      (20.0)
#define kBDLabelGap         (10.0)
#define kBDHeadImageHeight  (60.0)
#define kBDHeadImageOffset  (140.0)

@interface BDCoverController ()
{
    CGFloat maxHeight_;
    CGRect viewFrame_;
    
    BDDataPeople *host_;
    UILabel *birthLabel_;
    UILabel *dateLabel_;
    UIImageView *headView_;
}

@property (nonatomic, retain) BDDataPeople *host;

- (void)adjustComponentsFrame;
- (NSString *)birthStringFromDate:(NSDate *)date;

@end

@implementation BDCoverController

@synthesize host = host_;

- (id)initWithFrame:(CGRect)frame
{
    if (self = [super init])
    {
        viewFrame_ = frame;
        
        //contentMode设置为UIViewContentModeCenter, 只显示图片的中间1半
        maxHeight_ = frame.size.height * 2.0;
        
        self.host = [BDDataManager sharedInstance].hostData;
    }
    
    return self;
}

- (void)dealloc
{
    self.host = nil;
    [birthLabel_ release];
    [dateLabel_ release];
    [headView_ release];
    
    [super dealloc];
}

- (void)loadView
{
    CGFloat height = viewFrame_.size.height;
    
    //back image
    UIImage *image = [UIImage imageNamed:@"resource/default/group_cover.jpg"];
    image = [image imageToFitSize:CGSizeMake(viewFrame_.size.width, height * 2)];
    
    UIImageView *backView = [[UIImageView alloc] initWithImage:image];    
    backView.clipsToBounds = YES;
    backView.frame = viewFrame_;
    backView.contentMode = UIViewContentModeCenter;
    
    [self setView:backView];
    [backView release];
    
    //head
    image = [UIImage imageNamed:@"resource/default/man_image.jpg"];
    image = [image imageToFitSize:CGSizeMake(kBDHeadImageHeight, kBDHeadImageHeight)];
    image = [image imageInCircle:60.0];
        
    headView_ = [[UIImageView alloc] initWithImage:image];
    headView_.backgroundColor = [UIColor clearColor];
    headView_.frame = CGRectMake(20.0, self.view.frame.size.height - kBDHeadImageOffset, kBDHeadImageHeight, kBDHeadImageHeight);    
    [self.view addSubview:headView_];
    
    //birth label
    birthLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(kBDLabelX, self.view.frame.size.height - kBDBottonOffset - kBDLabelHeight * 2 - kBDLabelGap, kBDLabelWidth, kBDLabelHeight)];
    birthLabel_.backgroundColor = [UIColor clearColor];
    birthLabel_.textColor = [UIColor whiteColor];
    birthLabel_.font = [UIFont systemFontOfSize:14];
    birthLabel_.text = [self birthStringFromDate:host_.birth];
    [self.view addSubview:birthLabel_];
    
    //date label
    dateLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(kBDLabelX, self.view.frame.size.height - kBDBottonOffset - kBDLabelHeight, kBDLabelWidth, kBDLabelHeight)];
    dateLabel_.backgroundColor = [UIColor clearColor];
    dateLabel_.textColor = [UIColor whiteColor];
    dateLabel_.font = [UIFont systemFontOfSize:14];
    dateLabel_.text = [NSDate dateString];
    [self.view addSubview:dateLabel_];
}

- (void)adjustComponentsFrame
{
    CGRect rect = birthLabel_.frame;
    
    rect.origin.y = self.view.frame.size.height - kBDBottonOffset - kBDLabelHeight * 2 - kBDLabelGap;
    birthLabel_.frame = rect;
    
    rect = dateLabel_.frame;
    rect.origin.y = self.view.frame.size.height - kBDBottonOffset - kBDLabelHeight;
    dateLabel_.frame = rect;
    
    rect = headView_.frame;
    rect.origin.y = self.view.frame.size.height - kBDHeadImageOffset;
    headView_.frame = rect;
}

- (NSString *)birthStringFromDate:(NSDate *)date
{
    BDDateStruct dateData = [[NSDate date] yearMonthDayFromDate:date];
    
    NSString *birthString = nil;
    
    if (dateData.year > 0)
    {
        birthString = [NSString stringWithFormat:@"%d周岁", dateData.year];
    }
    
    if (dateData.month > 0)
    {
        if (birthString != nil)
        {
            birthString = [birthString stringByAppendingFormat: @"%d个月", dateData.month];
        }
        else
        {
            birthString = [NSString stringWithFormat:@"%d个月", dateData.month];
        }
    }
    
    if (dateData.day > 0)
    {
        if (birthString != nil)
        {
            birthString = [birthString stringByAppendingFormat:@"%d天", dateData.day];
        }
        else
        {
            birthString = [NSString stringWithFormat:@"%d天", dateData.day];
        }
    }
    else
    {
        if (birthString  != nil)
        {
            birthString = [birthString stringByAppendingString:@"整"];
        }
        else
        {
            birthString = @"0岁";
        }
    }
    
    return birthString;
}

#pragma mark --
#pragma mark UIScrollViewDelegate Methods
- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    CGFloat offset = scrollView.contentOffset.y + viewFrame_.size.height;
    CGRect rect = viewFrame_;
    
    //只处理下拉，上拉不做处理
    if (offset < 0.0)
    {
        rect.origin.y += offset;
        rect.size.height -= offset;
    }
    
    if (rect.size.height > maxHeight_)
    {
        rect.size.height = maxHeight_;
        rect.origin.y = -maxHeight_;
    }
    
    self.view.frame = rect;
    [self adjustComponentsFrame];
}

@end
