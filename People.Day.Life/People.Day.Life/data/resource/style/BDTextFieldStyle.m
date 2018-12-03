//
//  BDTextFieldStyle.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDTextFieldStyle.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceManager.h"

@implementation BDTextFieldStyle

@synthesize viewFrame = viewFrame_;
@synthesize textFieldFrame = textFieldFrame_;
@synthesize labelStyle = labelStyle_;
@synthesize barButtonStyles = barButtonStyles_;
@synthesize gap = gap_;

- (void)dealloc
{
    [labelStyle_ release];
    [barButtonStyles_ release];

    [super dealloc];
}

- (BDButtonStyle *)buttonStyleByType:(NSInteger)type
{
    BDButtonStyle *style = nil;
    
    for (BDButtonStyle *item in self.barButtonStyles)
    {
        if (item.buttonType == type)
        {
            style = item;
            break;
        }
    }
    
    return style;
}
@end
