//
//  BDMediaItem.m
//  DevTest
//
//  Created by majie on 12-4-24.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDMediaItem.h"

@implementation BDMediaItem


@synthesize x, y, w, h;
@synthesize type;
@synthesize url;


- (id)initWithString:(NSString *)data
{
    if (self = [super init])
    {
        NSString *p;
        
        //img,x,y,w,h,url
        NSArray  * array= [data componentsSeparatedByString:@","];
        
        p = [array objectAtIndex:0];
        if ([p isEqualToString:@"img"])
        {
            self.type = BDMEDIA_IMG;
        }
        else if ([p isEqualToString:@"video"])
        {
            self.type = BDMEDIA_VIDEO;
        }
        else
        {
            self.type = BDMEDIA_MAX;
        }
        
        p = [array objectAtIndex:1];
        self.x = [p integerValue];
        
        p = [array objectAtIndex:2];
        self.y = [p integerValue];

        p = [array objectAtIndex:3];
        self.w = [p integerValue];

        p = [array objectAtIndex:4];
        self.h = [p integerValue];

        p = [array objectAtIndex:5];
        NSString *tmp = [[NSString alloc] initWithString:p];
        self.url = tmp;
        [tmp release];
    }
    
    return self;
}

- (void)dealloc
{
    if (url != nil)
    {
        [url release];
    }
    
    [super dealloc];
}

@end
