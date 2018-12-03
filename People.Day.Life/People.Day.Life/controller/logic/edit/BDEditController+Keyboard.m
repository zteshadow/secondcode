//
//  BDEditController.m
//  People.Day.Life
//
//  Created by samuel on 12-12-3.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import "BDEditController+Keyboard.h"
#import "BDTextFieldController.h"

@implementation BDEditController(Kyeboard)

- (void)keyboardDidShow:(NSNotification *)notification
{
	// keyboard frame is in window coordinates
	NSDictionary *userInfo = [notification userInfo];
	CGRect keyboardFrame = [[userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    
    UIView *inputFieldView = inputFieldController_.view;
    CGRect inputFieldFrame = inputFieldView.frame;

    // convert own frame to window coordinates, frame is in superview's coordinates
	CGRect inputFieldFrameInWindow = [inputFieldView.window convertRect:inputFieldFrame fromView:inputFieldView.superview];
    
    CGFloat newHeight = inputFieldFrameInWindow.size.height = keyboardFrame.origin.y - inputFieldFrameInWindow.origin.y;

    [inputFieldController_ setHeight:newHeight];
    
 #if 0
    
	// calculate the area of own frame that is covered by keyboard
	CGRect coveredFrame = CGRectIntersection(ownFrame, keyboardFrame);
    
	// now this might be rotated, so convert it back
	coveredFrame = [self.window convertRect:coveredFrame toView:self.superview];
    
	// set inset to make up for covered array at bottom
	self.contentInset = UIEdgeInsetsMake(0, 0, coveredFrame.size.height, 0);
	self.scrollIndicatorInsets = self.contentInset;
#endif
}

- (void)keyboardWillHide:(NSNotification *)notification
{
    
}

- (void)addKeyboardObserver
{
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    
    [center addObserver:self selector:@selector(keyboardDidShow:)
                   name:UIKeyboardDidShowNotification object:nil];
    
    [center addObserver:self selector:@selector(keyboardWillHide:)
                   name:UIKeyboardWillHideNotification object:nil];
}

- (void)removeKeyboardObserver
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end
