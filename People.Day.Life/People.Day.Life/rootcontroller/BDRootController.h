//
//  BDRootController.h
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012å¹?samuel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BDToolBarDelegate.h"

@class BDRootView;
@class BDHomeController;
@class BDEditController;
@class BDBaseController;
@class BDImagePickerController;
@class BDGroupController;

@interface BDRootController : UIViewController
{
    UIView *mainView_;
    BDRootView *rootView_;
    
    BDBaseController *currentController_;
    BDHomeController *homeController_;
    BDEditController *editController_;
    BDImagePickerController *imagePickerController_;
    
    BDGroupController *groupController_;
}

- (id)init;

- (void)loadView;

#pragma rotate
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration;
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation;

@end
