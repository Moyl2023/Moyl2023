/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsSlave.c
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: �ֻ�������ModbusͨѶ
****************************************************************************************/
#include <includes.h>
#include "crc.h"
#include "rtc.h"
#include "Uart.h"
#include "MbsSlave.h"
#include "ModbusDef.h"
#include "appstart.h"

MBS_SLAVE MbsSlave;


UART_RCV_CFG     UartRcvCfg[5];                      /*���ڴ����ѽ��մ�������*/


uint8 tempbuff[8] = {0};


/* ������ַ�ͼĴ�����ַ���� */
const SECT_ADDR_AND_DATA SSectAddrAndData[] = {{0,0}
	,{S_BAT_V_DATA_ADDR,	 					S_BAT_V_DATA_ADDR+S_BAT_V_DATA_NUM}
	,{S_BAT_R_DATA_ADDR,	 					S_BAT_R_DATA_ADDR+S_BAT_R_DATA_NUM}
	,{S_CONNECT_R_DATA_ADDR,					S_CONNECT_R_DATA_ADDR+S_CONNECT_R_DATA_NUM}
	,{S_BAT_TEMP_DATA_ADDR,	 					S_BAT_TEMP_DATA_ADDR+S_BAT_TEMP_DATA_NUM}
	,{S_BAT_ERR_STAT_ADDR,	 					S_BAT_ERR_STAT_ADDR+S_BAT_ERR_STAT_NUM}
	,{S_MBS_SINGNAL_ADDR,	 					S_MBS_SINGNAL_ADDR+S_MBS_SINGNAL_NUM}
	,{S_DEV_STAT_ADDR,	 					  	S_DEV_STAT_ADDR+S_DEV_STAT_NUM}
	,{S_MEA_DATA_ADDR,	 						S_MEA_DATA_ADDR+S_MEA_DATA_NUM}
	,{S_CAL_PARA_ADDR,	 						S_CAL_PARA_ADDR+S_CAL_PARA_NUM}
	,{S_SYS_WORK_STAT_ADDR,	 					S_SYS_WORK_STAT_ADDR+S_SYS_WORK_STAT_NUM}
	,{S_ASSIST_STAT_ADDR,	 					S_ASSIST_STAT_ADDR+S_ASSIST_STAT_NUM}
	,{S_SYS_CTRL_ADDR,	 						S_SYS_CTRL_ADDR+S_SYS_CTRL_NUM}
	,{S_MSYS_CFG_ADDR,	 						S_MSYS_CFG_ADDR+S_MSYS_CFG_NUM}
	,{S_SYS_TIME_ADDR,	 						S_SYS_TIME_ADDR+S_SYS_TIME_NUM}
	,{S_COM_PARA_ADDR,	 						S_COM_PARA_ADDR+S_COM_PARA_NUM}
	,{S_GROUP_DATA_ADDR,	 					S_GROUP_DATA_ADDR+S_GROUP_DATA_NUM}
	,{S_GROUP_CAL_ADDR,	 						S_GROUP_CAL_ADDR+S_GROUP_CAL_NUM}
	,{S_GROUP_PARA_ADDR,	 					S_GROUP_PARA_ADDR+S_GROUP_PARA_NUM}
	,{S_GROUP_STAT_ADDR,	 					S_GROUP_STAT_ADDR+S_GROUP_STAT_NUM}
	,{S_R_RATIO_ADDR,	 						S_R_RATIO_ADDR+S_RATIO_NUM}
	,{S_V_DATA_DEC_ADDR,	 					S_V_DATA_DEC_ADDR+S_V_DATA_DEC_SUM}
	,{S_TEMP_SET_ADDR,	 						S_TEMP_SET_ADDR+S_TEMP_SET_SUM}
	,{S_TEMP_EXID_ADDR,	 						S_TEMP_EXID_ADDR+S_TEMP_EXID_SUM}
	,{S_SHIELD_INFO_ADDR,	 					S_SHIELD_INFO_ADDR+S_SHIELD_INFO_NUM}
	,{S_HISTORY_RECORD_ADDR,	 				S_HISTORY_RECORD_ADDR+S_HISTORY_RECORD_NUM}
	,{S_USART_TEST_ADDR,	 					S_USART_TEST_ADDR+S_USART_TEST_NUM}
	,{LCD_CMD_ADDR,	 							LCD_CMD_ADDR+LCD_CMD_NUM}
};


