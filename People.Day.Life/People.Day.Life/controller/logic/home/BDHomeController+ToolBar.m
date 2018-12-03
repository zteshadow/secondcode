//
//  BDHomeController+ToolBar.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012Âπ?__MyCompanyName__. All rights reserved.
//

#import "BDHomeController+ToolBar.h"
#import "BDHomeController+Private.h"
#import "BDToolBarController.h"
#import "BDToolBar.h"
#import "BDToolBarButton.h"
#import "BDResourceConst.h"

@implementation BDHomeController (ToolBar)

#pragma mark - BDToolBarDelegate
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button
{
    BDBarButtonType type = button.type;
    
    switch (type)
    {
        case kBarButtonTypeTakePicture:
        break;
        
        case kBarButtonTypeWriteNote:
        break;
            
        case kBarButtonTypeHome:
        {
        }
        break;
            
        case kBarButtonTypeCalendar:
        {
        }
        break;
            
        case kBarButtonTypePeople:
        {
        }
        break;
            
        case kBarButtonTypeSetting:
        {
        }
        break;

        default:
        break;
    }    
}

@end
