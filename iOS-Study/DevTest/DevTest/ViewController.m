//
//  ViewController.m
//  DevTest
//
//  Created by majie on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"
#import "AVFoundation/AVAsset.h"
#import "AVFoundation/AVAssetImageGenerator.h"
#import "CoreMedia/CMTime.h"

#import "BDMediaItem.h"
#import "BDMediaButton.h"
#import "BDActionViewController.h"

#import "BDGestureHandle.h"


#define BD_PIC_FRAME        (3.0)
#define BD_PIC_PAD          (10.0)


static const NSTimeInterval kWobbleTime = 0.1;//07;


@interface ViewController ()

@end


@implementation ViewController


@synthesize webView, waitView;
@synthesize jsCode, cover_view;
@synthesize player;
@synthesize itemCollection, imgData;
@synthesize actionController;

- (IBAction)goHome:(id)sender
{
    [self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.baidu.com"]]];
}

- (IBAction)goToPic:(id)sender
{
    [self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.douban.com"]]];
}

- (IBAction)goToVideo:(id)sender
{
#if 0
    [self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.iqiyi.com"]]];
    
#else
    [self goJSTest:nil];
#endif
}

- (IBAction)goBack:(id)sender
{
#if 1
    if ([self.webView canGoBack])
    {
        [self.webView goBack];
    }
#else
    [self.player.view.layer renderInContext:UIGraphicsGetCurrentContext()];
    //    [[UIApplication sharedApplication].keyWindow.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *tempImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    NSString *path = [NSHomeDirectory() stringByAppendingString:@"video.jpg"];
    NSLog(@"====%@",path);
    if ([UIImageJPEGRepresentation(tempImage, 1) writeToFile:path atomically:YES]) {
        NSLog(@"success");
    }
    else {
        NSLog(@"failed");
    }

#endif
}

- (IBAction)goForward:(id)sender
{
#if 0
    if ([self.webView canGoForward])
    {
        [self.webView goForward];
    }
#else
    NSURL *url = [NSURL URLWithString:@"http://meta.video.qiyi.com/124/f5bc5195b9dbc2c44bd6f7d35f290430.m3u8"];
    
    MPMoviePlayerController *moviePlayer;
	moviePlayer = [[MPMoviePlayerController alloc]
				   initWithContentURL:url];
	//初始化视频播放器对象，并传入被播放文件的地址
    
    [moviePlayer prepareToPlay];
    [moviePlayer.view setFrame: CGRectMake(0.0, 180.0, 768.0, 400.0)];  // player's frame must match parent's
    
	moviePlayer.controlStyle = MPMovieControlStyleFullscreen;
    //moviePlayer.movieSourceType = MPMovieSourceTypeStreaming;
    [self.view addSubview:[moviePlayer view]];
    
    self.player = moviePlayer;
    
	[moviePlayer release];
    [self.player play];
    
    
    
#endif
}

+ (UIImage *)imageWithSource:(UIImage *)image rect:(CGRect)rect
{
    NSInteger scale = 1;//BISkinIsRetina() ? 2 : 1;
    if (scale > 1)
    {
        //rect = CGRectScale(rect, scale); 
    }
    
    NSInteger width  = rect.size.width;
    NSInteger height = rect.size.height;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate (NULL, width, height, 8, width * 4, colorSpace, kCGImageAlphaPremultipliedFirst);
    CGImageRef imageRef1 = CGImageCreateWithImageInRect([image CGImage], rect);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef1);
    CGImageRef imageRef2 = CGBitmapContextCreateImage(context);
    UIImage   *imageRet  = [UIImage imageWithCGImage:imageRef2 scale:scale orientation:UIImageOrientationUp];
    CGImageRelease(imageRef1);
    CGImageRelease(imageRef2);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    
    return imageRet;
}

- (void)wobble 
{
	static BOOL wobblesLeft = NO;
	
	if (isJittering) 
    {
		CGFloat rotation = (1.0 * M_PI) / 180.0;
		CGAffineTransform wobbleLeft = CGAffineTransformMakeRotation(rotation);
		CGAffineTransform wobbleRight = CGAffineTransformMakeRotation(-rotation);
		
		[UIView beginAnimations:nil context:nil];
		
		NSInteger i = 0;
		NSInteger nWobblyButtons = 0;
		
		for (UIView *tempView in [self.cover_view subviews])
        {
			if (1) {
				++nWobblyButtons;
				if (i % 2) {
					tempView.transform = wobblesLeft ? wobbleRight : wobbleLeft;
				} else {
					tempView.transform = wobblesLeft ? wobbleLeft : wobbleRight;
				}
				++i;
			}
		}
		
		if (nWobblyButtons >= 1) {
			[UIView setAnimationDuration:kWobbleTime];
			[UIView setAnimationDelegate:self];
			[UIView setAnimationDidStopSelector:@selector(wobble)];
			wobblesLeft = !wobblesLeft;
			
		} else {
			[NSObject cancelPreviousPerformRequestsWithTarget:self];
			[self performSelector:@selector(wobble) withObject:nil afterDelay:kWobbleTime];
		}
		
		[UIView commitAnimations];
	}
}

- (void)stopWobble
{
    isJittering = NO;
        
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:kWobbleTime];
    [UIView setAnimationDelegate:self];
    //[UIView setAnimationDidStopSelector:@selector(endEditingAnimationDidStop:finished:context:)];
    
    for (UIView *tempView in [self.cover_view subviews])
    {
        tempView.transform = CGAffineTransformIdentity;
    }
    
    [UIView commitAnimations];
    
}

- (void)stopCollecting
{
    //    for (UIView* sv in [self.cover_view subviews])
    //    {
    //        [sv removeFromSuperview];
    //    }
    
    //[self stopWobble];
    self.cover_view.hidden = YES;
    //[self.cover_view removeFromSuperview];
    //[self.view setNeedsDisplay];

}

- (void)startDownloadPic
{
    itemIndex = -1;
    topRight.x = [self.cover_view frame].origin.x + BD_PIC_PAD;
    topRight.y = [self.cover_view frame].origin.y + BD_PIC_PAD;
    maxLineHeight = 0;
    
    NSMutableData *data = [[NSMutableData alloc] init];//]initWithCapacity:1];
    
    self.imgData = data;
    [data release];
    
}

- (void)downloadPicReq
{
    itemIndex++;
    
    if (itemIndex < [self.itemCollection count])
    {
        BDMediaItem *item = [self.itemCollection objectAtIndex:itemIndex];
        
        NSURLRequest *req = [[NSURLRequest alloc] initWithURL:[NSURL
                                                               URLWithString:item.url]];
        [[NSURLConnection alloc] initWithRequest:req delegate:self];
        
        [req release];
    }
}

- (void)selectButtonTouchedUpInside:(BDMediaButton*)btn 
{
    btn.selected = !btn.selected;
    
    if (btn.selected)
    {
        btn.backgroundColor = [UIColor redColor];
    }
    else
    {
        btn.backgroundColor = [UIColor blueColor];
    }
}

- (void)showWebViewPopView:(id)sender
{
    [ViewController parseViewHierarchy:[[UIApplication sharedApplication].delegate window]];
}

- (IBAction)goJSTest:(id)sender
{
    NSInteger x = (NSInteger)[self.webView frame].origin.x;
    NSInteger y = (NSInteger)[self.webView frame].origin.y;
    NSInteger w = (NSInteger)[self.webView frame].size.width;
    NSInteger h = (NSInteger)[self.webView frame].size.height;
    
    [self.webView stringByEvaluatingJavaScriptFromString: jsCode];
    
    NSString *jsGetImage = [NSString stringWithFormat:@"getImageAndVideo(%i, %i, %i, %i)", x, y, w, h];
    NSString *data = [self.webView stringByEvaluatingJavaScriptFromString:jsGetImage];
    NSArray  * array= [data componentsSeparatedByString:@"\n"];
    
    int i, total = [array count];
    NSMutableArray *collection = [[NSMutableArray alloc] initWithCapacity:total];
    
    for(i = 0; i < total; i++)
    {
        NSString *tmp = [array objectAtIndex:i];
        
        if ([tmp length] > 0)
        {
            BDMediaItem *item = [[BDMediaItem alloc] initWithString:tmp];
            [collection addObject:item];
            [item release];
        }
    }
    
    self.itemCollection = collection;
    [collection release];
    
    for (UIView* sv in [self.cover_view subviews])
    {
        [sv removeFromSuperview];
    }
    
    self.cover_view.alpha = 1;//0.7;
    self.cover_view.hidden = NO;

    //static int _needDownload = 1;
    
    //if (_needDownload)
    {
        //_needDownload = 0;
        [self startDownloadPic];
        [self downloadPicReq];
    }
    
#if 0
    CGSize mSize = [self.webView frame].size;
    //CGSize mSize = {100, 100};
    //这个size定义图片的大小
    UIGraphicsBeginImageContext(mSize);
    //读取当前画布的内容
    
    //CALayer *testLayer = self.webView.layer;
    //[testLayer display];
    
    NSArray *views = [UIApplication sharedApplication].windows;
    for (UIView *item in views) {
        NSLog(@"Window = %@", item);
    }
    
    [self.webView.layer renderInContext:UIGraphicsGetCurrentContext()];
//    [[UIApplication sharedApplication].keyWindow.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *tempImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    total = [collection count];
    for (i = 0; i < total; i++)
    {
        BDMediaItem *item = [collection objectAtIndex:i];
        
        if (1)//item.type == BDMEDIA_IMG)
        {
            UIImage *pic = [ViewController imageWithSource:tempImage rect:CGRectMake(item.x, item.y, item.w, item.h)];
            
            BDMediaButton *btn = [[BDMediaButton alloc] initWithFrame:CGRectMake(item.x - BD_PIC_FRAME, item.y - BD_PIC_FRAME, item.w + 2 * BD_PIC_FRAME, item.h + 2 * BD_PIC_FRAME)];
            btn.backgroundColor = [UIColor blueColor];
            btn.selected = NO;
            
            [btn addTarget:self action:@selector(selectButtonTouchedUpInside:)
                         forControlEvents:UIControlEventTouchUpInside];

            
            //Button *btn = [UIButton buttonWithType:UIButtonTypeInfoDark];
            //btn.frame = CGRectMake(item.x, item.y, item.w, item.h);
            
            [btn setImage:pic forState:UIControlStateNormal];
            
            
            //NSString *btnTitle = [[NSString alloc] initWithFormat:@"%d,%d", item.w, item.h];
            
//            NSUInteger len = [item.url length];
//            NSRange range1 = NSMakeRange(len - 12, 12);
//            NSString *btnTitle = [[NSString alloc] initWithString:[item.url substringWithRange:range1]];
//            btn.alpha = 0.5;
//            [btn setTitle:btnTitle forState:UIControlStateNormal];
//            [btn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
//            [btnTitle release];
            
//            
//            btn.adjustsImageWhenHighlighted = YES;
//            CALayer *viewLayer = [btn layer];
//            
//            [viewLayer setMasksToBounds:YES];
//            [btn.layer setCornerRadius:10.0]; //设置矩形四个圆角半径
//            [btn.layer setBorderWidth:5.0];   //边框宽度
//            CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
//            CGColorRef colorref = CGColorCreate(colorSpace,(CGFloat[]){ 1, 0, 0, 1 }); 
//            [btn.layer setBorderColor:colorref];//边框颜色
//            
            //picView.style = UIBarButtonItemStyleBordered;
            
        //        UIImageView *picView = [[UIImageView alloc] initWithFrame:CGRectMake(item.x, item.y, item.w, item.h)];
        //        [picView setImage:pic];
            
            [self.cover_view addSubview:btn];
            
            [btn release];
        }
        else 
        {
            AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:[NSURL URLWithString:item.url] options:nil];
            
            AVAssetImageGenerator *generate = [[AVAssetImageGenerator alloc] initWithAsset:asset];
            NSError *err = NULL;
            CMTime time = CMTimeMake(1, 60);
            CGImageRef imgRef = [generate copyCGImageAtTime:time actualTime:NULL error:&err];
            NSLog(@"err==%@, imageRef==%@", err, imgRef);
            
            //return [[UIImage alloc] initWithCGImage:imgRef];
            UIImage *pic = [[UIImage alloc] initWithCGImage:imgRef];
            
            UIButton *picView = [[UIButton alloc] initWithFrame:CGRectMake(item.x, item.y, item.w, item.h)];
            [picView setImage:pic forState:UIControlStateNormal];
            [self.cover_view addSubview:picView];
            
            [pic release];
            [picView release];
        }
    }
    
    //[collection removeAllObjects];
    //[collection release];
    
    self.cover_view.alpha = 1;//0.7;
    self.cover_view.hidden = NO;
    
    isJittering = YES;
    
    //[self wobble];
    
     //[self.view addSubview:self.cover_view];
    
    
//    NSString *path = [NSHomeDirectory() stringByAppendingString:@"small.jpg"];
//    NSLog(@"====%@",path);
//    if ([UIImageJPEGRepresentation(tempImage, 1) writeToFile:path atomically:YES]) {
//        NSLog(@"success");
//    }
//    else {
//        NSLog(@"failed");
//    }
//    path = [NSHomeDirectory() stringByAppendingString:@"pic.jpg"];
//    if ([UIImageJPEGRepresentation(pic, 1) writeToFile:path atomically:YES])
//    {
//        int tmp;
//        tmp = 1;
//    }
    
//    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Images" message:data delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
//    ;
//    [alertView show];
//    [alertView release];
    
    //if (1)//cover_view == nil)
    //{
    //UIImageView *picView = [[UIImageView alloc] initWithImage:pic];
    
//    [coverView addSubview:picView];
//    [self.view addSubview:coverView];
//        self.cover_view = coverView;
//        [picView release];
//    [coverView release];
//        
//        int i = 0;
//        for (UIView *tempView in [self.cover_view subviews]) {
//			if (1) {
//					++i;
//			}
//		}
//
//        
//        [self wobble];
    //}
    #endif
}

