//
//  BDPeoplePickController.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013Âπ?samuel. All rights reserved.
//

#import "BDPeoplePickController.h"
#import "BDDataManager.h"
#import "BDDataGroup.h"
#import "BDDataPeople.h"
#import "BDPeoplePickCell.h"

#import "BD_Debug.h"

@interface BDPeoplePickController () <UITableViewDataSource, UITableViewDelegate>
@end

@implementation BDPeoplePickController

@synthesize delegate = delegate_;

- (id)init
{
    self = [super init];
    if (self)
    {
        selectedPeople_ = [[NSMutableArray alloc] initWithCapacity:2];
    }
    
    return self;
}

- (void)dealloc
{
    [titleLabel_ release];
    [titleBar_ release];
    [tableView_ release];
    [backButton_ release];
    [selectButton_ release];
    [infoLabel_ release];
    
    [selectedPeople_ release];
    
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
    
    //select button
    selectButton_ = [[UIButton alloc] initWithFrame:CGRectMake(270.0, 7.0, 42.0, 30.0)];
    image = [UIImage imageNamed:@"resource/button/bar_button.png"];
    image = [image stretchableImageWithLeftCapWidth:image.size.width / 2.0
                                       topCapHeight:image.size.height / 2.0];
    [selectButton_ setBackgroundImage:image forState:UIControlStateNormal];
    image = [UIImage imageNamed:@"resource/button/bar_button_pressed.png"];
    image = [image stretchableImageWithLeftCapWidth:image.size.width / 2.0
                                       topCapHeight:image.size.height / 2.0];
    [selectButton_ setBackgroundImage:image forState:UIControlStateHighlighted];
    font = [UIFont systemFontOfSize:12];
    selectButton_.titleLabel.font = font;
    
    color = [UIColor whiteColor];
    [selectButton_ setTitle:@"选择" forState:UIControlStateNormal];
    [selectButton_ setTitleColor:color forState:UIControlStateNormal];
    [selectButton_ addTarget:self action:@selector(buttonHandle:) forControlEvents:UIControlEventTouchUpInside];
    [titleBar_ addSubview:selectButton_];
    
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
    BD_ASSERT(delegate_ != nil);
    
    if (delegate_)
    {
        [delegate_ peoplePicker:self pickPeople:selectedPeople_];
    }
}

- (NSString *)makeGroupAndFriendInfo
{
    NSString *info = nil;
    NSUInteger peopleCount = 0;
    
    if ([selectedPeople_ count] > 0) //selected
    {
        NSMutableString *infoString = [NSMutableString string];
        
        for (BDDataPeople *people in selectedPeople_)
        {
            peopleCount ++;
            [infoString appendFormat:@"%@, ", people.name];
        }
        
        [infoString appendFormat:@"共%i人", peopleCount];
        info = infoString;
    }
    else
    {
        NSUInteger count = [[BDDataManager sharedInstance] groupCount];
        
        for (NSUInteger i = 0; i < count; i++)
        {
            BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:i];
            peopleCount += [group peopleCount];
        }
        
        info = [NSString stringWithFormat:@"共%i组, %i个亲友", count, peopleCount];
    }
    
    return info;
}

- (void)refreshInfo
{
    infoLabel_.text = [self makeGroupAndFriendInfo];
}

#pragma mark UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    NSUInteger count = [[BDDataManager sharedInstance] groupCount];
    return count + 1; //add recent
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    BD_ASSERT(section >= 0 && section <= [[BDDataManager sharedInstance] groupCount]);
    
    NSInteger count = 0;
    
    if (section == 0) //recent
    {
        count = 2;
    }
    else
    {
        section -= 1;//group index
        BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:section];
        BD_ASSERT(group != nil);
        
        count = [group peopleCount];
    }
    
    return count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger section = indexPath.section;
    NSInteger row = indexPath.row;
    BD_ASSERT(section >= 0 && section <= [[BDDataManager sharedInstance] groupCount]);
    
    static NSString *BDPeoplePickControllerTableCell = @"BDPeoplePickControllerTableCell";
    
    BDPeoplePickCell *cellView =[tableView dequeueReusableCellWithIdentifier:BDPeoplePickControllerTableCell] ;
    if (cellView == nil)
    {
        cellView = [[[BDPeoplePickCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:BDPeoplePickControllerTableCell] autorelease];
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
    else
    {
        section -= 1; //group index
        BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:section];
        BDDataPeople *people = [group peopleAtIndex:row];
        cellView.people = people;
        cellView.textLabel.text = people.name;
    }
    
    return cellView;
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
    BD_ASSERT(section >= 0 && section <= [[BDDataManager sharedInstance] groupCount]);
    
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
    else
    {
        section -= 1; //group index
        BDDataGroup *group = [[BDDataManager sharedInstance] groupAtIndex:section];
        label.text = group.name;
    }
    
    return groupHeader;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    BDPeoplePickCell *cell = (BDPeoplePickCell *)[tableView cellForRowAtIndexPath:indexPath];
    BD_ASSERT(cell != nil);
        
    if (cell.accessoryType == UITableViewCellAccessoryNone)
    {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
        [selectedPeople_ addObject:cell.people];
    }
    else
    {
        cell.accessoryType = UITableViewCellAccessoryNone;
        [selectedPeople_ removeObject:cell.people];
    }
    
    [tableView_ deselectRowAtIndexPath:indexPath animated:YES];
    [self refreshInfo];
}

@end
