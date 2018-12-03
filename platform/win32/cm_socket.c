
#include "cm_socket.h"
#include "cm_lib.h"
#include "cm_string.h"
#include "cm_debug.h"
#include <stdio.h>


typedef struct
{
    cm_socket_cb    callback;
    void *          param;
    
} cm_socket_struct;


/*****************************************************************************
* Description:
* ------------
* 1: the new socket handle can be created on heap by 'malloc' if used in multi-
*    thread environment or use static variable to avoid memory process.
*
*****************************************************************************/
static cm_socket_struct *cm_alloc_sock_block(void);

/*****************************************************************************
* Description:
* ------------
* 1: the new socket handle can be created on heap by 'malloc' if used in multi-
*    thread environment or use static variable to avoid memory process.
*
*****************************************************************************/
static void cm_free_sock_block(cm_socket_struct *p);

/*****************************************************************************
* Description:
* ------------
* 1: The machenism in embeded system is always async base on message, so a
*    call bakck function as this notify is used to handle the async message
*
*****************************************************************************/
static void cm_sock_notify(void *msg);

#if 0
#endif

static cm_socket_struct *cm_alloc_sock_block(void)
{
    cm_socket_struct *p;

    p = (cm_socket_struct *)CM_MALLOC(sizeof(cm_socket_struct));
    CM_ASSERT(p != NULL);
    
    return p;
}

static void cm_free_sock_block(cm_socket_struct *p)
{
    CM_ASSERT(p != NULL);
    CM_FREE(p);
}

static void cm_sock_notify(void *msg)
{
    CM_ASSERT(msg != NULL);

    /*get sock id from msg*/    
}

#if 0
#endif

S8 cm_network_open(void)
{
    /*init network*/
    return 0;
}

void cm_network_close(void)
{
}

CM_HANDLE cm_sock_inst_init(cm_socket_cb func, S8 type, void *param)
{            
    cm_socket_struct *p;
    
    CM_ASSERT(func != NULL);
    
    p = cm_alloc_sock_block();
    p->callback = func;
    p->param = param;
    
    return p;
}

void cm_sock_inst_release(CM_HANDLE h)
{
    CM_ASSERT(h != NULL);
    
    cm_free_sock_block((cm_socket_struct *)h);
}

void cm_sock_dns(CM_HANDLE h, const S8 *url, S8 *ip, void *param)
{
}

S32 cm_sock_connect(CM_HANDLE h, const S8 *ip, S32 port)
{
    return 0;
}

S32 cm_sock_send(CM_HANDLE h, S32 socket, const S8 *data, S32 size)
{
    return 0;
}

S32 cm_sock_recv(CM_HANDLE h, S32 socket, S8 *buffer, S32 size)
{
    return 0;
}

S32 cm_sock_close(CM_HANDLE h, S32 socket)
{
    return 0;
}

#if 0
#endif

U32 cm_ntohl(U32 n)
{
    /*if local is big edian, just return n, reverse n if little edian*/
    return (n << 24) | ((n & 0x0000FF00) << 8) | ((n & 0x00FF0000) >> 8) | (n >> 24);
}

U32 cm_htonl(U32 h)
{
    return cm_ntohl(h);
}

S32 cm_inet_aton(const S8 *ip, U32 *inp)
{
    U32 *p, value[4] = {0, 0, 0, 0};    
    
    CM_ASSERT(ip != NULL && inp != NULL);

    for (p = (U32 *)value; *ip != '\0'; ip++)
    {
        if (*ip == '.')
        {
            p ++;
        }
        else
        {
            *p = (*p * 10) + (*ip - '0');
        }
    }
    
    *inp = (value[0] << 24) + (value[1] << 16) + (value[2] << 8) + value[3];    
    
    return 1;
}

void cm_inet_ntoa(U32 in, S8 *ip)
{
    U32 n = cm_htonl(in);
    U8 *p = (U8 *)(&n);    
    
    CM_ASSERT(ip != NULL);    
    
    sprintf(ip, "%d.%d.%d.%d", *p, *(p + 1), *(p + 2), *(p + 3));
}

