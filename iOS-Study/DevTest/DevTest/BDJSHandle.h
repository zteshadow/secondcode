//
//  BDJSHandle.h
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


@interface BDJSHandle : NSObject

+ (void)stopCallOut:(UIWebView *)webView;
+ (NSString *)pressOnLink:(UIWebView *)webView atPos:(CGPoint)pos;

@end
