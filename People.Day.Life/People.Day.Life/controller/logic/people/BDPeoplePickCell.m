//
//  BDPeoplePickCell.m
//  People.Day.Life
//
//  Created by samuel on 13-2-24.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDPeoplePickCell.h"
#import "BDDataPeople.h"

@implementation BDPeoplePickCell

@synthesize people = people_;

- (void)dealloc
{
    [people_ release];
    [super dealloc];
}

@end
