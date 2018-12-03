//
//  BDEditController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDEditController.h"
#import "BDEditController+Keyboard.h"
#import "BDToolBarController.h"
#import "BDCoreGlobalRect.h"
#import "BDResourceManager.h"
#import "BDToolBarButton.h"
#import "BDDataRecord.h"
#import "BDTextFieldController.h"
#import "BDTitleBarController.h"
#import "BDPeoplePickController.h"

@interface BDEditController()
{
    BDDataRecord *record_;
}
@end

@implementation BDEditController

@synthesize delegate = delegate_;

- (id)init
{
    self = [super init];
    if (self)
    {
        BDResourceManager * resourceManager = [BDResourceManager sharedInstance];

        inputFieldController_ = [[BDTextFieldController alloc] initWithStyle:resourceManager.textFieldStyle];
        inputFieldController_.delegate = self;
        
        titleController_ = [[BDTitleBarController alloc] initWithStyle:resourceManager.editTitleBarStyle];
        titleController_.delegate = self;
    }
    return self;
}

- (void)dealloc
{
    [inputFieldController_ release];
    [titleController_ release];
    [record_ release];
    
    [super dealloc];
}

- (void)loadView
{
    CGRect rect = [UIScreen mainScreen].applicationFrame;
    rect = CGRectNormalize(rect);
    
    UIView *view = [[UIView alloc] initWithFrame:rect];
    
    view.backgroundColor = [UIColor clearColor];
    [view addSubview:titleController_.view];
    [view addSubview:inputFieldController_.view];
    self.view = view;
    
    [view release];
    
    record_ = [[BDDataRecord alloc] init];
}

#pragma mark - BDToolBarDelegate
- (void)toolBar:(BDToolBar *)toolBar didClickBarButton:(BDToolBarButton *)button
{
    NSInteger type = button.type;
    
    switch (type)
    {
    case kBarButtonTypeSave:
    {
            //save
    } //no break;
    case kBarButtonTypeBackHome:
    {
        [inputFieldController_ resignFirstResponder];
        [delegate_ toolBar:toolBar didClickBarButton:button];
    }
    break;
            
    case kBarButtonTypeAddPeople:
    {
        BDPeoplePickController *peoplePicker = [[BDPeoplePickController alloc] init];
        peoplePicker.delegate = self;
        [self presentViewController:peoplePicker animated:YES completion:nil];
    }
    break;
            
    default:
    break;
    }
}

- (void)dismiss
{
    [self removeKeyboardObserver];
    [inputFieldController_ resignFirstResponder];
    [super dismiss];
}

- (void)showInView:(UIView *)view
{
    [self addKeyboardObserver];
    [super showInView:view];
    
    //must used after loadview
    inputFieldController_.text = nil;
    [inputFieldController_ becomeFirstResponder];
}

#pragma mark BDPeoplePickDelegate
- (void)peoplePicker:(BDPeoplePickController *)picker pickPeople:(NSArray *)peoples
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

@end
