//
//  BDImagePickerController.m
//  People.Day.Life
//
//  Created by samuel on 13-1-6.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDImagePickerController.h"
#import "BDToolBarController.h"
#import "BDResourceManager.h"
#import "BDCoreGlobalRect.h"
#import "BDToolBarDelegate.h"
#import "BDToolBarButton.h"
#import "BD_Debug.h"

@interface BDImagePickerController () <BDToolBarDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@end

@implementation BDImagePickerController

@synthesize delegate = delegate_;

- (id)init
{
    if (self = [super init])
    {
        BDResourceManager *resourceManager = [BDResourceManager sharedInstance];
        toolBarController_ = [[BDToolBarController alloc] initWithStyle:resourceManager.toolBarStyle];
        toolBarController_.delegate = self;
        sourceType_ = -1;
    }
    
    return self;
}

- (void)dealloc
{
    [toolBarController_ release];
    [super dealloc];
}

- (BOOL)initSource
{
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
    {
        sourceType_ = UIImagePickerControllerSourceTypeCamera;
    }
    else if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeSavedPhotosAlbum])
    {
        sourceType_ = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    }
    else if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary])
    {
        sourceType_ = UIImagePickerControllerSourceTypePhotoLibrary;
    }
    else
    {
        sourceType_ = -1;
    }
    
    return sourceType_ > -1;
}

- (void)loadView
{
    CGRect rect = [UIScreen mainScreen].applicationFrame;
    rect = CGRectNormalize(rect);
    
    UIView *view = [[UIView alloc] initWithFrame:rect];
    view.autoresizesSubviews = YES;
    view.backgroundColor = [UIColor whiteColor];
    [view addSubview:toolBarController_.view];
    self.view = view;
    [view release];
}

- (void)showInView:(UIView *)parentView
{
    [parentView addSubview:self.view];
    BD_SHOW_VIEW(parentView);
    
#if 1
    UIImagePickerController *picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;
    picker.sourceType = sourceType_;
    //picker.cameraOverlayView = self.view;
    [self presentViewController:picker animated:YES completion:nil];
    
    [picker release];
#endif
}

#pragma mark --
#pragma mark UIImagePickerControllerDelegate

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    //UIImage *image = [info objectForKey:UIImagePickerControllerOriginalImage];
    [picker dismissViewControllerAnimated:NO completion:nil];
    //[self dismiss];
    //[delegate_ imagePickerController:self didFinishPickingImage:image withInfo:YES];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
    //[self dismiss];
    //[delegate_ imagePickerController:self didFinishPickingImage:nil withInfo:YES];
}

#pragma mark --
#pragma mark BDToolBarDelegate
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button
{
    switch (button.type)
    {
    case kBarButtonTypeImagePickerExit:
    {
        [self dismiss];
    }
    break;

    default:
    break;
    }
}
@end
