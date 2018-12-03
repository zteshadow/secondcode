
#ifndef _CM_FACILITY_H
#define _CM_FACILITY_H


#include "cm_data_type.h"


#define CM_MAX_PATH         (256)


typedef enum
{
    CM_DEPOSIT_404,
    CM_DEPOSIT_ERROR,

    CM_DEPOSIT_MAX
    
} CM_DEPOSIT_FILE;

#ifdef __cplusplus
extern "C"
{
#endif


CM_SIZE cm_facility_get_max_mem(void);

//e:\nbk (without the last '\' or '/')
S32 cm_facility_get_work_dir(S8 *dir);

S32 cm_facility_get_deposit_fname(CM_DEPOSIT_FILE type, S8 *fname);

//用dir返回全路径，主要是由平台决定dir/sub or dir\sub
S32 cm_facility_add_sub_dir(S8 *dir, const S8 *sub);

S32 cm_get_imei(S8 *imei);


#ifdef __cplusplus
}
#endif


#endif //_CM_FACILITY_H

