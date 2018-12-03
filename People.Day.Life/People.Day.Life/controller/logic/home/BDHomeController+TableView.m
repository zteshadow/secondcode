//
//  BDHomeController+TableView.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDHomeController+TableView.h"
#import "BDCoverController.h"
#import "BDDataManager.h"
#import "BDDataRecord.h"
#import "BDHomeTableViewCell.h"

#import "BD_Debug.h"

@implementation BDHomeController(TableView)

#pragma mark UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger count = 0;
    
    if (section == 0) //recent
    {
        count = [[BDDataManager sharedInstance] recordCount];
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
    
    static NSString *BDPeoplePickControllerTableCell = @"BDHomeControllerTableCell";
    
    BDHomeTableViewCell *cellView =[tableView dequeueReusableCellWithIdentifier:BDPeoplePickControllerTableCell] ;
    if (cellView == nil)
    {
        cellView = [[[BDHomeTableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:BDPeoplePickControllerTableCell] autorelease];
    }
    
    if (section == 0)           //最近联系人
    {
        BDDataRecord *data = [[BDDataManager sharedInstance] recordAtIndex:row];
        [cellView loadDataRecord:data];
    }
    else
    {
        BD_ASSERT(0);
    }
    
    return cellView;
}

#pragma mark UITableViewDelegate
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger row = indexPath.row;
    BDDataRecord *record = [[BDDataManager sharedInstance] recordAtIndex:row];
    
    CGSize size = [BDHomeTableViewCell recordSize:record];
    return size.height;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView_ deselectRowAtIndexPath:indexPath animated:YES];
}

#pragma mark --
#pragma mark UIScrollViewDelegate Methods
- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    if ([coverController_ respondsToSelector:@selector(scrollViewDidScroll:)])
    {
        [coverController_ scrollViewDidScroll:scrollView];
    }
}


@end
