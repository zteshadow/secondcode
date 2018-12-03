
/*****************************************************************************
* Copyright Statement:
* --------------------
* This software is protected by Copyright and the information contained
* herein is confidential. The software may not be copied and the information
* contained herein may not be used or disclosed except with the written
* permission of XXX Inc. (C) 200X-20XX
*
* Version:
* --------
* Ver: 1.0              Author: Samuel.Song             Date: 2008-10-06
*
* Description:
* ------------
* This module contains some socket API wrapper such as send, rcv, etc.
* We use asynchronous socket here. At first you create a network handle with 
* callback, then you can create many socket with this handle.
*
*****************************************************************************/


#ifndef _CM_SOCKET_H
#define _CM_SOCKET_H


#include "cm_data_type.h"


typedef enum
{
    CM_SOCK_TYPE_TCP = 0,
    CM_SOCK_TYPE_UDP,
    CM_SOCK_TYPE_MAX
    
} cm_socket_type;

typedef enum
{
    CM_SOCK_EVENT_DNS = 0,
    CM_SOCK_EVENT_CONNECT,
    CM_SOCK_EVENT_RCV,
    CM_SOCK_EVENT_SEND,
    CM_SOCK_EVENT_CLOSE,
    CM_SOCK_EVENT_MAX
    
} cm_socket_event;

typedef struct
{
	S8      event;      //CM_SOCKET_DNS, etc.
	void    *inst_data;    
	void    *param2;
	void    *param3;
	
} cm_sock_info;

typedef void (*cm_socket_cb)(cm_sock_info *);


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* Return:
* ------------
* ERR: < 0(if network can not be well initialized or other error)
* OK : 0
*
* Description:
* ------------
* This is for embeded system such as MTK feature phone, network must be
* initialized before use and well released after use.
*
*****************************************************************************/
S8 cm_network_open(void);

/*****************************************************************************
* Description:
* ------------
* This is for embeded system such as MTK feature phone, network must be
* initialized before use and well released after use.
*
*****************************************************************************/
void cm_network_close(void);

/*****************************************************************************
* Input:
* ------
*  1 'func' : handle for the asynchronous message
*  2 'type' : CM_SOCK_TCP or CM_SOCK_UDP, just support these 2 types
*  3 'param': customer data
*
* Return:
* ------------
* ERR: NULL(if network can not be well initialized or other error)
* OK : socket instance handle, read\write\read\ can be done with this handle
*
* Description:
* ------------
* the customer param is a problem, 'int' can be transport directly, but 
* pointer of data should be transported if more than 'int'
*
*****************************************************************************/
CM_HANDLE cm_sock_inst_init(cm_socket_cb func, S8 type, void *param);

/*****************************************************************************
* Input:
* ------
*  1: h: handle of the init socket
*
* Description:
* ------------
* 1: network handle must be properly released, or memory will be leaked
* 2: handle of 'h' can not be used anymore
*
*****************************************************************************/
void cm_sock_inst_release(CM_HANDLE h);

/*****************************************************************************
* Input:
* ------
*  1 'h'    : network handle
*  2 'url'  : URL need parse
*  3 'ip'   : buffer to keep ip
*  4 'param': customer data
*
* Description:
* ------------
* 'ip' is for this case: if the kernal do not manage this buffer, this
* module have to manage it, so if we use this 'ip', higher level will do it.
*
*****************************************************************************/
void cm_sock_dns(CM_HANDLE h, const S8 *url, S8 *ip, void *param);

/*****************************************************************************
* Input:
* ------
*  1 'h'    : network handle
*  2 'ip'   : ip address to connect to
*  3 'port' : port number
*
* Return:
* -------
* usually return the socket created
* 
* Description:
* ------------
* This action will create a new socket and binded to 'ip' and 'port'.
* asynchronous message of 'connected' will be received later.
*
*****************************************************************************/
S32 cm_sock_connect(CM_HANDLE h, const S8 *ip, S32 port);

/*****************************************************************************
* Input:
* ------
*  1 'h'      : network handle
*  2 'socket' : socket to send data
*  3 'data'   : data to send
*  4 'size'   : data size
*
* Return:
* -------
* Length of data sent in fact. Because maybe can't send 'size' of data at 
* one time right now. 
*
* Description:
* ------------
* Send 'size' of 'data' to network, return is the real size have sent, if
* this send action is blocked, the next step of send will be triggered by
* asynchronous message.
* The 'socket' is the return value of 'cm_sock_connect'
*
*****************************************************************************/
S32 cm_sock_send(CM_HANDLE h, S32 socket, const S8 *data, S32 size);

/*****************************************************************************
* Input:
* ------
*  1 'h'      : network handle
*  2 'socket' : socket to send data
*  3 'data'   : data to send
*  4 'size'   : data size
*
* Return:
* -------
* Length of data received in fact. Because maybe can't read 'size' of data at 
* one time right now.
*
* Description:
* ------------
* Receive 'size' of data to 'buffer' from network. Return is the real size 
* have received. If this read action is blocked, the next step of read will
* be triggered by asynchronous message.
* The 'socket' is the return value of 'cm_sock_connect'
*
*****************************************************************************/
S32 cm_sock_recv(CM_HANDLE h, S32 socket, S8 *buffer, S32 size);

/*****************************************************************************
* Input:
* ------
*  1 'h'      : network handle
*  2 'socket' : socket to send data
*
* Description:
* ------------
* Close socket of 'socket'. Maybe this action will lead to a next 'socket-
* close' asynchronous message.
* The 'socket' is the return value of 'cm_sock_connect'
*
*****************************************************************************/
S32 cm_sock_close(CM_HANDLE h, S32 socket);

/*****************************************************************************
* Input:
* ------
*  1 'n'      : network int data
*
* Return:
* -------
* local edian of this 'n'
*
* Description:
* ------------
* Coz network use big-edian, so need this change from network big edian to
* local endian(little or big)
*
*****************************************************************************/
U32 cm_ntohl(U32 n);

/*****************************************************************************
* Input:
* ------
*  1 'h'      : local int data
*
* Return:
* -------
* network edian(big edian) of this 'n'
*
* Description:
* ------------
* change local edian data to network used big-edian
*
*****************************************************************************/
U32 cm_htonl(U32 h);

/*****************************************************************************
* Input:
* ------
*  1 'ip'     : dot divided decimal ip string
*  2 'inp'    : where translated unsigned int ip data to be saved 
*
* Return:
* -------
* ERR: 0 (this is different from linux programming habit)
* OK : 1
*
* Description:
* ------------
* unsigned int(big edian) ip address needed in network API.
* EG: "1.2.3.4" means 0x01020304
*
*****************************************************************************/
S32 cm_inet_aton(const S8 *ip, U32 *inp);

/*****************************************************************************
* Input:
* ------
*  1 'in'     : unsigned int format ip address
*  2 'ip'     : where translated dot divided decimal ip data to be saved 
*
* Description:
* ------------
* ip must at least 16 bytes.
*
*****************************************************************************/
void cm_inet_ntoa(U32 in, S8 *ip);

#ifdef __cplusplus
}
#endif


#endif //_CM_SOCKET_H

