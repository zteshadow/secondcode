//
//  BDCoreDataManager.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoreDataManager+Private.h"
#import "BDFileManager.h"
#import "BD_Debug.h"

//entity
const NSString *kBDEntityGroup = @"BDCoreDataGroup";
const NSString *kBDEntityPeople = @"BDCoreDataPeople";
const NSString *kBDEntityRecord = @"BDCoreDataRecord";

//group attribute
const NSString *kBDGroupName = @"name";
const NSString *kBDGroupImage = @"image";
const NSString *kBDGroupCover = @"cover";
const NSString *kBDGroupState = @"state";
const NSString *kBDGroupPeoples = @"peoples";

//people attributes
const NSString *kBDPeopleName = @"name";
const NSString *kBDPeopleImage = @"image";
const NSString *kBDPeopleCover = @"cover";
const NSString *kBDPeopleState = @"state";
const NSString *kBDPeopleBirth = @"birth";
const NSString *kBDPeopleGroup = @"group";

//record atributes
const NSString *kBDRecordID = @"id";
const NSString *kBDRecordText = @"text";
const NSString *kBDRecordImage = @"image";
const NSString *kBDRecordState = @"state";

@implementation BDCoreDataManager

- (id)init
{
    if (self = [super init])
    {
        managedObjectModel_ = [[NSManagedObjectModel mergedModelFromBundles:nil] retain];
        BD_ASSERT(managedObjectModel_ != nil);
        
        [self initPersistentStoreCoordinator:managedObjectModel_];
        
        managedObjectContext_ = [[NSManagedObjectContext alloc] init];
        BD_ASSERT(managedObjectContext_ != nil);
        
        [managedObjectContext_ setPersistentStoreCoordinator:persistentStoreCoordinator_];
    }
    
    return self;
}

- (void)dealloc
{
    [managedObjectContext_ release];
    [managedObjectModel_ release];
    [persistentStoreCoordinator_ release];

    [super dealloc];
}

- (void)saveContext
{
    NSManagedObjectContext* context = managedObjectContext_;
    
    if (context != nil)
    {
        NSError* error = nil;
        
        BOOL result = [context save:&error];
        BD_ASSERT(result == YES);
        
    #if 0
        if ([context hasChanges] && ![context save:&error])
        {
            //error handling
            NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
            abort();
        }
    #endif
    }
}

- (void)deleteObject:(NSManagedObject *)object
{
    [managedObjectContext_ deleteObject:object];
}

- (void)initPersistentStoreCoordinator:(NSManagedObjectModel *)model
{
    BD_ASSERT(persistentStoreCoordinator_ == nil);
    
    persistentStoreCoordinator_ = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:model];
    BD_ASSERT(persistentStoreCoordinator_ != nil);

    NSError* error;
    NSURL* sqlURL = [NSURL fileURLWithPath:[BDFileManager documentPath]];
    sqlURL = [sqlURL URLByAppendingPathComponent:@"People.Day.Life.sqlite"];
    
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSNumber numberWithBool:YES], NSMigratePersistentStoresAutomaticallyOption,
                             [NSNumber numberWithBool:YES], NSInferMappingModelAutomaticallyOption, nil];
    
    /*当CoreData中的Entity,Property或者Relationship发生改变以后,下面的语句会异常. 这个问题容易理解，我们就是通过一个ManagerdObjectModel(具体对应xcode里面的xxx.xcdatamodel文件，下面简称mom)来访问具体的存储数据，这里是xxx.sqlite文件，mom有一个hash table保存这个sqlite文件中的所有数据类型，版本等，当发现不匹配了就返回错误*/
    NSPersistentStore *store = [persistentStoreCoordinator_
                                addPersistentStoreWithType:NSSQLiteStoreType
                                configuration:nil
                                URL:sqlURL
                                options:options
                                error:&error];
    BD_ASSERT(store != nil);
}

@end
