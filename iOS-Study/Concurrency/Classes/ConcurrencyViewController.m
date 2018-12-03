//
//  ConcurrencyViewController.m
//  Concurrency
//
//  Created by samuel on 12-2-22.
//  Copyright Baidu ,. Ltd 2012. All rights reserved.
//

#import "ConcurrencyViewController.h"
#import "ConcurrencyAppDelegate.h"

#include "cm_time.h"


@implementation ConcurrencyViewController

@synthesize message;
@synthesize spinner;

- (IBAction)getData:(id)sender
{
    //message.text = @"Got the Key:12sde4fxsdfefsfdsfdsfss";
    
    [spinner startAnimating];
    spinner.hidden = NO;
    
    NSInvocationOperation *op = [[NSInvocationOperation alloc] initWithTarget:self selector:@selector(doDataProcess:) object:nil];
    ConcurrencyAppDelegate *app = (ConcurrencyAppDelegate *)([UIApplication sharedApplication].delegate);
    
    NSOperationQueue *queue = app.ccQueue;
    
    [queue setMaxConcurrentOperationCount:2];
    int cnt = [queue maxConcurrentOperationCount];
    [queue addOperation:(NSOperation *)op];
    
    //[app.ccQueue addOperation:op];
    //[op release];
    //[self doDataProcess:nil];
}

- (void)doDataProcess:(id)data
{
    CM_TIME_VAL start, end;
    
    cm_gettimeofday(&start);
    
    //loop for 5 seconds
    while (1)
    {
        int s, i;
        for (i = 1; i < 1000; i++)
        {
            s = s % i;
        }
        
        cm_gettimeofday(&end);
        
        if (end.tv_sec - start.tv_sec > 5)
        {
            break;
        }
    }
    
    //[self showData];
    [self dataProcessComplete];
}

- (void)dataProcessComplete
{
    [self performSelectorOnMainThread:@selector(showData) withObject:nil waitUntilDone:NO];
}

- (void)showData
{
    [spinner stopAnimating];
    spinner.hidden = YES;
    
    message.text = @"I got the key: wer234sdfs8923sd!@3sdf";
}

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
    [super viewDidLoad];
    
    spinner.hidden = YES;
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
    
    [message release];
    [spinner release];
    
    [super dealloc];
}

@end
