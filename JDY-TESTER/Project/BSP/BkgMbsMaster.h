/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.h
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions:    Modbus��̨ͨѶ���ݽṹ
****************************************************************************************/
#ifndef _BKGMBSMASTER_H
#define _BKGMBSMASTER_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "ModbusDef.h"

/*****************************BAT5K***********************************/






#define MAX_SECT USART_TEST_SECT
/*********************�������**************************/
#define MAX_BAT_NUM								18//ÿ��ģ�����18�ڵ��
#define MAX_DEV_NUM								14//���֧��240�ڵ�أ��߸�ģ��
#define MAX_GROUP_NUM							2//�ܵ������
#define MAX_GROUP_BAT_NUM						240//һ�����е�������������ʽʵ��Ҫ��


 

extern MBS_MASTER MbsMaster;

extern BOOL BkgMbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length);	
BOOL BkgUartMessageDeal(MSG_STRUCT* pMsg);
/*********************BAT5K**************************/



#ifdef  __cplusplus
}
#endif
#endif

