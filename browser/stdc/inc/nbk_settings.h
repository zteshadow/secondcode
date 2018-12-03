/*
 * nbk_settings.h
 *
 *  Created on: 2011-5-2
 *      Author: migr
 */

#ifndef NBK_SETTINGS_H_
#define NBK_SETTINGS_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
    
enum NESupport {
    NESupport,
    NENotSupportByFlyflow,
    NENotSupportByNbk
};

typedef struct _NSettings {
    
    int8    mainFontSize; // custom font size
    
    bd_bool    allowImage : 1;
    bd_bool    allowIncreateMode : 1;
    bd_bool    recognisePhoneSite : 1;
    bd_bool    selfAdaptionLayout : 1;
    
    bd_bool    clickToLoadImage : 1;
    
    nid     mode; // FF-Full, FF-Narrow or UCK
    nid     initMode; // direct or TC
    
    // used internal
    uint8   support;

} NSettings;
    
#ifdef __cplusplus
}
#endif

#endif /* NBK_SETTINGS_H_ */
