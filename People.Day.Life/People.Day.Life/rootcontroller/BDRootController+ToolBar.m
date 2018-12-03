//
//  BDRootController+ToolBar.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012???__MyCompanyName__. All rights reserved.
//

#import "BDRootController+ToolBar.h"
#import "BDRootController+Private.h"
#import "BDToolBarController.h"
#import "BDToolBar.h"
#import "BDToolBarButton.h"
#import "BDResourceConst.h"
#import "BDEditController.h"
#import "BDImagePickerController.h"
#import "BDGroupController.h"
#import "IFFiltersViewController.h"

@implementation BDRootController (ToolBar)

- (void)showHomeView
{
    [self.currentController dismiss];
    [homeController_ showInView:self.view];
    self.currentController = (BDBaseController *)homeController_;
}

- (void)switchToEditView
{
    [self.currentController dismiss];
    
    [editController_ showInView:self.view];
    self.currentController = (BDBaseController *)editController_;
    
    //[newEditor_ showInView:self.view];
    //self.currentController = (BDBaseController *)newEditor_;
}

- (void)switchToImagePickView
{
    //[self.currentController dismiss];
    //[imagePickerController_ showInView:self.view];
    //self.currentController = (BDBaseController *)imagePickerController_;
    
    IFFiltersViewController *controller = [[IFFiltersViewController alloc] init];
    controller.shouldLaunchAsAVideoRecorder = NO;
    controller.shouldLaunchAshighQualityVideo = NO;
    [self presentModalViewController:controller animated:YES];
}

- (void)switchToGroupView
{
    if (groupController_ == nil)
    {
        groupController_ = [[BDGroupController alloc] init];
    }
    
    [groupController_ showInView:self.view];    
}

#pragma mark - BDToolBarDelegate
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button
{
    BDBarButtonType type = button.type;
    
    switch (type)
    {
        case kBarButtonTypeSave:
        case kBarButtonTypeBackHome:
        {
            [self showHomeView];
        }
        break;
            
        case kBarButtonTypeTakePicture:
        break;

        case kBarButtonTypeWriteNote:
        {
            [self switchToEditView];
        }
        break;
            
        case kShortcutMenuButtonCamera:
        {
            if ([imagePickerController_ initSource])
            {
                [self switchToImagePickView];
            }
            else
            {
                
            }
        }
        break;
            
        case kShortcutMenuButtonNote:
        {
            [self switchToEditView];
        }
        break;
            
        case kBarButtonTypeSetting:
        {
            
        }
        break;
            
        case kBarButtonTypePeople:
        {
            [self switchToGroupView];
        }
        break;

        default:
            NSAssert(0, @"BDRootController");
        break;
    }
}

@end
