//
//  BDActionViewController.m
//  DevTest
//
//  Created by majie on 12-5-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDActionViewController.h"

@interface BDActionViewController ()

@end

@implementation BDActionViewController


@synthesize actionArray;
@synthesize popView;


- (void)dealloc
{
    [super dealloc];
    
    if (self.actionArray != nil)
    {
        [self.actionArray release];
    }
    
    if (popView != nil)
    {
        [popView release];
    }
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)loadView
{
    // If you create your views manually, you MUST override this method and use it to create your views.
    // If you use Interface Builder to create your views, then you must NOT override this method.
    [super loadView];
    
    self.actionArray = [NSArray arrayWithObjects:@"copy", @"open", @"collect", nil];
    
#if 1
    
    CGSize tableSize = CGSizeMake(180.0, 44.0 * [self.actionArray count] + 1);
    
    self.contentSizeForViewInPopover = tableSize;
    
    CGRect tableViewRect = CGRectMake(0.0, 0.0, tableSize.width, tableSize.height);
    
    UITableView *tableView = [[UITableView alloc] initWithFrame:tableViewRect style:UITableViewStylePlain];
    
    [tableView setDelegate:self];
    [tableView setDataSource:self];
    
    //[self.view addSubview:tableView];
    self.view = tableView;
    [tableView release];
#else
    CGSize tableSize = CGSizeMake(180.0, 44.0 * [self.actionArray count] + 1);
    
    self.contentSizeForViewInPopover = tableSize;
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:@"Hello"
                                                       delegate:nil cancelButtonTitle:@"Cancel" 	 destructiveButtonTitle:@"destructive"
                                              otherButtonTitles:@"Open", nil];
    
    sheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
    self.view = sheet;
    
    CGRect sheetRect = sheet.frame;
    self.contentSizeForViewInPopover = sheetRect.size;
    
    [sheet release];

#endif
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
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


#pragma mark -
#pragma UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.actionArray count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *actionViewCellFlag = @"ActionViewCellFlag";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:actionViewCellFlag];
    
    if (cell == nil)
    {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:actionViewCellFlag] autorelease];
    }
    
    NSUInteger row = [indexPath row];
    cell.textLabel.text = [self.actionArray objectAtIndex:row];
    
    return cell;
}


#pragma mark -
#pragma UITableViewDelegate


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //[self.popView dismissPopoverAnimated:YES];
    
    [[[[UIAlertView alloc]  
       initWithTitle:@"sorry"   
       message:@"1234567890"   
       delegate:nil   
       cancelButtonTitle:@"OK"   
       otherButtonTitles:nil, nil]  
      autorelease]  
     show];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return @"Name";
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}


@end
