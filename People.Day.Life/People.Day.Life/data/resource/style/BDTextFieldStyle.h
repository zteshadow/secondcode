//
//  BDTextFieldStyle.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDButtonStyle.h"

@class BDLabelStyle;
@class BDButtonStyle;

@interface BDTextFieldStyle : NSObject
{
    CGRect viewFrame_; //include textfield, label and button
    
    CGRect textFieldFrame_;
    CGFloat gap_; //标识button与上面的textview，下面的整个view的边缘的距离
    BDLabelStyle *labelStyle_;
    NSMutableArray *barButtonStyles_;
}

@property (nonatomic, readonly, assign) CGRect viewFrame;
@property (nonatomic, readonly, assign) CGRect textFieldFrame;
@property (nonatomic, readonly, assign) CGFloat gap;
@property (nonatomic, readonly, retain) BDLabelStyle *labelStyle;
@property (nonatomic, readonly, retain) NSMutableArray *barButtonStyles;

- (BDButtonStyle *)buttonStyleByType:(NSInteger)type;

@end
