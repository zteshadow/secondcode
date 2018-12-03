//
//  BibleAppDelegate.h
//  Bible
//
//  Created by samuel on 12-1-18.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BibleViewController;

@interface BibleAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    BibleViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet BibleViewController *viewController;

@end

