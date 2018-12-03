//
//  BDHomeTableViewCell.h
//  People.Day.Life
//
//  Created by samuel on 13-2-27.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BDDataRecord;

@interface BDHomeTableViewCell : UITableViewCell
{
    UIEdgeInsets backViewInsets_, textInsets_;
    BDDataRecord *dataRecord_;
}

+ (CGSize)recordSize:(BDDataRecord *)record;

- (void)loadDataRecord:(BDDataRecord *)record;

@end
