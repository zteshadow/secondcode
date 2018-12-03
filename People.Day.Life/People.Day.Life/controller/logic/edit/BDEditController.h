//
//  BDEditController.h
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

/*
 WARNING use showInView and dismiss rather than xxx addSubview:self.view
 */

#import "BDBaseController.h"
#import "BDToolBarDelegate.h"
#import "BDPeoplePickController.h"

@class BDTitleBarController;
@class BDTextFieldController;
@class BDTextField;

@interface BDEditController : BDBaseController <BDToolBarDelegate, BDPeoplePickDelegate>
{
    id<BDToolBarDelegate>   delegate_;
    BDTextFieldController *inputFieldController_;
    BDTitleBarController *titleController_;
}

@property (nonatomic, assign) id<BDToolBarDelegate> delegate;

@end
