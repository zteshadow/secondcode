//
//  UIImageAdditions.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (BDAdditions)

- (void)drawCenterInRect:(CGRect)rect;

//sacle = destSize / imageSize
- (UIImage *)imageToScale:(CGFloat)scaleSize;

//scale then capture
- (UIImage *)imageToFitSize:(CGSize)fitSize;

- (UIImage *)imageInRect:(CGRect)rect;

//从图片中裁剪出一个圆形的区域，不进行缩放
- (UIImage *)imageInCircle:(CGFloat)radius;

@end
