//
//  BDGroupController.h
//  People.Day.Life
//
//  Created by samuel on 13-2-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BDBaseController.h"

@class BDGroupDetailController;

@interface BDGroupController : BDBaseController
{
    UILabel *titleLabel_;
    UITextView *infoLabel_;
    UITableView *tableView_;
    UIImageView *titleBar_;
    UIButton *backButton_;
    
    BDGroupDetailController *detailController_;
}

@end
