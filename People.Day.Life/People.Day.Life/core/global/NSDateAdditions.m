//
//  NSDateAdditions.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "NSDateAdditions.h"
#import "BD_Debug.h"

#define kBDSecondsOfADay (60.0 * 60 * 24)

static NSString *s_week_map[]=
{
    nil,
    @"日",
    @"一",
    @"二",
    @"三",
    @"四",
    @"五",
    @"六"
};

@implementation NSDate (BDAdditions)

+ (id)dateWithYear:(NSInteger)y month:(NSInteger)m day:(NSInteger)d
{
    NSDateComponents *birthComponent = [[NSDateComponents alloc]init];
    [birthComponent setMonth:m];
    [birthComponent setDay:d];
    [birthComponent setYear:y];
    
    NSCalendar *birthCalendar = [[NSCalendar alloc]initWithCalendarIdentifier:NSGregorianCalendar];
    
    NSDate *birthDate = [birthCalendar dateFromComponents:birthComponent];
 
    [birthCalendar release];
    [birthComponent release];
    
    return birthDate;
}

+ (id)dateFromString:(NSString *)dateString withFormat:(NSString *)formatString
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:formatString];
    NSDate *date = [formatter dateFromString:dateString];
    [formatter release];

    return date;
}

+ (NSString *)dateString
{
    BDDateStruct dateData = [[NSDate date] dateData];
    
    NSString *dateString = [NSString stringWithFormat:@"%d年%d月%d日星期", dateData.year, dateData.month, dateData.day];
    
    BD_ASSERT(dateData.week < 8);
    dateString = [dateString stringByAppendingString:s_week_map[dateData.week]];
    return dateString;
}

- (NSString *)stringWithFormat:(NSString *)formatter
{
    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
    [dateFormat setDateFormat:formatter];
    NSString *dateString = [dateFormat stringFromDate:self];
    
    return dateString;
}

- (BDDateStruct)yearMonthDayFromDate:(NSDate *)date
{
    NSTimeInterval totalSeconds = [self timeIntervalSinceDate:date];
    
    BDDateStruct dateOffset = {0};
    
    if (totalSeconds > 0)
    {
        NSCalendar *calendar = [[NSCalendar alloc]initWithCalendarIdentifier:NSGregorianCalendar];
        
        NSUInteger units  = NSMonthCalendarUnit|NSDayCalendarUnit|NSYearCalendarUnit;
        NSDateComponents *component = [calendar components:units fromDate:self];
        
        NSInteger endYear = [component year];
        NSInteger endMonth = [component month];
        NSInteger endDay = [component day];
        
        component = [calendar components:units fromDate:date];
        [calendar release];
        
        NSInteger fromYear = [component year];
        NSInteger fromMonth = [component month];
        NSInteger fromDay = [component day];
        
        NSInteger year = endYear - fromYear;
        NSInteger month = endMonth - fromMonth;
        NSInteger day = endDay - fromDay;
        
        if (day < 0)
        {
            month--; //不能确定每个月有几天，只借一个月
        }
        
        if (month < 0)
        {
            month += 12; //一年一定有12个月
            year--;
            BD_ASSERT(year >= 0);
        }
        
        dateOffset.year = year;
        dateOffset.month = month;
        
        if (day < 0)
        {
            NSDate *lastMonthDate = [NSDate dateWithYear:fromYear + year month:fromMonth + month day:fromDay];
            day = [self timeIntervalSinceDate:lastMonthDate] / kBDSecondsOfADay;
            BD_ASSERT(day < 31);
        }
        
        dateOffset.day = day;
    }

    return dateOffset;
}

- (BDDateStruct)dateData
{
    BDDateStruct date = {0};
    
    NSCalendar *calendar = [[NSCalendar alloc]initWithCalendarIdentifier:NSGregorianCalendar];
    
    NSUInteger units  = NSMonthCalendarUnit|NSDayCalendarUnit|NSYearCalendarUnit|NSWeekdayCalendarUnit;
    NSDateComponents *component = [calendar components:units fromDate:self];
    [calendar release];
    
    date.year = [component year];
    date.month = [component month];
    date.day = [component day];
    date.week = [component weekday];
    
    return date;
}

- (NSString *)year
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy"];
    NSString *yearString = [formatter stringFromDate:self];
    [formatter release];
    
    return yearString;
}

- (NSString *)month
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"MM"];
    NSString *yearString = [formatter stringFromDate:self];
    [formatter release];
    
    return yearString;
}

- (NSString *)day
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"dd"];
    NSString *yearString = [formatter stringFromDate:self];
    [formatter release];
    
    return yearString;
}

@end
