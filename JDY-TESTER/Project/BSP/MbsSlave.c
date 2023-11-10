/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsSlave.c
** Created By: Abe
** Last modified Date: 2014-8-5
** Last Version: 1.0
** Descriptions: 分机与主机Modbus通讯
****************************************************************************************/
#include <includes.h>
#include "crc.h"
#include "rtc.h"
#include "Uart.h"
#include "MbsSlave.h"
#include "ModbusDef.h"
#include "appstart.h"

MBS_SLAVE MbsSlave;


UART_RCV_CFG     UartRcvCfg[5];                      /*用于处理已接收串口数据*/


uint8 tempbuff[8] = {0};


/* 段区地址和寄存器地址定义 */
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


/* 发送modbus错误响应 */
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
**   功能: 计算已接收数据包长度
**   参数: buf--数据接收完成后缓存区 
**   输出: ret-接收完成标志    buf-数据接收缓存起始地址 
**   描述: 
**************************************************************************************/
static uint8 GetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8 i, len, *ptr, ret = RCV_UNDONE;
	
	if(pchan->p_head >= pchan->p_tail)
	{
		len = pchan->p_head - pchan->p_tail;           /* 已接收的数据长度 */
	}
	else
	{
		len = pchan->p_head + RCV_QUEUE_SIZE - pchan->p_tail;
	}

	if(len >= 8)                                       /* modubs一帧数据最小长度为8个字节 */                                       
	{
		ptr = pchan->p_tail;                     
		for(i = 0; i < 8; i++)                         /*数据转入处理队列*/
		{
			buf[i] = *ptr;
			if(++ptr >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
			{
				ptr = pchan->rcvbuf;                   /* 循环队列 */
			}
		}
		
		if(buf[1] == MODBUS_WR_MULT_REG)               /*modbus写多个寄存器命令码*/
		{
			if(len >= (buf[6]+9))                      /*modbus写多个寄存器数据包已缓存*/
			{
				for(i = 8; i < (buf[6]+9); i++)        /*数据转入处理队列*/
				{
					buf[i] = *ptr;
					if(++ptr >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
					{
						ptr = pchan->rcvbuf;           /* 循环队列 */
					}
				}
				pchan->p_tail = ptr;                   /*数据包接收完成*/
				ret = RCV_OK;
			}
			else                            
			{
				ret = RCV_UNDONE;                      /*数据包接收未完成*/
			}
		}
		else                                           /*modbus查询或写单个寄存器命令码,一包数据长度为8*/
		{
			pchan->p_tail = ptr;       
			ret = RCV_OK;                              /*数据包接收完成*/  
		}
	}

	return ret;
}
/*************************************************************************************
**  uint8 IrqSectAddrMaster(uint16 startAddr,uint8 RegNum)
**   功能: 查找数据段
**   参数:  
**   输出:  
**   描述: 
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
**   功能: 数据打包发送
**   参数: txbuf--需发送的数据起始地址  com--串口ID  length--发送字节长度
**   输出: 
**   描述: 
**************************************************************************************/
static void ModbusPackage(uint8 com,uint8 *txbuf,uint8 length)
{
	uint16 crc = 0;

	crc = ModbusCRC(txbuf, length);
	
	txbuf[length++] = (uint8)(crc & 0x00ff);     /*CRC小端发送*/
	txbuf[length++] = (uint8)(crc >> 8);

	Uart_Write(UartHandler[com], txbuf, length);
}
/*************************************************************************************
**  void FixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   功能:  接收数据存储
**   参数:  src--modbus中的段地址   buf--需存储的数据起始地址  startAddr--段地址中的偏移地址
**   输出:  
**   描述: 
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
/*复制投电阻后的母线数据,用于不平衡桥计算*/
//static void FillBusTestVol(uint8 com)
//{
	//if(DevLocalData.DevLocalCfg.DevSysCfg.LinkModel == 1) 
	//{/*双主模式,COM2为二段数据*/
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
**   功能: 响应modbus命令
**   参数: com--串口ID  buf--接收缓存区  sectno--数据段编号  
**         addroffset--操作地址在数据段中的偏移量  regnum--操作的寄存器数量
**   输出: modbus成功或失败代号 
**   描述: 
**************************************************************************************/
uint8 ModbusRespond(uint8 com,uint8 *buf, uint8 SectNo, uint16 addrOffset, uint16 regNum)
{
	uint8 txlen = 0, ret = RCV_OK, dataOffset = 4;  /*写单寄存器命令，填充数据在MODBUS数据包中的偏移地址*/
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
				case S_BAT_V_DATA_SECT: /*电池电压数据段*/

					break;
				case S_V_DATA_DEC_SECT: /*电池电压数据段*/

					break;
				case S_BAT_R_DATA_SECT:/*电池内阻数段*/	

					break;
				case S_CONNECT_R_DATA_SECT: /*连接电阻数据段*/

					break;
				case S_BAT_TEMP_DATA_SECT:/*电池温度数据段*/

					break;
				case S_BAT_ERR_STAT_SECT:/*电池故障状态段*/ 

					break;
				case S_MBS_SINGNAL_SECT:/*MODBUS遥信数据段*/

					break;
				case S_DEV_STAT_SECT:/*模块运行状态段*/

					break;
				case S_MEA_DATA_SECT:/*测量数据段*/ 

					break;
				case S_CAL_PARA_SECT:	/*校准参数段*/		

					break;
				case S_GROUP_DATA_SECT:/*整组参数段*/	

					break;
				case S_SYS_WORK_STAT_SECT:/*系统工作状态段*/

					break;
				case S_ASSIST_STAT_SECT:/*辅助状态段*/ 

					break;
				case S_MSYS_CFG_SECT: /*系统配置段*/ 
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
				case S_SYS_TIME_SECT: /*系统时钟段*/
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
				case S_COM_PARA_SECT: /*通讯参数段*/

					break;
				case S_HISTORY_RECORD_SECT:/*历史记录段*/

					break;
				case S_USART_TEST_SECT:/*串口测试段*/

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
				case S_CAL_PARA_SECT:									/*校准参数设置**/
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
					
				case S_MSYS_CFG_SECT:										/*系统参数设置**/
					//FixReadData((uint8*)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, buf+dataOffset, addrOffset, regNum*2);
					//if(Eeprom_Write_I2c((uint8 *)(&MbsSlave.SysPara.MSysCfg.S_CellGroupNum+addrOffset), SYS_CFG_SAVE_OFFSET + addrOffset, BKP_SYS_CFG_SAVE_OFFSET + addrOffset,regNum*2)==FALSE)
					//{
					//	ret = RCV_REG_DATA_SAVE_ERR;
					//}
					break;
				case S_SYS_TIME_SECT:										/*系统时间设置**/
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
				case S_COM_PARA_SECT:										/*各个通信接口参数配置**/

					break;
				case S_SYS_CTRL_SECT:											/*启动电池，内阻，温度，连接条巡检功能*/
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

	if((ret == RCV_OK) && (buf[0] != BROADCAST_ADDR))               /*广播命令不回复*/
	{
		ModbusPackage(com, buf, txlen);                             /*响应主机已接收完成*/
	}
	
	return ret;
}
/*************************************************************************************
**  uint8  CheckRcvModbusData(uint8 *buf)
**   功能: 校验数据包
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8  CheckRcvModbusData(uint8 com, uint8 *buf)
{	
	uint8  SectNo, RcvLen, ret = RCV_OK;
	uint16 StartAddr = 0, RegNum = 0, addrOffset;

	if(buf[1] == MODBUS_RD_HOLD_REG)             /* 读寄存器 */	
	{
		RcvLen = 8;
		RegNum = (buf[4]<<8) | buf[5];
		if(RegNum > MODBUS_MAX_READ_REG)
		{
			return RCV_REG_NUM_ERR;
		}
	}
	else if(buf[1] == MODBUS_WR_SINGLE_REG)      /* 写单个寄存器 */
	{ 
		RegNum = 1;
		RcvLen = 8;
	}
	else if(buf[1] == MODBUS_WR_MULT_REG)        /* 写多个寄存器 */
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
		return  RCV_FUCTION_ERR;                 /* 无效的命令 */
	}

	if(ModbusCRC(buf,RcvLen))
	{
		ret = RCV_CRC_ERR;                       /* 校验出错 */
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
			addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*在段中的偏移地址*/	
			ret = ModbusRespond(com,buf,SectNo,addrOffset,RegNum);                /*数据包命令判断,接收正确后应答*/
		}
	}
	
	return ret;
}
/*************************************************************************************
**  uint8  SlaveCheckRcvDone(UART_RCV_CFG *pchan, uint8 DevAddr, uint8 *buf)
**   功能: 串口缓存区数据转入处理队列
**   参数: pchan--当前串口通道所用的接收数据结构  devaddr--设备地址  buf--接收缓存区  
**   输出:  接收完成标志
**   描述: 
**************************************************************************************/
uint8  SlaveCheckRcvDone(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8  ret = RCV_UNDONE;
	while(pchan->p_tail != pchan->p_head)                                         /*有新数据入队*/
	{
		if((*(pchan->p_tail) == 0) || (*(pchan->p_tail) == BROADCAST_ADDR))/*设备地址正确,开始接收*/
		{
			ret = GetRcvLen(pchan,buf);                                           /*判断一帧数据是否接收完*/

			break;
		}
		else
		{
			if(++(pchan->p_tail) >= (pchan->rcvbuf+RCV_QUEUE_SIZE)) 
			{
				pchan->p_tail = pchan->rcvbuf;                                     /* 循环队列 */
			}
		}
	}
	return ret;
}
/*************************************************************************************
**  void MbsSlaveProcess(uint8 com)
**   功能: 串口从设备接收处理
**   参数: com--串口ID
**   输出:  
**   描述: 读取已接收的串口数据,转存至数据处理数组中并处理
**************************************************************************************/
void MbsSlaveProcess(uint8 com)
{
	uint8 rcvLen = 0, ret = RCV_UNDONE;
	uint8 rcvtmpbuf[MAX_UART_BUF_LENTH];

	rcvLen = Uart_Read(UartHandler[com], UartRcvCfg[com].p_head, UartRcvCfg[com].rcvbuf+RCV_QUEUE_SIZE-UartRcvCfg[com].p_head);
	if(rcvLen > 0)                                                /* 接收到新数据 */
	{
		UartRcvCfg[com].FailCount = 0;
		UartRcvCfg[com].p_head += rcvLen;
		if(UartRcvCfg[com].p_head >= (UartRcvCfg[com].rcvbuf + RCV_QUEUE_SIZE)) 
		{
			UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;      /* 循环队列接收 */
		}
		/* 判断是否接收完成 */
		if(RCV_OK == SlaveCheckRcvDone(&UartRcvCfg[com],rcvtmpbuf))
		{
			ret = CheckRcvModbusData(com,rcvtmpbuf);              /* 校验与处理命令 */
//			SysFlag.LedFlag.LED_COMM_SALAVE = ON;					/*通信灯亮*/
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
					if(rcvtmpbuf[0] != BROADCAST_ADDR)            /* 广播命令不回复 */
					{
						//ModbusErrRespond(com, rcvtmpbuf[1], ret); /* 回复modbus错误码 */
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
			UartRcvCfg[com].p_tail = UartRcvCfg[com].p_head;      /* 通讯超时,复位接收指针 */
		}
	}
}
/***************************************************************************************/
/*                   End Of File                                                       */
/***************************************************************************************/

