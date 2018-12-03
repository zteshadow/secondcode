//
//  BDRootController+Private.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDRootController.h"
#import "BDImagePickerController.h"

@interface BDRootController()
@property (nonatomic, retain) BDBaseController *currentController;
@end

@interface BDRootController(Private)<BDImagePickerDelegate>

- (void) applicationWillResignActive:(NSNotification *)notification;

@end
