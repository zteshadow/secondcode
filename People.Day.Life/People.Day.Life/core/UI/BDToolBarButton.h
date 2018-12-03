//
//  BDToolBarButton.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDButton.h"

@class BDButtonStyle;

@interface BDToolBarButton : BDButton
{
    NSInteger type_;
    NSString *keyPath_;
}

@property (nonatomic, readonly, assign) NSInteger type;

+ (BDToolBarButton *)barButtonWithStyle:(BDButtonStyle *)style;
- (void)setupStyle:(BDButtonStyle *)style;

@end
