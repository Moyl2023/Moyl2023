/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.c
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: 主机与显示Modbus通讯
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
**   功能: 串口主设备接收处理
**   参数: com--串口ID buf--接收数据暂存区，MaxSize--一次允许处理的数据大小 timeout--超时间隔 
**   输出: TRUE 正常 FALSE 失败 
**   描述: 接收串口数据
**************************************************************************************/
static uint8 BkgMbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
{
	// 接收完成处理方法(转发没有数据帧格式，无法判断数据的长度)：
	// 1、一个字节数据都没有接收到，超时时间超过timeout
	// 2、接收数据后，如果在RCV_OVER_TIME毫秒没有继续受到数据，认为数据已经接收完成 
	uint8 rcvLen = 0, tmpLen = 0;
	uint32 StartTime = JdyTest.DataYc.g_TimerMs;
	uint8 bRcv = 0;	   // 是否接收到过数据
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
			  if(JdyTest.DataYc.g_TimerMs-lastTime>RCV_OVER_TIME)//超过20MS没有收到数据认为接收完成
			      break;
		   }
		}
		OSTimeDlyHMSM(0,0,0,20);
	}while(JdyTest.DataYc.g_TimerMs - StartTime < timeout);	  // 超时范围内，超过300MS，表示一帧数据接收完成

	return rcvLen;
}
/*************************************************************************************
**  static uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
**   功能: 查找地址所在数据段
**   参数: startAddr--数据起始地址   RegNum--寄存器数量
**   输出:  
**   描述:  
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
**   功能: modbus数据打包
**   参数: length--寄存器数量
**   输出: 数据包长度 
**   描述: 读命令需将length设置为1
**************************************************************************************/
static uint16 BkgModbusPackage(uint8 DevAddr, uint8 *txbuf,uint8 cmd, uint16 startAddr, uint16 *buf, uint8 length)
{
	uint16 crc = 0, i, ret;
	//uint8 DevAddr = MBSSLAVE_ADDRESS, count = 0;
	uint8 count = 0;
	
	txbuf[count++] = DevAddr;                            /*设备地址*/
	txbuf[count++] = cmd;                                /*功能码*/
	txbuf[count++] = (uint8)(startAddr >> 8);
	txbuf[count++] = (uint8)(startAddr & 0x00ff);        /*起始地址，大端发送*/
	//if((cmd == MODBUS_WR_MULT_REG)||(cmd == MODBUS_WR_SINGLE_REG))
	if(cmd == MODBUS_WR_MULT_REG)
	{												
		txbuf[count++] = 0;
		txbuf[count++] = length;                         /*寄存器数量*/
		txbuf[count++] = length*2;                       /*数据长度*/	
	}
	for(i = 0; i < length; i++)
	{
		txbuf[count+i*2] = (uint8)(buf[i] >> 8);
		txbuf[count+i*2+1] = (uint8)(buf[i] & 0x00ff);   /*数据大端发送*/
	}
	crc = ModbusCRC(txbuf, count+length*2);
	txbuf[count+length*2+1] = (uint8)(crc >> 8);
	txbuf[count+length*2] = (uint8)(crc & 0x00ff);       /*CRC小端发送*/
	ret = count+length*2+2;                              /*数据包总长度*/   
	
	return ret;
}

