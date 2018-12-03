//
//  BDRootController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012å¹?samuel. All rights reserved.
//

#import "BDRootController.h"
#import "BDRootController+ToolBar.h"
#import "BDRootView.h"
#import "BDCoreGlobalRect.h"
#import "BDRootController+Private.h"
#import "BDResourceManager.h"
#import "BDHomeController.h"
#import "BDEditController.h"
#import "BDImagePickerController.h"
#import "BDGroupController.h"

#import "BD_Debug.h"

@implementation BDRootController

@synthesize currentController = currentController_;

- (id)init
{
    self = [super init];
    if (self)
    {        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleOrientationChangedNotification:)
                                                     name:UIDeviceOrientationDidChangeNotification
                                                   object:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(applicationWillResignActive:)
                                                     name:UIApplicationWillResignActiveNotification
                                                   object:nil];
        
        homeController_ = [[BDHomeController alloc] init];
        homeController_.delegate = self;
        
        editController_ = [[BDEditController alloc] init];
        editController_.delegate = self;
        
        imagePickerController_ = [[BDImagePickerController alloc] init];
        imagePickerController_.delegate = self;
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [mainView_ release];
    [rootView_ release];
    
    [currentController_ release];
    [homeController_ release];
    [editController_ release];
    
    [groupController_ release];
    
    [super dealloc];
}

- (void)loadView
{
    CGRect rect = [UIScreen mainScreen].applicationFrame;
    
    mainView_ = [[UIView alloc] initWithFrame:CGRectNormalize(rect)];
    rootView_ = [[BDRootView alloc] initWithFrame:rect];
    rootView_.autoresizesSubviews = NO;
    [rootView_ addSubview:mainView_];
    
    //test
    rootView_.backgroundColor = [UIColor whiteColor];
    
    self.view = rootView_;
    
    [self showHomeView];
}

- (void)handleOrientationChangedNotification:(NSNotification *)notification
{
    UIDeviceOrientation orientation = [UIDevice currentDevice].orientation;
    if (orientation != UIDeviceOrientationUnknown && orientation != UIDeviceOrientationFaceUp && orientation != UIDeviceOrientationFaceDown)
    {
        if ([BDResourceManager sharedInstance].orientation != orientation)
        {
            //[[BDResourceManager sharedInstance] reloadWithInterfaceOrientation:orientation];
        }
    }
}

#pragma mark - Rotation
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    //notice child controller to rotate
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    //notice child controller to rotate
}

@end
