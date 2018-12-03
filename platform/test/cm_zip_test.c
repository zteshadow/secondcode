
#include "cm_zip_stream.h"
#include "cm_utility.h"
#include "cm_lib.h"
#include "cm_debug.h"


void cm_zip_test(const S8 *fname)
{
    CM_SIZE size;
    S8 *data = cm_read_file(fname, &size);
    
    if (data != 0)
    {
        void *zip = cm_zip_create(CM_ZIP_GZIP);
        if (zip)
        {
            S32 zipLen;
            S8 *zipData;
            
            zipLen = cm_zip_decompress(zip, data, size, &zipData);
            cm_zip_destory(zip);
        }
        
        CM_FREE(data);
    }
}

