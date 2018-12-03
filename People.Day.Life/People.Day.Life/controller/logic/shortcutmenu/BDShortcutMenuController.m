//
//  BDShortcutMenuController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-30.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDShortcutMenuController.h"
#import "BDShortcutMenuStyle.h"
#import "BDResourceManager.h"
#import "BDButtonStyle.h"
#import "BDToolbarButton.h"
#import "BDCoreGlobalRect.h"

typedef enum
{
    BDDockSideLeft,
    BDDockSideRight,
    BDDockSideMax
    
} BDDockSide;

#define kDefaultKeyShortCutMenuCenter @"ShortCutMenuCenter"

@interface BDShortcutMenuController ()
@property (nonatomic, retain) BDShortcutMenuStyle *style;
@end

@implementation BDShortcutMenuController

@synthesize style = style_;
@synthesize delegate = delegate_;

- (id)initWithStyle:(BDShortcutMenuStyle *)style
{
    self = [super init];
    if (self)
    {
        self.style = style;
        open_ = NO;
        subMenu_ = [[NSMutableArray alloc] initWithCapacity:5];
        
        viewRecognizer_ = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleViewPan:)];
        viewRecognizer_.maximumNumberOfTouches = 1;
        
        buttonRecognizer_ = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleButtonPan:)];
        buttonRecognizer_.maximumNumberOfTouches = 1;
    }
    
    return self;
}

- (void)dealloc
{
    [style_ release];
    [mainButton_ release];
    [subMenu_ release];
    [viewRecognizer_ release];
    [buttonRecognizer_ release];
    
    [super dealloc];
}