/* ����modbus������Ӧ */
static void ModbusErrRespond(uint8 com,uint8 Cmd,uint8 ErrCode)
{
	uint8 sendbuf[5];
	uint16 Crc = 0;
	
	sendbuf[0] = 0;
	sendbuf[1] = Cmd+0x80;
	sendbuf[2] = ErrCode;
	Crc = ModbusCRC(sendbuf,3);
	sendbuf[3] = Crc&0xFF;
	sendbuf[4] = (Crc>>8)&0xFF;
	
	Uart_Write(UartHandler[com], sendbuf, 5);
}
/*************************************************************************************
**  static uint8 GetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
**   ����: �����ѽ������ݰ�����
**   ����: buf--���ݽ�����ɺ󻺴��� 
**   ���: ret-������ɱ�־    buf-���ݽ��ջ�����ʼ��ַ 
**   ����: 
**************************************************************************************/
static uint8 GetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8 i, len, *ptr, ret = RCV_UNDONE;
	
	if(pchan->p_head >= pchan->p_tail)
	{
		len = pchan->p_head - pchan->p_tail;           /* �ѽ��յ����ݳ��� */
	}
	else
	{
		len = pchan->p_head + RCV_QUEUE_SIZE - pchan->p_tail;
	}

	if(len >= 8)                                       /* modubsһ֡������С����Ϊ8���ֽ� */                                       
	{
		ptr = pchan->p_tail;                     
		for(i = 0; i < 8; i++)                         /*����ת�봦�����*/
		{
			buf[i] = *ptr;
			if(++ptr >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
			{
				ptr = pchan->rcvbuf;                   /* ѭ������ */
			}
		}
		
		if(buf[1] == MODBUS_WR_MULT_REG)               /*modbusд����Ĵ���������*/
		{
			if(len >= (buf[6]+9))                      /*modbusд����Ĵ������ݰ��ѻ���*/
			{
				for(i = 8; i < (buf[6]+9); i++)        /*����ת�봦�����*/
				{
					buf[i] = *ptr;
					if(++ptr >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
					{
						ptr = pchan->rcvbuf;           /* ѭ������ */
					}
				}
				pchan->p_tail = ptr;                   /*���ݰ��������*/
				ret = RCV_OK;
			}
			else                            
			{
				ret = RCV_UNDONE;                      /*���ݰ�����δ���*/
			}
		}
		else                                           /*modbus��ѯ��д�����Ĵ���������,һ�����ݳ���Ϊ8*/
		{
			pchan->p_tail = ptr;       
			ret = RCV_OK;                              /*���ݰ��������*/  
		}
	}

	return ret;
}
/*************************************************************************************
**  uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
**   ����: �������ݶ�
**   ����:  
**   ���:  
**   ����: 
**************************************************************************************/
static uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
{
	uint16 i,AddrLimit;
	AddrLimit = startAddr + RegNum - 1;
	for(i = 1; i <= S_MAX_SECT; i++)
	{
		if((startAddr >= SSectAddrAndData[i].LowRegAddr) && (AddrLimit <= (SSectAddrAndData[i].HighRegAddr-1)))
		{
			return i;	
		}
	}
	return DEV_NON_SECT;
}
/*************************************************************************************
**  uint16 ModbusPackage(uint8 com,uint8 *txbuf,uint8 length)
**   ����: ���ݴ������
**   ����: txbuf--�跢�͵�������ʼ��ַ  com--����ID  length--�����ֽڳ���
**   ���: 
**   ����: 
**************************************************************************************/
static void ModbusPackage(uint8 com,uint8 *txbuf,uint8 length)
{
	uint16 crc = 0;

	crc = ModbusCRC(txbuf, length);
	
	txbuf[length++] = (uint8)(crc & 0x00ff);     /*CRCС�˷���*/
	txbuf[length++] = (uint8)(crc >> 8);

	Uart_Write(UartHandler[com], txbuf, length);
}
/*************************************************************************************
**  void FixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   ����:  �������ݴ洢
**   ����:  src--modbus�еĶε�ַ   buf--��洢��������ʼ��ַ  startAddr--�ε�ַ�е�ƫ�Ƶ�ַ
**   ���:  
**   ����: 
**************************************************************************************/
static void FixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i;

	for(i = 0; i < Lenth; i++)
	{
		if(i%2)         
		{
			src[StartAddr+i-1] = buf[i];
		}
		else
		{
			src[StartAddr+i+1] = buf[i];
		}	
	}
}


