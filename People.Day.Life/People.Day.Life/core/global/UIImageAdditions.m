//
//  UIImageAdditions.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "UIImageAdditions.h"
#import "BDCoreGlobalRect.h"
#import "BD_Debug.h"

@interface UIImage (BDUtils)
+ (UIImage *)createCircleMask:(CGFloat)radius;
@end

@implementation UIImage (BDUtils)

- (void)drawCenterInRect:(CGRect)rect
{
    CGSize  size    = self.size;
    CGFloat originX = (NSInteger)(rect.origin.x + (rect.size.width  - size.width)  / 2);
    CGFloat originY = (NSInteger)(rect.origin.y + (rect.size.height - size.height) / 2);
    [self drawInRect:CGRectMake(originX, originY, size.width, size.height)];
}

- (UIImage *)imageInRect:(CGRect)rect
{
    rect = CGRectSizeScale(rect, [UIScreen mainScreen].scale);
    
    NSInteger width  = rect.size.width;
    NSInteger height = rect.size.height;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate (NULL, width, height, 8, width * 4, colorSpace, kCGImageAlphaPremultipliedFirst);
    CGImageRef imageRef1 = CGImageCreateWithImageInRect([self CGImage], rect);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef1);
    CGImageRef imageRef2 = CGBitmapContextCreateImage(context);
    UIImage   *imageRet  = [UIImage imageWithCGImage:imageRef2 scale:[UIScreen mainScreen].scale 
                                         orientation:UIImageOrientationUp];
    CGImageRelease(imageRef1);
    CGImageRelease(imageRef2);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    return imageRet;
}

//sacle = destSize / imageSize
- (UIImage *)imageToScale:(CGFloat)scaleSize
{
    CGSize imageSize = CGSizeMake(self.size.width * scaleSize, self.size.height * scaleSize);
    CGRect imageRect = CGRectMake(0, 0, self.size.width * scaleSize, self.size.height * scaleSize);
    //imageRect = CGRectSizeScale(rect, [UIScreen mainScreen].scale);

    UIGraphicsBeginImageContextWithOptions(imageSize, YES, [UIScreen mainScreen].scale);
    [self drawInRect:imageRect];
    UIImage *scaledImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
#if 0
    NSString *filePath = [BDFileManager customCachePathWithType:kCachePathFavoriteImage];
    NSAssert(filePath != nil, @"%s", __func__);
    //NSLog(@"file path %@", filePath);
    
    filePath = [filePath stringByAppendingPathComponent:@"thumbnail"];
    filePath = [filePath stringByAppendingPathExtension:@"jpg"];

    BOOL result = [UIImageJPEGRepresentation(scaledImage, 1.0/*best*/) writeToFile:filePath atomically:YES];
    NSAssert(result == YES, @"%s", __func__);
#endif
    
    return scaledImage;
}

- (UIImage *)imageToFitSize:(CGSize)fitSize
{
    CGFloat hScale = fitSize.width / self.size.width;
    CGFloat vScale = fitSize.height / self.size.height;
    
    UIImage *scaleImage = [self imageToScale:MAX(hScale, vScale)];
    
    CGRect capRect = CGRectMake(0, 0, fitSize.width, fitSize.height);
    return [scaleImage imageInRect:capRect];
}

+ (UIImage *)createCircleMask:(CGFloat)radius
{
    CGFloat scale = [UIScreen mainScreen].scale > 1.0 ? 2.0 : 1.0;
    radius *= scale;
    CGRect rect = CGRectMake(0.0, 0.0, radius, radius);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    CGContextRef context = CGBitmapContextCreate(NULL, radius, radius, 8, (4 * radius), colorSpace, kCGImageAlphaNone);
    
    //white background
    CGContextSetGrayFillColor(context, 255.0, 1.0);
    CGContextAddRect(context,rect);
    CGContextFillPath(context);

    //black circle
    CGContextSetGrayFillColor(context, 0.0, 1.0);
    CGContextAddEllipseInRect(context, rect);
    CGContextFillPath(context);
    
    CGImageRef imageRef = CGBitmapContextCreateImage(context);
    UIImage *imageRet = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
    CGImageRelease(imageRef);
    
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    
    return imageRet;
}

