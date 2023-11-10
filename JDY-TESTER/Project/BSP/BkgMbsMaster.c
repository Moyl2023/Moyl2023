/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.c
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: ��������ʾModbusͨѶ
****************************************************************************************/
#include <includes.h>
#include "crc.h"
#include "gpio.h"	
#include "Uart.h"
#include "BkgMbsMaster.h"
#include "MbsMaster.h"
#include "MbsSlave.h"
#include "ModbusDef.h"
#include "appstart.h"
#include "message.h"

/*************************************************************************************
**  static uint8 MbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
**   ����: �������豸���մ���
**   ����: com--����ID buf--���������ݴ�����MaxSize--һ������������ݴ�С timeout--��ʱ��� 
**   ���: TRUE ���� FALSE ʧ�� 
**   ����: ���մ�������
**************************************************************************************/
static uint8 BkgMbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
{
	// ������ɴ�����(ת��û������֡��ʽ���޷��ж����ݵĳ���)��
	// 1��һ���ֽ����ݶ�û�н��յ�����ʱʱ�䳬��timeout
	// 2���������ݺ������RCV_OVER_TIME����û�м����ܵ����ݣ���Ϊ�����Ѿ�������� 
	uint8 rcvLen = 0, tmpLen = 0;
	uint32 StartTime = JdyTest.DataYc.g_TimerMs;
	uint8 bRcv = 0;	   // �Ƿ���յ�������
	uint32 lastTime = 0;	 
	
	do 
	{	 
		tmpLen = Uart_Read(UartHandler[com], buf+rcvLen, MaxSize-rcvLen);

		if(tmpLen > 0)
		{
			bRcv = 1; 
			lastTime = JdyTest.DataYc.g_TimerMs;
			rcvLen += tmpLen;	
		}	
		else
		{
		   if(bRcv ==1)
		   {
			  if(JdyTest.DataYc.g_TimerMs-lastTime>RCV_OVER_TIME)//����20MSû���յ�������Ϊ�������
			      break;
		   }
		}
		OSTimeDlyHMSM(0,0,0,20);
	}while(JdyTest.DataYc.g_TimerMs - StartTime < timeout);	  // ��ʱ��Χ�ڣ�����300MS����ʾһ֡���ݽ������

	return rcvLen;
}
/*************************************************************************************
**  static uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
**   ����: ���ҵ�ַ�������ݶ�
**   ����: startAddr--������ʼ��ַ   RegNum--�Ĵ�������
**   ���:  
**   ����:  
**************************************************************************************/
static uint8 BkgIrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
{
	uint16 i,AddrLimit;
	AddrLimit = startAddr + RegNum - 1;
	for(i = 1; i <= MAX_SECT; i++)
	{
		if( (startAddr >= SSectAddrAndData[i].LowRegAddr) && (AddrLimit <= (SSectAddrAndData[i].HighRegAddr-1)) )
		{
			return i;	
		}
	}
	return NON_SECT;
}
/*************************************************************************************
**  static uint16 ModbusPackage(uint8 *txbuf,uint8 cmd, uint16 startAddr, uint16 *buf, uint8 length)
**   ����: modbus���ݴ��
**   ����: length--�Ĵ�������
**   ���: ���ݰ����� 
**   ����: �������轫length����Ϊ1
**************************************************************************************/
static uint16 BkgModbusPackage(uint8 DevAddr, uint8 *txbuf,uint8 cmd, uint16 startAddr, uint16 *buf, uint8 length)
{
	uint16 crc = 0, i, ret;
	//uint8 DevAddr = MBSSLAVE_ADDRESS, count = 0;
	uint8 count = 0;
	
	txbuf[count++] = DevAddr;                            /*�豸��ַ*/
	txbuf[count++] = cmd;                                /*������*/
	txbuf[count++] = (uint8)(startAddr >> 8);
	txbuf[count++] = (uint8)(startAddr & 0x00ff);        /*��ʼ��ַ����˷���*/
	//if((cmd == MODBUS_WR_MULT_REG)||(cmd == MODBUS_WR_SINGLE_REG))
	if(cmd == MODBUS_WR_MULT_REG)
	{												
		txbuf[count++] = 0;
		txbuf[count++] = length;                         /*�Ĵ�������*/
		txbuf[count++] = length*2;                       /*���ݳ���*/	
	}
	for(i = 0; i < length; i++)
	{
		txbuf[count+i*2] = (uint8)(buf[i] >> 8);
		txbuf[count+i*2+1] = (uint8)(buf[i] & 0x00ff);   /*���ݴ�˷���*/
	}
	crc = ModbusCRC(txbuf, count+length*2);
	txbuf[count+length*2+1] = (uint8)(crc >> 8);
	txbuf[count+length*2] = (uint8)(crc & 0x00ff);       /*CRCС�˷���*/
	ret = count+length*2+2;                              /*���ݰ��ܳ���*/   
	
	return ret;
}

