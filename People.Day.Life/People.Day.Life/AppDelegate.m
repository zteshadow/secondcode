//
//  AppDelegate.m
//  People.Day.Life
//
//  Created by samuel on 12-12-2.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "AppDelegate.h"
#import "BDRootController.h"
#import "BDResourceManager.h"
#import "BDDataManager.h"
#import "BDFileManager.h"
#import "NSDateAdditions.h"
#import "BD_Debug.h"

@interface AppDelegate()
- (void)startUp;
- (void)initData;
- (void)initDataHost;
@end

@implementation AppDelegate

@synthesize rootController = rootController_;

- (void)dealloc
{
    [rootController_ release];
    [_window release];
    
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initData];
    [self startUp];
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    
    //[NSThread sleepForTimeInterval:5.0];
    
    BDRootController *root = [[BDRootController alloc] init];
    self.rootController = root;
    [root release];
    
    self.window.rootViewController = rootController_;
    
    [self.window makeKeyAndVisible];
    
    UIApplication *app = [UIApplication sharedApplication];
    [app setStatusBarStyle:UIStatusBarStyleBlackTranslucent];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [BDDataManager releaseSharedInstance];
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

#pragma Private Methods
- (void)startUp
{
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    [[BDResourceManager sharedInstance] reloadWithInterfaceOrientation:orientation];
}

- (void)initData
{
    [BDDataManager initSharedInstance];
    
    BDDataManager *dataManager = [BDDataManager sharedInstance];
    [dataManager resetDefaultDataIfNeeded];
    
    //init host
    //TEST
    [self initDataHost];
}

//测试代码，在document根目录写一个host.plist
- (void)initDataHost
{
    NSString *path = [BDFileManager commonPathForType:kBDCommonHost];
    BD_ASSERT(path != nil);
    
    if (![BDFileManager fileExistsAtPath:path])
    {
        NSMutableDictionary *pathDict = [[NSMutableDictionary alloc] initWithCapacity:0];
        
        [pathDict setObject:@"samuel" forKey:@"name"];
        [pathDict setObject:[BDFileManager defaultPathForType:kBDDefaultManImage] forKey:@"image"];
        [pathDict setObject:[BDFileManager defaultPathForType:kBDDefaultManCover] forKey:@"cover"];
        NSDate *birthDate = [NSDate dateWithYear:2012 month:12 day:15];
        NSString *birthString = [birthDate stringWithFormat:@"yyyy-MM-dd"];
        [pathDict setObject:birthString forKey:@"birth"];
        
        [pathDict setObject:@"NO" forKey:@"state"];
        
        BOOL value = [[NSPropertyListSerialization dataWithPropertyList:pathDict format:NSPropertyListBinaryFormat_v1_0 options:0 error:nil] writeToFile:path atomically:YES];
        
        BD_ASSERT(value == YES);
        
        [pathDict release];
    }
}

@end
