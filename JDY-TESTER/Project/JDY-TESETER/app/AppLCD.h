/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppLcd.h
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: ��RVMDK 4.72����汾
****************************************************************************************/
#ifndef _APPLCD_H
#define _APPLCD_H

#include "stdint.h"

#include <includes.h>

#ifdef  __cplusplus
extern  "C" {
#endif







/*�ֻ�����������ͨѶ�����ⲿ����*/
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

