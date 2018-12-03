
#include "cm_gdi.h"
#include "cm_lib.h"
#include "cm_debug.h"
#include "cm_string.h"

#include "bible_ios_platform.h"
#import <UIKit/UIFont.h>
#import <UIKit/UIImage.h>
#import <UIKit/UIView.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIStringDrawing.h>
#import <UIKit/UIGraphics.h>


typedef struct
{
    CGContextRef context;
    UIFont *font;
    CM_RECT clip, frame;
    UIView *view;

} CM_GDI_STRUCT;


void *bible_ios_graphic_create(CGContextRef context, UIView *view,
                            S32 x, S32 y, S32 w, S32 h)
{
	CM_GDI_STRUCT *p;
	
	CM_ASSERT(view != 0);

	p = (CM_GDI_STRUCT *)CM_MALLOC(sizeof(CM_GDI_STRUCT));
	if (p)
	{
		cm_memset(p, 0, sizeof(CM_GDI_STRUCT));
		p->context = context;
		p->view = view;
		p->frame.x = x;
		p->frame.y = y;
		p->frame.w = w;
		p->frame.h = h;
	}

	return (void *)p;
}

void bible_ios_graphic_set_context(void *graphic, CGContextRef context)
{
	CM_GDI_STRUCT *p = (CM_GDI_STRUCT *)graphic;
    p->context = context;
}

void bible_ios_graphic_destroy(void *graphic)
{
	CM_GDI_STRUCT *p = (CM_GDI_STRUCT *)graphic;
	
	CM_ASSERT(graphic != 0);
	if (p)
	{
		CM_FREE(p);
	}
}

void cm_gdi_draw_text(void* pfd, const U16* text, S32 len, const CM_POINT* pos)
{
    CM_GDI_STRUCT *p;
    NSString *data;
    UIFont *font;
    
    CM_ASSERT(pfd != 0 && text != 0 && len > 0 && pos != 0);
    
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        data = [[NSString alloc] initWithCharacters:text length: len];
        
        CGPoint point;
        point.x = (CGFloat)(pos->x);
        point.y = (CGFloat)(pos->y);
        
        font = p->font;
        CM_ASSERT(font != 0);
        
        UIGraphicsPushContext(p->context);
        [data drawAtPoint: point withFont: font];
        UIGraphicsPopContext();
        [data release];
    }
}

void cm_gdi_draw_rect(void* pfd, const CM_RECT* rect)
{
    CGRect currentRect;
    CM_GDI_STRUCT *p;
    
    CM_ASSERT(pfd != 0 && rect != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        currentRect = CGRectMake((CGFloat)rect->x, (CGFloat)rect->y, (CGFloat)rect->w, (CGFloat)rect->h);
        
        //CGContextSetLineWidth(context, 1.0);
        
        CM_ASSERT(p->context != 0);
        CGContextStrokeRect(p->context, currentRect);
    }
}

void cm_gdi_fill_rect(void* pfd, const CM_RECT* rect)
{
    CGRect currentRect;
    CM_GDI_STRUCT *p;
    
    CM_ASSERT(pfd != 0 && rect != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        currentRect = CGRectMake((CGFloat)rect->x, (CGFloat)rect->y, (CGFloat)rect->w, (CGFloat)rect->h);

        CM_ASSERT(p->context != 0);
        CGContextFillRect(p->context, currentRect);
    }    
}

void cm_gdi_draw_circle(void* pfd, const CM_RECT* rect)
{
    CGRect currentRect;
    CM_GDI_STRUCT *p;
    
    CM_ASSERT(pfd != 0 && rect != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        currentRect = CGRectMake((CGFloat)rect->x, (CGFloat)rect->y, (CGFloat)rect->w, (CGFloat)rect->h);

        CM_ASSERT(p->context != 0);
        CGContextAddEllipseInRect(p->context, currentRect);
        CGContextDrawPath(p->context, kCGPathFill);
    }
}

void cm_gdi_set_font(void* pfd, CM_FONT id)
{
    CM_GDI_STRUCT *p;
    CM_ASSERT(pfd != 0);
    
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        p->font = (UIFont *)id;
    }
}

CM_FONT cm_gdi_get_font(void* pfd, S16 pixel, CM_BOOL bold, CM_BOOL italic)
{
    UIFont *font;
    
    CM_ASSERT(pfd != 0 && pixel > 0);
    
    font = [UIFont systemFontOfSize:(CGFloat)pixel];
    
    return (CM_FONT)font;
}

S32 cm_gdi_get_font_height(void* pfd, CM_FONT id)
{
    UIFont *font = (UIFont *)id;
    S32 height = 0;
    
    CM_ASSERT(font != 0);
    
    if (font)
    {
        //height = (S32)font.ascender;
        //height = (S32)font.descender;
        //height = (S32)font.lineHeight;
        //height = (S32)font.pointSize;
        //height = (S32)font.xHeight;
        //height = (S32)font.capHeight;
        
        height = (S32)(font.ascender - font.descender);
    }
    
    return height;
}

