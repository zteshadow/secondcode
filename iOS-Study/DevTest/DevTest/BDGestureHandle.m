//
//  BDGestureHandle.m
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDGestureHandle.h"
#import "BDGestureHandle+Private.h"


@implementation BDGestureHandle


@synthesize webView = webView_;


- (id)initWithWebView:(UIWebView *)tmpWebView
{
    if (self = [super init])
    {
        self.webView = tmpWebView;
        
        recognizer_ = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];

        [recognizer_ setDelegate:self];
        [webView_ addGestureRecognizer:recognizer_];
    }
    
    return self;
}

- (void)dealloc
{
    [webView_ release];
    [link_ release];
    [recognizer_ release];
    
    [super dealloc];
}

#pragma mark -
#pragma mark UIGestureRecognizerDelegate
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    return YES;
}

#pragma mark -
#pragma mark UIActionSheetDelegate
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    
}

- (void)actionSheet:(UIActionSheet *)actionSheet willDismissWithButtonIndex:(NSInteger)buttonIndex
{
}

- (void)actionSheetCancel:(UIActionSheet *)actionSheet
{
    
}

- (void)didPresentActionSheet:(UIActionSheet *)actionSheet
{
    
}

- (void)willPresentActionSheet:(UIActionSheet *)actionSheet
{
    
}


@end