static void BkgFixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i;

	for(i = 0; i < Lenth; i++)
	{
		if(i%2)                                          /*С�˴洢*/
		{
			src[StartAddr+i-1] = buf[i];
		}
		else
		{
			src[StartAddr+i+1] = buf[i];
		}	
	}
}

// ======================================================================================================
//BOOL DownDevDeal(uint8 com, uint8 DevAddr, uint8 cmd, uint16 StartAddr, uint16 *buf, uint8 length)
// ��������д�����豸����
// ������com:ͨ�Ŷ˿ڣ� DevAddr:�豸��ַ 0~23�� cmd:������ ��ѯ�����ã�	startAddr:��ѯ�Ĵ�����ʼ��ַ, 
// data:��ѯʱ�����ѯ���ݸ���(�Ĵ�������)������ʱ�������õ�����ֵ	 com:���ں�
// ���أ��ɹ� T ʧ�� F 
// ======================================================================================================
static BOOL BkgDownDevDeal(uint8 com, uint8 DevAddr, uint8 cmd, uint16 StartAddr, uint16 *buf, uint8 length)
{
	uint16 rcvCrc = 0, addr = 0, TxLen ,ovtime = MODBUS_COMM_TIME_OUT;
	//uint8  com = COM_DISP, DevAddr = MBSSLAVE_ADDRESS, rcvLen = 0, SectAddr;
	uint8  rcvLen = 0, SectAddr;
	uint8 Rxbuf[COMM_BUF_SIZE];
	uint8 batgroup = 0;
	int16 TempDate[240];
	uint8 i;
//	int16 temp[MAX_BAT_NUM];
//	uint16 TmpData[DEV_STAT_NUM];
//	uint8 i;

	
	if (cmd == MODBUS_RD_HOLD_REG)                                    
	{
		if (length > MODBUS_MAX_REG)                                   /*��ѯ�Ĵ����������ܳ���125��*/
		{
			return FALSE;
		}
	}
	SectAddr = BkgIrqSectAddrMaster(StartAddr,length);                    /*�ε�ַ*/
	if (SectAddr == NON_SECT)                                          /*�ε�ַ����*/
	{
		return FALSE;
	}
	else
	{
		ovtime = MODBUS_COMM_TIME_OUT*3;                                 /*����ͨѶ��ʱʱ��*/
	}
	
	addr = (StartAddr - SSectAddrAndData[SectAddr].LowRegAddr)*2;       /*�ڶ��е�ƫ�Ƶ�ַ*/	 
	
	do 
	{	
	 	if(Uart_Read(UartHandler[com], Rxbuf, 32) <= 0)
			break;
	}while(1);                                                         /* �������ݻ����� */

	TxLen = BkgModbusPackage(DevAddr, Rxbuf, cmd, StartAddr, buf, length);
	if(com != COM5)
	{
		switch(com)
		{
			case COM1:
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
				break;
			case COM2:
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
				break;
			case COM3:
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
				break;
			case COM4:
				USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
				break;
		}
	}
	if(Uart_Write(UartHandler[com], Rxbuf, TxLen) != TxLen)
	{
		return FALSE;
	}
	if(DevAddr != BROADCAST_ADDR)//�㲥��ַû�лظ���
	{
		rcvLen = BkgMbsSlaveLinkReadFrame(Rxbuf, COMM_BUF_SIZE, ovtime, com); /*��ȡ��������*/	   
		if (rcvLen > 3)
		{
			if (DevAddr != Rxbuf[0])                                       /* ȷ���豸��ַ */
			{
				return FALSE;
			}
			if (cmd != Rxbuf[1])                                           /*ȷ�Ϲ�����*/
			{
				return FALSE;
			}
			if(cmd == MODBUS_RD_HOLD_REG)																	/*ȷ�����ݳ���*/
			{
				if(rcvLen > (Rxbuf[2] + 5))
				{
					rcvLen = (Rxbuf[2] + 5);
				}
			}
			memcpy(&rcvCrc,&Rxbuf[rcvLen-2], 2);                           /*У��CRC*/
			if (ModbusCRC(Rxbuf, rcvLen-2) != rcvCrc)
			{
				return FALSE;
			}

			if (cmd == MODBUS_RD_HOLD_REG)
			{
				if(DevAddr > 0)
				{//ģ���Ŵ�1��ʼ���洢��0��ʼ��1��ģ��洢��0��λ��
					DevAddr = DevAddr - 1;
				}
				switch(SectAddr)
				{ 
					case S_BAT_V_DATA_SECT:	    /*��ص�ѹ���ݶ�*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_BatV[0][0], &Rxbuf[3], addr, buf[0]*2);					
						//DataErrCheck(batgroup,DevAddr,VOLTAGE_CHECK,ON);//����ѹ�Ƿ��й���
						//LcdComm.RdDevFlag[batgroup] = OFF;//��ʾ��ģ��ͨ�Ž��������Խ�����һ��ģ��ĵ�����ݶ�ȡ
					}
					break;
					case S_BAT_R_DATA_SECT:	   /*�����������*/	
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_InterR[0][0], &Rxbuf[3], addr, buf[0]*2);
						//DataErrCheck(batgroup,DevAddr,INNER_R_CHECK,ON);//��������Ƿ��й���
						CountBatRStat(batgroup);//���������������Сֵ
						CountBatCRStat(batgroup);
						//LcdComm.InnerTest.RDataReadFlag[batgroup][DevAddr] = ON;//���ݶ�ȡ�ɹ���־
					}
					break;	
					case S_CONNECT_R_DATA_SECT:	 /*���ӵ������ݶ�*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[0][0], &Rxbuf[3], addr, buf[0]*2);
						//DataErrCheck(batgroup,DevAddr,CONNECT_R_CHECK,ON);//����������Ƿ��й���
						CountBatRStat(batgroup);//���������������Сֵ
						CountBatCRStat(batgroup);
						//LcdComm.InnerTest.CRDataReadFlag[batgroup][DevAddr] = ON;//���������ݶ�ȡ�ɹ���־
					}
					break;
					case S_BAT_TEMP_DATA_SECT:	    /*����¶����ݶ�*/
					{	
						BkgFixReadData((uint8*)TempDate, &Rxbuf[3], addr, buf[0]*2);
						for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum;i++)
						{
							MbsSlave.BatData[GROUP1].DevMeaData.S_Temp[i/18][i%18] = (float)TempDate[i]/10;
						}
							
//						BkgFixReadData((uint8*) &temp[0], &Rxbuf[3], addr, buf[0]*2);
//						BkgFixReadData((uint8*) &TmpData[0], &Rxbuf[3], addr, buf[0]*2);
//						for(i=0;i<MAX_BAT_NUM;i++)
//						{
//							if(TmpData[i] == 0xFAFA)
//							{
//								MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DevAddr][i] = 0x0000FAFA;
//							}
//							else
//							{
//								MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DevAddr][i] = ((float)temp[i])/10;
//							}
//						}
						//DataErrCheck(batgroup,DevAddr,TEMP_CHECK,ON);//����¶��Ƿ��й���
					}
					break;
					case S_V_DATA_DEC_SECT:	    /*��ص�ѹС�����ݶ�*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_VDec[0][0], &Rxbuf[3], addr, buf[0]*2);					
					}
					break;
					case S_DEV_STAT_SECT:	  	/*ģ������״̬��*/		   
					{	
						BkgFixReadData((uint8*)&MbsSlave.StatusData.MoudleStat[batgroup][0].S_Temp, &Rxbuf[3], addr, buf[0]*2);					
					}
					break;	
					case S_MSYS_CFG_SECT:											/*ϵͳ���ö�*/	
					{	
						BkgFixReadData((uint8*)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, &Rxbuf[3], addr, buf[0]*2);								
					}
					break;					
					case S_BAT_ERR_STAT_SECT:										/*��ع���״̬��*/
					{	
						BkgFixReadData((uint8*)&MbsSlave.StatusData.BatErrStat[batgroup].S_Quantity, &Rxbuf[3], addr, buf[0]*2);					
					}
					break;
 					case S_GROUP_DATA_SECT:											/*�ɼ����������ݶ�*/
					{	
						BkgFixReadData((uint8*)&MbsSlave.Group[batgroup].SysMeaData.GroupV, &Rxbuf[3], addr, buf[0]*2);	
						//LcdComm.Current[batgroup] = (float)MbsSlave.Group[batgroup].SysMeaData.GroupI/10;
						//LcdComm.BatSysStat.GroupV[batgroup] = (float)MbsSlave.Group[batgroup].SysMeaData.GroupV/10;
						
					}
					break;
					case S_MBS_SINGNAL_SECT:											/*MODBUSң�����ݶ�*/
					case S_MEA_DATA_SECT:											/*�������ݶ�*/
					case S_CAL_PARA_SECT:											/*У׼������*/
					case S_ASSIST_STAT_SECT:											/*����״̬��*/
					case S_SYS_CTRL_SECT:											/*ϵͳ���ƶ�*/
					case S_SYS_TIME_SECT:											/*ϵͳʱ�Ӷ�*/
					case S_COM_PARA_SECT:											/*ͨѶ������*/
					case S_GROUP_CAL_SECT:											/*�ɼ���У׼���ݶ�*/
					case S_GROUP_PARA_SECT:											/*�ɼ��в������ݶ�*/
					case S_GROUP_STAT_SECT:											/*�ɼ���״̬���ݶ�*/
					case S_R_RATIO_SECT:												/*����У׼ϵͳ*/
					case S_TEMP_SET_SECT:											/*�¶�̽ͷID���ö�*/
					case S_TEMP_EXID_SECT:											//�¶�̽ͷ�Զ���ID���ö�
					case S_HISTORY_RECORD_SECT:										/*��ʷ��¼��*/
					case S_USART_TEST_SECT:											/*���ڲ��Զ�*/	
					break;
					default:								             
					break;
				}
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*************************************************************************************
**   BOOL MbsMasterLoop(uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length)
**   ����: �������豸��ѯ���豸
**   ����: cmd--������  startAddr--������ʼ��ַ
buf--��ȡʱ��ʾ��ȡ�Ĵ���������д��ʱ��ʾҪд������ݣ�
length--��ȡʱֵΪ1��д��ʱ��ʾҪд��ļĴ�������
**   ���: ʧ�ܻ�ɹ���־
**   ����: �Ĵ�����������123�����ְ�����
**************************************************************************************/
BOOL BkgMbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length)
{
//	uint32 overTime = MODBUS_OVERTIME; // 1s
	uint16 ret = FALSE, tmp = 0, i = 0;
//	uint8 batgroup = 0;
	uint8 ReadSum;//��ȡ�Ĵ�������
	uint16 WRSum;
	if(cmd == MODBUS_WR_MULT_REG)
	{
		WRSum = length;
		for(i = 0; 0 < WRSum; i++)
		{
			if(WRSum > MODBUS_MAX_REG)                                /*���ݰ����ȴ�������,�ְ�����*/
			{
				tmp = MODBUS_MAX_REG;
			}
			else
			{
				tmp = WRSum;
			}
			WRSum -= tmp;
			if(BkgDownDevDeal(com,DecAddr,cmd, startAddr+i*MODBUS_MAX_REG, buf+i*MODBUS_MAX_REG, tmp) == TRUE)
			{
				MbsMaster.BkgCommErrStartTime = JdyTest.DataYc.g_TimerMs;
				MbsMaster.BkgCommErrCount = 0;	   
				MbsMaster.BkgCommErrStatus = 0;
				if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)
				{//�ж��Ƿ���ϻָ�
					ErrFlag.BkgCommErrFlag = BKG_COM_RECOVER;
					DataErrCheck(0,0,BKG_COMM_CHECK,ON);
				}
				else
				{
					ErrFlag.BkgCommErrFlag = NORMAL;
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				MbsMaster.BkgCommErrCount++;	  //ͨѶ���ϼ���
				if(MbsMaster.BkgCommErrCount >= COMM_ERR_COUNT)
				{
					MbsMaster.BkgCommErrCount = 0;
					MbsMaster.BkgCommErrStatus = 1;
					if(ErrFlag.BkgCommErrFlag != BKG_COM_ERR)
					{
						ErrFlag.BkgCommErrFlag = BKG_COM_ERR;
						DataErrCheck(0,0,BKG_COMM_CHECK,ON);
					}
			
					break;
				}
			} 
		}
	}
	else if(cmd == MODBUS_RD_HOLD_REG)
	{
		ReadSum = *buf;
		for(i = 0; 0 < ReadSum; i++)
		{
			if(ReadSum > MODBUS_MAX_REG)
			{
				tmp = MODBUS_MAX_REG;
			}
			else
			{
				tmp = ReadSum;
			}
			ReadSum -= tmp;
			if(BkgDownDevDeal(com, DecAddr, cmd, startAddr+i*MODBUS_MAX_REG, &tmp, length) == TRUE)
			{
				MbsMaster.BkgCommErrStartTime = JdyTest.DataYc.g_TimerMs;
				MbsMaster.BkgCommErrCount = 0;	   
				MbsMaster.BkgCommErrStatus = 0;
				if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)
				{//�ж��Ƿ���ϻָ�
					ErrFlag.BkgCommErrFlag = BKG_COM_RECOVER;
					DataErrCheck(0,0,BKG_COMM_CHECK,ON);
				}
				else
				{
					ErrFlag.BkgCommErrFlag = NORMAL;
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				MbsMaster.BkgCommErrCount++;	  //ͨѶ���ϼ���
				if (MbsMaster.BkgCommErrCount >= COMM_ERR_COUNT)
				{
					MbsMaster.BkgCommErrCount = 0;
					MbsMaster.BkgCommErrStatus = 1;
					if(ErrFlag.BkgCommErrFlag != BKG_COM_ERR)
					{
						ErrFlag.BkgCommErrFlag = BKG_COM_ERR;
						DataErrCheck(0,0,BKG_COMM_CHECK,ON);
					}
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_BatV[0],0,MAX_GROUP_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_VDec[0],0,MAX_GROUP_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_Temp[0],0,MAX_GROUP_BAT_NUM*4);//
					memset(&MbsSlave.StatusData.MoudleStat[GROUP1][0].S_Temp,0,(DEV_STAT_NUM+1)*2);//�¶�ռ�������Ĵ�����					
				}
			} 
		}
	}
	else
	{
		if(BkgDownDevDeal(com, DecAddr, cmd, startAddr, buf, length) == TRUE)
			{
				MbsMaster.BkgCommErrStartTime = JdyTest.DataYc.g_TimerMs;
				MbsMaster.BkgCommErrCount = 0;	   
				MbsMaster.BkgCommErrStatus = 0;
				if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)
				{//�ж��Ƿ���ϻָ�
					ErrFlag.BkgCommErrFlag = BKG_COM_RECOVER;
					DataErrCheck(0,0,BKG_COMM_CHECK,ON);
				}
				else
				{
					ErrFlag.BkgCommErrFlag = NORMAL;
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				MbsMaster.BkgCommErrCount++;	  //ͨѶ���ϼ���
				if (MbsMaster.BkgCommErrCount >= COMM_ERR_COUNT)
				{
					MbsMaster.BkgCommErrCount = 0;
					MbsMaster.BkgCommErrStatus = 1;
					if(ErrFlag.BkgCommErrFlag != BKG_COM_ERR)
					{
						ErrFlag.BkgCommErrFlag = BKG_COM_ERR;
						DataErrCheck(0,0,BKG_COMM_CHECK,ON);
					}
				}
			} 
		}
	return ret;
}

