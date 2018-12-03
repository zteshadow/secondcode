//
//  BDToolBarStyle.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDButtonStyle.h"

@interface BDToolBarStyle : NSObject
{
    CGRect              barFrame_;
    NSString           *backgroundImage_;
    
    NSMutableArray     *barButtonStyles_;
}

@property (nonatomic, readonly, assign) CGRect              barFrame;
@property (nonatomic, readonly, retain) NSString           *backgroundImage;
@property (nonatomic, readonly, retain) NSMutableArray     *barButtonStyles;

@end
