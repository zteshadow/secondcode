//
//  BDJSHandle.m
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDJSHandle.h"


static const NSString *jsGetURLAtPos = 
@"function getLinkAtPoint(x, y)\
{\
	var tags = '';\
	var e = document.elementFromPoint(x,y);\
	while (e)\
	{\
        if (e.href)\
        {\
            tags += e.href;\
            break;\
        }\
		e = e.parentNode;\
	}\
	return tags;\
}";

static const NSString *jsStopCallOut = 
@"document.documentElement.style.webkitTouchCallout = \"none\"";


@implementation BDJSHandle

+ (void)stopCallOut:(UIWebView *)webView
{
    [webView stringByEvaluatingJavaScriptFromString:jsStopCallOut];
}

+ (NSString *)pressOnLink:(UIWebView *)webView atPos:(CGPoint)pos
{
    [webView stringByEvaluatingJavaScriptFromString:jsGetURLAtPos];
    
    NSString *value = [webView stringByEvaluatingJavaScriptFromString:[NSString stringWithFormat:@"getLinkAtPoint(%i,%i);",(NSInteger)pos.x,(NSInteger)pos.y]];
    
    return value;
}

@end
