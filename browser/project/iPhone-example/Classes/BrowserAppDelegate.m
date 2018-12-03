//
//  BrowserAppDelegate.m
//  Browser
//
//  Created by samuel on 11-11-29.
//  Copyright Baidu ,. Ltd 2011. All rights reserved.
//

#import "BrowserAppDelegate.h"
#import "BrowserViewController.h"

@implementation BrowserAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
