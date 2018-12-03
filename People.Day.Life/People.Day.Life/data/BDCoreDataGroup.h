//
//  BDCoreDataGroup.h
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

@class BDDataGroup;

@interface BDCoreDataGroup : NSManagedObject

@property (nonatomic, retain) NSString * cover;
@property (nonatomic, retain) NSString * image;
@property (nonatomic, retain) NSString * name;
@property (nonatomic) int32_t state;
@property (nonatomic, retain) NSSet *peoples;
@end

@interface  BDCoreDataGroup(CoreDataGeneratedAccessors)

//customer interface
- (BDDataGroup *)makeDataGroup;

- (void)addPeoplesObject:(NSManagedObject *)value;
- (void)removePeoplesObject:(NSManagedObject *)value;
- (void)addPeoples:(NSSet *)values;
- (void)removePeoples:(NSSet *)values;

@end
