//
//  BDGroupController.m
//  People.Day.Life
//
//  Created by samuel on 13-2-10.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDGroupController.h"
#import "BDDataManager.h"
#import "BDDataGroup.h"
#import "BDDataPeople.h"
#import "BDGroupDetailController.h"

#import "BDDataGroup.h"
#import "BDDataPeople.h"

//test
#import "BDDataGroup+Private.h"

#import "BD_Debug.h"

@interface BDGroupController () <UITableViewDataSource, UITableViewDelegate>
- (NSString *)makeGroupAndFriendInfo;
- (void)refreshInfo;
@end

@implementation BDGroupController

- (id)init
{
    if (self = [super init])
    {
        detailController_ = [[BDGroupDetailController alloc] init];
    }
    
    return self;
}

- (void)dealloc
{   
    [titleLabel_ release];
    [titleBar_ release];
    [tableView_ release];
    [backButton_ release];
    [infoLabel_ release];
    
    [detailController_ release];
    
    [super dealloc];
}

- (void)loadView
{
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 460.0)];
    
    //title bar
    titleBar_ = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44)];
    UIImage *titleImage = [UIImage imageNamed:@"resource/titlebar/titlebar_bg.png"];
    titleImage = [titleImage stretchableImageWithLeftCapWidth:titleImage.size.width / 2.0 topCapHeight:titleImage.size.height / 2.0];
    titleBar_.image = titleImage;
    titleBar_.userInteractionEnabled = YES; //否则上面的button无法接受点击

    //back button
    backButton_ = [[UIButton alloc] initWithFrame:CGRectMake(6.0, 7.0, 42.0, 30.0)];
    UIImage * image = [UIImage imageNamed:@"resource/button/bar_back_button.png"];
    image = [image stretchableImageWithLeftCapWidth:15.0
                                       topCapHeight:image.size.height / 2.0];
    [backButton_ setBackgroundImage:image forState:UIControlStateNormal];
    
    image = [UIImage imageNamed:@"resource/button/bar_back_button_pressed.png"];
    image = [image stretchableImageWithLeftCapWidth:15.0
                                       topCapHeight:image.size.height / 2.0];
    [backButton_ setBackgroundImage:image forState:UIControlStateHighlighted];
    
    UIFont *font = [UIFont systemFontOfSize:12];
    backButton_.titleLabel.font = font;
    
    UIColor *color = [UIColor whiteColor];
    backButton_.titleEdgeInsets = UIEdgeInsetsMake(0.0, 6.0, 0.0, 0.0);
    [backButton_ setTitle:@"返回" forState:UIControlStateNormal];
    [backButton_ setTitleColor:color forState:UIControlStateNormal];
    [backButton_ addTarget:self action:@selector(buttonHandle:) forControlEvents:UIControlEventTouchUpInside];
    
    [titleBar_ addSubview:backButton_];
    
    //time label
    titleLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(80.0, 0.0, 160.0, 44)];
    titleLabel_.textColor = [UIColor whiteColor];
    titleLabel_.font = [UIFont boldSystemFontOfSize:20];
    titleLabel_.backgroundColor = [UIColor clearColor];
    titleLabel_.textAlignment = NSTextAlignmentCenter;
    titleLabel_.text = @"我的家人朋友";
    [titleBar_ addSubview:titleLabel_];
    
    [view addSubview:titleBar_];
    
    //info label
    infoLabel_ = [[UITextView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 60)];
    infoLabel_.userInteractionEnabled = NO;
    infoLabel_.textColor = [UIColor blackColor];
    infoLabel_.textAlignment = NSTextAlignmentLeft;
    infoLabel_.font =  [UIFont systemFontOfSize:12];
    infoLabel_.backgroundColor = [UIColor clearColor];
    infoLabel_.text = [self makeGroupAndFriendInfo];
    [view addSubview:infoLabel_];
    
    //table view
    tableView_ = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 44.0, 320, 460 - 44.0)];
    tableView_.delegate = self;
    tableView_.dataSource = self;
    tableView_.tableHeaderView = infoLabel_;
    [view addSubview:tableView_];
    
    view.backgroundColor = [UIColor whiteColor];
    self.view = view;
    [view release];
}

- (void)buttonHandle:(id)sender
{
    [self dismiss];
}

- (void)buttonAddGroupHandle:(id)sender
{
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"请输入组名" message:nil delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"确定", nil];
    alertView.alertViewStyle = UIAlertViewStylePlainTextInput;
    [alertView show];
    [alertView release];
}

- (NSString *)makeGroupAndFriendInfo
{
    //@"共5组，20个亲友\n本周记录20条，本月记录100条";
    NSUInteger count = [[BDDataManager sharedInstance] groupCount];
    NSString *info = [NSString stringWithFormat:@"共%i组\n本周记录%i条，本月记录%i条", count, 20, 100];
    
    return info;
}

- (void)refreshInfo
{
    infoLabel_.text = [self makeGroupAndFriendInfo];
}

