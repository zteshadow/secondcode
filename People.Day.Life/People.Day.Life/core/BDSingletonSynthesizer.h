//
//  BDSingletonSynthesizer.h
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-4-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef SYNTHESIZE_SINGLETON_FOR_CLASS

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

#define SYNTHESIZE_SINGLETON_FOR_CLASS_HEADER(SINGLETONCLASSNAME)	\
+ (SINGLETONCLASSNAME*)sharedInstance;	\
+ (void)purgeSharedInstance;

#define SYNTHESIZE_SINGLETON_FOR_CLASS_PROTOTYPE(SINGLETONCLASSNAME) \
@interface SINGLETONCLASSNAME (SingletonSynthesizer)	\
- (NSUInteger)retainCountDoNothing;	\
- (NSUInteger)retainCountDoSomething;	\
- (void)releaseDoNothing;	\
- (void)releaseDoSomething;	\
- (id)autoreleaseDoNothing;	\
- (id)autoreleaseDoSomething; \
@end

#define SYNTHESIZE_SINGLETON_FOR_CLASS(SINGLETONCLASSNAME)	\
\
static volatile SINGLETONCLASSNAME* sharedInstance##SINGLETONCLASSNAME = nil;	\
\
+ (SINGLETONCLASSNAME*)sharedInstanceNoSynch	\
{	\
return (SINGLETONCLASSNAME*)sharedInstance##SINGLETONCLASSNAME;	\
}	\
\
+ (SINGLETONCLASSNAME*)sharedInstanceSynch	\
{	\
@synchronized(self)	\
{	\
if(nil == sharedInstance##SINGLETONCLASSNAME)	\
{	\
sharedInstance##SINGLETONCLASSNAME = [[self alloc] init];	\
}	\
}	\
return (SINGLETONCLASSNAME*)sharedInstance##SINGLETONCLASSNAME;	\
}	\
\
+ (SINGLETONCLASSNAME*)sharedInstance	\
{	\
return [self sharedInstanceSynch]; \
}	\
\
+ (id)allocWithZone:(NSZone*) zone	\
{	\
@synchronized(self)	\
{	\
if (nil == sharedInstance##SINGLETONCLASSNAME)	\
{	\
sharedInstance##SINGLETONCLASSNAME = [super allocWithZone:zone];	\
if(nil != sharedInstance##SINGLETONCLASSNAME)	\
{	\
Method newSharedInstanceMethod = class_getClassMethod(self, @selector(sharedInstanceNoSynch));	\
method_setImplementation(class_getClassMethod(self, @selector(sharedInstance)), method_getImplementation(newSharedInstanceMethod));	\
method_setImplementation(class_getInstanceMethod(self, @selector(retainCount)), class_getMethodImplementation(self, @selector(retainCountDoNothing)));	\
method_setImplementation(class_getInstanceMethod(self, @selector(release)), class_getMethodImplementation(self, @selector(releaseDoNothing)));	\
method_setImplementation(class_getInstanceMethod(self, @selector(autorelease)), class_getMethodImplementation(self, @selector(autoreleaseDoNothing)));	\
}	\
}	\
}	\
return (id)sharedInstance##SINGLETONCLASSNAME;	\
}	\
\
+ (void)purgeSharedInstance	\
{	\
@synchronized(self)	\
{	\
if(nil != sharedInstance##SINGLETONCLASSNAME)	\
{	\
Method newSharedInstanceMethod = class_getClassMethod(self, @selector(sharedInstanceSynch));	\
method_setImplementation(class_getClassMethod(self, @selector(sharedInstance)), method_getImplementation(newSharedInstanceMethod));	\
method_setImplementation(class_getInstanceMethod(self, @selector(retainCount)), class_getMethodImplementation(self, @selector(retainCountDoSomething)));	\
method_setImplementation(class_getInstanceMethod(self, @selector(release)), class_getMethodImplementation(self, @selector(releaseDoSomething)));	\
method_setImplementation(class_getInstanceMethod(self, @selector(autorelease)), class_getMethodImplementation(self, @selector(autoreleaseDoSomething)));	\
[sharedInstance##SINGLETONCLASSNAME release];	\
sharedInstance##SINGLETONCLASSNAME = nil;	\
}	\
}	\
}	\
\
- (id)copyWithZone:(NSZone *)zone	\
{	\
return self;	\
}	\
\
- (id)retain	\
{	\
return self;	\
}	\
\
- (NSUInteger)retainCount	\
{	\
NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(NSUInteger)retainCount method did not get swizzled.", self);	\
return NSUIntegerMax;	\
}	\
\
- (NSUInteger)retainCountDoNothing	\
{	\
return NSUIntegerMax;	\
}	\
- (NSUInteger)retainCountDoSomething	\
{	\
return [super retainCount];	\
}	\
\
- (oneway void)release	\
{	\
NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(void)release method did not get swizzled.", self);	\
}	\
\
- (void)releaseDoNothing{}	\
\
- (void)releaseDoSomething	\
{	\
@synchronized(self)	\
{	\
[super release];	\
}	\
}	\
\
- (id)autorelease	\
{	\
NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(id)autorelease method did not get swizzled.", self);	\
return self;	\
}	\
\
- (id)autoreleaseDoNothing	\
{	\
return self;	\
}	\
\
- (id)autoreleaseDoSomething	\
{	\
return [super autorelease];	\
}

#endif
