//
//  ViewController.h
//  DevTest
//
//  Created by majie on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MPMoviePlayerController.h>
#import <Foundation/NSURLConnection.h>


@class BDActionViewController;

@interface ViewController : UIViewController <UIWebViewDelegate, NSURLConnectionDelegate,UIPopoverControllerDelegate, UIGestureRecognizerDelegate>
{
    BOOL isInPopover;
    BOOL isJittering;
    
    NSMutableArray *itemCollection;
    NSInteger itemIndex;
    CGPoint topRight;
    NSInteger maxLineHeight;
    NSMutableData *imgData;
    
    UIView *cover_view;
    NSString *jsCode;
    UIWebView *webView;
    UIActivityIndicatorView *waitView;
    MPMoviePlayerController *player;
    
    BDActionViewController *actionController;
}


@property(nonatomic, retain) NSMutableData *imgData;
@property(nonatomic, retain) NSMutableArray *itemCollection;

@property(nonatomic, retain) BDActionViewController *actionController;

@property(nonatomic, retain) UIView *cover_view;
@property(nonatomic, retain) IBOutlet UIWebView *webView;
@property(nonatomic, retain) IBOutlet UIActivityIndicatorView *waitView;
@property(nonatomic, retain) NSString *jsCode;
@property(nonatomic, retain) MPMoviePlayerController *player;


- (IBAction)goHome:(id)sender;
- (IBAction)goToPic:(id)sender;
- (IBAction)goToVideo:(id)sender;

- (IBAction)goBack:(id)sender;
- (IBAction)goForward:(id)sender;
- (IBAction)goJSTest:(id)sender;

- (void)wobble;
- (void)stopWobble;
- (void)stopCollecting;

- (void)startDownloadPic;
- (void)downloadPicReq;

@end