#pragma mark UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger count = 0;
    
    if (section == 0) //recent
    {
        count = 2;
    }
    else if (section == 1)
    {
        count = [[BDDataManager sharedInstance] groupCount];
    }
    else
    {
        BD_ASSERT(0);
    }
    
    return count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger section = indexPath.section;
    NSInteger row = indexPath.row;
    
    static NSString *BDPeoplePickControllerTableCell = @"BDPeoplePickControllerTableCell";
    
    UITableViewCell *cellView =[tableView dequeueReusableCellWithIdentifier:BDPeoplePickControllerTableCell] ;
    if (cellView == nil)
    {
        cellView = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:BDPeoplePickControllerTableCell] autorelease];
    }
    
    cellView.textLabel.font = [UIFont systemFontOfSize:14];
    
    if (section == 0)           //最近联系人
    {
        if (row == 0)
        {
            cellView.textLabel.text = @"大宝";
        }
        else
        {
            cellView.textLabel.text = @"臭蛋";
        }
    }
    else if (section == 1)      //分组
    {
        BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:row];
        cellView.textLabel.text = group.name;
        
        cellView.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    else
    {
        BD_ASSERT(0);
    }
    
    return cellView;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle) editingStyle forRowAtIndexPath:(NSIndexPath *) indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete)
    {
        //一定要先移除，再进行下面的deleteRow否则会死机
        [[BDDataManager sharedInstance] removeGroupAtIndex:indexPath.row];
        
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:2];
        [array addObject: indexPath];
        
        [tableView_ deleteRowsAtIndexPaths: array
                          withRowAnimation: UITableViewRowAnimationFade];
        
        [self refreshInfo];
    }
}

#pragma mark UITableViewDelegate
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 40.0;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 30.0;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    //bar
    UIImageView *groupHeader = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 30)];
    UIImage *titleImage = [UIImage imageNamed:@"resource/titlebar/titlebar_bg.png"];
    titleImage = [titleImage stretchableImageWithLeftCapWidth:titleImage.size.width / 2.0 topCapHeight:titleImage.size.height / 2.0];
    groupHeader.image = titleImage;
    
    //label
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(10.0, 0.0, 160.0, 30)];
    label.textColor = [UIColor whiteColor];
    label.font = [UIFont systemFontOfSize:16];
    label.backgroundColor = [UIColor clearColor];
    label.textAlignment = NSTextAlignmentLeft;
    [groupHeader addSubview:label];

    if (0 == section)
    {
        label.text = @"最近联系人";
    }
    else if (1 == section)
    {
        label.text = @"分组";
        groupHeader.userInteractionEnabled = YES; //否则上面的button无法接受点击
        
        //back button
        UIButton *addGroupButton = [[UIButton alloc] initWithFrame:CGRectMake(260.0, 3.0, 46.0, 24.0)];
        UIImage * image = [UIImage imageNamed:@"resource/button/bar_button.png"];
        image = [image stretchableImageWithLeftCapWidth:image.size.width / 2.0
                                           topCapHeight:image.size.height / 2.0];
        [addGroupButton setBackgroundImage:image forState:UIControlStateNormal];
        
        image = [UIImage imageNamed:@"resource/button/bar_button_pressed.png"];
        image = [image stretchableImageWithLeftCapWidth:image.size.width / 2.0
                                           topCapHeight:image.size.height / 2.0];
        [addGroupButton setBackgroundImage:image forState:UIControlStateHighlighted];
        
        UIFont *font = [UIFont systemFontOfSize:10];
        addGroupButton.titleLabel.font = font;
        
        UIColor *color = [UIColor whiteColor];
        //addGroupButton.titleEdgeInsets = UIEdgeInsetsMake(0.0, 6.0, 0.0, 0.0);
        [addGroupButton setTitle:@"添加分组" forState:UIControlStateNormal];
        [addGroupButton setTitleColor:color forState:UIControlStateNormal];
        [addGroupButton addTarget:self action:@selector(buttonAddGroupHandle:) forControlEvents:UIControlEventTouchUpInside];
        
        [groupHeader addSubview:addGroupButton];

    }
    else
    {
        BD_ASSERT(0);
    }
    
    return groupHeader;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger row = indexPath.row;

    BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:row];
    [detailController_ loadGroupData:group];
    
    CGRect rect = detailController_.view.frame;
    CGRect startRect = rect;
    startRect.origin.x = 320.0;
    
    detailController_.view.frame = startRect;
    [self.view addSubview:detailController_.view];
    [UIView beginAnimations:@"BDR@L" context:nil];
	[UIView setAnimationDuration:0.3];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
    detailController_.view.frame = rect;
	[UIView commitAnimations];
}

#pragma mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 1)
    {
        UITextField *textField = [alertView textFieldAtIndex:0];
        NSString *group = textField.text;
        
        if (group != nil && [group length] > 0)
        {
            [[BDDataManager sharedInstance] addGroupByName:group];
            [tableView_ reloadData];
            [self refreshInfo];
        }
    }
}

#pragma mark BDViewControllerDelegate
- (void)dismiss
{
    if (self.view.superview != nil)
    {
        [self.view removeFromSuperview];
    }
}

- (void)showInView:(UIView *)view
{
    [view addSubview:self.view];
}

@end
