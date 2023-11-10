/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppCom1.c
** Created By: myl 
** Last modified Date:  20170523
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               主机显示通信任务
****************************************************************************************/
#include <ucos_ii.h>
#include "Message.h"
#include "uart.h"
#include "appstart.h"
#include "stm32f10x_usart.h"
#include "AppUpdate.h"

uint8 CheckRcvData(uint8 com,uint8 *buf)
{
    if((buf[0] == 'R')&&(buf[1] == 'E')&&(buf[2] == 'S')&&(buf[3] == 'E')&&(buf[4] == 'T'))
    {
          JdyTest.DataYk.SystemReset = ON;

	}
}


uint8 UpdateComprocess(uint8 com)
{
	uint8 myl=0;
	uint8 rcvLen = 0, ret = LCD_RCV_UNDONE;
	uint8 rcvtmpbuf[MAX_UART_BUF_LENTH];	
	rcvLen = Uart_Read(UartHandler[com], UartRcvCfg[com].p_head, UartRcvCfg[com].rcvbuf+LCD_RCV_QUEUE_SIZE-UartRcvCfg[com].p_head);
	if(rcvLen > 0)                                                /* 接收到新数据 */
	{
			if(com == COM2)
		{
		myl = 2;

	}
	else if(com == COM4)
		{
		myl = 1;

	}
		UartRcvCfg[com].FailCount = 0;
		UartRcvCfg[com].p_head += rcvLen;
		if(UartRcvCfg[com].p_head >= (UartRcvCfg[com].rcvbuf + LCD_RCV_QUEUE_SIZE)) 
		{
			UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;      /* 循环队列接收 */
		}
		/* 判断是否接收完成 */
		//if(RCV_OK == CheckRcvDone(&UartRcvCfg[com],rcvtmpbuf))
		{
			ret = CheckRcvData(com,UartRcvCfg[com].rcvbuf);              /* 校验与处理命令 */

		}
	}
	else
	{
		if(++UartRcvCfg[com].FailCount > DEVCOMM_FAIL_NUM)
		{
			UartRcvCfg[com].FailCount = 0;
			UartRcvCfg[com].p_tail = UartRcvCfg[com].p_head;      /* 通讯超时,复位接收指针 */
		}
	}
return myl;
}
/***************************************************************************
**  void TaskUpdateCom(void *p_arg)
**   功能: 串口1通信任务
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void TaskUpdateCom(void *p_arg)
{ 
//	static uint32 com1flag = 0;
	MSG_STRUCT msg; 
	InitUartTask(COM2);
	while(1)
	{ 
		UpdateComprocess(COM2);
		OSTimeDlyHMSM(0,0,0,5);
	}
//		printf("\r\n AFTER 1111111 flag = %d  \r\n",com1flag++); 
		
}


/**************************************************************************************************
**                            End Of File
****************************************************************************************************/


