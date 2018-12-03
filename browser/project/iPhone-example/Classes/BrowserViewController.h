//
//  BrowserViewController.h
//  Browser
//
//  Created by samuel on 11-11-29.
//  Copyright Baidu ,. Ltd 2011. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BrowserViewController : UIViewController 
//<UIWebViewDelegate>
{
    UITextField *input;
    UIWebView *webView;
}

@property (nonatomic, retain) IBOutlet UITextField *input;
@property (nonatomic, retain )IBOutlet UIWebView *webView;

- (IBAction)inputDone:(id) sender;
- (IBAction)selectFunction:(id)sender;

@end

