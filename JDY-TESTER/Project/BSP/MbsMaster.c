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
#include "MbsMaster.h"
#include "MbsSlave.h"
#include "ModbusDef.h"
#include "appstart.h"
#include "message.h"

MBS_MASTER  MbsMaster;	           

/* ������ַ�ͼĴ�����ַ���� */
const SECT_ADDR_AND_DATA SectAddrAndData[] = {{0,0}
	,{BAT_V_DATA_ADDR,	 					BAT_V_DATA_ADDR+BAT_V_DATA_NUM}
	,{BAT_R_DATA_ADDR,	 					BAT_R_DATA_ADDR+BAT_R_DATA_NUM}
	,{CONNECT_R_DATA_ADDR,				CONNECT_R_DATA_ADDR+CONNECT_R_DATA_NUM}
	,{BAT_TEMP_DATA_ADDR,	 				BAT_TEMP_DATA_ADDR+BAT_TEMP_DATA_NUM}
	,{BAT_ERR_STAT_ADDR,	 				BAT_ERR_STAT_ADDR+BAT_ERR_STAT_NUM}
	,{DEV_STAT_ADDR,	 					  DEV_STAT_ADDR+DEV_STAT_NUM}
	,{MEA_DATA_ADDR,	 						MEA_DATA_ADDR+MEA_DATA_NUM}
	,{CAL_PARA_ADDR,	 						CAL_PARA_ADDR+CAL_PARA_NUM}
	,{SYS_WORK_STAT_ADDR,	 				SYS_WORK_STAT_ADDR+SYS_WORK_STAT_NUM}
	,{ASSIST_STAT_ADDR,	 					ASSIST_STAT_ADDR+ASSIST_STAT_NUM}
	,{SYS_CTRL_ADDR,	 						SYS_CTRL_ADDR+SYS_CTRL_NUM}
	,{MSYS_CFG_ADDR,	 						MSYS_CFG_ADDR+MSYS_CFG_NUM}
	,{SYS_TIME_ADDR,	 						SYS_TIME_ADDR+SYS_TIME_NUM}
	,{COM_PARA_ADDR,	 						COM_PARA_ADDR+COM_PARA_NUM}
	,{GROUP_DATA_ADDR,	 					GROUP_DATA_ADDR+GROUP_DATA_NUM}
	,{GROUP_CAL_ADDR,	 						GROUP_CAL_ADDR+GROUP_CAL_NUM}
	,{GROUP_PARA_ADDR,	 					GROUP_PARA_ADDR+GROUP_PARA_NUM}
	,{GROUP_STAT_ADDR,	 					GROUP_STAT_ADDR+GROUP_STAT_NUM}
	,{RATIO_ADDR,	 					RATIO_ADDR+RATIO_NUM}
	,{V_DATA_DEC_ADDR,	 					V_DATA_DEC_ADDR+V_DATA_DEC_SUM}
	,{TEMP_SET_ADDR,	 						TEMP_SET_ADDR+TEMP_SET_SUM}
	,{TEMP_EXID_ADDR,	 						TEMP_EXID_ADDR+TEMP_EXID_SUM}
	,{M_SHIELD_INFO_ADDR,	 					M_SHIELD_INFO_ADDR+SHIELD_INFO_NUM}
	,{HISTORY_RECORD_ADDR,	 			HISTORY_RECORD_ADDR+HISTORY_RECORD_NUM}
	,{USART_TEST_ADDR,	 					USART_TEST_ADDR+USART_TEST_NUM}
	
};

