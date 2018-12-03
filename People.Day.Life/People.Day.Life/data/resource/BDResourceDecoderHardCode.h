//
//  BDResourceDecoderHardCode.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceDecoding.h"

extern NSString *kHardCodeResourceDecoderFilePort; //横屏标记
extern NSString *kHardCodeResourceDecoderFileLand; //竖屏标记

@interface BDResourceDecoderHardCode : NSObject <BDResourceDecoding>
{
    BOOL landscape_;
}

@end
