//
//  BDCoreDataGroup.m
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoreDataGroup.h"
#import "BDDataGroup.h"
#import "BDDataGroup+Private.h"
#import "BDCoreDataPeople.h"

@implementation BDCoreDataGroup

@dynamic cover;
@dynamic image;
@dynamic name;
@dynamic state;
@dynamic peoples;

- (BDDataGroup *)makeDataGroup
{
    BDDataGroup *group = [[BDDataGroup alloc] init];
    
    group.name = self.name;
    group.image = self.image;
    
    NSMutableArray *peopleArray = [NSMutableArray arrayWithCapacity:4];
    
    for (BDCoreDataPeople *people in self.peoples)
    {
        [peopleArray addObject:[people makeDataPeople]];
    }
    
    group.peopleArray = peopleArray;
    group.coreData = self;
    
    return [group autorelease];
}

@end
