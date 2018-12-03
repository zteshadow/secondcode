//
//  BDToolBarController.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDBaseController.h"
#import "BDToolBarDelegate.h"
#import "BDResourceConst.h"

@class BDToolBar;
@class BDToolBarButton;
@class BDToolBarStyle;

@interface BDToolBarController : BDBaseController
{
    BDToolBarStyle          *toolBarStyle_;
    BDToolBar               *toolBar_;
    NSMutableArray          *toolBarButtons_;
    id<BDToolBarDelegate>   delegate_;
}

@property (nonatomic, assign) id<BDToolBarDelegate> delegate;
@property (nonatomic, retain) BDToolBar *toolBar;

- (id)initWithStyle:(BDToolBarStyle *)style;
- (BDToolBarButton *)toolBarButtonWithType:(BDBarButtonType)buttonType;

@end
