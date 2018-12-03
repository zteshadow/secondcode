//
//  BDToolBarController.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BDViewControllerDelegate.h"

@interface BDBaseController : UIViewController<BDViewControllerDelegate>

- (void)dismiss;
- (void)showInView:(UIView *)view;

@end
