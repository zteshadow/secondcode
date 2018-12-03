
#include "cm_gdi.h"
#include "cm_string.h"
#include "cm_debug.h"


void cm_gdi_test(void* g)
{
    {
        S32 str_width, str_len;
        CM_RECT frame, bounds;
        CM_POINT pos;
        CM_COLOR color;
        CM_FONT font;

        U16 ILoveYou[] = {0x6211, 0x7231, 0x4F60, 0};

        cm_gdi_get_view_rect(g, &frame);
        str_len = cm_ustrlen(ILoveYou);        
        
        {
        font = cm_gdi_get_font(g, 12, 0, 0);
        str_width = cm_gdi_get_text_width(g, font, ILoveYou, str_len);
       
        pos.x = (frame.w - str_width) / 2;
        pos.y = 100;
        
        cm_gdi_set_font(g, font);

        cm_memset(&color, 0, sizeof(color));
        color.r = 0xFF;
        color.a = 0xFF;
        
        cm_gdi_set_pen_color(g, &color);
        cm_gdi_draw_text(g, ILoveYou, str_len, &pos);

        bounds.x = pos.x - 5;
        bounds.y = pos.y - 5;
        bounds.w = str_width + 10;
        bounds.h = cm_gdi_get_font_height(g, font) + cm_gdi_get_font_ascent(g, font) + 10;
        cm_gdi_draw_rect(g, &bounds);
        }

        {
            font = cm_gdi_get_font(g, 20, 0, 0);
            str_width = cm_gdi_get_text_width(g, font, ILoveYou, str_len);
    
            pos.x = (frame.w - str_width) / 2;
            pos.y = 150;
            
            cm_gdi_set_font(g, font);
            
            cm_memset(&color, 0, sizeof(color));
            color.b = 0xFF;
            color.a = 0xFF;
            
            cm_gdi_set_pen_color(g, &color);
            cm_gdi_draw_text(g, ILoveYou, str_len, &pos);
            
            bounds.x = pos.x - 5;
            bounds.y = pos.y - 5;
            bounds.w = str_width + 10;
            bounds.h = cm_gdi_get_font_height(g, font) + cm_gdi_get_font_ascent(g, font) + 10;
            cm_gdi_draw_rect(g, &bounds);
        }        
        //cm_gdi_update_screen(g, &frame);
        
        {
            bounds.x = pos.x - 5;
            bounds.y += 50;

            color.r = 0;
            color.g = 0xFF;
            color.b = 0;
            
            cm_gdi_set_brush_color(g, &color);
            cm_gdi_fill_rect(g, &bounds);
            
            bounds.y += 50;
            cm_gdi_draw_circle(g, &bounds);
        }
    }

    {
    }
}

