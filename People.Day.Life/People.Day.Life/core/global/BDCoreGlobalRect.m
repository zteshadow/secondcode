//
//  BDCoreGlobalRect.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-6-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDCoreGlobalRect.h"

CGRect CGRectNormalize(CGRect rect)
{
    CGRect frame = rect;
    frame.origin = CGPointZero;
    return frame;
}

CGPoint CGRectCenterPoint(CGRect rect)
{
    return CGPointMake(CGRectGetMidX(rect), CGRectGetMidY(rect));
}

CGRect CGRectSizeScale(CGRect rect, CGFloat scale)
{
    rect.size.width  *= scale;
    rect.size.height *= scale;
    return rect;
}

CGRect CGRectMove(CGRect rect, CGSize size)
{
    return CGRectMake(rect.origin.x+size.width, rect.origin.y+size.height, rect.size.width, rect.size.height);
}

CGRect CGRectInsets(CGRect rect, UIEdgeInsets inset)
{
    return UIEdgeInsetsInsetRect(rect, inset);
//    rect.origin.x += inset.left;
//    rect.origin.y += inset.top;
//    rect.size.width -= (inset.left + inset.right);
//    rect.size.height -= (inset.top + inset.bottom);
//    return rect;
}

CGRect CGRectGrow(CGRect rect, UIEdgeInsets grow)
{
    rect.origin.x += grow.left;
    rect.origin.y += grow.top;
    rect.size.width += grow.right - grow.left;
    rect.size.height += grow.bottom - grow.top;
    return  rect;
}

CGRect CGRectScale(CGRect rect, CGFloat scale)
{
    rect.origin.x *= scale;
    rect.origin.y *= scale;
    rect.size.width *= scale;
    rect.size.height *= scale;
    return rect;
}
