//
//  BibleViewController.m
//  Bible
//
//  Created by samuel on 12-1-18.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "BibleViewController.h"
#import "BibleView.h"


typedef enum
{
    bible_control_fresh = 0,
    bible_control_backward,
    bible_control_home,
    bible_control_forward,
    bible_control_stop,
    
    bible_control_max
    
} bible_control_type;


@implementation BibleViewController



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


- (IBAction)toolBarProcess:(id)sender
{
    UISegmentedControl *control = sender;
    
    switch([control selectedSegmentIndex])
    {
        case bible_control_home:
        {
            BibleView *view = (BibleView *)(self.view);
            [view test];
        }
        break;
            
        default:
        break;
    }
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
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


- (void)dealloc {
    [super dealloc];
}

@end