/*****************************************************************************************************
* BOOL UartMessageDeal(MSG_STRUCT* pMsg) 
* ������������Ϣ
* ������pMsg ��Ϣ����
******************************************************************************************************/
BOOL BkgUartMessageDeal(MSG_STRUCT* pMsg) 
{
  	uint8 ret = FALSE;
	uint8 i;
	uint16 buff[2]={0};
	buff[0]=((uint16)pMsg->MsgData[4]<<8) + pMsg->MsgData[5];
		switch(pMsg->MsgType)
		{
			case MSG_RD_REG:
				for(i=0;i<RESEND_DATA_COUNT;i++)
				{
					if(BkgMbsMasterLoop(pMsg->MsgCOM, pMsg->MsgDevAddr, pMsg->MsgData[0], ((uint16)pMsg->MsgData[1]<<8)+pMsg->MsgData[2],buff, (uint16)pMsg->MsgData[3])== TRUE)
					{
						ret = TRUE;
						break;
					}
					ret = FALSE;
				}
				break;
			case MSG_WR_REG:
				for(i=0;i<RESEND_DATA_COUNT;i++)
				{
					if(BkgMbsMasterLoop(pMsg->MsgCOM, pMsg->MsgDevAddr, pMsg->MsgData[0], ((uint16)pMsg->MsgData[1]<<8)+pMsg->MsgData[2],(uint16 *)&pMsg->MsgData[4], (uint16)pMsg->MsgData[3]) == TRUE)
					{
						ret = TRUE;
						break;
					}
					ret = FALSE;
				}
				break;
			default:
				break;
		}
	return ret;
}

