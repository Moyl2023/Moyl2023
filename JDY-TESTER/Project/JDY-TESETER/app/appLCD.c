/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: appDEVCOMM.c
** Created By: Abe
** Last modified Date: 2016-3-15
** Last Version: 1.0
** Descriptions: �������߼��ϵͳ
**               ���̨ͨѶ����
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
**   ����: �ֻ����������ʼ�� 
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void InitLcdUsart(void)
{
	//UartHandler[LCDCOM] = Uart_Open(LCDCOM);      /* ��1#����COM */
	//assert_param(UartHandler[LCDCOM]); 
	//UartRcvCfg[LCDCOM].p_head = UartRcvCfg[LCDCOM].rcvbuf;
	//UartRcvCfg[LCDCOM].p_tail = UartRcvCfg[LCDCOM].rcvbuf;
	//UartRcvCfg[LCDCOM].FailCount = 0;               /* ���ڴ��������ʼ�� */
}

/***************************************************************************
uint8 IrqDevAddr(uint8 sectno,uint8 *buff)
**   ����: ��ѯģ���ַ
**   ����:  sectno:�κţ�buff:��Ϊ�ӻ�ʱ�Ľ������ݻ�����
**   ���:  ��Ҫ��ѯ��ģ���ַ
**   ����: �����������������ģ������ʱ���������Ӧ��ģ���ַ
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
**   ����: ��ַ�ռ�ת��
**   ����:  sectno:�κţ�buff:��Ϊ�ӻ�ʱ�Ľ������ݻ�����
**   ���:  ��Ҫ��ѯ��ģ��MODBUS ��ʼ��ַ
**   ����: ������ģ������ʱ�������������ĵ�ַת����������ģ��ĵ�ַ
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
**   ����: �ֻ��������� 
**   ����:  
**   ���:  
**   ����: �������� = 10ms
****************************************************************************/
void TaskLCD(void *p_arg)
{
//	static uint32 lcdtaskflag = 0;
	ATC_QueueCreat(APP_TASK_LCD_PRIO);	          /* ������Ϣ���� */
	InitUartTask(COM4);
	JdyTest.DataYk.PictureNO = PICTURE1;
	SwitchToPicX(COM4,CMD_SWITCH_PIC,PICTURE1);	
	while(1)
	{
//		printf("\r\n PRE 55555   flag = %d   \r\n",lcdtaskflag++); 
		JdyTest.DataAss.LcdTimeFlag = JdyTest.g_TimerMs;
		LcdProcess(COM4);//Һ�����������
//		printf("\r\n AFTER 55555   flag = %d   \r\n",lcdtaskflag++); 
		OSTimeDlyHMSM(0,0,0,5);
	}
}

/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

