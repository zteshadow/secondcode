//
//  BDDataRecord.m
//  People.Day.Life
//
//  Created by samuel on 13-1-6.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDDataRecord.h"

@interface BDDataRecord()
{
    NSString *name_;
    NSMutableArray *people_;
}
@end

@implementation BDDataRecord

@synthesize text = text_;
@synthesize image = image_;
@synthesize birth = birth_;

- (id)init
{
    if (self = [super init])
    {
        NSDate *dateTime = [NSDate date];
        
        self.birth = dateTime;
        name_ = [[self createNameByDate:dateTime] retain];
    }
    
    return self;
}

- (void)dealloc
{
    [text_ release];
    [image_ release];
    [birth_ release];
    
    [name_ release];
    [people_ release];
    
    [super dealloc];
}

- (NSString *)createNameByDate:(NSDate *)date
{
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
    
    NSInteger unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit |
                          NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit;
    
    NSDateComponents *comps  = [calendar components:unitFlags fromDate:date];
    
    NSInteger year = [comps year];
    NSInteger month = [comps month];
    NSInteger day = [comps day];
    NSInteger hour = [comps hour];
    NSInteger min = [comps minute];
    NSInteger sec = [comps second];
    NSString *name = [NSString stringWithFormat:@"%d-%d-%d-%d-%d-%d", year, month, day, hour, min, sec];
    [calendar release];

    return name;
}

- (BOOL)addPeople:(NSString *)name
{
    [people_ addObject:name];
    return YES;
}

- (BOOL)removePeople:(NSString *)name
{
    [people_ removeObject:name];
    return YES;
}

- (BOOL)synchronize
{
    return YES;
}

@end
