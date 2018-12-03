//
//  BDTextFieldController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-10.
//  Copyright (c) 2012Âπ?samuel. All rights reserved.
//

#import "BDTextFieldController.h"
#import "BDTextFieldStyle.h"
#import "BDLabelStyle.h"
#import "BDPeoplePickController.h"
#import "BDTitleBarButton.h"
#import "BDResourceManager.h"
#import "BD_Debug.h"

@interface BDTextFieldController()
@property (nonatomic, retain) BDTextFieldStyle *textFieldStyle;

- (void)initInputView:(BDTextFieldStyle *)style;
- (void)initLabel:(BDTextFieldStyle *)style;
- (void)initButton:(BDTextFieldStyle *)style;

- (void)updateLabel:(NSInteger)total;

@end

@implementation BDTextFieldController

@synthesize textFieldStyle = textFieldStyle_;
@synthesize delegate = delegate_;

- (id)initWithStyle:(BDTextFieldStyle *)style
{
    BD_ASSERT(style != nil);
    
    self = [super init];
    if (self)
    {
        self.textFieldStyle = style;
        toolBarButtons_ = [[NSMutableArray alloc] initWithCapacity:3];
    }
    
    return self;
}

- (void)dealloc
{
    [inputView_ release];
    [totalLabel_ release];
    [deleteButton_ release];
    
    [textFieldStyle_ release];
    [peoplePickController_ release];
    [toolBarButtons_ release];
    
    [super dealloc];
}

- (void)loadView
{
    BDTextFieldStyle *style = textFieldStyle_;
    
    UIView *view = [[UIView alloc] initWithFrame:style.viewFrame];
    self.view = view;
    [view release];
    
    [self initInputView:style];
    [self initLabel:style];
    [self initButton:style];
}

- (void)initInputView:(BDTextFieldStyle *)style
{
    inputView_ = [[UITextView alloc] init];
    inputView_.delegate = self;
    
    CGRect rect = style.textFieldFrame;
    BDButtonStyle *buttonStyle = [style buttonStyleByType:kBarButtonTypeAddPeople];
    rect.size.height = self.view.frame.size.height - 2 * style.gap - buttonStyle.buttonFrame.size.height;
    inputView_.frame = rect;
    
    inputView_.textColor = [UIColor grayColor];
    BDResourceManager *resourceManager = [BDResourceManager sharedInstance];
    inputView_.font = [resourceManager fontWithFontStyle:style.labelStyle.titleFont];
    
    [self.view addSubview:inputView_];
}

- (void)initLabel:(BDTextFieldStyle *)style
{
    totalLabel_ = [[UILabel alloc] init];
    totalLabel_.frame = style.labelStyle.viewFrame;
    totalLabel_.textColor = [UIColor grayColor];
    totalLabel_.text = @"0";
    BDResourceManager *resourceManager = [BDResourceManager sharedInstance];
    totalLabel_.font = [resourceManager fontWithFontStyle:style.labelStyle.titleFont];
    [self.view addSubview:totalLabel_];
}

- (void)initButton:(BDTextFieldStyle *)style
{
    for (BDButtonStyle *item in style.barButtonStyles)
    {
        BDToolBarButton *barButton;
        
        if (item.titleString)
        {
            barButton = [BDTitleBarButton titleBarButtonWithStyle:item];
        }
        else
        {
            barButton = [BDToolBarButton barButtonWithStyle:item];
        }
        
        if (item.alignType == BDButtonAlignBottom)
        {
            CGRect rect = barButton.frame;
            rect.origin.y = self.view.frame.size.height - style.gap - rect.size.height;
            barButton.frame = rect;
        }
        
        [barButton addTarget:self action:@selector(barButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:barButton];
        [toolBarButtons_ addObject:barButton];
    }
}

- (void)barButtonClicked:(BDToolBarButton *)sender
{
    [delegate_ toolBar:nil didClickBarButton:sender];
}

- (void)buttonClicked:(UIButton *)sender
{
#if 0
    NSInteger total = inputView_.text.length;
    
    if (total > 0)
    {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Clear"
                                                            message:@"?"
                                                           delegate:self
                                                  cancelButtonTitle:@"Yes"
                                                  otherButtonTitles:@"No", nil];
        
        [alertView show];
        [alertView release];
    }
#endif
    
    if (peoplePickController_ == nil)
    {
        peoplePickController_ = [[BDPeoplePickController alloc] init];
    }
    
    UINavigationController *testNav = [[UINavigationController alloc] initWithRootViewController:peoplePickController_];
    [self.view addSubview:testNav.view];
    //[self.navigationController pushViewController:peoplePickController_ animated:YES];
}

- (void)updateLabel:(NSInteger)total
{
    totalLabel_.text = [NSString stringWithFormat:@"%d", total];
}

#pragma mark --
#pragma mark UITextFieldDelegate
- (BOOL)textViewShouldBeginEditing:(UITextView *)textView
{
    return YES;
}

- (BOOL)textViewShouldEndEditing:(UITextView *)textView
{
    return YES;
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    return  YES;
}

//called when text is changed every time
- (void)textViewDidChange:(UITextView *)textView
{
    NSInteger total = textView.text.length;
    
    [self updateLabel:total];
}

- (BDToolBarButton *)barButtonWithType:(BDBarButtonType)buttonType
{
    for (BDToolBarButton * button in toolBarButtons_)
    {
        if (button.type == buttonType)
        {
            return button;
        }
    }
    
    return nil;
}

- (void)resetSubviewHeight
{
    CGRect rect = inputView_.frame;
    
    BDTextFieldStyle *style = textFieldStyle_;
    
    //input view
    BDButtonStyle *buttonStyle = [style buttonStyleByType:kBarButtonTypeAddPeople];
    rect.size.height = self.view.frame.size.height - 2 * style.gap - buttonStyle.buttonFrame.size.height;
    inputView_.frame = rect;
    
    //button
    for (BDButtonStyle *item in style.barButtonStyles)
    {
        if (item.alignType == BDButtonAlignBottom)
        {
            BDToolBarButton *barButton = [self barButtonWithType:item.buttonType];
            CGRect rect = barButton.frame;
            rect.origin.y = self.view.frame.size.height - style.gap - rect.size.height;
            barButton.frame = rect;
        }
    }
}

- (void)setHeight:(CGFloat)height
{
    CGRect rect = self.view.frame;

    rect.size.height = height;
    self.view.frame = rect;
    [self resetSubviewHeight];    
}

#pragma mark --
#pragma mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (0 == buttonIndex)   //clear
    {
        inputView_.text = nil;
        [self updateLabel:0];
    }
}

#pragma mark --
#pragma mark override
- (BOOL)resignFirstResponder
{
    [inputView_ resignFirstResponder];
    return [super resignFirstResponder];
}

- (BOOL)becomeFirstResponder
{
    return [inputView_ becomeFirstResponder];
}

- (NSString *)text
{
    return inputView_.text;
}

- (void)setText:(NSString *)text
{
    inputView_.text = text;
    [self updateLabel:text.length];
}

@end
