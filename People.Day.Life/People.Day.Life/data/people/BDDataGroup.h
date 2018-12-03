//
//  BDDataGroup.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BDDataPeople;

@interface BDDataGroup : NSObject
{
    NSString *name_;
    NSString *image_;
}

@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *image;

- (NSUInteger)peopleCount;
- (BDDataPeople *)peopleAtIndex:(NSUInteger)index;
- (void)removePeopleAtIndex:(NSUInteger)index;

- (NSUInteger)recordsOfThisWeek;
- (NSUInteger)recordsOfThisMonth;

- (void)addPeopleByName:(NSString *)name;

- (BOOL)save;

@end
