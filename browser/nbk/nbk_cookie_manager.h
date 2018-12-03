
#ifndef _NBK_COOKIE_MANAGER_H
#define _NBK_COOKIE_MANAGER_H


#include "cm_data_type.h"


typedef struct _nbk_cookie nbk_cookie, *nbk_cookie_ptr;

struct _nbk_cookie
{
	nbk_cookie *next;

	S8 *name;
	S8 *value;
	S8 *path;
	S8 *domain;
	SLong expires;
	S8 *expirestr;
	CM_BOOL tailmatch;

	// RFC 2109
	S8 *version;
	S8 *maxage;

	CM_BOOL secure;
	CM_BOOL livecookie;
	CM_BOOL httponly;
};

typedef struct _nbk_cookie_info
{
	nbk_cookie *cookies;	//cookie list
	S8 *filename;		    //cookie file
	CM_BOOL running;		//
	SLong numcookies;		//number of cookies
	CM_BOOL newsession;		//new session, discard session cookie on load

}nbk_cookie_info;


#ifdef __cplusplus
extern "C"
{
#endif


nbk_cookie_info* nbk_cookie_init(const S8 *file, CM_BOOL newsession);
void nbk_cookie_deinit(nbk_cookie_info *c);

nbk_cookie* nbk_cookie_add(nbk_cookie_info* c, CM_BOOL httpheader, S8 *lineptr, const S8 *domain, const S8 *path);
nbk_cookie* nbk_cookie_get_list(nbk_cookie_info *c, const S8 *host, const S8 *path, CM_BOOL secure);
void nbk_cookie_free_list(nbk_cookie *co, CM_BOOL cookietoo);

CM_BOOL nbk_cookie_output(nbk_cookie_info *c, const S8 *dumphere);


#ifdef __cplusplus
}
#endif


#endif //_NBK_COOKIE_MANAGER_H

