
#include "cm_md5.h"
#include "cm_debug.h"

#include <CommonCrypto/CommonDigest.h>


S32 cm_create_md5(const void *data, CM_SIZE len, S8 *md5)
{
    S32 value = -1;
    
    CC_MD5_CTX md5_ctx;
    
    CM_ASSERT(data != 0 && len > 0 && md5 != 0);
    
    if (data != 0 && len > 0 && md5 != 0)
    {
        CC_MD5_Init(&md5_ctx);  
        
        CC_MD5_Update(&md5_ctx, data, len);
        CC_MD5_Final(md5, &md5_ctx);
        value = 1;
    }

    return value;
}

