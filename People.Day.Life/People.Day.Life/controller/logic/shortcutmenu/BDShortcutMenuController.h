//
//  BDShortcutMenuController.h
//  People.Day.Life
//
//  Created by samuel on 12-12-30.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDBaseController.h"
#import "BDToolBarDelegate.h"

@class BDButton;
@class BDShortcutMenuStyle;
@class BDShortcutMenuView;

@interface BDShortcutMenuController : BDBaseController
{
    BOOL open_;
    CGPoint beginPoint_;
    NSInteger countDown_;
    NSInteger dockSide_;           //BDDockSideLeft
    BDShortcutMenuStyle *style_;
    BDButton *mainButton_;
    NSMutableArray *subMenu_;
    UIPanGestureRecognizer *viewRecognizer_;
    UIPanGestureRecognizer *buttonRecognizer_;
    
    id <BDToolBarDelegate> delegate_;
    
}

@property (nonatomic, assign) id<BDToolBarDelegate> delegate;

- (id)initWithStyle:(BDShortcutMenuStyle *)style;

@end