/*************************************************************************************
**  static uint8 MbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
**   ����: �������豸���մ���
**   ����: com--����ID buf--���������ݴ�����MaxSize--һ������������ݴ�С timeout--��ʱ��� 
**   ���: TRUE ���� FALSE ʧ�� 
**   ����: ���մ�������
**************************************************************************************/
static uint8 MbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
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
static uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
{
	uint16 i,AddrLimit;
	AddrLimit = startAddr + RegNum - 1;
	for(i = 1; i <= MAX_SECT; i++)
	{
		if( (startAddr >= SectAddrAndData[i].LowRegAddr) && (AddrLimit <= (SectAddrAndData[i].HighRegAddr-1)) )
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
static uint16 ModbusPackage(uint8 DevAddr, uint8 *txbuf,uint8 cmd, uint16 startAddr, uint16 *buf, uint8 length)
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

static void FixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
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
static BOOL DownDevDeal(uint8 com, uint8 DevAddr, uint8 cmd, uint16 StartAddr, uint16 *buf, uint8 length)
{
	uint16 rcvCrc = 0, addr = 0, TxLen ,ovtime = MODBUS_COMM_TIME_OUT;
	//uint8  com = COM_DISP, DevAddr = MBSSLAVE_ADDRESS, rcvLen = 0, SectAddr;
	uint8  rcvLen = 0, SectAddr;
	uint8 Rxbuf[COMM_BUF_SIZE];
	uint8 batgroup;
	int16 temp[MAX_BAT_NUM];
	int16 tempSTAT[DEV_STAT_NUM];
	uint16 TmpData[DEV_STAT_NUM];
	uint16 tmpflag;
	//uint16 TempV[MAX_BAT_NUM];
	uint8 i;


	
	if (cmd == MODBUS_RD_HOLD_REG)                                    
	{
		if (length > MODBUS_MAX_REG)                                   /*��ѯ�Ĵ����������ܳ���125��*/
		{
			return FALSE;
		}
	}
	SectAddr = IrqSectAddrMaster(StartAddr,length);                    /*�ε�ַ*/
	if (SectAddr == NON_SECT)                                          /*�ε�ַ����*/
	{
		return FALSE;
	}
	else
	{
		ovtime = MODBUS_COMM_TIME_OUT;                                 /*����ͨѶ��ʱʱ��*/
		if(DevAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
		{
			ovtime = MODBUS_COMM_TIME_OUT*2;		//�ɼ���Ӧ�ٶ���
		}
	}
	
	addr = (StartAddr - SectAddrAndData[SectAddr].LowRegAddr)*2;       /*�ڶ��е�ƫ�Ƶ�ַ*/	 
	
	do 
	{	
	 	if(Uart_Read(UartHandler[com], Rxbuf, 32) <= 0)
			break;
	}while(1);                                                         /* �������ݻ����� */

	TxLen = ModbusPackage(DevAddr, Rxbuf, cmd, StartAddr, buf, length);
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
		rcvLen = MbsSlaveLinkReadFrame(Rxbuf, COMM_BUF_SIZE, ovtime, com); /*��ȡ��������*/	   
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
					case BAT_V_DATA_SECT:	    /*��ص�ѹ���ݶ�*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DevAddr], &Rxbuf[3], addr, buf[0]*2);					
						//memset(TempV,0,MAX_BAT_NUM);
						//FixReadData((uint8*) TempV, &Rxbuf[3], addr, buf[0]*2);
						//for(i=0;i<MAX_BAT_NUM;i++)
						//{//ͨ�ŵ�λ��0.1mV�������������Ҫת����mV
						//	MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DevAddr] = ((TempV[i]&0x8000)!=0)?(TempV[i]&0x7FFF):(float)TempV[i]/10;
						//}
						DataErrCheck(batgroup,DevAddr,VOLTAGE_CHECK,ON);//����ѹ�Ƿ��й���
					}
					break;
					case V_DATA_DEC_SECT:	    /*��ص�ѹС�����ݶ�*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DevAddr], &Rxbuf[3], addr, buf[0]*2);					
					}
					break;

					case BAT_R_DATA_SECT:	   /*�����������*/	
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_InterR[DevAddr], &Rxbuf[3], addr, buf[0]*2);
						if((MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum == 103)&&(DevAddr == 5))
						{//���������һ��ֵΪ0
							MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][12]=(MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][6]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][7]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][8]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][9]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][10]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][11])/6;
						}
						DataErrCheck(batgroup,DevAddr,INNER_R_CHECK,ON);//��������Ƿ��й���
						//CountBatRStat(batgroup);//���������輫ֵ
					}
					break;
					case CONNECT_R_DATA_SECT:	 /*���ӵ������ݶ�*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[DevAddr], &Rxbuf[3], addr, buf[0]*2);
						if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum!=0)
						{//���������һ��ֵΪ0
							MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum-1)/18][(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum-1)%18]=0;
						}
						DataErrCheck(batgroup,DevAddr,CONNECT_R_CHECK,ON);//����������Ƿ��й���
					}
					break;

					break;
					case MEA_DATA_SECT:	   	 /*�������ݶ�*/ 
					{	
						
					}
					break;
					case CAL_PARA_SECT:	   	/*У׼������*/		
					{	
					}
					break;
					case SYS_WORK_STAT_SECT:	   	/*ϵͳ����״̬��*/	
					{	
					}
					break;
					case ASSIST_STAT_SECT:	 /*����״̬��*/ 
					{	
						
					}
					break;
					case SYS_CTRL_SECT://ϵͳ���ƶ�
					
					break;

					case SHIELD_INFO_SECT:

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
BOOL MbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length)
{
//	uint32 overTime = MODBUS_OVERTIME; // 1s
	uint16 ret = FALSE, tmp = 0, i = 0;
	uint8 batgroup = 0;
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
			if(DownDevDeal(com,DecAddr,cmd, startAddr+i*MODBUS_MAX_REG, buf+i*MODBUS_MAX_REG, tmp) == TRUE)
			{
				if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{
					MbsMaster.Bat5kCommErrStartTime[batgroup] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.Bat5kCommErrStatus[batgroup] = 0;	   
					MbsMaster.Bat5kCommErrCount[batgroup] = 0;
					if(ErrFlag.BAT5000SCommErrFlag[batgroup] == BAT5000_S_COM_ERR)
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_RECOVER;
						DataErrCheck(batgroup,DecAddr,BAT5000_S_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = NORMAL;
					}
				}
				else if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//ģ���ַ����1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//�ж��Ƿ���ϻָ�
						SendCfgParaToDev(batgroup,DecAddr-1);
						SendShieldInfoToDev(batgroup,DecAddr-1);
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_RECOVER;
						DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = NORMAL;
					}
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//ģ��ͨ�Ź���
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //ͨѶ���ϼ���
					if(MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//ͨ�Ź��ϣ��������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//�¶�ռ�������Ĵ�����
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_InnerRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_CRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_EQDev = 0xFFFF;

						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_DevERR = COMM_ERR;
						if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] != DEV_COMM_ERR)
						{
							ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_ERR;
							DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
						}
						break;
					}
				}
				else if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{//�ɼ���ͨ�Ź���
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //ͨѶ���ϼ���
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						//LcdComm.Current[batgroup] = 0;//��ŵ������0
						if(ErrFlag.BAT5000SCommErrFlag[batgroup] != BAT5000_S_COM_ERR)
						{
							ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_ERR;
							DataErrCheck(batgroup,DecAddr,BAT5000_S_COMM_CHECK,ON);
						}
					}
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
			if(DownDevDeal(com, DecAddr, cmd, startAddr+i*MODBUS_MAX_REG, &tmp, length) == TRUE)
			{
				if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{
					MbsMaster.Bat5kCommErrStartTime[batgroup] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.Bat5kCommErrStatus[batgroup] = 0;	   
					MbsMaster.Bat5kCommErrCount[batgroup] = 0;
					if(ErrFlag.BAT5000SCommErrFlag[batgroup] == BAT5000_S_COM_ERR)
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_RECOVER;
						DataErrCheck(batgroup,DecAddr,BAT5000_S_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = NORMAL;
					}
				}
				else if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//���ǲɼ��У���ģ���ַ����1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//�ж��Ƿ���ϻָ�
						SendCfgParaToDev(batgroup,DecAddr-1);
						SendShieldInfoToDev(batgroup,DecAddr-1);
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_RECOVER;
						DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = NORMAL;
					}
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//ģ��ͨ�Ź���
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //ͨѶ���ϼ���
					if (MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//ͨ�Ź��ϣ��������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//�¶�ռ�������Ĵ�����
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_InnerRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_CRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_EQDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_DevERR = COMM_ERR;
						if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] != DEV_COMM_ERR)
						{
							ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_ERR;
							DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
						}
						break;
					}
				}
				else if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{//�ɼ���ͨ�Ź���
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //ͨѶ���ϼ���
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						//LcdComm.Current[batgroup] = 0;//��ŵ������0
						if(ErrFlag.BAT5000SCommErrFlag[batgroup] != BAT5000_S_COM_ERR)
						{
							ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_ERR;
							DataErrCheck(batgroup,0,BAT5000_S_COMM_CHECK,ON);
						}
					}
				}
			} 
		}
	}
	else
	{
		if(DownDevDeal(com, DecAddr, cmd, startAddr, buf, length) == TRUE)
			{
				if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{
					MbsMaster.Bat5kCommErrStartTime[batgroup] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.Bat5kCommErrStatus[batgroup] = 0;	   
					MbsMaster.Bat5kCommErrCount[batgroup] = 0;
					if(ErrFlag.BAT5000SCommErrFlag[batgroup] == BAT5000_S_COM_ERR)
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_RECOVER;
						DataErrCheck(batgroup,DecAddr,BAT5000_S_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.BAT5000SCommErrFlag[batgroup] = NORMAL;
					}
				}
				else if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//���ǲɼ��У���ģ���ַ����1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//�ж��Ƿ���ϻָ�
						SendCfgParaToDev(batgroup,DecAddr-1);
						SendShieldInfoToDev(batgroup,DecAddr-1);
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_RECOVER;
						DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
					}
					else
					{
						ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = NORMAL;
					}
				}
				ret = TRUE;
			}
			else
			{
				ret = FALSE; 
				if((DecAddr > 0)&&(DecAddr <= MAX_DEV_NUM))
				{//ģ��ͨ�Ź���
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //ͨѶ���ϼ���
					if (MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//ͨ�Ź��ϣ��������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//�¶�ռ�������Ĵ�����
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_InnerRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_CRDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_EQDev = 0xFFFF;
						MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_DevERR = COMM_ERR;
						if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] != DEV_COMM_ERR)
						{
							ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] = DEV_COMM_ERR;
							DataErrCheck(batgroup,DecAddr-1,DEV_COMM_CHECK,ON);
						}
					}
				}
				else if(DecAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
				{//�ɼ���ͨ�Ź���
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //ͨѶ���ϼ���
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//ͨ��ʧ�ܣ������ģ��Ļ�������
						//LcdComm.Current[batgroup] = 0;//��ŵ������0
						if(ErrFlag.BAT5000SCommErrFlag[batgroup] != BAT5000_S_COM_ERR)
						{
							ErrFlag.BAT5000SCommErrFlag[batgroup] = BAT5000_S_COM_ERR;
							DataErrCheck(batgroup,DecAddr,BAT5000_S_COMM_CHECK,ON);
						}
					}
				}
			} 
		}
	return ret;
}


/*****************************************************************************************/
/*                       end of file                                                     */
/*****************************************************************************************/

