//
//  BDTextFieldController.h
//  People.Day.Life
//
//  Created by samuel on 12-12-10.
//  Copyright (c) 2012年 samuel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BDBaseController.h"
#import "BDToolBarDelegate.h"

@class BDTextFieldStyle;
@class BDPeoplePickController;
@class BDTextFieldController;
@class BDTitleBarButton;

@interface BDTextFieldController : BDBaseController <UITextViewDelegate>
{
    UITextView *inputView_;
    UILabel *totalLabel_;
    BDTitleBarButton *deleteButton_;
    
    BDPeoplePickController *peoplePickController_;
    BDTextFieldStyle *textFieldStyle_;
    
    NSMutableArray *toolBarButtons_;
    id<BDToolBarDelegate> delegate_;
}

@property (nonatomic, retain) NSString *text;
@property (nonatomic, assign) id<BDToolBarDelegate> delegate;

- (id)initWithStyle:(BDTextFieldStyle *)style;

/*MUST USED AFTER LOAD VIEW - samuel*/
- (BOOL)resignFirstResponder;
- (BOOL)becomeFirstResponder;

//调节输入框的高度时设置，如果不设置，使用默认的style里面的设置
- (void)setHeight:(CGFloat)height;

@end
