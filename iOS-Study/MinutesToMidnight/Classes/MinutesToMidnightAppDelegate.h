//
//  MinutesToMidnightAppDelegate.h
//  MinutesToMidnight
//
//  Created by samuel on 12-2-14.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MinutesToMidnightViewController;

@interface MinutesToMidnightAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MinutesToMidnightViewController *viewController;
    NSTimer *timer;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MinutesToMidnightViewController *viewController;

- (void)onTimer;

@end