/***************************************************************************
**  void MasterProcess(void)
**   ����: ��ΪMODBUS��̨����ʱ�Ĵ��� 
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void MasterProcess(uint8 DEVCOM,uint8 GROUP)
{
	struct tm ttm;
	if((JdyTest.DataYc.g_TimerMs - BATComTime.SysStatusTimeMs[GROUP]) > BATComTime.DevStatusReadTime[GROUP])
	{
		BATComTime.SysStatusTimeMs[GROUP] = JdyTest.DataYc.g_TimerMs;
		//LcdComm.DevNUM[GROUP1] = (MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum + BAT_V_DATA_NUM - 1)/BAT_V_DATA_NUM;
		MasterCfgRead(GROUP,DEVCOM,19*2);//��ȡ������Ϣ,������������ͨ��������Ϣ
		if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum != 0)
		{
			MasterBatVRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//��ȡ��ص�ѹ
			MasterBatVDecRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//��ȡ��ص�ѹС������
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].TempDevStat == ON)
			{
				MasterBatTRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum*2);//��ȡ�¶�,��ȡ���Ǹ�����
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].InnerRDevStat == ON)
			{				
				//LcdComm.InnerTest.LastTestTimesFlag[GROUP] = 1;//ֻ��ʾ��ǰʵʱֵ
				//LcdComm.InnerTest.TotalTestTimes[GROUP] = 1;//ֻ��ʾ��ǰʵʱֵ
				ttm = Time_GetCalendarTime(); 
				//LcdComm.InnerTest.TestTime[GROUP].MeaYear  = ttm.tm_year;
				//LcdComm.InnerTest.TestTime[GROUP].MeaMon = ttm.tm_mon;
				//LcdComm.InnerTest.TestTime[GROUP].MeaDay = ttm.tm_mday;
				//LcdComm.InnerTest.TestTime[GROUP].MeaHour = ttm.tm_hour;
				//LcdComm.InnerTest.TestTime[GROUP].MeaMin = ttm.tm_min;
				//LcdComm.InnerTest.TestTime[GROUP].MeaSec = ttm.tm_sec;
				MasterBatRRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//��ȡ����
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].CRDevStat == ON)
			{
				MasterBatCRRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//��ȡ��������ֵ
			}
			if(MbsSlave.StatusData.BatErrStat[GROUP].S_Quantity == 0)
			{//ֻ��ȡ��������
				MasterErrStatRead(GROUP,DEVCOM,1);
			}
			else
			{//��ȡ���������͹�����Ϣ
				MasterErrStatRead(GROUP,DEVCOM,MbsSlave.StatusData.BatErrStat[GROUP].S_Quantity*2+1);
			}
		}
	}
}

/*****************************************************************************************/
/*                       end of file                                                     */
/*****************************************************************************************/

