//
//  BDDataGroup.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDDataGroup.h"
#import "BDDataGroup+Private.h"
#import "BDDataPeople.h"
#import "BDDataPeople+Private.h"
#import "BDCoreDataGroup.h"
#import "BDCoreDataPeople.h"
#import "BDCoreDataManager+People.h"
#import "BDCoreDataManager+Private.h"
#import "BDFileManager.h"
#import "BD_Debug.h"

@implementation BDDataGroup

@synthesize name = name_;
@synthesize image = image_;
@synthesize peopleArray = peopleArray_;
@synthesize coreData = coreData_;
@synthesize manager = manager_;

- (void)dealloc
{
    [coreData_ release];
    [image_ release];
    [peopleArray_ release];
    [name_ release];
    [manager_ release];
    
    [super dealloc];
}

- (NSUInteger)peopleCount
{
    return [peopleArray_ count];
}

- (BDDataPeople *)peopleAtIndex:(NSUInteger)index
{
    BD_ASSERT(index >= 0 && index < [peopleArray_ count]);
    
    return (BDDataPeople *)[peopleArray_ objectAtIndex:index];
}

- (NSUInteger)recordsOfThisWeek
{
    return 5;
}

- (NSUInteger)recordsOfThisMonth
{
    return 25;
}

- (void)addPeopleByName:(NSString *)name
{
    BD_ASSERT(manager_ != nil && name != nil && [name length] > 0);
    
    BDDataPeople *p = [[BDDataPeople alloc] init];
    
    p.name = name;
    
    //people
    BDCoreDataPeople *people = [manager_ appendPeople:name];
    BD_ASSERT(people != nil);
    
    [people setValue:[BDFileManager defaultPathForType:kBDDefaultManCover] forKey:kBDPeopleCover];
    [people setValue:[BDFileManager defaultPathForType:kBDDefaultManImage] forKey:kBDPeopleImage];
    [people setValue:0 forKey:kBDPeopleState];
    [people setValue:[NSDate date] forKey:kBDPeopleBirth]; //birh is right now
    [people setValue:self.coreData forKey:kBDPeopleGroup];
    
    //add to group
    NSMutableArray *groupMembers = [NSMutableArray arrayWithArray:[self.coreData.peoples allObjects]];
    [groupMembers addObject:people];
    NSSet *groupSets = [NSSet setWithArray:groupMembers];
    [self.coreData setValue:groupSets forKey:kBDGroupPeoples];
    
    [manager_ saveContext];
    
    [peopleArray_ addObject:p];
    [p release];
}

- (void)removePeopleAtIndex:(NSUInteger)index
{
    BD_ASSERT(index < [peopleArray_ count]);

    BDDataPeople *people = [peopleArray_ objectAtIndex:index];
    BDCoreDataPeople *corePeople = people.coreData;
    [manager_ deleteObject:corePeople];
    NSMutableSet *set = [NSMutableSet setWithSet:coreData_.peoples];
    [set removeObject:corePeople];
    [coreData_ setValue:set forKey:kBDGroupPeoples];
    [manager_ saveContext];
    
    [peopleArray_ removeObjectAtIndex:index];
}

- (BOOL)save
{
    [manager_ saveContext];
    
    return YES;
}

@end
