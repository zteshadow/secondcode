//
//  BDDataManager.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum
{
    BDDAtaErrorNone,
    BDDataErrorDuplicated,
    BDDataError,
    BDDataErrorMax
    
} eBDDataError;

@class BDDataGroup;
@class BDDataPeople;
@class BDDataRecord;

@interface BDDataManager : NSObject

@property (nonatomic, assign) BDDataPeople *hostData;

+ (void)initSharedInstance;
+ (void)releaseSharedInstance;
+ (BDDataManager *)sharedInstance;

//如果group表中没有数据，则初始化group，people，以及第一个record数据
- (void)resetDefaultDataIfNeeded;

//group
- (NSUInteger)groupCount;
- (BDDataGroup *)groupAtIndex:(NSUInteger)index;
- (void)removeGroupAtIndex:(NSUInteger)index;
- (eBDDataError)addGroupByName:(NSString *)name;

- (NSUInteger)recordCount;
- (BDDataRecord *)recordAtIndex:(NSUInteger)index;
- (void)removeRecordAtIndex:(NSUInteger)index;
- (eBDDataError)addRecord:(BDDataRecord *)record;

- (NSString *)errorMessageFromCode:(eBDDataError)code;

@end
