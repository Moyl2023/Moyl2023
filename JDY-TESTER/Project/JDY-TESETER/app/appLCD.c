/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: appDEVCOMM.c
** Created By: Abe
** Last modified Date: 2016-3-15
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               与后台通讯处理
****************************************************************************************/
#include <ucos_ii.h>
#include "appstart.h"
#include "appLCD.h"
#include "MbsSlave.h"
#include "MbsMaster.h"
#include "message.h"
#include "Uart.h"




/***************************************************************************
**  void InitDevCommTask(void)
**   功能: 分机串口任务初始化 
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void InitLcdUsart(void)
{
	//UartHandler[LCDCOM] = Uart_Open(LCDCOM);      /* 打开1#主机COM */
	//assert_param(UartHandler[LCDCOM]); 
	//UartRcvCfg[LCDCOM].p_head = UartRcvCfg[LCDCOM].rcvbuf;
	//UartRcvCfg[LCDCOM].p_tail = UartRcvCfg[LCDCOM].rcvbuf;
	//UartRcvCfg[LCDCOM].FailCount = 0;               /* 串口处理变量初始化 */
}

/***************************************************************************
uint8 IrqDevAddr(uint8 sectno,uint8 *buff)
**   功能: 查询模块地址
**   参数:  sectno:段号，buff:作为从机时的接收数据缓冲区
**   输出:  需要查询的模块地址
**   描述: 屏对主机板进行请求模块数据时，计算出对应的模块地址
****************************************************************************/
uint8_t IrqDevAddr(uint8_t sectno,uint8_t *buff)
{
	uint8_t temp;
	switch(sectno)
	{
		case S_MEA_DATA_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = (temp - S_MEA_DATA_ADDR)  / S_MEA_DATA_NUM;
			break;
		case S_CAL_PARA_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = (temp - S_CAL_PARA_ADDR)  / S_CAL_PARA_NUM;
			break;
		case S_SYS_CTRL_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = (temp - S_SYS_CTRL_ADDR)  / S_SYS_CTRL_NUM;
			break;
		default:
			break;
	}
	return temp;
}


/***************************************************************************
uint16 SlaveToMasterStartAddr(uint8_t sectno,uint8_t *buff)
**   功能: 地址空间转换
**   参数:  sectno:段号，buff:作为从机时的接收数据缓冲区
**   输出:  需要查询的模块MODBUS 起始地址
**   描述: 屏请求模块数据时，将屏对主机的地址转换成主机对模块的地址
****************************************************************************/
uint16_t SlaveToMasterStartAddr(uint8_t sectno,uint8_t *buff)
{
	uint16_t temp;
	switch(sectno)
	{
		case S_MEA_DATA_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = ((temp - S_MEA_DATA_ADDR)  % S_MEA_DATA_NUM) + MEA_DATA_ADDR;
			break;
		case S_CAL_PARA_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = ((temp - S_CAL_PARA_ADDR)  % S_CAL_PARA_NUM) + CAL_PARA_ADDR;
			break;
		case S_SYS_CTRL_SECT:
			temp = (uint16)(buff[2]<<8)+buff[3];
			temp = ((temp - S_SYS_CTRL_ADDR)  % S_SYS_CTRL_NUM) + S_SYS_CTRL_ADDR;
			break;
		default:
			break;
	}
	return temp;
}
/***************************************************************************
**  void TaskDEV(void *p_arg)
**   功能: 分机串口任务 
**   参数:  
**   输出:  
**   描述: 调度周期 = 10ms
****************************************************************************/
void TaskLCD(void *p_arg)
{
//	static uint32 lcdtaskflag = 0;
	ATC_QueueCreat(APP_TASK_LCD_PRIO);	          /* 创建消息队列 */
	InitUartTask(COM4);
	JdyTest.DataYk.PictureNO = PICTURE1;
	SwitchToPicX(COM4,CMD_SWITCH_PIC,PICTURE1);	
	while(1)
	{
//		printf("\r\n PRE 55555   flag = %d   \r\n",lcdtaskflag++); 
		JdyTest.DataAss.LcdTimeFlag = JdyTest.g_TimerMs;
		LcdProcess(COM4);//液晶屏处理程序
//		printf("\r\n AFTER 55555   flag = %d   \r\n",lcdtaskflag++); 
		OSTimeDlyHMSM(0,0,0,5);
	}
}

/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

