
#import "BD_Debug.h"
#import <objc/runtime.h>
#import <QuartzCore/QuartzCore.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _BD_DEBUG_
void bd_assert(int flag, const char *file, unsigned int line)
{
    if (0 == flag)
    {
        bd_trace(@"killed by: %s@%d", file, line);
        assert(0);
    }
}

void bd_trace(NSString *fmt, ...)
{
    char buf[201];
    const char *format = [fmt UTF8String];
    
	va_list ap;
	va_start(ap, format);
    
	vsnprintf(buf, 200, format, ap);         /*DO NOT MORE THAN 200*/
	//fprintf(stderr, "%s", buf);
    NSLog(@"%s", buf);
	
	va_end(ap);
}

void bd_show_view_hierarchy(UIView *view, NSInteger level)
{
    NSMutableString *indent = [NSMutableString string];
    for (NSInteger i = 0; i < level; i++)
    {
        [indent appendString:@"    "];
    }
    
    NSLog(@"%@%@", indent, [view description]);
    
    for (UIView *item in view.subviews)
    {
        bd_show_view_hierarchy(item, level + 1);
    }
}

void bd_show_stack(void)
{
    NSLog(@"%@", [NSThread callStackSymbols]);
}

double bd_get_current_time(void)
{
    CFAbsoluteTime time = CFAbsoluteTimeGetCurrent();
    return (double)time;
}

void bd_show_size(CGSize size)
{
    NSLog(@"size: %@", NSStringFromCGSize(size));
}

void bd_show_rect(CGRect rect)
{
    NSLog(@"rect: %@", NSStringFromCGRect(rect));
}

#if 0
@interface UIView(Border)
@end

@implementation UIView(Border)

- (id)swizzled_initWithFrame:(CGRect)frame
{
    //    NSLog(@"===> ChangeFrame");
    // This is the confusing part (article explains this line).
    id result = [self swizzled_initWithFrame:frame];
    //Class cls = NSClassFromString(@"BI_PanelViewSDK");
    // Safe guard: do we have an UIView (or something that has a layer)?
    //if ([result respondsToSelector:@selector(layer)] && [self isKindOfClass:NSClassFromString(@"UIPeripheralHostView")])
        //if ([result respondsToSelector:@selector(layer)])
    {
        // Get layer for this view.
        CALayer *layer = [result layer];
        // Set border on layer.
        layer.borderWidth = 1;
        layer.borderColor = [[UIColor redColor] CGColor];
    }
    
    // Return the modified view.
    return result;
}

- (id)swizzled_initWithCoder:(NSCoder *)aDecoder
{
    // This is the confusing part (article explains this line).
    id result = [self swizzled_initWithCoder:aDecoder];
    //Class cls = NSClassFromString(@"BI_PanelViewSDK");
    // Safe guard: do we have an UIView (or something that has a layer)?
    //if ([result respondsToSelector:@selector(layer)] && [self isKindOfClass:cls])
    //if ([result respondsToSelector:@selector(layer)] && [self isKindOfClass:NSClassFromString(@"UIPeripheralHostView")])
        //if ([result respondsToSelector:@selector(layer)])
    {
        // Get layer for this view.
        CALayer *layer = [result layer];
        // Set border on layer.
        layer.borderWidth = 1;
        layer.borderColor = [[UIColor redColor] CGColor];
    }
    
    return result;
}

- (void)swizzled_setFrame:(CGRect)frame
{
    [self swizzled_setFrame:frame];
    
#if 0
    if ([self isKindOfClass:NSClassFromString(@"UITextEffectsWindow")])
    {
        NSLog(@"UITextEffectsWindow set frame: %@-%@", NSStringFromCGRect(frame), [NSThread callStackSymbols]);
    }
#endif
}

+ (void)load
{
    // The "+ load" method is called once, very early in the application life-cycle.
    // It's called even before the "main" function is called. Beware: there's no
    // autorelease pool at this point, so avoid Objective-C calls.
    Method original, swizzle;
    
    original = class_getInstanceMethod(self, @selector(initWithFrame:));
    swizzle = class_getInstanceMethod(self, @selector(swizzled_initWithFrame:));
    method_exchangeImplementations(original, swizzle);
    
    original = class_getInstanceMethod(self, @selector(initWithCoder:));
    swizzle = class_getInstanceMethod(self, @selector(swizzled_initWithCoder:));
    method_exchangeImplementations(original, swizzle);
    
    //setframe
    original = class_getInstanceMethod(self, @selector(setFrame:));
    swizzle = class_getInstanceMethod(self, @selector(swizzled_setFrame:));
    method_exchangeImplementations(original, swizzle);
}

@end

#endif //border

#endif //_BD_DEBUG_
