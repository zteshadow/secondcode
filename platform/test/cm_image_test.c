
#include "cm_gdi.h"
#include "cm_utility.h"
#include "cm_debug.h"


void cm_image_test(const S8 *fname, void* g)
{
    CM_SIZE size;
    S8 *imgData = cm_read_file(fname, &size);
    
    if (imgData != 0)
    {
        CM_IMAGE *img = cm_image_decode((const void *)imgData, size);
        
        if (img)
        {
            cm_image_draw(g, 50, 200, img);
            cm_image_free(img);
        }
    }
}

