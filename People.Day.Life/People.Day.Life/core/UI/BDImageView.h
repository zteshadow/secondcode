//
//  BDImageView.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-2-27.
//  Copyright (c) 2012年 yunyi. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BDImageView : UIView
{
    BOOL     highlighted_;      //高亮状态
    UIImage *imageNormal_;      //普通图片
    UIImage *imageHighlighted_; //高亮图片
}

@property (nonatomic, assign) BOOL      highlighted;
@property (nonatomic, retain) UIImage   *image;    //普通状态图片

- (void)setImage:(UIImage *)image forState:(UIControlState)state;

@end
