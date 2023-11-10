/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppCom1.h
** Created By: 
** Last modified Date: 
** Last Version: 1.0
** Descriptions:  
****************************************************************************************/
#ifndef __SYSCTRL_H 
#define __SYSCTRL_H

#include "Def.h"
#include "stdio.h"
#include "config.h"

void LedCtrl(void);
uint8 BeepCtrl(uint32 *p,uint8 flag);
void FeedDog(uint32 WdtTimeFlag);
void ErrOutputCheck(void);
void DisChargeCtrl(void);
void CheckBatStat(uint8 batgroup);

#endif
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/


