//
//  BDResourceDecoding.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceTheme.h"

@protocol BDResourceDecoding <NSObject>
- (BOOL)decodeBeginWithFile:(NSString *)file;
- (BOOL)decodeEndWithFile:(NSString *)file;
- (BDToolBarStyle *)decodeToolBarStyle;
- (BDToolBarStyle *)decodeTitleBarStyle;
- (BDTextFieldStyle *)decodeTextFieldStyle;
- (BDTitleBarStyle *)decodeEditTitleBarStyle;
- (BDShortcutMenuStyle *)decodeShortcutMenuStyle;
@end
