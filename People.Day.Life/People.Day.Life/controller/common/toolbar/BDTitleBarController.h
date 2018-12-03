//
//  BDTitleBarController.h
//  People.Day.Life
//
//  Created by samuel on 13-1-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDToolBarController.h"

@class BDTitleBarStyle;

@interface BDTitleBarController : BDToolBarController
{
    UILabel *titleLabel_;
}

- (id)initWithStyle:(BDTitleBarStyle *)style;

@end
