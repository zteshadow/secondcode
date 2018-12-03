//
//  BDGestureHandle.m
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDGestureHandle+Private.h"
#import "BDJSHandle.h"


@implementation BDGestureHandle (Private)


- (void)handleLongPress:(UILongPressGestureRecognizer *)recognizer
{
    NSLog(@"LongPress: %d", [recognizer state]);
    
    [BDJSHandle stopCallOut:self.webView];
    
    if ([recognizer state] == UIGestureRecognizerStateBegan)
    {
        [BDJSHandle stopCallOut:self.webView];
        
        CGPoint pos = [recognizer locationInView:self.webView];
        
        NSString *url = [BDJSHandle pressOnLink:self.webView atPos:pos];
        
        if ([url length] > 0)
        {
            link_ = [[NSString alloc]initWithString:url];
            
            [BDJSHandle stopCallOut:self.webView];
            [self showLinkAction:pos];
        }
        else
        {
            [recognizer.view becomeFirstResponder];
            [self showTextAction];
        }
    }
    else if ([recognizer state] == UIGestureRecognizerStateEnded)
    {
        //[self showTextAction];
    }
}

#pragma mark -
#pragma mark Text Process

- (void)showTextAction
{
    UIMenuController *popMenu = [UIMenuController sharedMenuController];
    
    UIMenuItem *makeNotes = [[UIMenuItem alloc] initWithTitle:@"Make notes" action:@selector(makeTextNotes:)];

    NSArray *array = [NSArray arrayWithObjects:makeNotes, nil];
    [popMenu setMenuItems:array];
    
    [popMenu setMenuVisible:YES animated:YES];
    
    //[popMenu setTargetRect:CGRectMake(0, 0, 80, 80) inView:self.webView];

    [makeNotes release];
}

- (void)makeTextNotes:(id)sender
{
    int tmp;
    
    tmp = 1;
    
}

#pragma mark -
#pragma mark Link Process
- (void)showLinkAction:(CGPoint)pos
{
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:link_
                                                       delegate:self
                                              cancelButtonTitle:nil 	 destructiveButtonTitle:nil
                                              otherButtonTitles:@"Open", @"Open in background", nil];
    
    sheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
    CGRect popRect = CGRectMake(pos.x, pos.y, 2.0, 2.0);

    [sheet showFromRect:popRect inView:self.webView animated:YES];
    [sheet release];
}

@end
