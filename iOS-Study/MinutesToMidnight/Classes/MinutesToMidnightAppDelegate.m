//
//  MinutesToMidnightAppDelegate.m
//  MinutesToMidnight
//
//  Created by samuel on 12-2-14.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "MinutesToMidnightAppDelegate.h"
#import "MinutesToMidnightViewController.h"

@implementation MinutesToMidnightAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    timer = [NSTimer scheduledTimerWithTimeInterval:(1.0) target:self selector:@selector(onTimer) userInfo:nil repeats:YES];
    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [timer invalidate];
}

- (void)dealloc {
    [timer release];
    [viewController release];
    [window release];
    [super dealloc];
}

- (void)onTimer
{
    [viewController updateLabel];
}

@end
