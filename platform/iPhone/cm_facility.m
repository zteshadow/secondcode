
#include "cm_facility.h"
#include "cm_debug.h"
#include "cm_string.h"

#import <Foundation/NSPathUtilities.h>


#define CM_MAX_MEMORY_SIZE  (10 * 1024 * 1024)


CM_SIZE cm_facility_get_max_mem(void)
{
    return CM_MAX_MEMORY_SIZE;
}

//e:\nbk (without the last '\' or '/')
S32 cm_facility_get_work_dir(S8 *dir)
{
    CM_ASSERT(dir != 0);

    if (dir)
    {
        NSArray *path = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory,
                                                             NSUserDomainMask, YES);
        NSString *doc = [path objectAtIndex: 0];
        //NSString *tmp = NSTemporaryDirectory();
        cm_strcpy(dir, [doc UTF8String]);
        return 1;
    }
    
    return -1;
}

S32 cm_facility_get_deposit_fname(CM_DEPOSIT_FILE type, S8 *fname)
{
	S32 value = -1;
	NSString *name = 0;

    CM_ASSERT(fname != 0);

	switch(type)
	{
	case CM_DEPOSIT_404:
		name = @"nbk_404";
	break;

	case CM_DEPOSIT_ERROR:
		name = @"nbk_error";
	break;

	default:
		CM_ASSERT(0);
	break;
	}

    if (name != 0 && fname != 0)
    {
    	NSString * path = [[NSBundle mainBundle] pathForResource:name ofType:@"htm"];
    	cm_strcpy(fname, [path UTF8String]);

        value = 1;
    }
    
    return value;
}

//dir/sub or dir\sub
S32 cm_facility_add_sub_dir(S8 *dir, const S8 *sub)
{
    CM_ASSERT(dir != 0 && sub != 0);

    if (dir != 0 && sub != 0)
    {
        cm_strcat(dir, "/");
        cm_strcat(dir, sub);
        return 1;
    }
    
    return -1;
}

S32 cm_get_imei(S8 *imei)
{
    CM_ASSERT(0);
    return -1;
}

