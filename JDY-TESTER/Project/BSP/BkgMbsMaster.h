/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.h
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions:    Modbus后台通讯数据结构
****************************************************************************************/
#ifndef _BKGMBSMASTER_H
#define _BKGMBSMASTER_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "ModbusDef.h"

/*****************************BAT5K***********************************/






#define MAX_SECT USART_TEST_SECT
/*********************电池数据**************************/
#define MAX_BAT_NUM								18//每个模块最多18节电池
#define MAX_DEV_NUM								14//最多支持240节电池，七个模块
#define MAX_GROUP_NUM							2//总电池组数
#define MAX_GROUP_BAT_NUM						240//一组电池中电池最大数量，形式实验要求


 

extern MBS_MASTER MbsMaster;

extern BOOL BkgMbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length);	
BOOL BkgUartMessageDeal(MSG_STRUCT* pMsg);
/*********************BAT5K**************************/



#ifdef  __cplusplus
}
#endif
#endif

