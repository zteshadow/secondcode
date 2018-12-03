//
//  BDDataManager.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013Âπ?samuel. All rights reserved.
//

#import "BDDataManager.h"
#import "BDCoreDataManager+Private.h"
#import "BDCoreDataManager+People.h"
#import "BDFileManager.h"
#import "BDDataGroup.h"
#import "BDDataGroup+Private.h"
#import "BDDataPeople.h"
#import "NSDateAdditions.h"
#import "BD_Debug.h"

static BDDataManager *s_dataManager = nil;

@interface BDDataManager()
{
    BDCoreDataManager *coreDataManager_;
    
    NSUInteger groupCount_;
    NSMutableArray *groupData_;
    
    NSUInteger recordCount_;
    NSMutableArray *recordData_;
}

@property (nonatomic, retain) NSMutableArray *groupData;
@property (nonatomic, retain) NSMutableArray *recordData;

- (void)refreshGroupData;
- (void)refreshRecordData;

@end

@implementation BDDataManager

@synthesize groupData = groupData_;
@synthesize recordData = recordData_;

+ (void)initSharedInstance
{
    BD_ASSERT(s_dataManager == nil);
    s_dataManager = [[BDDataManager alloc] init];
}

+ (void)releaseSharedInstance
{
    BD_ASSERT(s_dataManager != nil);
    
    [s_dataManager release];
    s_dataManager = nil;
}

+ (BDDataManager *)sharedInstance
{
    BD_ASSERT(s_dataManager != nil);
    
    return s_dataManager;
}

- (id)init
{
    if (self = [super init])
    {
        coreDataManager_ = [[BDCoreDataManager alloc] init];
    }
    
    return self;
}

- (void)dealloc
{
    [groupData_ release];
    [recordData_ release];
    
    [coreDataManager_ release];

    [super dealloc];
}

- (void)refreshGroupData
{
    groupCount_ = [coreDataManager_ countForEntity:kBDEntityGroup];
    NSArray *groups = [[coreDataManager_ entity:kBDEntityGroup fromIndex:0 toIndex:groupCount_ - 1] retain];
    self.groupData = [NSMutableArray arrayWithArray:groups];
}

- (void)refreshRecordData
{
    recordCount_ = [coreDataManager_ countForEntity:kBDEntityRecord];
    NSArray *records = [[coreDataManager_ entity:kBDEntityRecord fromIndex:0 toIndex:groupCount_ - 1] retain];
    self.recordData = [NSMutableArray arrayWithArray:records];
}

- (NSUInteger)groupCount
{
    if (groupCount_ == 0)
    {
        [self refreshGroupData];
    }
    
    return groupCount_;
}

- (BDDataGroup *)groupAtIndex:(NSUInteger)index
{
    BD_ASSERT(groupData_ != nil);
    BD_ASSERT(index < [coreDataManager_ countForEntity:kBDEntityGroup]);
    
    BDCoreDataGroup *coreDataGroup = (BDCoreDataGroup *)[groupData_ objectAtIndex:index];
    
    BDDataGroup *group = [coreDataGroup makeDataGroup];
    group.manager = coreDataManager_;
    
    return group;
}

- (void)removeGroupAtIndex:(NSUInteger)index
{
    BD_ASSERT(groupData_ != nil);
    BD_ASSERT(index < [coreDataManager_ countForEntity:kBDEntityGroup]);
    
    BDCoreDataGroup *g = (BDCoreDataGroup *)[groupData_ objectAtIndex:index];
    if (g != nil)
    {
        [coreDataManager_ deleteObject:g];
        [coreDataManager_ saveContext];
        [self refreshGroupData];
    }
}

- (eBDDataError)addGroupByName:(NSString *)name
{    
    eBDDataError error = BDDAtaErrorNone;
    
    if (![coreDataManager_ isGroupExist:name])
    {
        BDCoreDataGroup *group = [coreDataManager_ appendGroup:name];
        [group setValue:[BDFileManager defaultPathForType:kBDDefaultGroupCover] forKey:kBDGroupCover];
        [group setValue:[BDFileManager defaultPathForType:kBDDefaultGroupImage] forKey:kBDGroupImage];
        [group setValue:0 forKey:kBDGroupState];
        
        [coreDataManager_ saveContext];
        [self refreshGroupData];
    }
    else
    {
        error = BDDataErrorDuplicated;
    }
    
    return error;
}

- (NSUInteger)recordCount
{
    if (recordCount_ == 0)
    {
        [self refreshRecordData];
    }
    
    return recordCount_;
}

- (BDDataRecord *)recordAtIndex:(NSUInteger)index
{
    BD_ASSERT(recordData_ != nil);
    BD_ASSERT(index < [coreDataManager_ countForEntity:kBDEntityRecord]);
    
    BDCoreDataRecord *coreDataRecord = (BDCoreDataRecord *)[recordData_ objectAtIndex:index];
    
    BDDataRecord *record = [coreDataRecord makeData];
    
    return record;
}

- (void)removeRecordAtIndex:(NSUInteger)index
{
    
}

- (eBDDataError)addRecord:(BDDataRecord *)record
{
    return BDDataError;
}

- (NSString *)errorMessageFromCode:(eBDDataError)code
{
    return @"错误";
}