static uint8 FillTransData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i, txlen = 0;

	buf[txlen++] = Lenth;
	
	for(i = 0; i < Lenth; i++)
	{
		if(i%2)         
		{
			buf[txlen++] = src[StartAddr+i-1];
		}
		else
		{
			buf[txlen++] = src[StartAddr+i+1];
		}
	}
	
	return txlen;
}
/*����Ͷ������ĸ������,���ڲ�ƽ���ż���*/
//static void FillBusTestVol(uint8 com)
//{
	//if(DevLocalData.DevLocalCfg.DevSysCfg.LinkModel == 1) 
	//{/*˫��ģʽ,COM2Ϊ��������*/
	//	memcpy(&DevBusVol[MbsSlaveData.DevAnalog.DevBusCfgAi.TestResNo].BusCfgAi[com-1],
	//		MbsSlaveData.DevAnalog.DevBusCfgAi.DevbusCfgVol.BusCfgAi, sizeof(DEVBUS_CFG_VOL)/2);
	//}
	//else
	//{
	//	memcpy(&DevBusVol[MbsSlaveData.DevAnalog.DevBusCfgAi.TestResNo],
	//		MbsSlaveData.DevAnalog.DevBusCfgAi.DevbusCfgVol.BusCfgAi, sizeof(DEVBUS_CFG_VOL));
	//}
