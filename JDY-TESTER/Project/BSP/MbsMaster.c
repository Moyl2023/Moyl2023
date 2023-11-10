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
#include "MbsMaster.h"
#include "MbsSlave.h"
#include "ModbusDef.h"
#include "appstart.h"
#include "message.h"

MBS_MASTER  MbsMaster;	           

/* 段区地址和寄存器地址定义 */
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
**   功能: 串口主设备接收处理
**   参数: com--串口ID buf--接收数据暂存区，MaxSize--一次允许处理的数据大小 timeout--超时间隔 
**   输出: TRUE 正常 FALSE 失败 
**   描述: 接收串口数据
**************************************************************************************/
static uint8 MbsSlaveLinkReadFrame(uint8 *buf, uint8 MaxSize, uint32 timeout, uint8 com)
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
**   功能: modbus数据打包
**   参数: length--寄存器数量
**   输出: 数据包长度 
**   描述: 读命令需将length设置为1
**************************************************************************************/
static uint16 ModbusPackage(uint8 DevAddr, uint8 *txbuf,uint8 cmd, uint16 startAddr, uint16 *buf, uint8 length)
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

static void FixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
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
		if (length > MODBUS_MAX_REG)                                   /*查询寄存器数量不能超过125个*/
		{
			return FALSE;
		}
	}
	SectAddr = IrqSectAddrMaster(StartAddr,length);                    /*段地址*/
	if (SectAddr == NON_SECT)                                          /*段地址错误*/
	{
		return FALSE;
	}
	else
	{
		ovtime = MODBUS_COMM_TIME_OUT;                                 /*正常通讯超时时间*/
		if(DevAddr == MbsSlave.Group[batgroup].SysPara.DevAddr)
		{
			ovtime = MODBUS_COMM_TIME_OUT*2;		//采集响应速度慢
		}
	}
	
	addr = (StartAddr - SectAddrAndData[SectAddr].LowRegAddr)*2;       /*在段中的偏移地址*/	 
	
	do 
	{	
	 	if(Uart_Read(UartHandler[com], Rxbuf, 32) <= 0)
			break;
	}while(1);                                                         /* 读空数据缓存区 */

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
	if(DevAddr != BROADCAST_ADDR)//广播地址没有回复，
	{
		rcvLen = MbsSlaveLinkReadFrame(Rxbuf, COMM_BUF_SIZE, ovtime, com); /*读取返回数据*/	   
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
					case BAT_V_DATA_SECT:	    /*电池电压数据段*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DevAddr], &Rxbuf[3], addr, buf[0]*2);					
						//memset(TempV,0,MAX_BAT_NUM);
						//FixReadData((uint8*) TempV, &Rxbuf[3], addr, buf[0]*2);
						//for(i=0;i<MAX_BAT_NUM;i++)
						//{//通信单位是0.1mV，测出来的数据要转换成mV
						//	MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DevAddr] = ((TempV[i]&0x8000)!=0)?(TempV[i]&0x7FFF):(float)TempV[i]/10;
						//}
						DataErrCheck(batgroup,DevAddr,VOLTAGE_CHECK,ON);//检测电压是否有故障
					}
					break;
					case V_DATA_DEC_SECT:	    /*电池电压小数数据段*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DevAddr], &Rxbuf[3], addr, buf[0]*2);					
					}
					break;

					case BAT_R_DATA_SECT:	   /*电池内阻数段*/	
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_InterR[DevAddr], &Rxbuf[3], addr, buf[0]*2);
						if((MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum == 103)&&(DevAddr == 5))
						{//连接条最后一个值为0
							MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][12]=(MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][6]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][7]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][8]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][9]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][10]+MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[5][11])/6;
						}
						DataErrCheck(batgroup,DevAddr,INNER_R_CHECK,ON);//检测内阻是否有故障
						//CountBatRStat(batgroup);//计算电池内阻极值
					}
					break;
					case CONNECT_R_DATA_SECT:	 /*连接电阻数据段*/
					{
						FixReadData((uint8*) MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[DevAddr], &Rxbuf[3], addr, buf[0]*2);
						if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum!=0)
						{//连接条最后一个值为0
							MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum-1)/18][(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum-1)%18]=0;
						}
						DataErrCheck(batgroup,DevAddr,CONNECT_R_CHECK,ON);//检测连接条是否有故障
					}
					break;

					break;
					case MEA_DATA_SECT:	   	 /*测量数据段*/ 
					{	
						
					}
					break;
					case CAL_PARA_SECT:	   	/*校准参数段*/		
					{	
					}
					break;
					case SYS_WORK_STAT_SECT:	   	/*系统工作状态段*/	
					{	
					}
					break;
					case ASSIST_STAT_SECT:	 /*辅助状态段*/ 
					{	
						
					}
					break;
					case SYS_CTRL_SECT://系统控制段
					
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
**   功能: 串口主设备查询从设备
**   参数: cmd--命令码  startAddr--操作起始地址
buf--读取时表示读取寄存器数量，写入时表示要写入的数据，
length--读取时值为1，写入时表示要写入的寄存器数量
**   输出: 失败或成功标志
**   描述: 寄存器数量大于123个，分包操作
**************************************************************************************/
BOOL MbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length)
{
//	uint32 overTime = MODBUS_OVERTIME; // 1s
	uint16 ret = FALSE, tmp = 0, i = 0;
	uint8 batgroup = 0;
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
				{//模块地址，减1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//判断是否故障恢复
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
				{//模块通信故障
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //通讯故障计数
					if(MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//通信故障，清除数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//温度占用两个寄存器，
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
				{//采集盒通信故障
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //通讯故障计数
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//通信失败，清除该模块的缓存数据
						//LcdComm.Current[batgroup] = 0;//充放电电流清0
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
				{//不是采集盒，是模块地址，减1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//判断是否故障恢复
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
				{//模块通信故障
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //通讯故障计数
					if (MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//通信故障，清楚数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//温度占用两个寄存器，
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
				{//采集盒通信故障
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //通讯故障计数
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//通信失败，清除该模块的缓存数据
						//LcdComm.Current[batgroup] = 0;//充放电电流清0
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
				{//不是采集盒，是模块地址，减1
					MbsMaster.CommErrStartTime[batgroup][DecAddr-1] = JdyTest.DataYc.g_TimerMs;
					MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 0;	   
					MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
					if(ErrFlag.DevCommErrFlag[batgroup][DecAddr-1] == DEV_COMM_ERR)
					{//判断是否故障恢复
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
				{//模块通信故障
					MbsMaster.CommErrCount[batgroup][DecAddr-1]++;	  //通讯故障计数
					if (MbsMaster.CommErrCount[batgroup][DecAddr-1] >= COMM_ERR_COUNT)
					{
						MbsMaster.CommErrCount[batgroup][DecAddr-1] = 0;
						MbsMaster.CommErrStatus[batgroup][DecAddr-1] = 1;
						//通信故障，清楚数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_VDec[DecAddr-1],0,MAX_BAT_NUM*2);//通信失败，清除该模块的缓存数据
						memset(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[DecAddr-1],0,MAX_BAT_NUM*4);//
						memset(&MbsSlave.StatusData.MoudleStat[batgroup][DecAddr-1].S_Temp,0,(DEV_STAT_NUM+1)*2);//温度占用两个寄存器，
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
				{//采集盒通信故障
					MbsMaster.Bat5kCommErrCount[batgroup]++;	  //通讯故障计数
					if (MbsMaster.Bat5kCommErrCount[batgroup] >= COMM_ERR_COUNT)
					{
						MbsMaster.Bat5kCommErrCount[batgroup] = 0;
						MbsMaster.Bat5kCommErrStatus[batgroup] = 1;
						memset((uint8 *)&MbsSlave.Group[batgroup].MeaData.VoltageValue,0,GROUP_DATA_NUM*2);//通信失败，清除该模块的缓存数据
						//LcdComm.Current[batgroup] = 0;//充放电电流清0
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

