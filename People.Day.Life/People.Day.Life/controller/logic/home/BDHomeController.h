//
//  BDHomeController.h
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDBaseController.h"
#import "BDToolBarDelegate.h"

@class BDToolBarController;
@class BDShortcutMenuController;
@class BDHomeView;
@class BDToolBarButton;
@class BDCoverController;

@interface BDHomeController : BDBaseController
{
    UIView *mainView_;
    BDHomeView *rootView_;
    
    id<BDToolBarDelegate>   delegate_;
    
    UILabel *titleLabel_;
    BDToolBarController *titleController_;
    BDShortcutMenuController *shortcutMenuController_;
    
    BDCoverController *coverController_;
    UITableView *tableView_;
}

@property (nonatomic, assign) id<BDToolBarDelegate> delegate;

- (id)init;
- (void)loadView;

@end
