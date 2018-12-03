//
//  BDCoreDataManager+People.m
//  People.Day.Life
//
//  Created by samuel on 13-1-18.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoreDataManager+People.h"

@implementation BDCoreDataManager (People)

- (NSUInteger)countForEntity:(NSString *)name
{
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    
    NSEntityDescription* entity = [NSEntityDescription entityForName:name inManagedObjectContext:managedObjectContext_];
    [request setEntity:entity];
    [request setResultType:NSCountResultType];
    
    NSUInteger count = [managedObjectContext_ countForFetchRequest:request error:nil];
    
    [request release];
    
    return count;
}

- (NSArray *)entity:(NSString *)name fromIndex:(NSUInteger)begin toIndex:(NSUInteger)end
{
    NSInteger limit = end - begin + 1;
    
    if (limit < 0)
    {
        return nil;
    }
    
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    NSEntityDescription* entity = [NSEntityDescription entityForName:name inManagedObjectContext:managedObjectContext_];
    [request setEntity:entity];
    
    [request setFetchOffset:begin];
    [request setFetchLimit:limit];
    
    [request setReturnsObjectsAsFaults:YES];
    
    NSError* error = nil;
    NSArray* result = [managedObjectContext_ executeFetchRequest:request error:&error];
    [request release];
    
    return [[result mutableCopy] autorelease];
}

- (BOOL)isGroupExist:(NSString *)name
{
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    
    NSEntityDescription* entity = [NSEntityDescription entityForName:kBDEntityGroup inManagedObjectContext:managedObjectContext_];
    [request setEntity:entity];
        
    NSPredicate* predicate = [NSPredicate predicateWithFormat:@"%K like %@", kBDGroupName, name];
    [request setPredicate:predicate];
    
    NSArray* result = [managedObjectContext_ executeFetchRequest:request error:nil];
    [request release];
    
    if ([result count] > 0)
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

//唯一，插入前需要isGroupExist判断是否存在
- (BDCoreDataGroup *)appendGroup:(NSString *)name
{
    BDCoreDataGroup* data = [NSEntityDescription insertNewObjectForEntityForName:kBDEntityGroup inManagedObjectContext:managedObjectContext_];
    [data setValue:name forKey:kBDGroupName];
    [data setValue:@"" forKey:kBDGroupImage];
    [data setValue:@"" forKey:kBDGroupCover];
    [data setValue:0 forKey:kBDGroupState];
    
    //save by user
    //[self saveContext];
    
    return data;
}

- (BOOL)isPeopleExist:(NSString *)name
{
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    
    NSEntityDescription* entity = [NSEntityDescription entityForName:kBDEntityPeople inManagedObjectContext:managedObjectContext_];
    [request setEntity:entity];
    
    NSPredicate* predicate = [NSPredicate predicateWithFormat:@"%K like %@", kBDPeopleName, name];
    [request setPredicate:predicate];
    
    NSArray* result = [managedObjectContext_ executeFetchRequest:request error:nil];
    [request release];
    
    if ([result count] > 0)
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

- (BDCoreDataPeople *)appendPeople:(NSString *)name
{
    BDCoreDataPeople* data = [NSEntityDescription insertNewObjectForEntityForName:kBDEntityPeople inManagedObjectContext:managedObjectContext_];
    [data setValue:name forKey:kBDPeopleName];
    [data setValue:@"" forKey:kBDPeopleImage];
    [data setValue:@"" forKey:kBDPeopleCover];
    [data setValue:0 forKey:kBDPeopleState];
    [data setValue:0 forKey:kBDPeopleBirth];
    
    //save by user
    //[self saveContext];
    
    return data;
}

- (BDCoreDataRecord *)appendRecord:(NSDate *)createTime
{
    BDCoreDataRecord* data = [NSEntityDescription insertNewObjectForEntityForName:kBDEntityRecord inManagedObjectContext:managedObjectContext_];
    [data setValue:createTime forKey:kBDRecordID];
    [data setValue:@"" forKey:kBDRecordImage];
    [data setValue:0 forKey:kBDPeopleState];

    return data;
}

@end