- (void)dealloc
{
    if (itemCollection != nil)
    {
        [itemCollection removeAllObjects];
        [itemCollection release];
    }
    
    if (imgData != nil)
    {
        [imgData release];
    }
    
    [cover_view release];
    [webView release];
    [waitView release];
    [jsCode release];
    
    [super dealloc];
}

- (void)disableUIWebViewLongPress
{
    //return;
    
    for (UIView* sv in [self.webView subviews])
    {
        for (UIView* s2 in [sv subviews])
        {
            for (UIGestureRecognizer *recognizer in s2.gestureRecognizers)
            {
                if ([recognizer isKindOfClass:[UILongPressGestureRecognizer class]])
                {
                    NSLog(@"first layer: %@", sv);    
                    NSLog(@"second layer: %@ *** %@", s2, [s2 class]);    
                    recognizer.enabled = NO;
                    
                }
                
            }
            
        }
        
    }
}

- (void)makeTextNotes:(id)sender
{
    int tmp;
    tmp = 1;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    //NSString *path = [[NSBundle mainBundle] pathForResource:@"address" ofType:@"png"];
    NSString *path = [[NSBundle mainBundle] pathForResource:@"getImage" ofType:@"js"];
    
    NSString *js = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    NSString* code = [[NSString alloc] initWithString:js];
    self.jsCode = code;
    [code release];
    
#if 1
    BDGestureHandle *gestureHandle = [[BDGestureHandle alloc] initWithWebView:self.webView];
    
#else
    UILongPressGestureRecognizer *longPressRecognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(myHandleLongTouch:)];
    [longPressRecognizer setDelegate:self];
