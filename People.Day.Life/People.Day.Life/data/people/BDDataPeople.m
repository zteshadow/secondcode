//
//  BDDataPeople.m
//  People.Day.Life
//
//  Created by samuel on 13-1-8.
//  Copyright (c) 2013年 samuel. All rights reserved.
//

#import "BDDataPeople.h"
#import "BDDataPeople+Private.h"
#import "BDCoreDataPeople.h"

@implementation BDDataPeople

@synthesize name = name_;
@synthesize birth = birth_;
@synthesize image = image_;
@synthesize cover = cover_;
@synthesize coreData = coreData_;

- (void)dealloc
{
    [name_ release];
    [birth_ release];
    [image_ release];
    [cover_ release];
    
    [coreData_ release];
    
    [super dealloc];
}

@end