S32 cm_gdi_get_font_ascent(void* pfd, CM_FONT id)
{
    UIFont *font = (UIFont *)id;
    S32 ascent = 0;
    
    CM_ASSERT(font != 0);

    if (font)
    {
        //ascent = (S32)font.ascender; //15
        //ascent = (S32)font.descender; //-3
        ascent = 2;
    }
    
    return ascent;    
}

void cm_gdi_release_font(void* pfd)
{
}

S32 cm_ged_gdi_char_width(void* pfd, CM_FONT id, const U16 ch)
{
    return cm_gdi_get_text_width(pfd, id, &ch, 1);
}

S32 cm_gdi_get_text_width(void* pfd, CM_FONT id, const U16* text, S32 len)
{
    S32 width = 0;
    CM_GDI_STRUCT *p;
    NSString *data;
    UIFont *font;

    CM_ASSERT(pfd != 0 && text != 0 && len > 0);
    
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        data = [[NSString alloc] initWithCharacters:text length: len];
        
        font = (UIFont *)id;
        CGSize size = [data sizeWithFont: font];
        width = size.width;
        [data release];
    }
    
    CM_ASSERT(width > 0);
    
    return width;
}

void cm_gdi_set_pen_color(void* pfd, const CM_COLOR* color)
{
    CM_GDI_STRUCT *p;
    UIColor *currentColor;
    
    CM_ASSERT(pfd != 0 && color != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        currentColor = [UIColor colorWithRed:color->r / 255.0 green: color->g / 255.0
                                        blue: color->b / 255.0 alpha: color->a / 255.0];

        CGContextSetFillColorWithColor(p->context, currentColor.CGColor);

        //NSString的绘制需要Fill Color
        CGContextSetStrokeColorWithColor(p->context, currentColor.CGColor);
    }
}

void cm_gdi_set_brush_color(void* pfd, const CM_COLOR* color)
{
    CM_GDI_STRUCT *p;
    UIColor *currentColor;
    
    CM_ASSERT(pfd != 0 && color != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        currentColor = [UIColor colorWithRed:color->r / 255.0 green: color->g / 255.0
                                        blue: color->b / 255.0 alpha: color->a / 255.0];
        CGContextSetFillColorWithColor(p->context, currentColor.CGColor);
    }
}

void cm_gdi_set_clip(void* pfd, const CM_RECT* rect)
{
    CM_GDI_STRUCT *p;
    UIView *view;
    CGRect currentRect;
    
    CM_ASSERT(pfd != 0 && rect != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        //p->clip = *rect;
        //view = p->view;
        
        //currentRect = CGRectMake((CGFloat)rect->x, (CGFloat)rect->y, (CGFloat)rect->w, (CGFloat)rect->h);
        //[view setNeedsDisplayInRect:currentRect];
    }
}

void cm_gdi_cancel_clip(void* pfd)
{
    CM_GDI_STRUCT *p;
    UIView *view;
    
    CM_ASSERT(pfd != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        //view = p->view;
        //[view setNeedsDisplay];
    }
}

void cm_gdi_get_view_rect(void* pfd, CM_RECT* view)
{
    CM_GDI_STRUCT *p;
    
    CM_ASSERT(pfd != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        *view = p->frame;
    }
}

CM_IMAGE *cm_image_decode(const void *data, S32 len)
{
    CGImageRef cgImage;
    CM_IMAGE *cmImage = (CM_IMAGE *)CM_MALLOC(sizeof(CM_IMAGE));
    
    if (cmImage != 0)
    {
        NSData *imageData = [[NSData alloc] initWithBytes: data length: len];
        UIImage *image = [[UIImage alloc] initWithData:imageData];
        
        cmImage->user_data = (void *)image;
        cgImage = image.CGImage;
        
        cmImage->width = CGImageGetWidth(cgImage);
        cmImage->height = CGImageGetHeight(cgImage);
        
        [imageData release];
    }
    
    return cmImage;
}

S32 cm_image_draw(void *pfd, S32 x, S32 y, CM_IMAGE *img)
{
    UIImage *image;
    CGPoint point = CGPointMake(x, y);
    
    CM_ASSERT(pfd != 0 && img != 0);
    image = (UIImage *)(img->user_data);
    
    [image drawAtPoint:point];
    return 1;
}

void cm_image_free(CM_IMAGE *img)
{
    UIImage *image = (UIImage *)img->user_data;
    [image release];
    
    CM_FREE(img);
}

void cm_gdi_update_screen(void* pfd, const CM_RECT* rect)
{
    CM_GDI_STRUCT *p;
    UIView *view;
    
    CM_ASSERT(pfd != 0 && rect != 0);
    p = (CM_GDI_STRUCT *)pfd;
    if (p)
    {
        view = p->view;
        //CGRect drawRect = CGRectMake((CGFloat)rect->x, (CGFloat)rect->y, (CGFloat)rect->w, (CGFloat)rect->h);
        //[view setNeedsDisplayInRect:drawRect];
        [view setNeedsDisplay];
    }
}

