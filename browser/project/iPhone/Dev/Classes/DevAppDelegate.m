//
//  DevAppDelegate.m
//  Dev
//
//  Created by samuel on 12-2-2.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "DevAppDelegate.h"
#import "DevViewController.h"

@implementation DevAppDelegate

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
