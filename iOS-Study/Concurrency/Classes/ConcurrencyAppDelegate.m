//
//  ConcurrencyAppDelegate.m
//  Concurrency
//
//  Created by samuel on 12-2-22.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "ConcurrencyAppDelegate.h"
#import "ConcurrencyViewController.h"

@implementation ConcurrencyAppDelegate

@synthesize window;
@synthesize viewController;

@synthesize ccQueue;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
    
    NSOperationQueue *q = [[NSOperationQueue alloc] init];
    self.ccQueue = q;
    [q release];
}


- (void)dealloc {
    
    [ccQueue release];
    [viewController release];
    [window release];
    [super dealloc];
}


@end