//    
//    [self disableUIWebViewLongPress];
//    

    longPressRecognizer.cancelsTouchesInView = NO;
    
    [self.webView addGestureRecognizer:longPressRecognizer];        
//    //[self.view addGestureRecognizer:longPressRecognizer]; 
    [longPressRecognizer release];
#endif
    
    self.webView.delegate = self;
    
    UIView *coverView = [[UIView alloc] initWithFrame:self.webView.frame];
    coverView.backgroundColor = [UIColor grayColor];
    
    coverView.alpha = 1;//0.5;
    coverView.hidden = YES;
    self.cover_view = coverView;
    [coverView release];
    [self.view addSubview:self.cover_view];
    
//    [self.webView stringByEvaluatingJavaScriptFromString: jsCode];
//    NSString *tmp = [self.webView stringByEvaluatingJavaScriptFromString:@"stopCallout();"];
//    
//    int l = [tmp length];
//    [self.webView stringByEvaluatingJavaScriptFromString: @"document.documentElement.style.webkitUserSelect=\"none\”;"];
//    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

#pragma mark -
#pragma mark UIWebView Delegate
- (BOOL)webView:(UIWebView *)tmp shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    //return isInPopover ? NO : YES;
    return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    [waitView startAnimating];
    return;
}

- (void)webViewDidFinishLoad:(UIWebView *)tmp
{
    [waitView stopAnimating];  
    //[self disableUIWebViewLongPress];
    //[self.webView stringByEvaluatingJavaScriptFromString: self.jsCode];
}

