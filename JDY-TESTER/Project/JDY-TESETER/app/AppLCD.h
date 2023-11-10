/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppLcd.h
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: 基RVMDK 4.72软件版本
****************************************************************************************/
#ifndef _APPLCD_H
#define _APPLCD_H

#include "stdint.h"

#include <includes.h>

#ifdef  __cplusplus
extern  "C" {
#endif







/*分机与主机串口通讯任务外部调用*/
extern void TaskLCD(void *p_arg);

uint8_t IrqDevAddr(uint8_t sectno,uint8_t *buff);
uint16_t SlaveToMasterStartAddr(uint8_t sectno,uint8_t *buff);


#ifdef  __cplusplus
}
#endif
#endif
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

