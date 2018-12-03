//
//  BDPeoplePickController.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDBaseController.h"

@class BDPeoplePickController;

@protocol BDPeoplePickDelegate <NSObject>
- (void)peoplePicker:(BDPeoplePickController *)picker pickPeople:(NSArray *)peoples;
@end

@interface BDPeoplePickController : BDBaseController
{
    UILabel *titleLabel_;
    UITextView *infoLabel_;
    UITableView *tableView_;
    UIImageView *titleBar_;
    UIButton *backButton_;
    UIButton *selectButton_;

    NSMutableArray *selectedPeople_;
    
    id<BDPeoplePickDelegate> delegate_;
}

@property (nonatomic, assign) id<BDPeoplePickDelegate> delegate;

@end
