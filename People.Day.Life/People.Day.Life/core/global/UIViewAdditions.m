//
//  UIViewAdditions.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/CALayer.h>
#import <QuartzCore/CAAnimation.h>
#import "UIImageAdditions.h"
#import "UIViewAdditions.h"

@implementation UIView (BDUtils)

- (UIViewController*)viewController 
{
    for (UIView* next = self; next; next = next.superview) 
    {
        UIResponder* nextResponder = [next nextResponder];
        if ([nextResponder isKindOfClass:[UIViewController class]]) 
        {
            return (UIViewController *)nextResponder;
        }
    }
    return nil;
}

- (void)removeAllSubviews {
    while (self.subviews.count)
    {
        UIView* view = self.subviews.lastObject;
        [view removeFromSuperview];
    }
}

- (UIImage *)snapshot
{
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, YES, [UIScreen mainScreen].scale);
    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (void)animatefromPoint:(CGPoint)fromPoint toPoint:(CGPoint)toPoint withDelegate:(id)delegate
{
    //path
    UIBezierPath *movePath = [UIBezierPath bezierPath];
    [movePath moveToPoint:fromPoint];
    //
    // original path
//    [movePath addQuadCurveToPoint:toPoint controlPoint:CGPointMake(fromPoint.x + (toPoint.x - fromPoint.x) / 2, fromPoint.y)];
    // new path by Jubal -- we want to simulate parabola
    [movePath addQuadCurveToPoint:toPoint
                     controlPoint:CGPointMake(fromPoint.x + (toPoint.x - fromPoint.y) / 10,
                                              fromPoint.y + (toPoint.y - fromPoint.y) / 2)];
    
    CAKeyframeAnimation *moveAnim = [CAKeyframeAnimation animationWithKeyPath:@"position"];
    moveAnim.path = movePath.CGPath;
    moveAnim.removedOnCompletion = YES;
    
    CABasicAnimation *scaleAnim = [CABasicAnimation animationWithKeyPath:@"transform"];
    scaleAnim.fromValue = [NSValue valueWithCATransform3D:CATransform3DIdentity];
    scaleAnim.toValue = [NSValue valueWithCATransform3D:CATransform3DMakeScale(0.1, 0.1, 1.0)];
    scaleAnim.removedOnCompletion = YES;
    
    CABasicAnimation *opacityAnim = [CABasicAnimation animationWithKeyPath:@"alpha"];
    opacityAnim.fromValue = [NSNumber numberWithFloat:1.0];
    opacityAnim.toValue = [NSNumber numberWithFloat:0.1];
    opacityAnim.removedOnCompletion = YES;
    
    CAAnimationGroup *animGroup = [CAAnimationGroup animation];
    animGroup.animations = [NSArray arrayWithObjects:moveAnim, scaleAnim, opacityAnim, nil];
    animGroup.duration = 0.5;
    animGroup.delegate = delegate;
    [animGroup setValue:self forKey:@"tag"];
    
    [self.layer addAnimation:animGroup forKey:@"baiduCollect"];
    
    //restore
    self.center = toPoint;
    self.transform = CGAffineTransformMakeScale(0.1, 0.1);
    self.alpha = 1.0;
    
    return;
}

+ (UIView *)keyView
{
    UIWindow *window = [[UIApplication sharedApplication] keyWindow];
    if (window.subviews.count > 0) 
    {
        return [window.subviews objectAtIndex:0];
    }
    else
    {
        return window;
    }
}

- (BOOL) isChildOfClass:(Class)aClass
{
    UIView* view = self;
    BOOL ret = NO;
    while (view)
    {
        if ([view isKindOfClass:aClass])
        {
            ret = YES;
            break;
        }
        else
        {
            view = view.superview;
        }
    }
    return ret;
}
@end

