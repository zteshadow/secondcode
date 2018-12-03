//
//  BDTitleBarStyle.h
//  People.Day.Life
//
//  Created by samuel on 13-1-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDToolBarStyle.h"

@class BDLabelStyle;

//tool bar with title(UILabel)
@interface BDTitleBarStyle : BDToolBarStyle
{
    BDLabelStyle *titleLableStyle_;
}

@property (nonatomic, readonly, retain) BDLabelStyle *titleLabelStyle;

@end
