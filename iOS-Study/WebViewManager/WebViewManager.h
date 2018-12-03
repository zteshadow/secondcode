//
//  WebViewManager.h
//  WebViewTest
//
//  Created by majie on 12-3-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>


typedef NSString *HistoryItem;

typedef struct
{
    UIWebView *webView;
    NSString *url;
    BOOL isLoaded;
} ViewItem;


@interface WebViewManager : NSObject
{
    NSUInteger historyCapacity;
    NSUInteger historyMax;
    NSInteger historyCurrent;
    HistoryItem * history;
    
    NSUInteger viewMax;
    NSInteger viewCurrent;
    ViewItem *view;
}

- (id)initWith:(NSUInteger)viewCnt withDelegate:(id)delegate;
- (void)dealloc;

- (UIWebView *)loadURL:(NSString *)url;
- (void)setURL:(NSString *)url;

- (void)reload;
- (UIWebView *)goBack;
- (UIWebView *)goForward;

- (void)reset;
- (BOOL)canGoBack;
- (BOOL)canGoForward;

- (BOOL)isViewLoaded:(UIWebView *)view;
- (void)setViewLoaded:(UIWebView *)view;

//private
- (void)expand;
- (NSInteger)findView:(UIWebView *)view;


@end