#pragma mark -
#pragma mark Long Press Process
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
	return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
	return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
	return YES;
}

- (CGSize)windowSize
{
	CGSize size;
	size.width = [[self.webView stringByEvaluatingJavaScriptFromString:@"window.innerWidth"] integerValue];
	size.height = [[self.webView stringByEvaluatingJavaScriptFromString:@"window.innerHeight"] integerValue];
	return size;
}

- (CGPoint)scrollOffset
{
	CGPoint pt;
	pt.x = [[self.webView stringByEvaluatingJavaScriptFromString:@"window.pageXOffset"] integerValue];
	pt.y = [[self.webView stringByEvaluatingJavaScriptFromString:@"window.pageYOffset"] integerValue];
	return pt;
}

+ (void)showViewHierarchy:(UIView *)view level:(NSInteger)level 
{
    NSMutableString *indent = [NSMutableString string];
    for (NSInteger i = 0; i < level; i++)
    {
        [indent appendString:@"    "];
    }
    
    NSLog(@"%@%@", indent, [view description]);
    
    for (UIView *item in view.subviews)
    {
        [ViewController showViewHierarchy:item level:level + 1];
    }
}

+ (void)parseViewHierarchy:(UIView *)view 
{
    [ViewController showViewHierarchy:view level:0];
}

