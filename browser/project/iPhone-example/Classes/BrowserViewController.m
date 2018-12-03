//
//  BrowserViewController.m
//  Browser
//
//  Created by samuel on 11-11-29.
//  Copyright Baidu ,. Ltd 2011. All rights reserved.
//

#import "BrowserViewController.h"

@implementation BrowserViewController

@synthesize input;
@synthesize webView;


- (IBAction)selectFunction:(id)sender
{
    UISegmentedControl *control = (UISegmentedControl *)sender;
    
    int i = [control selectedSegmentIndex];
    
    if (i == 2)
    {
        [self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://m.baidu.com"]]];
    }
    else if (i == 1) //test javascript
    {
        //NSString *javascript = @"window.location=\"http://3g.163.com\"";
        //NSString *javascript = @"window.external.notify('hello');";
        //NSString *javascript = @"document.location.href";
        /*
        NSString *javascript = 
        @"var script = document.createElement('script');"  
        "script.type = 'text/javascript';"  
        "script.text = \"function myFunction() { " 
        "var links = document.getElementsByTagName('src');"//.getElementsByTagName('src');"
        "return links.nodeName;"
        "}\";"  
        "document.getElementsByTagName('head')[0].appendChild(script);";
        */
        
        //NSString *value = [self.webView stringByEvaluatingJavaScriptFromString:javascript];
        //value = [self.webView stringByEvaluatingJavaScriptFromString:@"myFunction();"];
        
        NSString *javascript = 
        @"var script = document.createElement('script');"  
        "script.type = 'text/javascript';"  
        "script.text = \"function myFunction() { " 
        "return 'hello world';"
        "}\";"  
        "document.getElementsByTagName('head')[0].appendChild(script);";
        
        NSString *value = [self.webView stringByEvaluatingJavaScriptFromString:javascript];
        value = [self.webView stringByEvaluatingJavaScriptFromString:@"myFunction();"];
        
        int len = [value length];
        
    }
}

- (IBAction)inputDone:(id) sender
{
    [sender resignFirstResponder];
    
    //NSString *      infoFilePath;
    NSURLRequest *  request;
    
    //infoFilePath = [[NSBundle mainBundle] pathForResource:@"Info" ofType:@"html"];
    //assert(infoFilePath != nil);
    
    //request = [NSURLRequest requestWithURL:[NSURL fileURLWithPath:infoFilePath]];
    request = [NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.baidu.com"]];
    assert(request != nil);
    
    [self.webView loadRequest:request];
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
    
    //webView.delegate = self;
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
    self.input = nil;
    self.webView = nil;
}


- (void)dealloc {
    
    [input release];
    [webView release];
    [super dealloc];
}


#if 0
#pragma mark -
#pragma mark UIWebView Delegate
- (void)webViewDidStartLoad:(UIWebView *)webView
{
    
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    
}
#endif

@end
