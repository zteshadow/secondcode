//
//  MinutesToMidnightViewController.h
//  MinutesToMidnight
//
//  Created by samuel on 12-2-14.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MinutesToMidnightViewController : UIViewController {
    UILabel *countdownLabel;
}

@property(retain, nonatomic) IBOutlet UILabel *countdownLabel;
- (void)updateLabel;

@end

