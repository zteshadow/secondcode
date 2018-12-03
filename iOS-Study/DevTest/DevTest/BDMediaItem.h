//
//  BDMediaItem.h
//  DevTest
//
//  Created by majie on 12-4-24.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


typedef enum
{
    BDMEDIA_IMG,
    BDMEDIA_VIDEO,
    BDMEDIA_MAX
    
} BDMEDIA_TYPE;


@interface BDMediaItem : NSObject
{
    BDMEDIA_TYPE type;
    NSInteger x, y, w, h;
    NSString *url;
}

@property (nonatomic) NSInteger x;
@property (nonatomic) NSInteger y;
@property (nonatomic) NSInteger w;
@property (nonatomic) NSInteger h;
@property (nonatomic) BDMEDIA_TYPE type;

@property (nonatomic, retain) NSString *url;

- (id)initWithString:(NSString *)data;

@end
