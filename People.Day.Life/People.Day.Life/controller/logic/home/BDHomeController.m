//
//  BDHomeController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDHomeController.h"
#import "BDHomeController+ToolBar.h"
#import "BDToolBarController.h"
#import "BDHomeView.h"
#import "BDCoreGlobalRect.h"
#import "BDHomeController+Private.h"
#import "BDResourceManager.h"
#import "BDShortcutMenuController.h"
#import "BDHomeController+TableView.h"
#import "BDCoverController.h"

#import "BD_Debug.h"

@implementation BDHomeController

@synthesize titleController = titleController_;
@synthesize shortcutMenuController = shortcutMenuController_;
@synthesize delegate = delegate_;

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
        
        BDResourceManager * resourceManager = [BDResourceManager sharedInstance];
        
        titleController_ = [[BDToolBarController alloc] initWithStyle:resourceManager.titleBarStyle];
        
        shortcutMenuController_ = [[BDShortcutMenuController alloc] initWithStyle:resourceManager.shortcutMenuStyle];
        
        coverController_ = [[BDCoverController alloc] initWithFrame:CGRectMake(0.0, -160, 320.0, 160)];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [mainView_ release];
    [rootView_ release];

    [titleLabel_ release];
    [titleController_ release];
    [shortcutMenuController_ release];
    
    [tableView_ release];
    [coverController_ release];
    
    [super dealloc];
}

- (void)loadView
{
    CGRect contentRect = [UIScreen mainScreen].applicationFrame;
    CGRect rect = CGRectNormalize(contentRect);
    
    mainView_ = [[UIView alloc] initWithFrame:rect];
    rootView_ = [[BDHomeView alloc] initWithFrame:rect];
    rootView_.autoresizesSubviews = NO;
    [rootView_ addSubview:mainView_];
    
    //test
    rootView_.backgroundColor = [UIColor whiteColor];
    
    rect.origin.y += 44.0;
    tableView_ = [[UITableView alloc] initWithFrame:rect style:UITableViewStylePlain];
    //tableView_.backgroundColor = [UIColor yellowColor];
    tableView_.delegate = self;
    tableView_.dataSource = self;
    tableView_.scrollEnabled = YES;
    tableView_.allowsSelection = NO;
    [tableView_ setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    [tableView_ addSubview:coverController_.view];
    tableView_.contentInset = UIEdgeInsetsMake(160.0, 0, 0, 0);
    
    UIImage *backImage = [UIImage imageNamed:@"resource/table/table_bg.png"];
    backImage = [backImage stretchableImageWithLeftCapWidth:backImage.size.width / 2.0 topCapHeight:backImage.size.height / 2.0];
    UIImageView *backView = [[UIImageView alloc] initWithImage:backImage];
    //backView.backgroundColor = [UIColor purpleColor];
    //tableView_.backgroundView = backView;
    [backView release];
    
    [mainView_ addSubview:tableView_];
    
    self.view = rootView_;
    
    [self setupComponents];
    
    titleLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(80.0, 0.0, 160.0, 44)];
    titleLabel_.textColor = [UIColor whiteColor];
    titleLabel_.text = @"我的生活";
    titleLabel_.font = [UIFont boldSystemFontOfSize:20];
    titleLabel_.backgroundColor = [UIColor clearColor];
    titleLabel_.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:titleLabel_];
}

- (void)setDelegate:(id<BDToolBarDelegate>)delegate
{
    delegate_ = delegate;
    titleController_.delegate = delegate;
    shortcutMenuController_.delegate = delegate;
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

@end
