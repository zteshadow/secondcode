//
//  BDToolBarDelegate.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BDToolBar;
@class BDToolBarButton;

@protocol BDToolBarDelegate <NSObject>
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button;
@end
