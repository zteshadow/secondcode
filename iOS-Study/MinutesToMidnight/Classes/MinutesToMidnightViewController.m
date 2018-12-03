//
//  MinutesToMidnightViewController.m
//  MinutesToMidnight
//
//  Created by samuel on 12-2-14.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "MinutesToMidnightViewController.h"

@implementation MinutesToMidnightViewController

@synthesize countdownLabel;


/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [countdownLabel setFont:[UIFont fontWithName:@"DBLCDTempBlack" size: 58.0]];
    countdownLabel.text = @"10";
    [super viewDidLoad];
}



/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)updateLabel
{
    NSDate *now = [NSDate date];
    NSCalendar *calendar = [NSCalendar currentCalendar];
    NSDateComponents *p = [calendar components:(NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit) fromDate:now];
    
    int hour = 23 - [p hour];
    int min = 59 - [p minute];
    int sec = 59 - [p second];
    
    NSString *text = [NSString stringWithFormat:@"%02d:%02d:%02d", hour, min, sec];
    
    countdownLabel.text = text;
    	
    
    //int hour = 23 - [[now dateWithCalendarFomat:nil timeZone:nil] hourOfDay];
}

- (void)dealloc {
    [super dealloc];
}

@end
