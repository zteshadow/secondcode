//
//  BDCoreDataPeopleRecord.h
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface BDCoreDataPeopleRecord : NSManagedObject
@property (nonatomic, retain) NSSet *people;
@property (nonatomic, retain) NSSet *record;
@end

@interface BDCoreDataPeopleRecord (CoreDataGeneratedAccessors)

- (void)addPeopleObject:(NSManagedObject *)value;
- (void)removePeopleObject:(NSManagedObject *)value;
- (void)addPeople:(NSSet *)values;
- (void)removePeople:(NSSet *)values;

- (void)addRecordObject:(NSManagedObject *)value;
- (void)removeRecordObject:(NSManagedObject *)value;
- (void)addRecord:(NSSet *)values;
- (void)removeRecord:(NSSet *)values;

@end
