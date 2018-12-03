//
//  BDButton.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDButton.h"
#import "UIImageAdditions.h"

@interface BDButton ()
@property (nonatomic, retain) UIImage *imageBackground;
@property (nonatomic, retain) UIImage *imageBackgroundHighlighted;
- (UIImage *)imageForCurrentState;
- (UIImage *)backImageForCurrentState;
@end

@implementation BDButton

@synthesize imageBackground = imageBackground_;
@synthesize imageBackgroundHighlighted = imageBackgroundHighlighted_;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];
        self.contentVerticalAlignment   = UIControlContentVerticalAlignmentCenter;
        self.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
    }
    return self;
}

- (void)dealloc
{
    [imageNormal_      release];
    [imageDisabled_    release];
    [imageBackground_  release];
    [imageHighlighted_ release];
    [imageBackgroundHighlighted_ release];
    
    [super dealloc];
}

- (void)drawRect:(CGRect)rect
{
    UIImage *backImage = [self backImageForCurrentState];
    if (backImage)
    {
        [backImage drawInRect:rect];
    }
    
    UIImage *image = [self imageForCurrentState];
    if (image)
    {
        CGRect imageRect = [self imageRectForBounds:rect];
        [image drawCenterInRect:imageRect];
    }
}

- (void)setEnabled:(BOOL)enabled
{
    if (self.enabled != enabled)
    {
        [super setEnabled:enabled];
        [self setNeedsDisplay];
    }
}

- (void)setHighlighted:(BOOL)highlighted
{
    if (self.highlighted != highlighted)
    {
        [super setHighlighted:highlighted];
        [self  setNeedsDisplay];
    }
}

- (void)setBackgroundImage:(UIImage *)image forState:(UIControlState)state
{
    switch (state)
    {
        case UIControlStateNormal:
        {
            self.imageBackground = image;
        }
        break;
            
        case UIControlStateHighlighted:
        {
            self.imageBackgroundHighlighted = image;
        }
        break;
            
        case UIControlStateDisabled:
        {
            if (image != imageDisabled_)
            {
                [imageDisabled_ release];
                imageDisabled_ = [image retain];
            }
        }
        break;
            
        default:
        break;
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
            }
            break;
        }
            
        case UIControlStateHighlighted:
        {
            if (image != imageHighlighted_)
            {
                [imageHighlighted_ release];
                imageHighlighted_ = [image retain];
            }
            break;
        }
            
        case UIControlStateDisabled:
        {
            if (image != imageDisabled_)
            {
                [imageDisabled_ release];
                imageDisabled_ = [image retain];
            }
            break;
        }
            
        default: break;
    }
}

#pragma mark - Layout

- (CGRect)rectWithImage:(UIImage *)image bounds:(CGRect)bounds
{
    CGSize size = image.size;
    CGRect rect = CGRectMake(0, 0, size.width, size.height);
    switch (self.contentVerticalAlignment) 
    {
        case UIControlContentVerticalAlignmentFill:
        {
            rect.size.height = bounds.size.height;
            break;
        }
            
        case UIControlContentVerticalAlignmentCenter:
        {
            rect.origin.y = roundf((bounds.size.height - rect.size.height) / 2);
            break;
        }
            
        case UIControlContentVerticalAlignmentBottom:
        {
            rect.origin.y = bounds.size.height - rect.size.height;
            break;
        }
        default: break;
    }
    
    switch (self.contentHorizontalAlignment)
    {
        case UIControlContentHorizontalAlignmentFill:
        {
            rect.size.width = bounds.size.width;
            break;
        }
            
        case UIControlContentHorizontalAlignmentCenter:
        {
            rect.origin.x = roundf((bounds.size.width - rect.size.width) / 2);
            break;
        }
            
        case UIControlContentHorizontalAlignmentRight:
        {
            rect.origin.x = bounds.size.width - rect.size.width;
            break;
        }
            
        default: break;
    }
    return rect;
}

- (CGRect)imageRectForBounds:(CGRect)bounds
{
    UIImage *image = [self imageForCurrentState];
    return image ? [self rectWithImage:image bounds:bounds] : bounds;
}

- (CGRect)backgroundRectForBounds:(CGRect)bounds
{
    return imageBackground_ ? [self rectWithImage:imageBackground_ bounds:bounds] : bounds;
}

#pragma mark - Privarte

- (UIImage *)imageForCurrentState
{
    UIImage *image = nil;
    if (self.enabled)
    {
        if (self.selected || self.highlighted)
        {
            image = imageHighlighted_;
        }
        else
        {
            image = imageNormal_;
        }
    }
    else
    {
        image = imageDisabled_;
    }
    return image;
}

- (UIImage *)backImageForCurrentState
{
    UIImage *image = nil;
    if (self.enabled)
    {
        if (self.selected || self.highlighted)
        {
            image = imageBackgroundHighlighted_;
        }
        else
        {
            image = imageBackground_;
        }
    }
    else
    {
        image = imageDisabled_;
    }
    return image;
}

@end
