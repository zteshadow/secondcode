//
//  BDRootController+Private.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDRootController+Private.h"
#import "BDToolBarController.h"

@implementation BDRootController(Private)

- (void)applicationWillResignActive:(NSNotification *)notification
{
    //
    // do something to record the app's state
#if 0
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    [defaults synchronize];
#endif
}

- (void)imagePickerController:(BDImagePickerController *)picker didFinishPickingImage:(UIImage *)image
                     withInfo:(BOOL)result
{
    [imagePickerController_ dismiss];
}

@end
