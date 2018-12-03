//
//  BDResourceParser.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDResourceDecoder.h"

@interface BDResourceParser : NSObject {
    id<BDResourceDecoding> decoder_;
}

- (id)initWithDecoder:(id)decoder;

- (BDResourceTheme *)parse:(NSString *)file;

@end
