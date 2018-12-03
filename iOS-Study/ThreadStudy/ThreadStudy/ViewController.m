//
//  ViewController.m
//  ThreadStudy
//
//  Created by majie on 12-3-21.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

@synthesize thread;

- (void)dealloc
{
    if (thread != nil)
    {
        [thread release];
    }
    
    [super dealloc];
}

- (void)process:(id)param
{
    int cnt = 0;
    
    ViewController *vc = (ViewController *)param;
    
    NSLog(@"I'm in child thread now");
    [vc showThreadInfo];
    
    NSThread *t = [NSThread currentThread];
    NSLog(@"%s", [t name]);
    
    do
    {
        [NSThread sleepForTimeInterval:0.2];
        NSLog(@"%d", ++cnt);
        
    } while ([t isCancelled] == NO);
    
    NSLog(@"Child thread quit...");
    
}

- (void)showThreadInfo
{
    CFRunLoopRef ref = CFRunLoopGetMain();
    NSLog(@"Main run loop: %x", ref);
    
    ref = CFRunLoopGetCurrent();
    NSLog(@"Current run loop: %x", ref);
    
    if ([NSThread isMainThread] == YES)
    {
        NSLog(@"This is main thread");
    }
    else
    {
        NSLog(@"This is NOT main thread");
    }
    
    if ([NSThread isMultiThreaded] == YES)
    {
        NSLog(@"Multithread exist");
    }
    else
    {
        NSLog(@"Single Thread");
    }
}

- (IBAction)selectStart:(id)sender
{
    if (thread == nil)
    {
        NSThread *p = [[NSThread alloc] initWithTarget:self selector:@selector(process:) object:self];
        self.thread = p;
        [p release];
        [self.thread start];
    }
}

- (IBAction)selectStop:(id)sender
{
    if (thread != nil)
    {
        [thread cancel];
    }
}

- (IBAction)selectTest:(id)sender
{
    [self showThreadInfo];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

@end
