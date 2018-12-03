//
//  BDCoreDataRecord.h
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

@class BDCoreDataPeopleRecord;
@class BDDataRecord;

@interface BDCoreDataRecord : NSManagedObject

@property (nonatomic) NSTimeInterval id;
@property (nonatomic, retain) NSString * image;
@property (nonatomic) int32_t state;
@property (nonatomic, retain) NSString * text;
@property (nonatomic, retain) NSSet *peoples;
@end

@interface BDCoreDataRecord (CoreDataGeneratedAccessors)

- (void)addPeoplesObject:(BDCoreDataPeopleRecord *)value;
- (void)removePeoplesObject:(BDCoreDataPeopleRecord *)value;
- (void)addPeoples:(NSSet *)values;
- (void)removePeoples:(NSSet *)values;

- (BDDataRecord *)makeData;

@end
