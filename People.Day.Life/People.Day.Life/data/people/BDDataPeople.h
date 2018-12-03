//
//  BDDataPeople.h
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BDDataPeople : NSObject
{
    NSString *name_;
    NSDate *birth_;
    UIImage *image_;
    UIImage *cover_;
}

@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSDate *birth;
@property (nonatomic, retain) UIImage *image;
@property (nonatomic, retain) UIImage *cover;

@end
