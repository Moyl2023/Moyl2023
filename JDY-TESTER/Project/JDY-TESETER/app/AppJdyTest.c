/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: TaskJdyTest.c
** Created By: myl
** Last modified Date:  2017-05-23
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               主机显示通信任务
****************************************************************************************/
#include <ucos_ii.h>
#include "Message.h"
#include "uart.h"
#include "appstart.h"
#include "stm32f10x_usart.h"
#include "AppJdyTest.h"



/***************************************************************************
**  void TaskCom2(void *p_arg)
**   功能: 串口2通信任务
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void TaskJdyTest(void *p_arg)
{ 
//	static uint32 com2flag = 0;
	MSG_STRUCT msg; 

	while(1)
	{ 
//		printf("\r\n pre 222222  flag  = %d   \r\n",com2flag++); 
    JdyTest.DataAss.JdyTestTimeFlag = JdyTest.g_TimerMs;
		OSTimeDlyHMSM(0,0,0,5);
	} 
}


/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

