//
//  BDResourceDecoderHardCode.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012Âπ?__MyCompanyName__. All rights reserved.
//

#import "BDResourceDecoderHardCode.h"
#import "BDResourceTheme+Private.h"
#import "BDResourceConst.h"
#import "BD_Debug.h"

NSString *kHardCodeResourceDecoderFilePort = @"LayoutPort";
NSString *kHardCodeResourceDecoderFileLand = @"LayoutLand";

@interface BDResourceDecoderHardCode ()
- (BDToolBarStyle *)decodeToolBarStylePort;
- (BDToolBarStyle *)decodeToolBarStyleLand;

//title bar in home
- (BDToolBarStyle *)decodeTitleBarStylePort;
- (BDToolBarStyle *)decodeTitleBarStyleLand;

- (BDTextFieldStyle *)decodeTextFieldStyleLand;
- (BDTextFieldStyle *)decodeTextFieldStylePort;

//title bar in edit view
- (BDTitleBarStyle *)decodeEditTitleBarStylePort;
- (BDTitleBarStyle *)decodeEditTitleBarStyleLand;

- (BDShortcutMenuStyle *)decodeShortcutMenuStylePort;
- (BDShortcutMenuStyle *)decodeShortcutMenuStyleLand;
@end

@implementation BDResourceDecoderHardCode

- (BOOL)decodeBeginWithFile:(NSString *)file
{
    BOOL valid = YES;
    if ([file isEqualToString:kHardCodeResourceDecoderFilePort]) 
    {
        landscape_ = NO;
    }
    else if ([file isEqualToString:kHardCodeResourceDecoderFileLand]) 
    {
        landscape_ = YES;
    }
    else
    {
        valid = NO;
    }
    
    return valid;
}

- (BOOL)decodeEndWithFile:(NSString *)file
{
    return YES;
}

- (BDToolBarStyle *)decodeToolBarStyle
{
    if (landscape_)
    {
        return [self decodeToolBarStyleLand];
    }
    else
    {
        return [self decodeToolBarStylePort];
    }
}

- (BDToolBarStyle *)decodeTitleBarStyle
{
    if (landscape_)
    {
        return [self decodeTitleBarStyleLand];
    }
    else
    {
        return [self decodeTitleBarStylePort];
    }
}

- (BDTitleBarStyle *)decodeEditTitleBarStyle
{
    if (landscape_)
    {
        return [self decodeEditTitleBarStyleLand];
    }
    else
    {
        return [self decodeEditTitleBarStylePort];
    }    
}

- (BDShortcutMenuStyle *)decodeShortcutMenuStyle
{
    if (landscape_)
    {
        return [self decodeShortcutMenuStyleLand];
    }
    else
    {
        return [self decodeShortcutMenuStylePort];
    }
}

- (BDTextFieldStyle *)decodeTextFieldStyle
{
    if (landscape_)
    {
        return [self decodeTextFieldStyleLand];
    }
    else
    {
        return [self decodeTextFieldStylePort];
    }    
}

#pragma mark - Private

