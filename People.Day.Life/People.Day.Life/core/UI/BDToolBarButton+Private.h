//
//  BDToolBarButton+Private.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-5-5.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBarButton.h"

@interface BDToolBarButton (Private)
- (id)initWithType:(NSInteger)type;
- (void)setKeyPath:(NSString *)keyPath;
@end

@interface BDToolBarDiaryButton : BDToolBarButton
@end

@interface BDToolBarCalendarButton : BDToolBarButton
@end

@interface BDToolBarPeopleButton : BDToolBarButton
@end

@interface BDToolBarSettingButton : BDToolBarButton
@end
