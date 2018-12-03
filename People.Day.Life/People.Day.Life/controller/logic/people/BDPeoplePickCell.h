//
//  BDPeoplePickCell.h
//  People.Day.Life
//
//  Created by samuel on 13-2-24.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BDDataPeople;

@interface BDPeoplePickCell : UITableViewCell
{
    BDDataPeople *people_;
}

@property (nonatomic, retain) BDDataPeople *people;

@end