- (UIImage *)imageInCircle:(CGFloat)radius
{
    UIImage *maskImage = [UIImage createCircleMask:radius];
    BD_ASSERT(maskImage != nil);
    
    UIImage *value = [self imageWithMask:maskImage];
    BD_ASSERT(value != nil);
    
    return value;
}

-(UIImage*)imageWithMask:(UIImage*)maskImage
{
    CGImageRef maskRef = maskImage.CGImage;
    
#if 0 //for debug
	NSLog(@">>>%@", CGImageGetColorSpace(maskImage.CGImage));
	NSLog(@">>> grayImage alpha=%d width=%zd height=%zd bitsPerCom=%zd bitsPerPixel=%zd bytesPerRow=%zd  ",CGImageGetAlphaInfo(maskRef),CGImageGetWidth(maskRef),
		  CGImageGetHeight(maskRef),
		  CGImageGetBitsPerComponent(maskRef),
		  CGImageGetBitsPerPixel(maskRef),
		  CGImageGetBytesPerRow(maskRef)
		  );
#endif
    
    CGImageRef mask = CGImageMaskCreate(CGImageGetWidth(maskRef),
                                        CGImageGetHeight(maskRef),
                                        CGImageGetBitsPerComponent(maskRef),
                                        CGImageGetBitsPerPixel(maskRef),
                                        CGImageGetBytesPerRow(maskRef),
                                        CGImageGetDataProvider(maskRef), NULL, true);
    
    maskRef = mask;
    
#if 0//for debug
	NSLog(@">>>%@", CGImageGetColorSpace(mask));
	NSLog(@">>> grayImage alpha=%d width=%zd height=%zd bitsPerCom=%zd bitsPerPixel=%zd bytesPerRow=%zd  ",CGImageGetAlphaInfo(maskRef),CGImageGetWidth(maskRef),
		  CGImageGetHeight(maskRef),
		  CGImageGetBitsPerComponent(maskRef),
		  CGImageGetBitsPerPixel(maskRef),
		  CGImageGetBytesPerRow(maskRef)
		  );
#endif
    
	CGImageRef masked = CGImageCreateWithMask(self.CGImage, mask);
	CGImageRelease(mask);
	
	UIImage* retImage = [UIImage imageWithCGImage:masked];
    CGImageRelease(masked);
    return retImage;
}

#if 0
+(UIImage *)grayImage:(UIImage *)source
{
    
    NSInteger scale = BISkinIsRetina() ? 2 : 1;
    CGSize size  = source.size;
    
    if (scale > 1)
    {
        size = CGSizeScale(size, scale);
    }
    
    int width = size.width;
    int height = size.height;
    
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    CGContextRef context = CGBitmapContextCreate (nil,
                                                  width,
                                                  height,
                                                  8,      // bits per component
                                                  0,
                                                  colorSpace,
                                                  kCGImageAlphaNone);
    CGColorSpaceRelease(colorSpace);
    if (context == NULL)
    {
        return nil;
    }
    CGContextDrawImage(context,
                       CGRectMake(0, 0, width, height), source.CGImage);
    CGImageRef imageRef = CGBitmapContextCreateImage(context);
    UIImage *grayImage = [UIImage imageWithCGImage: imageRef scale:scale orientation:UIImageOrientationUp];
    CGImageRelease(imageRef);
    CGContextRelease(context);
    return grayImage;
}

+ (UIImage*)imageWithBackground:(UIColor*)backColor size:(CGSize) size
{
    NSInteger scale = BISkinIsRetina() ? 2 : 1;
    if (scale > 1)
    {
        size = CGSizeScale(size, scale);
    }
    
    backColor = backColor==nil?[UIColor clearColor]:backColor;
    CGColorRef colorRef = [backColor CGColor];
    const CGFloat *components = CGColorGetComponents(colorRef);
    
    NSInteger width  = size.width;
	NSInteger height = size.height;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(NULL, width, height, 8, (4 * width), colorSpace, kCGImageAlphaPremultipliedFirst);
    CGContextSetRGBFillColor(context, components[0],components[1],components[2], components[3]);
    CGRect rect1 = CGRectMake(0,0,size.width,size.height);
    CGContextAddRect(context,rect1);
    CGContextFillPath(context);
    
    CGImageRef imageRef = CGBitmapContextCreateImage(context);
    UIImage   *imageRet = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
	CGImageRelease(imageRef);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    return  imageRet;
}

#endif

@end
