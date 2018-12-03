//
//  BDFileManager.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-6-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

enum
{
    kCachePathFavoriteRoot,
    kCachePathFavoriteNote,
    kCachePathFavoriteImage,
    kCachePathFavoriteVideo,
    kCachePathFavoriteCache,
    kCachePathDownloadPath,
    kCachePathCount,
};

typedef enum
{
    kBDDefaultGroupImage,
    kBDDefaultGroupCover,

    kBDDefaultManImage,
    kBDDefaultManCover,

    kBDDefaultWomanImage,
    kBDDefaultWomanCover,

    kBDDefaultMax
    
}eBDDefaultType;

typedef enum
{
    kBDCommonHost,
    kBDCommonMax
    
}eBDCommonType;

@interface BDFileManager : NSObject

//默认的男头像，女头像，cover的图片位置等默认数据路径
+ (NSString *)defaultPathForType:(eBDDefaultType)type;

+ (NSString *)commonPathForType:(eBDCommonType)type;

+ (NSString *)cachePath;

+ (NSString *)resourcePath;

+ (NSString *)documentPath;

+ (NSString *)customCachePathWithType:(NSInteger)type;

+ (BOOL)fileExistsAtPath:(NSString *)path;

+ (BOOL)createDirectoryAtPathIfNeeded:(NSString *)path;

+ (BOOL)removeItemAtPath:(NSString *)path;

+ (BOOL)copyItemAtPath:fromPath toPath:toPath;

+ (BOOL)moveItemAtPath:fromPath toPath:toPath;

+ (NSUInteger)fileSizeAtPath:(NSString *)path;

+ (NSString*) screenShotPath;

+ (NSString *)getUIDFilePath;

@end
