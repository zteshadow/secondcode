//
//  BDDataGroup.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDDataGroup.h"

@class BDCoreDataGroup;
@class BDCoreDataManager;

@interface BDDataGroup()
{
    NSMutableArray *peopleArray_;
    BDCoreDataGroup *coreData_;
    BDCoreDataManager *manager_;
}

@property (nonatomic, retain) NSMutableArray *peopleArray;
@property (nonatomic, retain) BDCoreDataGroup *coreData;
@property (nonatomic, retain) BDCoreDataManager *manager;

@end
