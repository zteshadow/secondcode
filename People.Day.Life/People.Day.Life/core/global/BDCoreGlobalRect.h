//
//  BDCoreGlobalRect.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-6-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

/*
 Return CGRect with same size and zero origin
 */

extern CGRect CGRectNormalize(CGRect rect);

extern CGPoint CGRectCenterPoint(CGRect rect);

extern CGRect CGRectSizeScale(CGRect rect, CGFloat scale);

extern CGRect CGRectMove(CGRect rect, CGSize size);

extern CGRect CGRectInsets(CGRect rect, UIEdgeInsets inset);

extern CGRect CGRectGrow(CGRect rect, UIEdgeInsets grow);

extern CGRect CGRectScale(CGRect rect, CGFloat scale);