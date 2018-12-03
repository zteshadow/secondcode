//
//  BDToolBarButton.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBarButton+Private.h"
#import <UIKit/UIKit.h>
#import "BDResourceConst.h"
#import "BDButtonStyle.h"
#import "UIImageAdditions.h"

@interface BDToolBarButton()
+ (BDToolBarButton *)barButtonWithType:(NSInteger)type;
@end

@implementation BDToolBarButton

@synthesize type = type_;

- (void)setupStyle:(BDButtonStyle *)style
{
    [self setFrame:style.buttonFrame];
    
    // set images
    if (style.imageNormal)
    {
        [self setImage:[UIImage imageNamed:style.imageNormal]
                forState:UIControlStateNormal];
    }
    
    if (style.imageDisabled)
    {
        [self setImage:[UIImage imageNamed:style.imageDisabled]
                forState:UIControlStateDisabled];
    }
    
    if (style.imageHighlighted)
    {
        [self setImage:[UIImage imageNamed:style.imageHighlighted]
                forState:UIControlStateHighlighted];
    }
    
    //背景拉伸
    UIImage *backImage;
    
    if (style.imageBackNormal)
    {
        backImage = [UIImage imageNamed:style.imageBackNormal];
        backImage = [backImage stretchableImageWithLeftCapWidth:backImage.size.width / 2.0 topCapHeight:backImage.size.height / 2.0];
        [self setBackgroundImage: backImage forState:UIControlStateNormal];
    }
    
    if (style.imageBackHighlighted)
    {
        backImage = [UIImage imageNamed:style.imageBackHighlighted];
        backImage = [backImage stretchableImageWithLeftCapWidth:backImage.size.width / 2.0 topCapHeight:backImage.size.height / 2.0];
        [self setBackgroundImage:backImage forState:UIControlStateHighlighted];
    }

}

+ (BDToolBarButton *)barButtonWithType:(NSInteger)type
{
    BDToolBarButton *button = nil;
    
    switch (type)
    {
        case kBarButtonTypeHome:
        {
            button = [[[BDToolBarDiaryButton alloc] initWithType:type] autorelease];
        }
        break;
            
        case kBarButtonTypeCalendar:
        {
            button = [[[BDToolBarCalendarButton alloc] initWithType:type] autorelease];
        }
        break;
            
        case kBarButtonTypePeople:
        {
            button = [[[BDToolBarPeopleButton alloc] initWithType:type] autorelease];
        }
        break;
            
        case kBarButtonTypeSetting:
        {
            button = [[[BDToolBarSettingButton alloc] initWithType:type] autorelease];
        }
        break;

        default: 
        {
            button = [[[BDToolBarButton alloc] initWithType:type] autorelease];
        }
        break;
    }
    
    return button;
}

+ (BDToolBarButton *)barButtonWithStyle:(BDButtonStyle *)style
{
    BDToolBarButton *button = [BDToolBarButton barButtonWithType:style.buttonType];
    [button setupStyle:style];
    return button;
}

@end
