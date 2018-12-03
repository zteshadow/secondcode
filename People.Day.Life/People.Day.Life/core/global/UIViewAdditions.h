//
//  UIViewAdditions.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (BDAdditions)

- (UIImage *)snapshot;

- (UIViewController*)viewController;

- (void)removeAllSubviews;

//add this view to superview before animate
- (void)animatefromPoint:(CGPoint)fromPoint toPoint:(CGPoint)toPoint withDelegate:(id)delegate;

+ (UIView *)keyView;

- (BOOL) isChildOfClass:(Class)aClass;

@end
