//
//  BDFileManager.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-6-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDFileManager.h"
#import "BD_Debug.h"

@implementation BDFileManager

+ (NSString *)defaultPathForType:(eBDDefaultType)type
{
    NSString *value = nil;
    
    NSString *defaultImageDir = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"resource/default/"];
    
    switch (type)
    {
    case kBDDefaultGroupImage:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"group_image.jpg"];
    }
    break;
        
    case kBDDefaultGroupCover:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"group_cover.jpg"];
    }
    break;

    case kBDDefaultManCover:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"man_cover.jpg"];
    }
    break;
    
    case kBDDefaultManImage:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"man_image.jpg"];
    }
    break;

    case kBDDefaultWomanCover:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"woman_cover.jpg"];
    }
    break;
    
    case kBDDefaultWomanImage:
    {
        value = [defaultImageDir stringByAppendingPathComponent:@"woman_image.jpg"];
    }
    break;

    default:
        BD_ASSERT(0);
    break;
    }
    
    return value;
}

+ (NSString *)commonPathForType:(eBDCommonType)type
{
    NSString *path = nil;
    
    switch (type)
    {
    case kBDCommonHost:
    {
        path = [[BDFileManager documentPath] stringByAppendingPathComponent:@"host.plist"];
    }
    break;
            
    default:
        BD_ASSERT(0);
    break;
    }
    
    return path;
}

+ (NSString *)cachePath
{
    static NSString *path = nil;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        path = [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0] retain];
    });
    return path;
}

+ (NSString *)resourcePath
{
    static NSString *path = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        path = [[[NSBundle mainBundle] resourcePath] retain];
    });
    return path;
}

+ (NSString *)documentPath
{
    static NSString *path = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        path = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] retain];
    });
    return path;
}

+ (NSString *)customCachePathWithType:(NSInteger)type
{
    switch (type)
    {
        case kCachePathFavoriteRoot:
        {
            static NSString *rootPath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                rootPath = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"Favorite"] retain];
            });
            return rootPath;            
        }
        break;
        
        case kCachePathFavoriteImage:
        {
            static NSString *imagePath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                imagePath = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"Favorite/Image"] retain];
            });
            return imagePath;
        }
        break;
            
        case kCachePathFavoriteVideo:
        {
            static NSString *videoPath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                videoPath = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"Favorite/Video"] retain];
            });
            return videoPath;          
        }
        break;
            
        case kCachePathFavoriteNote:
        {
            static NSString *notePath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                notePath = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"Favorite/Notes"] retain];
            });
            return notePath;            
        }
        break;
            
        case kCachePathFavoriteCache:
        {
            static NSString *cachePath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                cachePath = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"Favorite/Cache"] retain];
            });
            return cachePath;
        }
        break;
            
        case kCachePathDownloadPath:
        {
            static NSString *cachePath = nil;
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                cachePath = [[[BDFileManager cachePath] stringByAppendingPathComponent:@"Download"] retain];
            });
            return cachePath;
        }
            break;

        default: NSAssert(NO, @"%i", type); break;
    }
    return nil;
}

+ (BOOL)fileExistsAtPath:(NSString *)path
{
    return [[NSFileManager defaultManager] fileExistsAtPath:path];
}

+ (BOOL)createDirectoryAtPathIfNeeded:(NSString *)path
{
    BOOL success = YES;
    if (NO == [BDFileManager fileExistsAtPath:path])
    {
        success = [[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil]; 
        //NSAssert(success == YES, @"%s", __func__);
    }
    return success;
}

+ (BOOL)removeItemAtPath:(NSString *)path
{
    BOOL success = YES;
    if ([BDFileManager fileExistsAtPath:path])
    {
        success = [[NSFileManager defaultManager] removeItemAtPath:path error:nil]; 
        //NSAssert(success == YES, @"%s", __func__);
    }
    return success;
}

+ (BOOL)copyItemAtPath:fromPath toPath:toPath
{
    BOOL success = NO;
    if ([BDFileManager fileExistsAtPath:fromPath])
    {
        success = [[NSFileManager defaultManager] copyItemAtPath:fromPath toPath:toPath error:nil]; 
        //NSAssert(success == YES, @"%s", __func__);
    }
    return success;
}

+ (BOOL)moveItemAtPath:fromPath toPath:toPath
{
    BOOL success = NO;
    if ([BDFileManager fileExistsAtPath:fromPath])
    {
        success = [[NSFileManager defaultManager] moveItemAtPath:fromPath toPath:toPath error:nil]; 
        //NSAssert(success == YES, @"%s", __func__);
    }
    return success;
}

+ (NSUInteger)fileSizeAtPath:(NSString *)path
{
    if ([BDFileManager fileExistsAtPath:path])
    {
        NSDictionary *fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil];
        return [[fileAttributes objectForKey:NSFileSize] unsignedLongValue];
    }
    return 0;
}

+ (NSString*) screenShotPath
{
    NSString* base = [BDFileManager customCachePathWithType:kCachePathFavoriteImage];
    NSString* path = nil;
    int i = 1;
    do 
    {
        path = [base stringByAppendingFormat:@"/ScreenShot%.4d.png", i];
        i++;
    } while ([BDFileManager fileExistsAtPath:path]);
    return path;
}

+ (NSString *)getUIDFilePath
{    
    static NSString *path = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        path = [[[BDFileManager documentPath] stringByAppendingPathComponent:@"uid.dat"] retain];
    });
    return path;
}
@end
