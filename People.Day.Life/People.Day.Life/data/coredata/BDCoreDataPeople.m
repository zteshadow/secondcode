//
//  BDCoreDataPeople.m
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoreDataPeople.h"
#import "BDCoreDataGroup.h"
#import "BDCoreDataPeopleRecord.h"
#import "BDDataPeople.h"
#import "BDDataPeople+Private.h"

@implementation BDCoreDataPeople

@dynamic birth;
@dynamic image;
@dynamic name;
@dynamic state;
@dynamic group;
@dynamic records;

- (BDDataPeople *)makeDataPeople
{
    BDDataPeople *people = [[BDDataPeople alloc] init];
    
    people.name = self.name;
    people.coreData = self;
    return [people autorelease];
}

@end
