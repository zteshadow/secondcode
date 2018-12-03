
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
* This module is imply the 'new' and 'delete' method if platform does not
* support
*
*****************************************************************************/


#ifndef _CM_TIME_H
#define _CM_TIME_H


#include "cm_data_type.h"


typedef struct
{
    S32 tm_sec;
    S32 tm_min;
    S32 tm_hour;
    S32 tm_mday;
	S32 tm_mon;
    S32 tm_year;    /*��1970�꿪ʼ������*/
    S32 tm_wday;    /* ���ڨCȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
    S32 tm_yday;    /* ��ÿ���1��1�տ�ʼ�������Cȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
    S32 tm_isdst;   /* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/

} CM_DATE_TIME;

typedef struct
{
    SLong tv_sec;   //seconds since CUT(1970...)
    SLong tv_usec;  //micro seconds(ע����΢��, ����milliseconds)

} CM_TIME_VAL;

typedef void (*CM_TIMER_FUNC)(void *usr);


#ifdef __cplusplus
extern "C"
{
#endif


CM_TIME cm_time(CM_TIME *time);
CM_TIME cm_mktime(const CM_DATE_TIME *time);
S32 cm_gettimeofday(CM_TIME_VAL *time);
S32 cm_localtime(const CM_TIME *time, CM_DATE_TIME *dtime);
S32 cm_get_current_ms(void);

void *cm_timer_create(CM_TIMER_FUNC f, void *usr);

//timeout:����
//ѭ��timer
void cm_timer_start(void *timer, S32 timeout);
void cm_timer_stop(void *timer);
void cm_timer_destory(void *timer);


#ifdef __cplusplus
}
#endif


#endif //_CM_TIME_H

