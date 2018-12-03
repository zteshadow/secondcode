//
//  BDToolBarButton+Private.m
//  BaiduBrowser
//
//  Created by Hu Guanqin on 12-5-5.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BDToolBarButton+Private.h"
//#import "BDCoreData.h"

@implementation BDToolBarButton (Private)

- (id)initWithType:(NSInteger)type
{
    self = [super init];
    if (self)
    {
        type_ = type;
    }
    return self;
}

- (void)dealloc
{
    [self  setKeyPath:nil];
    [super dealloc];
}

- (void)setKeyPath:(NSString *)keyPath
{
#if 0 //open for observe some state
    if (keyPath_)
    {
        [[BDAppStateManager sharedInstance] removeObserver:self forKeyPath:keyPath_];
    }
    
    [keyPath_ release];
    keyPath_ = [keyPath copy];
    
    if (keyPath_)
    {
        [[BDAppStateManager sharedInstance] addObserver:self 
                                             forKeyPath:keyPath_ 
                                                options:NSKeyValueObservingOptionNew 
                                                context:nil];
    }
#endif
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    self.enabled = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
}

@end

@implementation BDToolBarDiaryButton

- (id)initWithType:(NSInteger)type
{
    self = [super initWithType:type];
    if (self)
    {
        //[self setKeyPath:kAppStateKeyPathHomePage];
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    self.enabled = ![[change objectForKey:NSKeyValueChangeNewKey] boolValue];
}

@end

@implementation BDToolBarCalendarButton

- (id)initWithType:(NSInteger)type
{
    self = [super initWithType:type];
    if (self)
    {
        //[self setKeyPath:kAppStateKeyPathCanGoBack];
    }
    return self;
}

@end

@implementation BDToolBarPeopleButton

- (id)initWithType:(NSInteger)type
{
    self = [super initWithType:type];
    if (self)
    {
        //[self setKeyPath:kAppStateKeyPathCanGoForward];
    }
    return self;
}

@end

@implementation BDToolBarSettingButton

- (id)initWithType:(NSInteger)type
{
    self = [super initWithType:type];
    if (self)
    {
        //[self setKeyPath:kAppStateKeyPathHomePage];
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    self.enabled = ![[change objectForKey:NSKeyValueChangeNewKey] boolValue];
}

@end
