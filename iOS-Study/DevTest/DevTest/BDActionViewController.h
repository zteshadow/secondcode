//
//  BDActionViewController.h
//  DevTest
//
//  Created by majie on 12-5-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BDActionViewController : UIViewController <UITableViewDelegate, UITableViewDataSource>
{
    UIPopoverController *popView;
    NSArray *actionArray;
}


@property(nonatomic, retain) NSArray *actionArray;
@property(nonatomic, retain) UIPopoverController *popView;

@end
