//
//  BDCoreDataRecord.m
//  People.Day.Life
//
//  Created by samuel on 13-1-20.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDCoreDataRecord.h"
#import "BDCoreDataPeopleRecord.h"
#import "BDDataRecord.h"

@implementation BDCoreDataRecord

@dynamic id;
@dynamic image;
@dynamic state;
@dynamic text;
@dynamic peoples;

- (BDDataRecord *)makeData
{
    BDDataRecord *record = [[BDDataRecord alloc] init];
    
    record.text = self.text;
    
    return [record autorelease];
}

@end
