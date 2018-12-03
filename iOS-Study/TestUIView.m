//
//  TestUIView.m
//  UIKitStudy
//
//  Created by samuel on 13-2-26.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "TestUIView.h"

@implementation TestUIView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)drawRectTest1
{
    // Drawing code
    UIColor *color = [UIColor colorWithRed:0.5f green:0.0f blue:0.5f alpha:1.0f];
    [color set];
    
    CGColorRef colorRef = [color CGColor];
    const CGFloat *components = CGColorGetComponents(colorRef);
    NSUInteger count = CGColorGetNumberOfComponents(colorRef);
    
    //rgba，是默认的格式吗？
    for (NSUInteger i = 0; i < count; i++)
    {
        NSLog(@"color[%d]: %f", i, components[i]);
    }
    
    UIFont *font = [UIFont fontWithName:@"HelveticaNeue-Bold" size:30.0];
    
    NSString *hello = @"Hello, world!";
    
    CGRect stringRect = CGRectMake(10.0, 180.0, 320, 400);
    //[hello drawAtPoint:CGPointMake(40.0, 180.0) withFont:font];
    [hello drawInRect:stringRect withFont:font];
    
    UIImage *image = [UIImage imageNamed:@"icon.png"];
    if (image != nil)
    {
        NSLog(@"image is ready");
    }
    
    CGSize imageSize = [image size];
    
    [image drawAtPoint:CGPointMake(10.0, 20.0)];
    [image drawAtPoint:CGPointMake(40 + imageSize.width, 20.0) blendMode:kCGBlendModeNormal alpha:0.5];
    [image drawInRect:CGRectMake(10.0, 30.0 + imageSize.height, imageSize.width, imageSize.height)];
    [image drawAsPatternInRect:CGRectMake(20 + imageSize.width, 30 + imageSize.height, imageSize.width, imageSize.height)];
    
    [[UIColor brownColor] set];
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSetShadowWithColor(context, CGSizeMake(10.0, 10.0), 2.0, [[UIColor grayColor] CGColor]);
    
    CGContextSetLineWidth(context, 5.0);
    CGContextSetLineJoin(context, kCGLineJoinRound);
    CGContextMoveToPoint(context, 50.0, 200.0);
    CGContextAddLineToPoint(context, 100.0, 400.0);
    CGContextAddLineToPoint(context, 300.0, 100.0);
    CGContextStrokePath(context);
    
    //path
    CGMutablePathRef path = CGPathCreateMutable();
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    CGPathMoveToPoint(path, NULL, screenBounds.origin.x, screenBounds.origin.y);
    CGPathAddLineToPoint(path, NULL, screenBounds.origin.x + screenBounds.size.width, screenBounds.origin.y + screenBounds.size.height);
    CGPathMoveToPoint(path, NULL, screenBounds.origin.x + screenBounds.size.width, screenBounds.origin.y);
    CGPathAddLineToPoint(path, NULL, screenBounds.origin.x, screenBounds.origin.y + screenBounds.size.height);
    
    CGRect r = CGRectMake(10.0, 10.0, 200.0, 300.0);
    CGPathAddRect(path, NULL, r);
    
    CGContextAddPath(context, path);
    [[UIColor blueColor] setStroke];
    CGContextDrawPath(context, kCGPathFill);
    CGPathRelease(path);
    
    
}

- (void)gradientTest
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    UIColor *startColor = [UIColor blueColor];
    CGFloat *startComponents = (CGFloat *)CGColorGetComponents([startColor CGColor]);
    
    UIColor *endColor = [UIColor greenColor];
    CGFloat *endCompoents = (CGFloat *)CGColorGetComponents([endColor CGColor]);
    
    CGFloat colorComponents[8] =
    {
      startComponents[0], startComponents[1], startComponents[2], startComponents[3],
      endCompoents[0], endCompoents[1], endCompoents[2], endCompoents[3]
    };
    
    CGFloat colorIndices[2] = {0.0, 1.0};
    
    CGGradientRef gradient = CGGradientCreateWithColorComponents(colorSpace, (const CGFloat *)colorComponents, (const CGFloat *)colorIndices, 2);
    
    CGPoint start = CGPointMake(50.0, 200.0);
    CGPoint end = CGPointMake(200.0, 200.0);
    
    CGContextDrawLinearGradient(context, gradient, start, end, 0);
    //CGContextDrawLinearGradient(context, gradient, start, end, kCGGradientDrawsBeforeStartLocation);
    //CGContextDrawLinearGradient(context, gradient, start, end, kCGGradientDrawsAfterEndLocation);
    //CGContextDrawLinearGradient(context, gradient, start, end, kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);
    
    
    CGGradientRelease(gradient);
    
    CGColorSpaceRelease(colorSpace);
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    //[self drawRectTest1];
    [self gradientTest];
}

@end
