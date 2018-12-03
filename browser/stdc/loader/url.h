/*
 * url.h
 *
 *  Created on: 2011-2-16
 *      Author: wuyulun
 */

#ifndef URL_H_
#define URL_H_

#include "../inc/config.h"

#ifdef __cplusplus
extern "C" {
#endif
    
void url_initPhoneSite(void);
void url_delPhoneSite(void);
bd_bool url_checkPhoneSite(const char* url);

void url_parseWapWhiteList(uint8* data, int len);
void url_freeWapWhiteList(void);
bd_bool url_inWapWhiteList(const char* url);
uint8* url_getWL(int* num);

void url_unescape(char* dst, const char* src, bd_bool forUrl);

// return: new url. caller is responsible for release.
char* url_parse(const char* base, const char* url);

char* url_hex_to_str(char* url, char* value);
char* url_str_to_hex(char* url, char* str);

bd_bool url_has_query(const char* url);
    
#ifdef __cplusplus
}
#endif

#endif /* URL_H_ */