//}
/*************************************************************************************
**  void  ModbusRespond(uint8 *buf)
**   ����: ��Ӧmodbus����
**   ����: com--����ID  buf--���ջ�����  sectno--���ݶα��  
**         addroffset--������ַ�����ݶ��е�ƫ����  regnum--�����ļĴ�������
**   ���: modbus�ɹ���ʧ�ܴ��� 
**   ����: 
**************************************************************************************/
uint8 ModbusRespond(uint8 com,uint8 *buf, uint8 SectNo, uint16 addrOffset, uint16 regNum)
{
	uint8 txlen = 0, ret = RCV_OK, dataOffset = 4;  /*д���Ĵ���������������MODBUS���ݰ��е�ƫ�Ƶ�ַ*/
	MSG_STRUCT msg;
	uint8 i;
	uint16 temp;
	int16 TempDate[240];
	struct tm ttm;
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  
	switch(buf[1])
	{
		case  MODBUS_RD_HOLD_REG:
			dataOffset = 2;
			switch(SectNo)
			{
				case S_BAT_V_DATA_SECT: /*��ص�ѹ���ݶ�*/

					break;
				case S_V_DATA_DEC_SECT: /*��ص�ѹ���ݶ�*/

					break;
				case S_BAT_R_DATA_SECT:/*�����������*/	

					break;
				case S_CONNECT_R_DATA_SECT: /*���ӵ������ݶ�*/

					break;
				case S_BAT_TEMP_DATA_SECT:/*����¶����ݶ�*/

					break;
				case S_BAT_ERR_STAT_SECT:/*��ع���״̬��*/ 

					break;
				case S_MBS_SINGNAL_SECT:/*MODBUSң�����ݶ�*/

					break;
				case S_DEV_STAT_SECT:/*ģ������״̬��*/

					break;
				case S_MEA_DATA_SECT:/*�������ݶ�*/ 

					break;
				case S_CAL_PARA_SECT:	/*У׼������*/		

					break;
				case S_GROUP_DATA_SECT:/*���������*/	

					break;
				case S_SYS_WORK_STAT_SECT:/*ϵͳ����״̬��*/

					break;
				case S_ASSIST_STAT_SECT:/*����״̬��*/ 

					break;
				case S_MSYS_CFG_SECT: /*ϵͳ���ö�*/ 
					{
						if(1)
						{
//						if(Eeprom_Read_I2c((uint8 *)(&MbsSlave.SysPara.MSysCfg.S_CellGroupNum+addrOffset), SYS_CFG_SAVE_OFFSET + addrOffset, BKP_SYS_CFG_SAVE_OFFSET + addrOffset,regNum*2)==FALSE)
//						{
//							ret = RCV_REG_DATA_READ_ERR;
//						}
//						else
//						{
							txlen = dataOffset + FillTransData((uint8 *)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum,buf + dataOffset,addrOffset,regNum*2);
//						}
						}
						else
						{
							txlen = dataOffset + FillTransData((uint8 *)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum,buf + dataOffset,addrOffset,regNum*2);
						}
					}
					break;
				case S_SYS_TIME_SECT: /*ϵͳʱ�Ӷ�*/
					{
						ttm = Time_GetCalendarTime();
						MbsSlave.SysPara.CalTime.Year = ttm.tm_year;
						MbsSlave.SysPara.CalTime.Month = ttm.tm_mon;
						MbsSlave.SysPara.CalTime.Day = ttm.tm_mday;
						MbsSlave.SysPara.CalTime.Hour = ttm.tm_hour;
						MbsSlave.SysPara.CalTime.Minute = ttm.tm_min;
						MbsSlave.SysPara.CalTime.Second = ttm.tm_sec;
						txlen = dataOffset + FillTransData((uint8 *)&MbsSlave.SysPara.CalTime.Year,buf + dataOffset,addrOffset,regNum*2);
					}
					break;
				case S_COM_PARA_SECT: /*ͨѶ������*/

					break;
				case S_HISTORY_RECORD_SECT:/*��ʷ��¼��*/

					break;
				case S_USART_TEST_SECT:/*���ڲ��Զ�*/

					break;
				default:
					break;
			}
			break;
		case  MODBUS_WR_MULT_REG:
			
		case  MODBUS_WR_SINGLE_REG:
			if(buf[1] == MODBUS_WR_MULT_REG)
				{dataOffset = 7;}
			else if(buf[1] == MODBUS_WR_SINGLE_REG)
				{dataOffset = 4;}
			txlen = 6;
			switch(SectNo)
			{
				case S_CAL_PARA_SECT:									/*У׼��������**/
					FixReadData((uint8*)&MbsSlave.SysPara.CalPara[0], buf+dataOffset, addrOffset, regNum*2);
					msg.MsgType = MSG_WR_REG;
					msg.MsgLenth = 1;
					msg.MsgCOM = com;
					msg.MsgDevAddr = IrqDevAddr(SectNo,buf);
					msg.MsgData[0] = MODBUS_WR_MULT_REG;
					temp = SlaveToMasterStartAddr(SectNo,buf);
					msg.MsgData[1] = (uint8)(buf[3] & 0xFF);
					msg.MsgData[2] = (uint8)(temp>>8) & 0x00FF;
					msg.MsgData[3] = regNum;
					for(i=0;i<regNum;i++)
					{
						msg.MsgData[i*2+4] = buf[i*2+dataOffset];
						msg.MsgData[i*2+5] = buf[i*2+dataOffset+1];
					}
					

						//SendMsg2Queue(&msg,DevTask.TaskBat1);

					break;
					
				case S_MSYS_CFG_SECT:										/*ϵͳ��������**/
					//FixReadData((uint8*)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, buf+dataOffset, addrOffset, regNum*2);
					//if(Eeprom_Write_I2c((uint8 *)(&MbsSlave.SysPara.MSysCfg.S_CellGroupNum+addrOffset), SYS_CFG_SAVE_OFFSET + addrOffset, BKP_SYS_CFG_SAVE_OFFSET + addrOffset,regNum*2)==FALSE)
					//{
					//	ret = RCV_REG_DATA_SAVE_ERR;
					//}
					break;
				case S_SYS_TIME_SECT:										/*ϵͳʱ������**/
					FixReadData((uint8*)&MbsSlave.SysPara.CalTime.Year, buf+dataOffset, addrOffset, regNum*2);
					ttm.tm_year = MbsSlave.SysPara.CalTime.Year;
					ttm.tm_mon = MbsSlave.SysPara.CalTime.Month;
					ttm.tm_mday = MbsSlave.SysPara.CalTime.Day;
					ttm.tm_hour = MbsSlave.SysPara.CalTime.Hour;
					ttm.tm_min = MbsSlave.SysPara.CalTime.Minute;
					ttm.tm_sec = MbsSlave.SysPara.CalTime.Second;
					OS_ENTER_CRITICAL();
					Time_Adjust(ttm);
					OS_EXIT_CRITICAL();
					break;
				case S_COM_PARA_SECT:										/*����ͨ�Žӿڲ�������**/

					break;
				case S_SYS_CTRL_SECT:											/*������أ����裬�¶ȣ�������Ѳ�칦��*/
					FixReadData((uint8*)&MbsSlave.SysPara.SysCtrl[0].S_StartCellMea, buf+dataOffset, addrOffset, regNum*2);
					msg.MsgType = MSG_WR_REG;
					msg.MsgLenth = 1;
					msg.MsgCOM = com;
					msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
					temp = SlaveToMasterStartAddr(SectNo,buf);
					msg.MsgData[1] = (uint8)(buf[3] & 0xFF);
					msg.MsgData[2] = (uint8)(temp>>8) & 0x00FF;
					msg.MsgData[3] = regNum;
					for(i=0;i<regNum;i++)
						{
							msg.MsgData[i*2+4] = buf[i*2+dataOffset];
							msg.MsgData[i*2+5] = buf[i*2+dataOffset+1];
						}

						//SendMsg2Queue(&msg,DevTask.TaskBat1);

				break;
				default:
					ret = RCV_REG_ADD_ERR;
					break;
			}
			break;

		default:
			ret = RCV_FUCTION_ERR;
			break;
	}

	if((ret == RCV_OK) && (buf[0] != BROADCAST_ADDR))               /*�㲥����ظ�*/
	{
		ModbusPackage(com, buf, txlen);                             /*��Ӧ�����ѽ������*/
	}
	
	return ret;
}
/*************************************************************************************
**  uint8  CheckRcvModbusData(uint8 *buf)
**   ����: У�����ݰ�
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8  CheckRcvModbusData(uint8 com, uint8 *buf)
{	
	uint8  SectNo, RcvLen, ret = RCV_OK;
	uint16 StartAddr = 0, RegNum = 0, addrOffset;

	if(buf[1] == MODBUS_RD_HOLD_REG)             /* ���Ĵ��� */	
	{
		RcvLen = 8;
		RegNum = (buf[4]<<8) | buf[5];
		if(RegNum > MODBUS_MAX_READ_REG)
		{
			return RCV_REG_NUM_ERR;
		}
	}
	else if(buf[1] == MODBUS_WR_SINGLE_REG)      /* д�����Ĵ��� */
	{ 
		RegNum = 1;
		RcvLen = 8;
	}
	else if(buf[1] == MODBUS_WR_MULT_REG)        /* д����Ĵ��� */
	{ 		
		RegNum = (buf[4]<<8) | buf[5];
		RcvLen = buf[6] + 9;
		if(RegNum > MODBUS_MAX_WRITE_REG)
		{
			return RCV_REG_NUM_ERR;
		}
	}
	else
	{
		return  RCV_FUCTION_ERR;                 /* ��Ч������ */
	}

	if(ModbusCRC(buf,RcvLen))
	{
		ret = RCV_CRC_ERR;                       /* У����� */
	}
	else
	{
		StartAddr = (buf[2]<<8) | buf[3];
		if(StartAddr == 0x1023)
		{
			StartAddr = StartAddr;
		}
		SectNo = IrqSectAddrMaster(StartAddr, RegNum);
		if((SectNo == DEV_NON_SECT) || ((StartAddr + RegNum) > SSectAddrAndData[SectNo].HighRegAddr))
		{
			ret = RCV_REG_ADD_ERR;
		}
		else
		{
			addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*�ڶ��е�ƫ�Ƶ�ַ*/	
			ret = ModbusRespond(com,buf,SectNo,addrOffset,RegNum);                /*���ݰ������ж�,������ȷ��Ӧ��*/
		}
	}
	
	return ret;
}
/*************************************************************************************
**  uint8  SlaveCheckRcvDone(UART_RCV_CFG *pchan, uint8 DevAddr, uint8 *buf)
**   ����: ���ڻ���������ת�봦�����
**   ����: pchan--��ǰ����ͨ�����õĽ������ݽṹ  devaddr--�豸��ַ  buf--���ջ�����  
**   ���:  ������ɱ�־
**   ����: 
**************************************************************************************/
uint8  SlaveCheckRcvDone(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8  ret = RCV_UNDONE;
	while(pchan->p_tail != pchan->p_head)                                         /*�����������*/
	{
		if((*(pchan->p_tail) == 0) || (*(pchan->p_tail) == BROADCAST_ADDR))/*�豸��ַ��ȷ,��ʼ����*/
		{
			ret = GetRcvLen(pchan,buf);                                           /*�ж�һ֡�����Ƿ������*/

			break;
		}
		else
		{
			if(++(pchan->p_tail) >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
			{
				pchan->p_tail = pchan->rcvbuf;                                     /* ѭ������ */
			}
		}
	}
	return ret;
}
/*************************************************************************************
**  void MbsSlaveProcess(uint8 com)
**   ����: ���ڴ��豸���մ���
**   ����: com--����ID
**   ���:  
**   ����: ��ȡ�ѽ��յĴ�������,ת�������ݴ��������в�����
**************************************************************************************/
void MbsSlaveProcess(uint8 com)
{
	uint8 rcvLen = 0, ret = RCV_UNDONE;
	uint8 rcvtmpbuf[MAX_UART_BUF_LENTH];

	rcvLen = Uart_Read(UartHandler[com], UartRcvCfg[com].p_head, UartRcvCfg[com].rcvbuf+RCV_QUEUE_SIZE-UartRcvCfg[com].p_head);
	if(rcvLen > 0)                                                /* ���յ������� */
	{
		UartRcvCfg[com].FailCount = 0;
		UartRcvCfg[com].p_head += rcvLen;
		if(UartRcvCfg[com].p_head >= (UartRcvCfg[com].rcvbuf + RCV_QUEUE_SIZE)) 
		{
			UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;      /* ѭ�����н��� */
		}
		/* �ж��Ƿ������� */
		if(RCV_OK == SlaveCheckRcvDone(&UartRcvCfg[com],rcvtmpbuf))
		{
			ret = CheckRcvModbusData(com,rcvtmpbuf);              /* У���봦������ */
//			SysFlag.LedFlag.LED_COMM_SALAVE = ON;					/*ͨ�ŵ���*/
			switch(ret)
			{
				case RCV_UNDONE:
					break;

				case RCV_OK:
					break;

				case RCV_COMM_ADD_ERR:
				case RCV_CRC_ERR:
				case RCV_FUCTION_ERR:
				case RCV_LEN_ERR:
				case RCV_REG_DATA_SAVE_ERR:
				case RCV_REG_DATA_READ_ERR:
				case RCV_REG_ADD_ERR:
				case RCV_REG_DATA_ERR:
					if(rcvtmpbuf[0] != BROADCAST_ADDR)            /* �㲥����ظ� */
					{
						//ModbusErrRespond(com, rcvtmpbuf[1], ret); /* �ظ�modbus������ */
					}
					break;

				default:
					break;
			}
		}
	}
	else
	{
		if(++UartRcvCfg[com].FailCount > DEVCOMM_FAIL_NUM)
		{
			UartRcvCfg[com].FailCount = 0;
			UartRcvCfg[com].p_tail = UartRcvCfg[com].p_head;      /* ͨѶ��ʱ,��λ����ָ�� */
		}
	}
}
/***************************************************************************************/
/*                   End Of File                                                       */
/***************************************************************************************/

