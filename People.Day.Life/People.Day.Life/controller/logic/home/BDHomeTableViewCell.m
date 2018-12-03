//
//  BDHomeTableViewCell.m
//  People.Day.Life
//
//  Created by samuel on 13-2-27.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDHomeTableViewCell.h"
#import "BDDataRecord.h"

#import "BD_Debug.h"

#define kBDMaxTextHeight            (9999.0)

#define kBDBackViewTopOffset        (10.0f)
#define kBDBackViewLeftOffset       (40.0f)
#define kBDBackViewBottomOffset     (10)
#define kBDBackViewRightOffset      (10)

#define kBDTextTopOffset            (10)
#define kBDTextLeftOffset           (10)
#define kBDTextBottomOffset         (10)
#define kBDTextRightOffset          (10)

#define kBDHomeCellFont             (12.0)

#define kBDHomeCellTextWidth        (320.0 - kBDBackViewLeftOffset - kBDBackViewRightOffset - kBDTextLeftOffset - kBDTextRightOffset)

@interface BDHomeTableViewCell()
@property (nonatomic, retain) BDDataRecord *dataRecord;
@end

@implementation BDHomeTableViewCell

@synthesize dataRecord = dataRecord_;

+ (CGSize)recordSize:(BDDataRecord *)record
{
    NSString *text = record.text;
    CGSize size = CGSizeMake(kBDHomeCellTextWidth, kBDMaxTextHeight);
    CGSize textSize = [text sizeWithFont:[UIFont systemFontOfSize:kBDHomeCellFont] constrainedToSize:size lineBreakMode:NSLineBreakByCharWrapping];
    
    textSize.height += kBDBackViewTopOffset + kBDBackViewBottomOffset + kBDTextTopOffset + kBDTextBottomOffset;
    return textSize;    
}

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self)
    {
        backViewInsets_ = UIEdgeInsetsMake(kBDBackViewTopOffset, kBDBackViewLeftOffset, kBDBackViewBottomOffset, kBDBackViewRightOffset);

        textInsets_ = UIEdgeInsetsMake(kBDTextTopOffset, kBDTextLeftOffset, kBDTextBottomOffset, kBDTextRightOffset);
    }
    
    return self;
}

- (void)dealloc
{
    [dataRecord_ release];
    
    [super dealloc];
}

- (void)drawRect:(CGRect)rect
{
    [super drawRect:rect];

    //back image
    UIImage *backImage = [UIImage imageNamed:@"resource/table/table_text_bg.png"];
    backImage = [backImage stretchableImageWithLeftCapWidth:backImage.size.width / 2 topCapHeight:backImage.size.height / 2.0 + 6.0];
    CGRect backRect = UIEdgeInsetsInsetRect(rect, backViewInsets_);
    [backImage drawInRect:backRect];

    //text
    NSString *text = dataRecord_.text;
    CGRect textRect = UIEdgeInsetsInsetRect(backRect, textInsets_);
    [text drawInRect:textRect withFont:[UIFont systemFontOfSize:kBDHomeCellFont]];
}

- (void)loadDataRecord:(BDDataRecord *)record
{
    self.dataRecord = record;    
}

@end