//如果group表中没有数据，则初始化group，people，以及第一个record数据
- (void)resetDefaultDataIfNeeded
{
    NSUInteger count = [coreDataManager_ countForEntity:kBDEntityGroup];
    
    if (count == 0)
    {
        //default family
        BDCoreDataGroup *family = [coreDataManager_ appendGroup:@"家庭"];
        [family setValue:[BDFileManager defaultPathForType:kBDDefaultGroupCover] forKey:kBDGroupCover];
        [family setValue:[BDFileManager defaultPathForType:kBDDefaultGroupImage] forKey:kBDGroupImage];
        [family setValue:0 forKey:kBDGroupState];

        //father
        BDCoreDataPeople *father = [coreDataManager_ appendPeople:@"父亲"];
        [father setValue:[BDFileManager defaultPathForType:kBDDefaultManCover] forKey:kBDPeopleCover];
        [father setValue:[BDFileManager defaultPathForType:kBDDefaultManImage] forKey:kBDPeopleImage];
        [father setValue:0 forKey:kBDPeopleState];
        
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd"];
        [father setValue:[dateFormatter dateFromString:@"1957-04-18"] forKey:kBDPeopleBirth];
        [dateFormatter release];
        
        [father setValue:family forKey:kBDPeopleGroup];
        
        //mother
        BDCoreDataPeople *mother = [coreDataManager_ appendPeople:@"母亲"];
        [mother setValue:[BDFileManager defaultPathForType:kBDDefaultWomanCover] forKey:kBDPeopleCover];
        [mother setValue:[BDFileManager defaultPathForType:kBDDefaultWomanImage] forKey:kBDPeopleImage];
        [mother setValue:0 forKey:kBDPeopleState];
        
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd"];
        [mother setValue:[dateFormatter dateFromString:@"1954-01-09"] forKey:kBDPeopleBirth];
        [mother setValue:family forKey:kBDPeopleGroup];

        NSSet *familyMembers = [NSSet setWithObjects:father, mother,nil];
        [family setValue:familyMembers forKey:kBDGroupPeoples];
        
        //add friend group
        BDCoreDataGroup *friends = [coreDataManager_ appendGroup:@"朋友"];
        [friends setValue:[BDFileManager defaultPathForType:kBDDefaultGroupCover] forKey:kBDGroupCover];
        [friends setValue:[BDFileManager defaultPathForType:kBDDefaultGroupImage] forKey:kBDGroupImage];
        [friends setValue:0 forKey:kBDGroupState];

        //add colleague
        BDCoreDataGroup *colleagues = [coreDataManager_ appendGroup:@"同事"];
        [colleagues setValue:[BDFileManager defaultPathForType:kBDDefaultGroupCover] forKey:kBDGroupCover];
        [colleagues setValue:[BDFileManager defaultPathForType:kBDDefaultGroupImage] forKey:kBDGroupImage];
        [colleagues setValue:0 forKey:kBDGroupState];

        [coreDataManager_ saveContext];
    }
    
    //add record test
    count = [coreDataManager_ countForEntity:kBDEntityRecord];
    if (count == 0)
    {
        NSDate *currentTime = [NSDate date];
        
        BDCoreDataRecord *record = [coreDataManager_ appendRecord:currentTime];
        [record setValue:@"hello, world" forKey:kBDRecordText];
        
        currentTime = [NSDate dateWithTimeInterval:1 sinceDate:currentTime];
        record = [coreDataManager_ appendRecord:currentTime];
        [record setValue:@"大宝，臭蛋，我爱你们, 大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们,大宝，臭蛋，我爱你们" forKey:kBDRecordText];
        
        currentTime = [NSDate dateWithTimeInterval:1 sinceDate:currentTime];
        record = [coreDataManager_ appendRecord:currentTime];
        [record setValue:@"爸爸，妈妈，我爱你们" forKey:kBDRecordText];

        currentTime = [NSDate dateWithTimeInterval:1 sinceDate:currentTime];
        record = [coreDataManager_ appendRecord:currentTime];
        [record setValue:@"小妹，sky，菡菡，我爱你们" forKey:kBDRecordText];
        
        [coreDataManager_ saveContext];
    }
}

- (BDDataPeople *)hostData
{
    NSString *path = [BDFileManager commonPathForType:kBDCommonHost];
    BD_ASSERT(path != nil);
    
    NSDictionary *hostDict = [NSDictionary dictionaryWithContentsOfFile:path];
    BD_ASSERT(hostDict != nil);
    
    BDDataPeople *host = [[BDDataPeople alloc] init];
    
    host.name = [hostDict valueForKey:@"name"];
    
    NSString *filePath = [hostDict valueForKey:@"image"];
    UIImage *image = [UIImage imageWithContentsOfFile:filePath];
    host.image = image;
    
    filePath = [hostDict valueForKey:@"cover"];
    image = [UIImage imageWithContentsOfFile:filePath];
    host.cover = image;
    
    NSString *birthString = [hostDict valueForKey:@"birth"];
    NSDate *birth = [NSDate dateFromString:birthString withFormat:@"yyyy-MM-dd"];
    host.birth = birth;
        
    return [host autorelease];
}

- (void)setHostData:(BDDataPeople *)hostData
{
    
}

@end
