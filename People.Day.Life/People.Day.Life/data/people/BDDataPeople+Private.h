//
//  BDDataPeople+Private.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BDCoreDataPeople;

@interface BDDataPeople()
{
    BDCoreDataPeople *coreData_;
}

@property (nonatomic, retain) BDCoreDataPeople *coreData;

@end
