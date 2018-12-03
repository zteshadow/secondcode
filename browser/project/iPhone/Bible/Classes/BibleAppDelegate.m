//
//  BibleAppDelegate.m
//  Bible
//
//  Created by samuel on 12-1-18.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "BibleAppDelegate.h"
#import "BibleViewController.h"

@implementation BibleAppDelegate

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
