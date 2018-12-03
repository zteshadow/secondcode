//
//  WebViewManager.m
//  WebViewTest
//
//  Created by majie on 12-3-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "WebViewManager.h"
#include <stdlib.h>


#define MAX_HISTORY (50)
#define SaveURL(history, i, url) \
{\
    if ((history)[(i)] != 0)\
    {\
        [(history)[(i)] release];\
    }\
    (history)[(i)] = [[NSString alloc] initWithString:(url)];\
}

#define IncViewIndex(d, max) \
{\
    (d) = (d) + 1;\
    if ((d) >= (max))\
    {\
        (d) = (d) - (max);\
    }\
}

#define DecViewIndex(d, max) \
{\
    (d) = (d) - 1;\
    if ((d) < 0)\
    {\
        (d) = (d) + (max);\
    }\
}


@implementation WebViewManager

- (id)initWith:(NSUInteger)viewCnt withDelegate:(id)delegate
{
    if (self = [super init])
    {
        //init view
        viewMax = viewCnt;
        viewCurrent = -1;
        view = (ViewItem *)malloc(sizeof(ViewItem) * viewCnt);
        assert(view != 0);
        
        for (int i = 0; i < viewCnt; i++)
        {
            UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectMake(0.0, 88.0, 320.0, 328.0)];
            
            //webView.clearsContextBeforeDrawing = YES;
            webView.backgroundColor = [UIColor whiteColor];
            webView.delegate = delegate;
            
            view[i].webView = webView;
            view[i].isLoaded = NO;
            view[i].url = 0;
        }
    
        //init history
        historyMax = 0;
        historyCurrent = -1;
        historyCapacity = MAX_HISTORY;
        history = (HistoryItem *)malloc(sizeof(HistoryItem) * MAX_HISTORY);
        assert(history != 0);
        
        memset(history, 0, sizeof(HistoryItem) * MAX_HISTORY);
    }
    
    return self;
}

- (void)dealloc
{
    //clear view
    if (view != 0)
    {
        for (int i = 0; i < viewMax; i++)
        {
            [view[i].webView release];
        }
        
        free(view);
    }
    
    //clear history
    if (history != 0)
    {
        for (int i = 0; i < historyMax; i++)
        {
            [history[i] release];
        }
        
        free(history);
    }
    
    [super dealloc];
}

- (UIWebView *)loadURL:(NSString *)url
{
    UIWebView *p = 0;
    
    if (++historyCurrent >= historyCapacity)
    {
        [self expand];
    }
    
    historyMax = historyCurrent + 1;
    SaveURL(history, historyCurrent, url);
    
    IncViewIndex(viewCurrent, viewMax);
    view[viewCurrent].isLoaded = NO;
    view[viewCurrent].url = history[historyCurrent];

    p = view[viewCurrent].webView;
    if (p != 0)
    {
        [p loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:url]]];
    }
    
    return p;
}

- (void)setURL:(NSString *)url
{
    //SaveURL(history, historyCurrent, url);
}

- (void)reload
{
    UIWebView *webView = view[viewCurrent].webView;
    
    view[viewCurrent].isLoaded = NO;
    [webView reload];
}

- (UIWebView *)goBack
{
    UIWebView *webView = 0;
    
    if ([self canGoBack])
    {
        historyCurrent--;
        DecViewIndex(viewCurrent, viewMax);
        
        webView = view[viewCurrent].webView;
        //NSString *webViewURL = [webView stringByEvaluatingJavaScriptFromString:@"document.URL"];
        
        NSString *webViewURL = view[viewCurrent].url;
        NSString *historyURL = history[historyCurrent];
        
        //if ([webViewURL isEqualToString:historyURL] == NO)
        if (webViewURL != historyURL)
        {
            view[viewCurrent].isLoaded = NO;
            view[viewCurrent].url = historyURL;
            [webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:historyURL]]];
        }
    }
    
    return webView;
}

- (UIWebView *)goForward
{
    UIWebView *webView = 0;
    
    if ([self canGoForward])
    {
        historyCurrent++;
        IncViewIndex(viewCurrent, viewMax);
        
        webView = view[viewCurrent].webView;
        
        NSString *webViewURL = view[viewCurrent].url;
        NSString *historyURL = history[historyCurrent];

        //NSString *webViewURL = [webView stringByEvaluatingJavaScriptFromString:@"document.URL"];
        
        //if (![webViewURL isEqualToString:history[historyCurrent]])
        if (webViewURL != historyURL)
        {
            view[viewCurrent].isLoaded = NO;
            view[viewCurrent].url = historyURL;
            [webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:historyURL]]];
        }
    }
    
    return webView;    
}

- (void)reset
{
    viewCurrent = -1;
    historyCurrent = -1;
    historyMax = 0;
}

- (BOOL)canGoBack
{
    if (historyCurrent > 0)
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

- (BOOL)canGoForward
{
    if (historyCurrent + 1 >= historyMax)
    {
        return NO;
    }
    else
    {
        return YES;
    }
}

- (BOOL)isViewLoaded:(UIWebView *)webView
{
    NSInteger i = [self findView:webView];
    
    if (i >= 0)
    {
        return view[i].isLoaded;
    }
    else
    {
        return NO;
    }
}

- (void)setViewLoaded:(UIWebView *)webView
{
    NSInteger i = [self findView:webView];
    
    if (i >= 0)
    {
        view[i].isLoaded = YES;
    }
}

- (void)expand
{
    NSUInteger newSize = historyCapacity * 2;
    
    HistoryItem *p = (HistoryItem *)malloc(sizeof(HistoryItem) * newSize);
    if (p != 0)
    {
        memset(p, 0, sizeof(HistoryItem) * newSize);
        memcpy(p, history, sizeof(HistoryItem) * historyCapacity);
    }
    
    free(history);
    history = p;
    
    historyCapacity = newSize;
}

- (NSInteger)findView:(UIWebView *)webView
{
    NSInteger i;
    
    for (i = 0; i < viewMax; i++)
    {
        if (view[i].webView == webView)
        {
            return i;
        }
    }
    
    return -1;
}

@end
