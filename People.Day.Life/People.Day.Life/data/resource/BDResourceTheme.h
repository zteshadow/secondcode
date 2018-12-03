//
//  BDResourceTheme.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BDToolBarStyle;
@class BDTextFieldStyle;
@class BDShortcutMenuStyle;
@class BDTitleBarStyle;

@interface BDResourceTheme : NSObject
{
    //home
    BDToolBarStyle  *toolBarStyle_;
    BDToolBarStyle  *titleBarStyle_;
    
    //edit
    BDTextFieldStyle *textFieldStyle_;
    BDTitleBarStyle  *editTitleBarStyle_;
    
    //shortcut menu
    BDShortcutMenuStyle *shortcutMenuStyle_;
}

@property (nonatomic, readonly, retain) BDToolBarStyle *toolBarStyle;
@property (nonatomic, readonly, retain) BDToolBarStyle *titleBarStyle;
@property (nonatomic, readonly, retain) BDTextFieldStyle *textFieldStyle;
@property (nonatomic, readonly, retain) BDTitleBarStyle *editTitleBarStyle;
@property (nonatomic, readonly, retain) BDShortcutMenuStyle *shortcutMenuStyle;

@end
