//
//  BDRootController+ToolBar.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDRootController.h"
#import "BDToolBarDelegate.h"
#import "BDToolBarButton.h"

@interface BDRootController (ToolBar) <BDToolBarDelegate>

- (void)showHomeView;
- (void)switchToEditView;
- (void)switchToImagePickView;
- (void)switchToGroupView;
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button;

@end
