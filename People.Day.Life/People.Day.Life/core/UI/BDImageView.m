//
//  BDImageView.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-2-27.
//  Copyright (c) 2012年 yunyi. All rights reserved.
//

#import "BDImageView.h"
#import "UIImageAdditions.h"

@interface BDImageView ()
- (UIImage *)imageForCurrentState;
@end

@implementation BDImageView

@synthesize highlighted = highlighted_;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];
        self.userInteractionEnabled = NO;
    }
    return self;
}

- (void)dealloc
{
    [imageNormal_      release];
    [imageHighlighted_ release];
    [super dealloc];
}

- (void)drawRect:(CGRect)rect
{
    UIImage *image = [self imageForCurrentState];
    if (self.contentMode == UIViewContentModeCenter)
    {
        [image drawCenterInRect:rect];
    }
    else
    {
        [image drawInRect:rect];
    }
}

- (void)setHighlighted:(BOOL)highlighted
{
    if (highlighted != highlighted_)
    {
        highlighted_ = highlighted;
        [self setNeedsDisplay];
    }
}

- (void)setImage:(UIImage *)image forState:(UIControlState)state
{
    switch (state)
    {
        case UIControlStateNormal:
        {
            if (image != imageNormal_)
            {
                [imageNormal_ release];
                imageNormal_ = [image retain];
                
                if (NO == self.highlighted)
                {
                    [self setNeedsDisplay];
                }
            }
            break;
        }
            
        case UIControlStateHighlighted:
        {
            if (image != imageHighlighted_)
            {
                [imageHighlighted_ release];
                imageHighlighted_ = [image retain];
                
                if (self.highlighted)
                {
                    [self setNeedsDisplay];
                }
            }
            break;
        }
            
        default: break;
    }
}

- (void)setImage:(UIImage *)image
{
    [self setImage:image forState:UIControlStateNormal];
}

- (UIImage *)image
{
    return imageNormal_;
}

#pragma mark - Privarte

- (UIImage *)imageForCurrentState
{
    UIImage *image = nil;
    if (self.highlighted)
    {
        image = imageHighlighted_;
    }
    else
    {
        image = imageNormal_;
    }
    return image;
}

@end