- (BDToolBarStyle *)decodeToolBarStylePort
{    
    BDToolBarStyle *style = [[BDToolBarStyle alloc] init];
    
    NSMutableArray *buttonStyles = [NSMutableArray arrayWithCapacity:4];
    
    BDButtonStyle *diaryButton = [[BDButtonStyle alloc] init];
    diaryButton.buttonType  = kBarButtonTypeImagePickerAlbum;
    diaryButton.buttonFrame = CGRectMake(18, 5, 34, 34);
    diaryButton.imageNormal = @"resource/toolbar/toolbar_home.png";
    diaryButton.imageHighlighted = @"resource/toolbar/toolbar_home_sel.png";
    [buttonStyles addObject:diaryButton];
    [diaryButton release];
    
    BDButtonStyle *calendarButton = [[BDButtonStyle alloc] init];
    calendarButton.buttonType  = kBarButtonTypeImagePickerCamera;
    calendarButton.buttonFrame = CGRectMake(98, 5, 34, 34);
    calendarButton.imageNormal = @"resource/toolbar/toolbar_calendar.png";
    calendarButton.imageHighlighted = @"resource/toolbar/toolbar_calendar_sel.png";
    [buttonStyles addObject:calendarButton];
    [calendarButton release];
    
    BDButtonStyle *peopleButton = [[BDButtonStyle alloc] init];
    peopleButton.buttonType  = kBarButtonTypeImagePickerVideo;
    peopleButton.buttonFrame = CGRectMake(178, 5, 34, 34);
    peopleButton.imageNormal = @"resource/toolbar/toolbar_people.png";
    peopleButton.imageHighlighted = @"resource/toolbar/toolbar_people_sel.png";
    [buttonStyles addObject:peopleButton];
    [peopleButton release];
    
    BDButtonStyle *settingButton = [[BDButtonStyle alloc] init];
    settingButton.buttonType  = kBarButtonTypeImagePickerExit;
    settingButton.buttonFrame = CGRectMake(258, 5, 34, 34);
    settingButton.imageNormal = @"resource/toolbar/toolbar_setting.png";
    settingButton.imageHighlighted = @"resource/toolbar/toolbar_setting_sel.png";
    [buttonStyles addObject:settingButton];
    [settingButton release];
    
    style.barButtonStyles = buttonStyles;
    style.barFrame = CGRectMake(0, 416, 320, 49); //shit there is a 5 pixels shadow
    style.backgroundImage = @"resource/titlebar/titlebar_bg.png";

    return [style autorelease];
}

- (BDToolBarStyle *)decodeToolBarStyleLand
{
    NSAssert(0, @"decodeToolBarStyleLand");
    return nil;
}

- (BDToolBarStyle *)decodeTitleBarStylePort
{
    BDToolBarStyle *style = [[BDToolBarStyle alloc] init];

    NSMutableArray *buttonStyles = [NSMutableArray arrayWithCapacity:3];
    
    BDButtonStyle *settingButton = [[BDButtonStyle alloc] init];
    settingButton.buttonType  = kBarButtonTypeSetting;
    settingButton.buttonFrame = CGRectMake(6, 7, 42, 30);
    settingButton.imageNormal = @"resource/titlebar/titlebar_setting.png";
    settingButton.imageHighlighted = @"resource/titlebar/titlebar_setting.png";
    settingButton.imageBackNormal = @"resource/button/bar_button.png";
    settingButton.imageBackHighlighted = @"resource/button/bar_button_pressed.png";
    [buttonStyles addObject:settingButton];
    [settingButton release];
    
    BDButtonStyle *groupButton = [[BDButtonStyle alloc] init];
    groupButton.buttonType  = kBarButtonTypePeople;
    groupButton.buttonFrame = CGRectMake(272, 7, 42, 30);
    groupButton.imageNormal = @"resource/titlebar/titlebar_group.png";
    groupButton.imageHighlighted = @"resource/titlebar/titlebar_group.png";
    groupButton.imageBackNormal = @"resource/button/bar_button.png";
    groupButton.imageBackHighlighted = @"resource/button/bar_button_pressed.png";
    [buttonStyles addObject:groupButton];
    [groupButton release];
    
    style.barButtonStyles = buttonStyles;
    
    style.barFrame = CGRectMake(0, 0, 320, 44);
    style.backgroundImage = @"resource/titlebar/titlebar_bg.png";
    
    return [style autorelease];
}

- (BDToolBarStyle *)decodeTitleBarStyleLand
{
    NSAssert(0, @"decodeTitleBarStyleLand");
    return nil;    
}