- (void)testAction:(id)sender
{
    
}

- (void)showTextSelectMenu:(int)x yOffset:(int)y
{
    UIMenuController *popMenu = [UIMenuController sharedMenuController];
    
    //UIMenuItem *resetMenuItem = [[UIMenuItem alloc] initWithTitle:@"Select" action:@selector(testAction:)];
    UIMenuItem *resetMenuItem1 = [[UIMenuItem alloc] initWithTitle:@"Make notes" action:@selector(testAction:)];
    
    [popMenu setMenuItems:[NSArray arrayWithObjects:/*resetMenuItem,*/resetMenuItem1,nil]]; 
  
    [popMenu setMenuVisible:YES animated:YES];
    //[popMenu setTargetRect:CGRectMake(x, y, 40, 30) inView:self.webView];
    //[popMenu setTargetRect:CGRectMake(0, 0, 40, 30) inView:self.webView];
    
    //[resetMenuItem release];
    [resetMenuItem1 release];
}

- (void)myHandleLongTouch:(UILongPressGestureRecognizer *)recognizer
{
    NSLog(@"LongPress: %d", [recognizer state]);
    CGPoint pt = [recognizer locationInView:self.webView];
    [self.webView stringByEvaluatingJavaScriptFromString: self.jsCode];
    [self.webView stringByEvaluatingJavaScriptFromString: @"stopCallout();"];
    
#if 1
    if ([recognizer state] == UIGestureRecognizerStateBegan)
    {
        NSString *jsData = [self.webView stringByEvaluatingJavaScriptFromString:@"getSelectedText();"];
        
        if ([jsData length] > 0)
        {
            int x = pt.x;
            int y = pt.y;
            
            [self showTextSelectMenu:x yOffset:y];
        }
        else
        {
#if 1
            UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:@"Hello"
                                                               delegate:nil cancelButtonTitle:@"Cancel" 	 destructiveButtonTitle:@"destructive"
                                                      otherButtonTitles:@"Open", nil];

            sheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
            CGRect popRect = CGRectMake(pt.x, pt.y, 2.0, 20.0);
            
            //[sheet showInView:self.webView];
            [sheet showFromRect:popRect inView:self.webView animated:YES];
            [sheet release];
#else       
            isInPopover = YES;
            
            BDActionViewController *ctrl = [[BDActionViewController alloc] init];
            
            self.actionController = ctrl;
            [ctrl release];
            
            UIPopoverController *pop = [[UIPopoverController alloc] initWithContentViewController:self.actionController];
            
            
            CGSize popSize = self.actionController.view.frame.size;
            
            self.actionController.popView = pop;
            [pop setPopoverContentSize:popSize animated:YES];
            //pop.popoverContentSize =  CGSizeMake(320,480);
            CGRect popRect = CGRectMake(pt.x, pt.y, 2.0, 20.0);
            
            pop.delegate = self;
            //[self.actionController.view becomeFirstResponder];
            ctrl.modalInPopover = YES;
            
            [pop presentPopoverFromRect:popRect inView:self.webView permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
#endif
            //
            //popup action sheet.

            //pop up
        }
        //NSLog(@"selected text: %@", [self.webView stringByEvaluatingJavaScriptFromString:@"getSelectedText();"]);
        
        NSLog(@"selected text: %@", [self.webView stringByEvaluatingJavaScriptFromString:@"getSelectedText();"]);
    
        NSLog(@"element: %@", [self.webView stringByEvaluatingJavaScriptFromString:
                               [NSString stringWithFormat:@"getHTMLElementsAtPoint(%i,%i);",(NSInteger)pt.x,(NSInteger)pt.y]]);
        
        //NSLog(@"focus node: %@", [self.webView stringByEvaluatingJavaScriptFromString:@"document.selection.createRange()"]);
    //NSLog(@"focus node: %@", [self.webView stringByEvaluatingJavaScriptFromString:@"getFocusNode();"]);
        //return;
    }
#endif
    
    return;
    
    if ([recognizer state]==UIGestureRecognizerStateEnded)
    {
        CGPoint pt = [recognizer locationInView:self.webView];
        int x = pt.x;
        int y = pt.y;

    NSLog(@"LongPress: %d", [recognizer state]);
    //[self performSelector:@selector(showWebViewPopView:) withObject:self afterDelay:2.5];
    
        [self showTextSelectMenu:x yOffset:y];
    return;
    }
    
    return;
    
	//if ([recognizer state]==UIGestureRecognizerStateBegan)
	{
        CGPoint pt = [recognizer locationInView:self.webView];
        int x = pt.x;
        int y = pt.y;
        
		//pt = [self.webView convertPoint:pt fromView:nil];
		x = pt.x;
        y = pt.y;
        
//		CGPoint offset  = [self scrollOffset];
//		CGSize viewSize = [self.webView frame].size;
//		CGSize windowSize = [self windowSize];
//		
//		CGFloat f = windowSize.width / viewSize.width;
//		pt.x = pt.x * f + offset.x;
//		pt.y = pt.y * f + offset.y;
		
		[self.webView stringByEvaluatingJavaScriptFromString: self.jsCode];
         
		NSString *tags = [NSString stringWithString:[self.webView stringByEvaluatingJavaScriptFromString:
													 [NSString stringWithFormat:@"getHTMLElementsAtPoint(%i,%i);",(NSInteger)pt.x,(NSInteger)pt.y]]];

//		NSString *tags = [NSString stringWithString:[self.webView stringByEvaluatingJavaScriptFromString:
//													 [NSString stringWithFormat:@"displayAlert();"]]];
//        
//        return;
        
        //[self.waitView becomeFirstResponder];
        
        //[self goJSTest:nil];
        NSLog(@"point(%i, %i)--%@", (NSInteger)pt.x, (NSInteger)pt.y, tags);
        
        if ([recognizer state] == UIGestureRecognizerStateEnded /*copy text*/
            || [recognizer state] == UIGestureRecognizerStateCancelled )
        {
            //[ViewController showViewHierarchy:self.webView];
        }
        
//        if ([tags rangeOfString:@",IMG,"].location != NSNotFound
//         || [tags rangeOfString:@",A,"].location != NSNotFound)
//        {
//            UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:@"Hello"
//                                                               delegate:nil cancelButtonTitle:@"Cancel" 	 destructiveButtonTitle:@"destructive button title"
//                                                      otherButtonTitles:@"ViewBigPic", nil];
//            
//            [sheet showInView:self.view];
//            [sheet release];
//        }
	}
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
}

