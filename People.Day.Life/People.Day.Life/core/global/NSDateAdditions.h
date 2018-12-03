//
//  NSDateAdditions.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef struct
{
    NSInteger year;
    NSInteger month;
    NSInteger day;
    
    NSInteger week;
    
} BDDateStruct;

@interface NSDate (BDAdditions)

@property (nonatomic, readonly) NSString *year;
@property (nonatomic, readonly) NSString *month;
@property (nonatomic, readonly) NSString *day;

+ (id)dateWithYear:(NSInteger)y month:(NSInteger)m day:(NSInteger)d;

+ (id)dateFromString:(NSString *)dateString withFormat:(NSString *)formatString;

//年月日星期
+ (NSString *)dateString;

//yyyy-MM-dd
- (NSString *)stringWithFormat:(NSString *)formatter;

//week is not used
- (BDDateStruct)yearMonthDayFromDate:(NSDate *)date;

- (BDDateStruct)dateData;

@end
