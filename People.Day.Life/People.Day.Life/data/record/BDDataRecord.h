//
//  BDDataRecord.h
//  People.Day.Life
//
//  Created by samuel on 13-1-6.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BDDataRecord : NSObject
{
    NSString *text_;
    NSString *image_;
    NSDate *birth_;
}

@property (nonatomic, retain) NSString *text;
@property (nonatomic, retain) NSString *image;
@property (nonatomic, retain) NSDate *birth;

- (BOOL)addPeople:(NSString *)name;
- (BOOL)removePeople:(NSString *)name;
- (BOOL)synchronize;

@end