#pragma mark -
#pragma mark Touch Event Methods
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UIView* sv in [self.cover_view subviews])
    {
        [sv removeFromSuperview];
    }
    
    [self stopCollecting];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

#pragma mark -
#pragma mark NSURLConnection Delegate Method
- (void)connection:(NSURLConnection *)connection
didReceiveResponse:(NSURLResponse *)response
{
    [imgData setLength:0];
}

- (void)connection:(NSURLConnection *)connection
    didReceiveData:(NSData *)data
{
    [imgData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    UIImage *img = [UIImage imageWithData:imgData];
    [connection release];
    
    BDMediaItem *item = [self.itemCollection objectAtIndex:itemIndex];
    
    CGRect rect = [self.cover_view frame];
    CGPoint pos = CGPointMake(topRight.x, topRight.y);
    
    if (topRight.x + item.w > rect.origin.x + rect.size.width)
    {
        pos.x = BD_PIC_PAD;
        pos.y += BD_PIC_PAD + maxLineHeight;
    }
    
    BDMediaButton *btn = [[BDMediaButton alloc] initWithFrame:CGRectMake(pos.x - BD_PIC_FRAME, pos.y - BD_PIC_FRAME, item.w + 2 * BD_PIC_FRAME, item.h + 2 * BD_PIC_FRAME)];
    btn.backgroundColor = [UIColor blueColor];
    btn.selected = NO;
    
    [btn addTarget:self action:@selector(selectButtonTouchedUpInside:)
  forControlEvents:UIControlEventTouchUpInside];
    
    [btn setImage:img forState:UIControlStateNormal];
    
    [self.cover_view addSubview:btn];
    
    [btn release];
    
    topRight.x = pos.x + item.w + BD_PIC_PAD;
    topRight.y = pos.y;
    
    if (maxLineHeight < item.h)
    {
        maxLineHeight = item.h;
    }
    
    [imgData setLength:0];
    [self downloadPicReq];
}

- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController
{
    isInPopover = NO;
    return YES;
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
    int tmp;
    tmp = 1;
    
    isInPopover = NO;
    
    if (popoverController != nil)
    {
        [popoverController dismissPopoverAnimated:YES];
        [popoverController release];
    }
}

@end
