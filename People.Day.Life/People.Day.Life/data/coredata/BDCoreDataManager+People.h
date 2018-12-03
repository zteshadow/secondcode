//
//  BDCoreDataManager+People.h
//  People.Day.Life
//
//  Created by samuel on 13-1-18.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BDCoreDataManager+Private.h"
#import "BDCoreDataGroup.h"
#import "BDCoreDataPeople.h"
#import "BDCoreDataRecord.h"

//entity
extern NSString *kBDEntityGroup;
extern NSString *kBDEntityPeople;
extern NSString *kBDEntityRecord;

//group attribute
extern NSString *kBDGroupName;
extern NSString *kBDGroupImage;
extern NSString *kBDGroupCover;
extern NSString *kBDGroupState;
extern NSString *kBDGroupPeoples;

//people attribute
extern NSString *kBDPeopleName;
extern NSString *kBDPeopleImage;
extern NSString *kBDPeopleCover;
extern NSString *kBDPeopleState;
extern NSString *kBDPeopleBirth;
extern NSString *kBDPeopleGroup;

//record atributes
extern NSString *kBDRecordID;
extern NSString *kBDRecordText;
extern NSString *kBDRecordImage;
extern NSString *kBDRecordState;

@interface BDCoreDataManager (People)

- (NSUInteger)countForEntity:(NSString *)name;

//from和to要根据countForEntity确认好，里面不做保护
- (NSArray *)entity:(NSString *)name fromIndex:(NSUInteger)from toIndex:(NSUInteger)to;

- (BOOL)isGroupExist:(NSString *)name;
- (BDCoreDataGroup *)appendGroup:(NSString *)name;

- (BOOL)isPeopleExist:(NSString *)name;
- (BDCoreDataPeople *)appendPeople:(NSString *)name;

- (BDCoreDataRecord *)appendRecord:(NSDate *)createTime;

@end
