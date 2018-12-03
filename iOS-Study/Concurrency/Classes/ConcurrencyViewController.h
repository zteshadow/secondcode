//
//  ConcurrencyViewController.h
//  Concurrency
//
//  Created by samuel on 12-2-22.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ConcurrencyViewController : UIViewController {
    UILabel *message;
    UIActivityIndicatorView *spinner;
}

@property (retain, nonatomic) IBOutlet UILabel *message;
@property (retain, nonatomic) IBOutlet UIActivityIndicatorView *spinner;

- (IBAction)getData:(id)sender;
- (void)showData;

- (void)doDataProcess:(id)data;
- (void)dataProcessComplete;

@end

