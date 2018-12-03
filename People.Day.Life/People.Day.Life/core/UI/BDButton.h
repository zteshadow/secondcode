//
//  BDButton.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BDButton : UIControl
{
    UIImage *imageNormal_;
    UIImage *imageHighlighted_;
    UIImage *imageDisabled_;
    UIImage *imageBackground_;
    UIImage *imageBackgroundHighlighted_;
}

- (void)setBackgroundImage:(UIImage *)image forState:(UIControlState)state;

- (void)setImage:(UIImage *)image forState:(UIControlState)state;

- (CGRect)imageRectForBounds:(CGRect)bounds;

- (CGRect)backgroundRectForBounds:(CGRect)bounds;

@end
