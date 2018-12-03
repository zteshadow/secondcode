//
//  BDHomeController+Private.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDHomeController.h"

@interface BDHomeController()
@property (nonatomic, readonly) BDToolBarController *titleController;
@property (nonatomic, readonly) BDShortcutMenuController *shortcutMenuController;
@end

@interface BDHomeController(Private)

- (void)setupComponents;

- (void)animateMainViewToRect:(CGRect)rect;

- (void) applicationWillResignActive:(NSNotification *)notification;

@end