- (void)loadView
{
    BDResourceManager * resourManager = [BDResourceManager sharedInstance];
    
    UIView *view = [[UIView alloc] initWithFrame:style_.viewFrameClose];
    CGPoint center = [self getInitPostion];
    if (center.x >= [UIScreen mainScreen].applicationFrame.size.width / 2.0)
    {
        dockSide_ = BDDockSideRight;
    }
    else
    {
        dockSide_ = BDDockSideLeft;
    }
    view.center = center;
    
    mainButton_ = [[BDButton alloc] initWithFrame:style_.mainButtonStyle.buttonFrame];
    [mainButton_ setImage:[resourManager imageNamed:style_.mainButtonStyle.imageNormal] forState:UIControlStateNormal];
    [mainButton_ setImage:[resourManager imageNamed:style_.mainButtonStyle.imageHighlighted] forState:UIControlStateHighlighted];
    [mainButton_ addTarget:self action:@selector(switchButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [view addSubview:mainButton_];
    
    for (BDButtonStyle *buttonStyle in style_.menuButtonStyles)
    {
        BDToolBarButton *button = [BDToolBarButton barButtonWithStyle:buttonStyle];
        button.frame = buttonStyle.buttonFrame;
        [button setImage:[resourManager imageNamed:buttonStyle.imageNormal] forState:UIControlStateNormal];
        [button setImage:[resourManager imageNamed:buttonStyle.imageHighlighted] forState:UIControlStateHighlighted];
        [button addTarget:self action:@selector(onShortcutItem:) forControlEvents:UIControlEventTouchUpInside];
        button.hidden = YES;
        button.tag = buttonStyle.buttonType;
        [view addSubview:button];
        [subMenu_ addObject:button];
    }
    
    [view addGestureRecognizer:viewRecognizer_];
    self.view = view;
    [view release];
}

- (CGPoint)getInitPostion
{
    CGPoint center;
    
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    NSString * centerValue = [defaults objectForKey:kDefaultKeyShortCutMenuCenter];
    if (centerValue)
    {
        center = CGPointFromString(centerValue);
        if (CGRectContainsPoint([UIScreen mainScreen].applicationFrame, center))
        {
            return center;
        }
    }

    center = CGRectCenterPoint(style_.viewFrameClose);
    [self setInitPosition:center];
    return center;
}

- (void)setInitPosition:(CGPoint)center
{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    NSString * centerValue = NSStringFromCGPoint(center);
    [defaults setObject:centerValue forKey:kDefaultKeyShortCutMenuCenter];
    [defaults synchronize];
}

- (void)relayout:(BOOL)open
{
    BDShortcutMenuStyle *style = style_;
    CGRect subRect;
    
    if (open)
    {
        subRect = self.view.frame;
        
        self.view.frame = style.viewFrameOpen;
        mainButton_.frame = subRect;
        
        for (BDToolBarButton *button in subMenu_)
        {
            button.frame = subRect;
        }
    }
    else
    {
        CGRect mainRect = mainButton_.frame;
        subRect = CGRectNormalize(mainButton_.frame);
        
        self.view.frame = mainRect;
        mainButton_.frame = subRect;

        for (BDToolBarButton *button in subMenu_)
        {
            button.frame = subRect;
        }
    }
}

- (void)delayShowSubmenu
{
    [self showSubmenu:open_];
}

- (void)checkRelayout
{
    if (--countDown_ <= 0)
    {
        [self performSelector:@selector(delayRelayout) withObject:nil afterDelay:0.1f];
    }
}

- (void)delayRelayout
{
    [self relayout:open_];
}

- (void)action:(BOOL)open
{
    open_ = open;
    mainButton_.selected = open;
    
    if (open)
    {
        [self.view removeGestureRecognizer:viewRecognizer_];
        [self adjustVerticalCenter];
        [self relayout:open];
        [self performSelector:@selector(delayShowSubmenu) withObject:nil afterDelay:0.0f];
        [mainButton_ addGestureRecognizer:buttonRecognizer_];
    }
    else
    {
        [self.view addGestureRecognizer:viewRecognizer_];
        [self showSubmenu:open_];
        [mainButton_ removeGestureRecognizer:buttonRecognizer_];
    }
}

- (void)switchButtonTouchUpInside:(id)sender
{
    open_ = !open_;
    [self action:open_];
}

- (CGPoint)newCenter:(CGPoint)oldCenter ofSubmenu:(NSUInteger)index radius:(CGFloat)radius ofTotal:(NSUInteger)total
{
    CGFloat x, y;
    CGPoint center = oldCenter;
    float degree;
    
    degree = 90.0 - index * (90.0 / (total - 1));
    x = radius * cosf(degree * M_PI / 180.0);
    y = radius * sinf(degree * M_PI / 180.0);
    
    if (dockSide_ == BDDockSideRight)
    {
        x = -x;
    }
    
    center.x += x;
    center.y -= y;
    
    return center;
}

- (void)showSubmenu:(BOOL)open
{
    NSUInteger index = 0;
    NSUInteger count = [subMenu_ count];
    
    countDown_ = count;
    
    for (BDToolBarButton *button in subMenu_)
    {
        CGPoint center;
        if (open)
        {
            center = [self newCenter:button.center ofSubmenu:index radius:style_.radius ofTotal:count];
        }
        else
        {
            center = mainButton_.center;
        }
        [self submenuAnimated:button direction:open endCenter:center];
        
        index ++;
    }
}

- (void)submenuAnimated:(UIView *)item direction:(BOOL)open endCenter:(CGPoint)endCenter
{
    CGFloat beginAlpha,endAlpha;
    
    if (open)
    {
        beginAlpha = 0.0f;
        endAlpha = 1.0f;
        item.hidden = NO;
    }
    else
    {
        beginAlpha = 1.0f;
        endAlpha = 0.0f;
    }
    
    item.alpha = beginAlpha;
    [UIView animateWithDuration:0.1f
                          delay:0.0f
                        options:UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionAllowUserInteraction
                     animations:^{
                         item.center = endCenter;
                         item.alpha = endAlpha;
                     }
                     completion:^(BOOL finish){
                         if (!open)
                         {
                             item.hidden = YES;
                             [self checkRelayout];
                         }
                         else
                         {
                             item.userInteractionEnabled = YES;
                         }
                     }];
}

- (void)reset
{
    open_ = NO;
    mainButton_.selected = NO;

    [self.view addGestureRecognizer:viewRecognizer_];
    [mainButton_ removeGestureRecognizer:buttonRecognizer_];
    
    CGRect mainRect = mainButton_.frame;
    CGRect subRect = CGRectNormalize(mainButton_.frame);
    
    self.view.frame = mainRect;
    mainButton_.frame = subRect;
    
    for (BDToolBarButton *button in subMenu_)
    {
        button.frame = subRect;
        button.hidden = YES;
    }
}

- (void)onShortcutItem:(id)sender
{
    [self reset];
    [delegate_ toolBar:nil didClickBarButton:(BDToolBarButton *)sender];
}

- (void)adjustVerticalCenter
{
    CGFloat top = style_.top + style_.radius;
    
    if (self.view.center.y < top)
    {
        CGPoint newCenter = self.view.center;
        newCenter.y = top;
        
        [UIView animateWithDuration:0.1f
                              delay:0.0f
                            options:UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionAllowUserInteraction
                         animations:^{
                             self.view.center = newCenter;
                         }
                         completion:^(BOOL finish){
                             [self setInitPosition:newCenter];
                         }];
        
    }
}

- (void)moveViewVertical:(CGFloat)offset
{
    CGPoint center = self.view.center;
    
    CGFloat newY = center.y + offset;
    
    if (newY > style_.bottom)
    {
        newY = style_.bottom;
    }
    else
    {
        CGFloat top = 0.0;
        
        if (open_)
        {
            top = style_.top + style_.radius;
        }
        else
        {
            top = style_.top;
        }
        
        if (newY < top)
        {
            newY = top;
        }
    }
    
    center.y = newY;
    
    self.view.center = center;
}

- (void)moveButtonVertical:(CGFloat)offset
{
    CGPoint center = mainButton_.center;
    
    CGFloat newY = center.y + offset;
    
    if (newY > style_.bottom)
    {
        newY = style_.bottom;
    }
    else
    {
        CGFloat top = 0.0;
        
        if (open_)
        {
            top = style_.top + style_.radius;
        }
        else
        {
            top = style_.top;
        }
        
        if (newY < top)
        {
            newY = top;
        }
    }
    
    offset = newY - center.y;
    center.y = newY;
    
    mainButton_.center = center;
    
    for (BDToolBarButton *button in subMenu_)
    {
        center = button.center;
        center.y += offset;
        button.center = center;
    }
}

- (BOOL)handleDockWithOffset:(CGFloat)offset andRecognizer:(UIPanGestureRecognizer *)recognizer
{
    BDDockSide side = BDDockSideMax;
    
    if (dockSide_ == BDDockSideLeft)
    {
        if (offset >= style_.changeSideOffset)
        {
            side = BDDockSideRight;
        }
    }
    else
    {
        if (offset <= 0.0 - style_.changeSideOffset)
        {
            side = BDDockSideLeft;
        }
    }
    
    if (side != BDDockSideMax)
    {
        dockSide_ = side;
        
        [recognizer setEnabled:NO];
        
        [UIView animateWithDuration:0.5f
                              delay:0.0f
                            options:UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionAllowUserInteraction
                         animations:^{
                             CGPoint center = self.view.center;
                             
                             if (side == BDDockSideLeft) //left
                             {
                                 center.x = roundf(self.view.frame.size.width / 2);
                                 self.view.center = center;
                             }
                             else   //right
                             {
                                 center.x = style_.right;
                                 self.view.center = center;
                             }
                         }
                         completion:^(BOOL finish){
                             [recognizer setEnabled:YES];
                             [self setInitPosition:self.view.center];
                         }];
    }
    
    return side != BDDockSideMax;
}

- (void)handleViewPan:(UIPanGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateBegan)
    {
        beginPoint_ = [recognizer locationInView:self.view];
    }
    else if (recognizer.state == UIGestureRecognizerStateChanged)
    {
        CGPoint pos = [recognizer locationInView:self.view];
        
        [self moveViewVertical:pos.y - beginPoint_.y];
        
        if (!open_)
        {
            [self handleDockWithOffset:pos.x - beginPoint_.x andRecognizer:recognizer];
        }
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded)
    {
        [self setInitPosition:self.view.center];
    }
}

- (void)handleButtonPan:(UIPanGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateBegan)
    {
        beginPoint_ = [recognizer locationInView:mainButton_];
    }
    else if (recognizer.state == UIGestureRecognizerStateChanged)
    {
        CGPoint pos = [recognizer locationInView:mainButton_];
        
        [self moveButtonVertical:pos.y - beginPoint_.y];
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded)
    {
        [self setInitPosition:mainButton_.center];
    }
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesBegan:touches withEvent:event];
    [[self nextResponder] touchesBegan:touches withEvent:event];
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{    
    open_ = !open_;
    [self action:open_];
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesCancelled:touches withEvent:event];
    [[self nextResponder] touchesCancelled:touches withEvent:event];
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesMoved:touches withEvent:event];
    [[self nextResponder] touchesMoved:touches withEvent:event];
}

@end
