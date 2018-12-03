//
//  BDNewEditController.h
//  People.Day.Life
//
//  Created by samuel on 13-2-14.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDBaseController.h"

@interface BDNewEditController : BDBaseController <UITextViewDelegate>
{
    UILabel *countLabel_;
    UILabel *timeLabel_;
    UITextView *textView_;
    UIImageView *titleBar_;

}

- (void)adjustHeight:(CGFloat)h;

@end
