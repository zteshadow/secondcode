//
//  BDGestureHandle.h
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


@interface BDGestureHandle : NSObject <UIGestureRecognizerDelegate, UIActionSheetDelegate>
{
    NSString *link_;
    UILongPressGestureRecognizer *recognizer_;
    UIWebView *webView_;
}


@property(nonatomic, retain) UIWebView *webView;


- (id)initWithWebView:(UIWebView *)webView;


@end
