//
//  DevAppDelegate.h
//  Dev
//
//  Created by samuel on 12-2-2.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DevViewController;

@interface DevAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    DevViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet DevViewController *viewController;

@end

