//
//  BDResourceConst.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum
{
    kBarButtonTypeNone,         //0
    
    //home
    kBarButtonTypeHome,
    kBarButtonTypeCalendar,
    kBarButtonTypePeople,       //3
    kBarButtonTypeSetting,
    
    //title
    kBarButtonTypeTakePicture,
    kBarButtonTypeTitle,
    kBarButtonTypeWriteNote,
    
    //edit title
    kBarButtonTypeBackHome,
    kBarButtonTypeSave,
    kBarButtonTypeAddPeople,
    kBarButtonTypeAddLocation,
    kBarButtonTypeAddImage,
    
    //shortcut menu
    kShortcutMenuButtonMain,
    kShortcutMenuButtonCamera,
    kShortcutMenuButtonNote,
    kShortcutMenuButtonPeople,
    kShortcutMenuButtonCalendar,
    kShortcutMenuButtonSetting,
    
    //image picker
    kBarButtonTypeImagePickerExit,
    kBarButtonTypeImagePickerCamera,
    kBarButtonTypeImagePickerVideo,
    kBarButtonTypeImagePickerAlbum,
    
    kBarButtonTypeMax

} BDBarButtonType;

extern NSString *kHelloMessageHome;