static void BkgFixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i;

	for(i = 0; i < Lenth; i++)
	{
		if(i%2)                                          /*小端存储*/
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
// 描述：读写下行设备数据
// 参数：com:通信端口， DevAddr:设备地址 0~23， cmd:命令码 查询和设置，	startAddr:查询寄存器起始地址, 
// data:查询时代表查询数据个数(寄存器个数)，设置时代表设置的数据值	 com:串口号
// 返回：成功 T 失败 F 
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
		if (length > MODBUS_MAX_REG)                                   /*查询寄存器数量不能超过125个*/
		{
			return FALSE;
		}
	}
	SectAddr = BkgIrqSectAddrMaster(StartAddr,length);                    /*段地址*/
	if (SectAddr == NON_SECT)                                          /*段地址错误*/
	{
		return FALSE;
	}
	else
	{
		ovtime = MODBUS_COMM_TIME_OUT*3;                                 /*正常通讯超时时间*/
	}
	
	addr = (StartAddr - SSectAddrAndData[SectAddr].LowRegAddr)*2;       /*在段中的偏移地址*/	 
	
	do 
	{	
	 	if(Uart_Read(UartHandler[com], Rxbuf, 32) <= 0)
			break;
	}while(1);                                                         /* 读空数据缓存区 */

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
	if(DevAddr != BROADCAST_ADDR)//广播地址没有回复，
	{
		rcvLen = BkgMbsSlaveLinkReadFrame(Rxbuf, COMM_BUF_SIZE, ovtime, com); /*读取返回数据*/	   
		if (rcvLen > 3)
		{
			if (DevAddr != Rxbuf[0])                                       /* 确认设备地址 */
			{
				return FALSE;
			}
			if (cmd != Rxbuf[1])                                           /*确认功能码*/
			{
				return FALSE;
			}
			if(cmd == MODBUS_RD_HOLD_REG)																	/*确认数据长度*/
			{
				if(rcvLen > (Rxbuf[2] + 5))
				{
					rcvLen = (Rxbuf[2] + 5);
				}
			}
			memcpy(&rcvCrc,&Rxbuf[rcvLen-2], 2);                           /*校验CRC*/
			if (ModbusCRC(Rxbuf, rcvLen-2) != rcvCrc)
			{
				return FALSE;
			}

			if (cmd == MODBUS_RD_HOLD_REG)
			{
				if(DevAddr > 0)
				{//模块编号从1开始，存储从0开始，1号模块存储在0号位置
					DevAddr = DevAddr - 1;
				}
				switch(SectAddr)
				{ 
					case S_BAT_V_DATA_SECT:	    /*电池电压数据段*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_BatV[0][0], &Rxbuf[3], addr, buf[0]*2);					
						//DataErrCheck(batgroup,DevAddr,VOLTAGE_CHECK,ON);//检测电压是否有故障
						//LcdComm.RdDevFlag[batgroup] = OFF;//表示该模块通信结束，可以进行下一个模块的电池数据读取
					}
					break;
					case S_BAT_R_DATA_SECT:	   /*电池内阻数段*/	
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_InterR[0][0], &Rxbuf[3], addr, buf[0]*2);
						//DataErrCheck(batgroup,DevAddr,INNER_R_CHECK,ON);//检测内阻是否有故障
						CountBatRStat(batgroup);//计算电池内阻最大，最小值
						CountBatCRStat(batgroup);
						//LcdComm.InnerTest.RDataReadFlag[batgroup][DevAddr] = ON;//数据读取成功标志
					}
					break;	
					case S_CONNECT_R_DATA_SECT:	 /*连接电阻数据段*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[0][0], &Rxbuf[3], addr, buf[0]*2);
						//DataErrCheck(batgroup,DevAddr,CONNECT_R_CHECK,ON);//检测连接条是否有故障
						CountBatRStat(batgroup);//计算电池内阻最大，最小值
						CountBatCRStat(batgroup);
						//LcdComm.InnerTest.CRDataReadFlag[batgroup][DevAddr] = ON;//连接条数据读取成功标志
					}
					break;
					case S_BAT_TEMP_DATA_SECT:	    /*电池温度数据段*/
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
						//DataErrCheck(batgroup,DevAddr,TEMP_CHECK,ON);//检测温度是否有故障
					}
					break;
					case S_V_DATA_DEC_SECT:	    /*电池电压小数数据段*/
					{
						BkgFixReadData((uint8*)&MbsSlave.BatData[batgroup].DevMeaData.S_VDec[0][0], &Rxbuf[3], addr, buf[0]*2);					
					}
					break;
					case S_DEV_STAT_SECT:	  	/*模块运行状态段*/		   
					{	
						BkgFixReadData((uint8*)&MbsSlave.StatusData.MoudleStat[batgroup][0].S_Temp, &Rxbuf[3], addr, buf[0]*2);					
					}
					break;	
					case S_MSYS_CFG_SECT:											/*系统配置段*/	
					{	
						BkgFixReadData((uint8*)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, &Rxbuf[3], addr, buf[0]*2);								
					}
					break;					
					case S_BAT_ERR_STAT_SECT:										/*电池故障状态段*/
					{	
						BkgFixReadData((uint8*)&MbsSlave.StatusData.BatErrStat[batgroup].S_Quantity, &Rxbuf[3], addr, buf[0]*2);					
					}
					break;
 					case S_GROUP_DATA_SECT:											/*采集盒整组数据段*/
					{	
						BkgFixReadData((uint8*)&MbsSlave.Group[batgroup].SysMeaData.GroupV, &Rxbuf[3], addr, buf[0]*2);	
						//LcdComm.Current[batgroup] = (float)MbsSlave.Group[batgroup].SysMeaData.GroupI/10;
						//LcdComm.BatSysStat.GroupV[batgroup] = (float)MbsSlave.Group[batgroup].SysMeaData.GroupV/10;
						
					}
					break;
					case S_MBS_SINGNAL_SECT:											/*MODBUS遥信数据段*/
					case S_MEA_DATA_SECT:											/*测量数据段*/
					case S_CAL_PARA_SECT:											/*校准参数段*/
					case S_ASSIST_STAT_SECT:											/*辅助状态段*/
					case S_SYS_CTRL_SECT:											/*系统控制段*/
					case S_SYS_TIME_SECT:											/*系统时钟段*/
					case S_COM_PARA_SECT:											/*通讯参数段*/
					case S_GROUP_CAL_SECT:											/*采集盒校准数据段*/
					case S_GROUP_PARA_SECT:											/*采集盒参数数据段*/
					case S_GROUP_STAT_SECT:											/*采集盒状态数据段*/
					case S_R_RATIO_SECT:												/*内阻校准系统*/
					case S_TEMP_SET_SECT:											/*温度探头ID设置段*/
					case S_TEMP_EXID_SECT:											//温度探头自定义ID设置段
					case S_HISTORY_RECORD_SECT:										/*历史记录段*/
					case S_USART_TEST_SECT:											/*串口测试段*/	
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
**   功能: 串口主设备查询从设备
**   参数: cmd--命令码  startAddr--操作起始地址
buf--读取时表示读取寄存器数量，写入时表示要写入的数据，
length--读取时值为1，写入时表示要写入的寄存器数量
**   输出: 失败或成功标志
**   描述: 寄存器数量大于123个，分包操作
**************************************************************************************/
BOOL BkgMbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length)
{
//	uint32 overTime = MODBUS_OVERTIME; // 1s
	uint16 ret = FALSE, tmp = 0, i = 0;
//	uint8 batgroup = 0;
	uint8 ReadSum;//读取寄存器数量
	uint16 WRSum;
	if(cmd == MODBUS_WR_MULT_REG)
	{
		WRSum = length;
		for(i = 0; 0 < WRSum; i++)
		{
			if(WRSum > MODBUS_MAX_REG)                                /*数据包长度大于最大包,分包发送*/
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
				{//判断是否故障恢复
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
				MbsMaster.BkgCommErrCount++;	  //通讯故障计数
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
				{//判断是否故障恢复
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
				MbsMaster.BkgCommErrCount++;	  //通讯故障计数
				if (MbsMaster.BkgCommErrCount >= COMM_ERR_COUNT)
				{
					MbsMaster.BkgCommErrCount = 0;
					MbsMaster.BkgCommErrStatus = 1;
					if(ErrFlag.BkgCommErrFlag != BKG_COM_ERR)
					{
						ErrFlag.BkgCommErrFlag = BKG_COM_ERR;
						DataErrCheck(0,0,BKG_COMM_CHECK,ON);
					}
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_BatV[0],0,MAX_GROUP_BAT_NUM*2);//通信失败，清除该模块的缓存数据
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_VDec[0],0,MAX_GROUP_BAT_NUM*2);//通信失败，清除该模块的缓存数据
					memset(MbsSlave.BatData[GROUP1].DevMeaData.S_Temp[0],0,MAX_GROUP_BAT_NUM*4);//
					memset(&MbsSlave.StatusData.MoudleStat[GROUP1][0].S_Temp,0,(DEV_STAT_NUM+1)*2);//温度占用两个寄存器，					
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
				{//判断是否故障恢复
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
				MbsMaster.BkgCommErrCount++;	  //通讯故障计数
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
* 描述：解析消息
* 参数：pMsg 消息数据
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
**   功能: 作为MODBUS后台主机时的处理 
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void MasterProcess(uint8 DEVCOM,uint8 GROUP)
{
	struct tm ttm;
	if((JdyTest.DataYc.g_TimerMs - BATComTime.SysStatusTimeMs[GROUP]) > BATComTime.DevStatusReadTime[GROUP])
	{
		BATComTime.SysStatusTimeMs[GROUP] = JdyTest.DataYc.g_TimerMs;
		//LcdComm.DevNUM[GROUP1] = (MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum + BAT_V_DATA_NUM - 1)/BAT_V_DATA_NUM;
		MasterCfgRead(GROUP,DEVCOM,19*2);//读取配置信息,包括电流测量通道配置信息
		if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum != 0)
		{
			MasterBatVRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//读取电池电压
			MasterBatVDecRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//读取电池电压小数部分
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].TempDevStat == ON)
			{
				MasterBatTRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum*2);//读取温度,读取的是浮点数
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].InnerRDevStat == ON)
			{				
				//LcdComm.InnerTest.LastTestTimesFlag[GROUP] = 1;//只显示当前实时值
				//LcdComm.InnerTest.TotalTestTimes[GROUP] = 1;//只显示当前实时值
				ttm = Time_GetCalendarTime(); 
				//LcdComm.InnerTest.TestTime[GROUP].MeaYear  = ttm.tm_year;
				//LcdComm.InnerTest.TestTime[GROUP].MeaMon = ttm.tm_mon;
				//LcdComm.InnerTest.TestTime[GROUP].MeaDay = ttm.tm_mday;
				//LcdComm.InnerTest.TestTime[GROUP].MeaHour = ttm.tm_hour;
				//LcdComm.InnerTest.TestTime[GROUP].MeaMin = ttm.tm_min;
				//LcdComm.InnerTest.TestTime[GROUP].MeaSec = ttm.tm_sec;
				MasterBatRRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//读取内阻
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].CRDevStat == ON)
			{
				MasterBatCRRead(GROUP,DEVCOM,MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP].S_CellNum);//读取连接条阻值
			}
			if(MbsSlave.StatusData.BatErrStat[GROUP].S_Quantity == 0)
			{//只读取故障数量
				MasterErrStatRead(GROUP,DEVCOM,1);
			}
			else
			{//读取故障数量和故障信息
				MasterErrStatRead(GROUP,DEVCOM,MbsSlave.StatusData.BatErrStat[GROUP].S_Quantity*2+1);
			}
		}
	}
}

/*****************************************************************************************/
/*                       end of file                                                     */
/*****************************************************************************************/