- (BDTitleBarStyle *)decodeEditTitleBarStylePort
{
    BDTitleBarStyle *style = [[BDTitleBarStyle alloc] init];
    
    NSMutableArray *buttonStyles = [NSMutableArray arrayWithCapacity:3];
    
    BDButtonStyle *backButton = [[BDButtonStyle alloc] init];
    backButton.buttonType  = kBarButtonTypeBackHome;
    backButton.buttonFrame = CGRectMake(18, 5, 34, 34);
    backButton.imageNormal = @"resource/titlebar/titlebar_back_home.png";
    backButton.imageHighlighted = @"resource/titlebar/titlebar_back_home_sel.png";
    [buttonStyles addObject:backButton];
    [backButton release];
    
    //label
    BDLabelStyle *label = [[BDLabelStyle alloc] init];
    label.viewFrame = CGRectMake(100.0, 4.0, 120.0, 28.0);
    BDFontStyle *labelFont = [[BDFontStyle alloc] init];
    labelFont.fontSize = 12;
    labelFont.fontType = BDFontTypeBold;
    label.titleFont = labelFont;
    [labelFont release];
    label.titleColor = 0xFFFFFFFF;
    style.titleLabelStyle = label;
    [label release];
    
    BDButtonStyle *saveButton = [[BDButtonStyle alloc] init];
    saveButton.buttonType  = kBarButtonTypeSave;
    saveButton.buttonFrame = CGRectMake(248, 5, 34, 34);
    saveButton.imageNormal = @"resource/titlebar/titlebar_save.png";
    saveButton.imageHighlighted = @"resource/titlebar/titlebar_save_sel.png";
    [buttonStyles addObject:saveButton];
    [saveButton release];
    
    style.barButtonStyles = buttonStyles;
    style.barFrame = CGRectMake(0, 0, 320, 44);
    style.backgroundImage = @"resource/titlebar/titlebar_bg.png";
    
    return [style autorelease];
}

- (BDTitleBarStyle *)decodeEditTitleBarStyleLand
{
    BD_ASSERT(0);
    return nil;
}

- (BDShortcutMenuStyle *)decodeShortcutMenuStylePort
{
    BDShortcutMenuStyle *style = [[BDShortcutMenuStyle alloc] init];

    //main button
    BDButtonStyle *mainButton = [[BDButtonStyle alloc] init];
    mainButton.buttonType  = kShortcutMenuButtonMain;
    mainButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    mainButton.imageNormal = @"resource/shortcut/menu.png";
    mainButton.imageHighlighted = @"resource/shortcut/menu_sel.png";
    style.mainButtonStyle = mainButton;
    [mainButton release];
    
    //other button
    NSMutableArray *buttonStyles = [NSMutableArray arrayWithCapacity:5];
    
    BDButtonStyle *cameraButton = [[BDButtonStyle alloc] init];
    cameraButton.buttonType  = kShortcutMenuButtonCamera;
    cameraButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    cameraButton.imageNormal = @"resource/shortcut/menu_camera.png";
    cameraButton.imageHighlighted = @"resource/shortcut/menu_camera_sel.png";
    [buttonStyles addObject:cameraButton];
    [cameraButton release];
    
    BDButtonStyle *noteButton = [[BDButtonStyle alloc] init];
    noteButton.buttonType  = kShortcutMenuButtonNote;
    noteButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    noteButton.imageNormal = @"resource/shortcut/menu_note.png";
    noteButton.imageHighlighted = @"resource/shortcut/menu_note_sel.png";
    [buttonStyles addObject:noteButton];
    [noteButton release];
    
    BDButtonStyle *peopleButton = [[BDButtonStyle alloc] init];
    peopleButton.buttonType  = kShortcutMenuButtonPeople;
    peopleButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    peopleButton.imageNormal = @"resource/shortcut/menu_people.png";
    peopleButton.imageHighlighted = @"resource/shortcut/menu_people_sel.png";
    [buttonStyles addObject:peopleButton];
    [peopleButton release];

    BDButtonStyle *calendarButton = [[BDButtonStyle alloc] init];
    calendarButton.buttonType  = kShortcutMenuButtonCalendar;
    calendarButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    calendarButton.imageNormal = @"resource/shortcut/menu_calendar.png";
    calendarButton.imageHighlighted = @"resource/shortcut/menu_calendar_sel.png";
    [buttonStyles addObject:calendarButton];
    [calendarButton release];

    BDButtonStyle *settingButton = [[BDButtonStyle alloc] init];
    settingButton.buttonType  = kShortcutMenuButtonSetting;
    settingButton.buttonFrame = CGRectMake(0, 0, 44, 44);
    settingButton.imageNormal = @"resource/shortcut/menu_setting.png";
    settingButton.imageHighlighted = @"resource/shortcut/menu_setting_highlight.png";
    [buttonStyles addObject:settingButton];
    [settingButton release];

    style.menuButtonStyles = buttonStyles;
    
    style.top = 42.0;
    style.bottom = 440.0;
    style.right = 298.0;
    style.radius = 120.0;
    style.changeSideOffset = 60;
    style.viewFrameClose = CGRectMake(272, 360, 44, 44);
    style.viewFrameOpen = CGRectMake(0.0, 0.0, 320.0, 460.0);
    
    return [style autorelease];
}

