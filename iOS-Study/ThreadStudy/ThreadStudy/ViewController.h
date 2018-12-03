//
//  ViewController.h
//  ThreadStudy
//
//  Created by majie on 12-3-21.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    NSThread *thread;
}

@property (nonatomic, retain) NSThread *thread;


- (void)process:(id)param;

- (void)showThreadInfo;

- (IBAction)selectStart:(id)sender;
- (IBAction)selectStop:(id)sender;
- (IBAction)selectTest:(id)sender;

@end
