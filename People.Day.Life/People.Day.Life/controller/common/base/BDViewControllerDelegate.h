//
//  BDViewControllerDelegate.h
//  People.Day.Life
//
//  Created by samuel on 13-2-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol BDViewControllerDelegate <NSObject>

@required
- (void)dismiss;
- (void)showInView:(UIView *)view;

@end