- (BDShortcutMenuStyle *)decodeShortcutMenuStyleLand
{
    BD_ASSERT(0);
    return nil;
}

- (BDTextFieldStyle *)decodeTextFieldStyleLand
{
    BD_ASSERT(0);
    return nil;
}

- (BDTextFieldStyle *)decodeTextFieldStylePort
{
    BDTextFieldStyle *style = [[BDTextFieldStyle alloc] init];
    
    style.viewFrame = CGRectMake(0.0, 44.0, 320.0, 200.0);
    style.textFieldFrame = CGRectMake(42.0, 0.0, 278.0, 165.0);
    style.gap = 8.0;

    //label
    BDLabelStyle *label = [[BDLabelStyle alloc] init];
    label.viewFrame = CGRectMake(10.0, 68.0, 24.0, 24.0);
    BDFontStyle *labelFont = [[BDFontStyle alloc] init];
    labelFont.fontSize = 14;
    label.titleFont = labelFont;
    [labelFont release];
    
    label.titleColor = 0xFF7F7F7F;
    label.colorDisabled = 0xFF7F7F7F;
    style.labelStyle = label;
    [label release];
    
    NSMutableArray *buttonStyles = [NSMutableArray arrayWithCapacity:3];
    
    BDButtonStyle *cameraButton = [[BDButtonStyle alloc] init];
    cameraButton.buttonType  = kBarButtonTypeAddImage;
    cameraButton.buttonFrame = CGRectMake(6, 6, 34, 34);
    cameraButton.imageNormal = @"resource/titlebar/titlebar_camera.png";
    cameraButton.imageHighlighted = @"resource/titlebar/titlebar_camera_sel.png";
    [buttonStyles addObject:cameraButton];
    [cameraButton release];

    BDFontStyle *buttonFont = [[BDFontStyle alloc] init];
    buttonFont.fontSize = 14;

    BDButtonStyle *addLocation = [[BDButtonStyle alloc] init];
    addLocation.buttonType  = kBarButtonTypeAddLocation;
    addLocation.alignType = BDButtonAlignBottom;
    addLocation.buttonFrame = CGRectMake(10, 8, 145, 22);
    addLocation.imageBackNormal = @"resource/edit/add_people.png";
    addLocation.imageBackHighlighted = @"resource/edit/add_people_sel.png";
    addLocation.titleFont = buttonFont;
    addLocation.titleColor = 0xFFFFFFFF;
    addLocation.titleString = @"我在...";
    [buttonStyles addObject:addLocation];
    [addLocation release];
    
    BDButtonStyle *addPeopleButton = [[BDButtonStyle alloc] init];
    addPeopleButton.buttonType  = kBarButtonTypeAddPeople;
    addPeopleButton.alignType = BDButtonAlignBottom;
    addPeopleButton.buttonFrame = CGRectMake(165, 8, 145, 22); //y标识与上面的textview，下面的整个view的边缘的距离
    addPeopleButton.imageBackNormal = @"resource/edit/add_people.png";
    addPeopleButton.imageBackHighlighted = @"resource/edit/add_people_sel.png";
    addPeopleButton.titleFont = buttonFont;
    addPeopleButton.titleColor = 0xFFFFFFFF;
    addPeopleButton.titleString = @"我和...";
    [buttonStyles addObject:addPeopleButton];
    [addPeopleButton release];
    
    [buttonFont release];
    
    style.barButtonStyles = buttonStyles;

    return [style autorelease];;
}

@end
