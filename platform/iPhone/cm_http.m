
#include "cm_http.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"

#import <Foundation/NSURLConnection.h>


@class CMHttpDelegate;
typedef struct _CM_HTTP_STRUCT
{
    CM_HTTP_FUNC callback;
    void *usr;
    
    NSURLConnection *connection;
    CMHttpDelegate *agent;
    
} CM_HTTP_STRUCT;


@interface CMHttpDelegate : NSObject
{
    CM_HTTP_STRUCT *handle;
}

@property CM_HTTP_STRUCT *handle;

@end

@implementation CMHttpDelegate

@synthesize handle;

- (id) initWithHandle:(CM_HTTP_STRUCT *)h
{
    if (self = [super init])
    {
        self.handle = h;
    }
    return self;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    CM_HTTP_FUNC func = self.handle->callback;
    func(CM_HTTP_ON_HEADER, 0, 0, self.handle->usr);
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    CM_HTTP_FUNC func = self.handle->callback;
    func(CM_HTTP_ON_BODY, [data bytes], [data length], self.handle->usr);    
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    CM_HTTP_FUNC func = self.handle->callback;
    func(CM_HTTP_ON_ERROR, 0, 0, self.handle->usr);    
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    CM_HTTP_FUNC func = self.handle->callback;
    func(CM_HTTP_ON_END, 0, 0, self.handle->usr);    
}
@end


void *cm_http_create(CM_HTTP_FUNC f, void *usr)
{
    CM_HTTP_STRUCT *p = (CM_HTTP_STRUCT *)CM_MALLOC(sizeof(CM_HTTP_STRUCT));
    if (p != 0)
    {
        cm_memset(p, 0, sizeof(CM_HTTP_STRUCT));
        p->callback = f;
        p->usr = usr;
    }
    return p;
}

S32 cm_http_set_header(void *http, const S8 *name, const S8 *value)
{
    return 0;
}

S8 *cm_http_get_header(void *http, const S8 *name)
{
    return 0;
}

S32 cm_http_get_code(void *http)
{
    return 0;
}

S8 *cm_http_get_response_url(void *http)
{
    return 0;
}

S32 cm_http_get_header_cnt(void *http, const S8 *name)
{
    return 0;
}

S8 *cm_http_get_header_item(void *http, const S8 *name, S32 index)
{
    return 0;
}

S32 cm_http_set_data(void *http, void *data, S32 size)
{
    return 0;
}

S32 cm_http_send_request(void *http, CM_HTTP_REQ_TYPE type, const S8 *url)
{
    CM_ASSERT(http != 0 && url != 0);
    CM_HTTP_STRUCT *p = (CM_HTTP_STRUCT *)http;
    if (p != 0)
    {
        NSString *urlString = [[NSString alloc] initWithUTF8String:url];
        
        NSURLRequest *req = [[NSURLRequest alloc] initWithURL:[NSURL URLWithString:urlString]];
        CMHttpDelegate *agent = [[CMHttpDelegate alloc] initWithHandle:p];
        
        p->agent = agent;
        
        NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:req delegate: agent];
        p->connection = con;
        
        [req release];
        [urlString release];        
        
        return 0;
    }
    
    return -1;
}

void cm_http_destory(void *http)
{
    CM_ASSERT(http != 0);
    if (http != 0)
    {
        CM_FREE(http);
    }
}

