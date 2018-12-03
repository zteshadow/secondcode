//
//  BDGestureHandle.h
//  DevTest
//
//  Created by songliantao@baidu.com on 12-5-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDGestureHandle.h"


@interface BDGestureHandle (Private)
- (void)handleLongPress:(UILongPressGestureRecognizer *)recognizer;

- (void)showTextAction;
- (void)makeTextNotes:(id)sender;

- (void)showLinkAction:(CGPoint)pos;

@end
