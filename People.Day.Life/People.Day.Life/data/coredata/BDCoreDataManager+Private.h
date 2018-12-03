//
//  BDCoreDataManager+Private.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

@interface BDCoreDataManager : NSObject
{
    NSManagedObjectContext *managedObjectContext_;
    NSManagedObjectModel* managedObjectModel_;
    NSPersistentStoreCoordinator* persistentStoreCoordinator_;    
}

- (void)saveContext;
- (void)deleteObject:(NSManagedObject *)object;

@end
