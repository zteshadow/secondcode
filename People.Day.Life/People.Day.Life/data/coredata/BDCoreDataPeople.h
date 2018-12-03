//
//  BDCoreDataPeople.h
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

@class BDDataPeople;
@class BDCoreDataGroup, BDCoreDataPeopleRecord;

@interface BDCoreDataPeople : NSManagedObject

@property (nonatomic) NSTimeInterval birth;
@property (nonatomic, retain) NSString * image;
@property (nonatomic, retain) NSString * name;
@property (nonatomic) int32_t state;
@property (nonatomic, retain) BDCoreDataGroup *group;
@property (nonatomic, retain) NSSet *records;
@end

@interface BDCoreDataPeople (CoreDataGeneratedAccessors)

- (void)addRecordsObject:(BDCoreDataPeopleRecord *)value;
- (void)removeRecordsObject:(BDCoreDataPeopleRecord *)value;
- (void)addRecords:(NSSet *)values;
- (void)removeRecords:(NSSet *)values;

- (BDDataPeople *)makeDataPeople;

@end
