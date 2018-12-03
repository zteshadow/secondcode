//
//  ConcurrencyAppDelegate.h
//  Concurrency
//
//  Created by samuel on 12-2-22.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ConcurrencyViewController;

@interface ConcurrencyAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    ConcurrencyViewController *viewController;
    
    NSOperationQueue *ccQueue;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet ConcurrencyViewController *viewController;

@property (retain) NSOperationQueue *ccQueue;

@end

