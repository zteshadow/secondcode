//
//  BDToolBarController.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBarController.h"
#import "BDToolBarButton.h"
#import "BDToolBar.h"
#import "BDToolBarStyle.h"
#import "BDResourceConst.h"
#import "BDResourceManager.h"

@interface BDToolBarController()
@property (nonatomic, retain) BDToolBarStyle *toolBarStyle;
@end

@implementation BDToolBarController

@synthesize delegate = delegate_;
@synthesize toolBar = toolBar_;
@synthesize toolBarStyle = toolBarStyle_;

- (id)initWithStyle:(BDToolBarStyle *)style
{
    self = [super init];
    if (self)
    {
        self.toolBarStyle = style;
        toolBarButtons_ = [[NSMutableArray alloc] initWithCapacity:4];
    }
    
    return self;
}

- (id)init
{
    NSAssert(0, @"--use initWithStyle instead--");
    return nil;
}

- (void)dealloc
{
    [toolBarStyle_ release];
    [toolBar_ release];
    [toolBarButtons_ release];
    
    [super dealloc];
}

- (BDToolBarButton *)toolBarButtonWithType:(BDBarButtonType)buttonType
{
    for (BDToolBarButton * button in toolBarButtons_)
    {
        if (button.type == buttonType)
        {
            return button;
        }
    }
    
    return nil;
}

#pragma mark - View lifecycle

- (void)reloadToolBar
{
    BDResourceManager * resourceManager = [BDResourceManager sharedInstance];
    BDToolBarStyle *barStyle = toolBarStyle_;
    
    UIImage *barBackgroundImage = [resourceManager imageNamed:barStyle.backgroundImage];
    [toolBar_ setFrame:barStyle.barFrame];
    [toolBar_ setBackgroundImage:barBackgroundImage];
    
    [toolBarButtons_ removeAllObjects];
    for (BDButtonStyle *item in barStyle.barButtonStyles)
    {
        BDToolBarButton *barButton = [BDToolBarButton barButtonWithStyle:item];
        [barButton addTarget:self action:@selector(barButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [toolBarButtons_  addObject:barButton];
    }
    
    [toolBar_ setBackgroundImage:[resourceManager imageNamed:toolBarStyle_.backgroundImage]];
    [toolBar_ setItems:toolBarButtons_ animated:NO];
}

- (void)loadView
{
    toolBar_ = [[BDToolBar alloc] initWithFrame:CGRectZero];
    
    [self setView:toolBar_];
    [self reloadToolBar];
}

- (void)viewDidUnload
{
    self.toolBar = nil;
    [super viewDidUnload];
}

- (void)barButtonClicked:(BDToolBarButton *)sender
{
    [delegate_ toolBar:toolBar_ didClickBarButton:sender];
}

@end
