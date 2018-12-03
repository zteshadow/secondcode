//
//  AppDelegate.h
//  People.Day.Life
//
//  Created by samuel on 12-12-2.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BDRootController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
    BDRootController *rootController_;
}

@property (nonatomic, retain) BDRootController *rootController;
@property (strong, nonatomic) UIWindow *window;

@end
