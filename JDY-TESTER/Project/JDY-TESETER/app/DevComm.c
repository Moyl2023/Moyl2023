/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特迅电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppUART.c
** Created By: abe
** Last modified Date:  2016-03-16
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               主机显示通信任务
****************************************************************************************/
#include <ucos_ii.h>
#include "DevComm.h"
#include "MbsMaster.h"
#include "MbsSlave.h"
#include "Message.h"
#include "uart.h"
#include "appstart.h"
#include "stm32f10x_usart.h"
#include "stdlib.h"
/*****************************************************************************************************
* BOOL UartMessageDeal(MSG_STRUCT* pMsg) 
* 描述：解析消息
* 参数：pMsg 消息数据
******************************************************************************************************/
BOOL UartMessageDeal(MSG_STRUCT* pMsg) 
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
					if(MbsMasterLoop(pMsg->MsgCOM, pMsg->MsgDevAddr, pMsg->MsgData[0], ((uint16)pMsg->MsgData[1]<<8)+pMsg->MsgData[2],buff, (uint16)pMsg->MsgData[3])== TRUE)
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
					if(MbsMasterLoop(pMsg->MsgCOM, pMsg->MsgDevAddr, pMsg->MsgData[0], ((uint16)pMsg->MsgData[1]<<8)+pMsg->MsgData[2],(uint16 *)&pMsg->MsgData[4], (uint16)pMsg->MsgData[3]) == TRUE)
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

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
int fputc(int ch, FILE *f)
{
#if 0
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(UART4, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
  {}
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
  #endif
  return ch;
}
/***************************************************************************
**  uint8 SOCCount(uint8 batgroup,uint16 Vmin)
**   功能: SOC估算
**   参数: 
**   输出:  SOC估算值，单位百分号
**   描述: 
****************************************************************************/
void SOCCount(uint8 batgroup,uint16 Vmin)
{
	float OldSocValue;
	uint32 DeltTime;
	float TempCur = 0;
	OldSocValue = MbsSlave.Group[batgroup].SysMeaData.Soc;
/**********************开路电压法 + 安时积分法计算SOC******************************/
	if((g_TimerMs - BATComTime.SocCountTimeMs[batgroup])>1000)
	{
		if(MbsSlave.SysPara.MSysCfg.CurrentChannel[batgroup] != NO_SENSOR)//有电流采集
		{//如果
			DeltTime = g_TimerMs - BATComTime.SocCountTimeMs[batgroup];
			MbsSlave.Group[batgroup].SysMeaData.Soc = (MbsSlave.Group[batgroup].SysMeaData.Soh*MbsSlave.Group[batgroup].SysMeaData.Soc + LcdComm.Current[batgroup]*DeltTime/36000)/MbsSlave.Group[batgroup].SysMeaData.Soh;
			BATComTime.SocCountTimeMs[batgroup] = g_TimerMs;
		}
		else if(MbsSlave.SysPara.MSysCfg.CurrentChannel[batgroup] == NO_SENSOR)//无电流采集
		{
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 0)
			{// 2V电池
				MbsSlave.Group[batgroup].SysMeaData.Soc = (float)Vmin*100/2250;
			}
			else if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 1)
			{// 6V电池
				MbsSlave.Group[batgroup].SysMeaData.Soc = (float)Vmin*100/67500;
			}
			else if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 2)
			{// 12V电池
				MbsSlave.Group[batgroup].SysMeaData.Soc = (float)Vmin*100/13500;
			}
		}
	}
	if(MbsSlave.Group[batgroup].SysMeaData.Soc > 100)
	{
		MbsSlave.Group[batgroup].SysMeaData.Soc = 100;
	}
	else if(MbsSlave.Group[batgroup].SysMeaData.Soc < 0)
	{
		MbsSlave.Group[batgroup].SysMeaData.Soc = 0;
	}
	if(LcdComm.Current[batgroup] < -3)
	{//如果处于放电状态，则计算SOF，功能状态,小于-3A的时候才计算
		TempCur = LcdComm.Current[batgroup] *(-1);
		MbsSlave.Group[batgroup].SysMeaData.Sof = (MbsSlave.Group[batgroup].SysMeaData.Soc*MbsSlave.Group[batgroup].SysMeaData.Soh/100)/TempCur;
		if(MbsSlave.Group[batgroup].SysMeaData.Sof > 999)
		{
			MbsSlave.Group[batgroup].SysMeaData.Sof = 999;
		}
	}
	else
	{
		MbsSlave.Group[batgroup].SysMeaData.Sof = 999;
	}
	if(abs(OldSocValue - MbsSlave.Group[batgroup].SysMeaData.Soc)>5)
	{//偏移5%保存一次
		switch(batgroup)
		{
			case GROUP1:
				Eeprom_Write((uint8 *)&MbsSlave.Group[batgroup].SysMeaData.Soc, SOC_SAVE1_OFFSET,SOC_SAVE1_NUM);
				break;
			case GROUP2:
				Eeprom_Write((uint8 *)&MbsSlave.Group[batgroup].SysMeaData.Soc, SOC_SAVE2_OFFSET,SOC_SAVE2_NUM);
				break;
		}
	}
}
/***************************************************************************
**  uint8 CheckTestCondition(uint8 batgroup,uint8 timecheckflag)
**   功能: 检测内阻测试条件是否满足
**   参数: timecheckflag:是否需要进行时间间隔检测
**   输出:  满足测试条件，则输出TRUE,不满足测试条件输出FALSE；
**   描述: 
****************************************************************************/
uint8 CheckTestCondition(uint8 batgroup,uint8 timecheckflag)
{
	uint8 ret = TRUE;
	uint8 TempFlag = TEST_IDLE;
	if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].InnerRDevStat != ON)
	{//无内阻测量功能
		TempFlag = TEST_NO_FUNCTION;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}

	if(LcdComm.BatSysStat.BatStat[batgroup] == BATDISCHARGE)
	{//放电中
		TempFlag = TEST_DISCHARGE;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(CheckVlow(batgroup) == FALSE)
	{//电池欠压
		TempFlag = TEST_V_LOW;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(ErrFlag.GroupVoltageErrFlag[batgroup] == GROUP_V_OVER)
	{//组电压过压
		TempFlag = TEST_GROUP_V_OVER;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(ErrFlag.GroupVoltageErrFlag[batgroup] == GROUP_V_LOW)
	{//组电压欠压
		TempFlag = TEST_GROUP_V_LOW;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
    if(((g_TimerMs - BATComTime.BatRLastTestTimeMs[batgroup])<BATComTime.BatRTestTime[batgroup])&&(timecheckflag==ON))
	{//测试频繁,//在启动内阻测试时才需要进行间隔时间判断，
		TempFlag = TEST_BUSY;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(TempFlag != TEST_IDLE)
	{
		LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
	}
	
	return ret;

}
/***************************************************************************
**  uint8 CheckSysInnerRStat(uint8 groupnum)
**   功能: 检测模块内阻巡检状态，看是否所有模块都已经测试完毕
**   参数: 
**   输出:  所有模块都测量完成才输出TRUE；
**   描述: 如果模块内阻都测试完成，则准备读取内阻数据
****************************************************************************/
uint8 CheckSysInnerRStat(uint8 groupnum,uint8 devaddr)
{
	uint8 DevNO;
	uint8 ret = TRUE;
	if(LcdComm.InnerTest.InnerRTestDevNO > 0)
	{
		DevNO = LcdComm.InnerTest.InnerRTestDevNO - 1;
	}
	if(LcdComm.InnerTest.InnerRTestFlag[groupnum] == R_TEST_ING)
	{
		if(MbsSlave.StatusData.MoudleStat[groupnum][DevNO].S_MeaInterRStat == R_TEST_SUCCESS)
		{//没有测试成功，返回，等待测试或测试失败而停止测试
			if(LcdComm.InnerTest.InnerRTestDevNO < LcdComm.DevNUM[groupnum])
			{//上一个模块测试成功，判断是否还有下一个模块需要进行内阻测试，如果有下一个模块需要测试，则需要间隔一定时间才能启动测量
				if(g_TimerMs - BATComTime.InnerRTestCtrlTimeMs[groupnum]>(LcdComm.InnerTest.TestTimePeriod[groupnum]+12)*1000)
				{
					StartInnerRTest(groupnum,DevNO+1);
					ReadRTestAddrDATA(groupnum,DevNO+1);//读取1023位置的数据，看模块是否接收到启动指令
					LcdComm.LcdSysCtrl[groupnum][DevNO].LCD_StartInterMea = RESET;//清除上一个模块的参数
					LcdComm.LcdSysCtrl[groupnum][LcdComm.InnerTest.InnerRTestDevNO].LCD_StartInterMea = RESET;//参数重置，方便查询模块是否接收到指令
					LcdComm.InnerTest.InnerRTestDevNO++;//需要测量的模块编号加1
					BATComTime.InnerRTestCtrlTimeMs[groupnum] = g_TimerMs;//记录启动该模块的测试时间，计算是否测量超时
					BATComTime.TesttingStatusReadTimeMs[groupnum] = g_TimerMs;//启动测试后，过3S再去读取状态
				}
				return FALSE;
			}
			if((devaddr == (DevNO))&&(MbsSlave.StatusData.MoudleStat[groupnum][DevNO].S_MeaInterRStat == R_TEST_SUCCESS))
			{//如果最后一个模块完成内阻测试，则内阻测试完成，成功
				switch(groupnum)
				{
					case GROUP1:
						LcdComm.InnerTest.InnerRTestFlag[GROUP1] = R_TEST_SUCCESS;
						break;
					case GROUP2:
						LcdComm.InnerTest.InnerRTestFlag[GROUP2] = R_TEST_SUCCESS;
						break;
					default:
						break;
				}
			}
		}
//		else if(MbsSlave.StatusData.MoudleStat[groupnum][DevNO].S_MeaInterRStat != R_TEST_ING)
//		{//启动失败，再次启动
//				StartInnerRTest(groupnum,DevNO);
//				ReadRTestAddrDATA(groupnum,DevNO);//读取1023位置的数据，看模块是否接收到启动指令
//				BATComTime.InnerRTestCtrlTimeMs[groupnum] = g_TimerMs;//记录启动该模块的测试时间，计算是否测量超时
//				BATComTime.TesttingStatusReadTimeMs[groupnum] = g_TimerMs;//启动测试后，过3S再去读取状态
//		}
		else
		{
			ret = FALSE;
		}
	}
	return ret;
}
/***************************************************************************
**  void CalDataRead(uint8 DEVCOM)
**   功能:模块校准参数读取,在数据初始化时，读取所有模块的校准参数
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void AllCalDataRead(void)
{


}
/***************************************************************************
**  void GroupDataRead(uint8 DEVCOM)
**   功能:读取电池电压值
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void GroupDataRead(uint8 DEVCOM,uint8 group)
{
	MSG_STRUCT msg;
	int32 tmptime;
	tmptime = (int32)g_TimerMs - BATComTime.GroupDataReadTimeMs[group];
	if(tmptime > BATComTime.GroupDataReadTime[group])		/****读取电池电压*****/
	{
		msg.MsgType = MSG_RD_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (GROUP_DATA_ADDR>>8) & 0xFF;
		msg.MsgData[2] = GROUP_DATA_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(8>>8) & 0xFF;
		msg.MsgData[5] = (uint8)8 & 0xFF;
		if(group == GROUP1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(group == GROUP2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
		BATComTime.GroupDataReadTimeMs[group] = g_TimerMs;
	}
}
/***************************************************************************
**  void GroupCliParaRead(uint8 DEVCOM)
**   功能:读取校准参数
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void GroupCliParaRead(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr;
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (GROUP_CAL_ADDR>>8) & 0xFF;
	msg.MsgData[2] = GROUP_CAL_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(16>>8) & 0xFF;
	msg.MsgData[5] = (uint8)16 & 0xFF;
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendRatioToDev(uint8 group)
**   功能:发送校准系数到巡检模块
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendRRatioToDev(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = 1;//校准系数只发给巡检模块1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = (RATIO_ADDR>>8) & 0xFF;
	msg.MsgData[2] = RATIO_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.InnerRRatio,2);//只需要发送1个寄存器	
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendRatioToDev(uint8 group)
**   功能:发送校准系数到巡检模块
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendCRRatioToDev(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = 1;//校准系数只发给巡检模块1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+1)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+1) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.CRRatio,2);//只需要发送1个寄存器	
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendRatioToDev(uint8 group)
**   功能:发送校准系数到巡检模块
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendTRatioToDev(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = 1;//校准系数只发给巡检模块1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+2)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+2) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.TempCliPara,2);//只需要发送1个寄存器	
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendRatioToDev(uint8 group)
**   功能:发送校准系数到巡检模块
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendVRatioToDev(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = 1;//校准系数只发给巡检模块1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+3)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+3) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.VCliPara,2);//只需要发送1个寄存器	
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendTempEXIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
**   功能:发送温度探头自定义ID到设备
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendTempEXIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//温度探头在每个模块内部的编码，从1到18，TempNO是系统温度探头编码，从1到240
	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	DevTempNO = TempNO%MAX_BAT_NUM;
	if(DevTempNO == 0)
	{
		DevTempNO = MAX_BAT_NUM;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	if(TempType == 0)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//根据温度探头编号，发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+DevTempNO-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+DevTempNO-1) & 0xFF;
	}
	else if(TempType == 1)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+19-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+19-1) & 0xFF;
	}
	else if(TempType == 2)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+20-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+20-1) & 0xFF;
	}
	else if(TempType == 3)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+21-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+21-1) & 0xFF;
	}
	msg.MsgData[3] = 1;// 2个字节，1个寄存器
	memcpy(&msg.MsgData[4],(uint8 *)&MbsSlave.SysPara.TempInfo.WrExid,2);// 2个字节，1个寄存器
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void SendTempIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
**   功能:发送温度探头ID到设备
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void SendTempIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//温度探头在每个模块内部的编码，从1到18，TempNO是系统温度探头编码，从1到240
	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	DevTempNO = TempNO%MAX_BAT_NUM;
	if(DevTempNO == 0)
	{
		DevTempNO = MAX_BAT_NUM;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	if(TempType == 0)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//根据温度探头编号，发送给相应的模块
		msg.MsgData[1] = ((TEMP_SET_ADDR+DevTempNO*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+DevTempNO*4-4) & 0xFF;
	}
	else if(TempType == 1)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_SET_ADDR+19*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+19*4-4) & 0xFF;
	}
	msg.MsgData[3] = 4;//10个字节，5个寄存器
	memcpy(&msg.MsgData[4],MbsSlave.SysPara.TempInfo.WrID,8);//10个字节，5个寄存器
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void ReadTempEXID(uint8 group)
**   功能:读取内阻校准系数
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void ReadTempEXID(uint8 group,uint8 TempType,uint8 TempNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//温度探头在每个模块内部的编码，从1到18，TempNO是系统温度探头编码，从1到240

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}

	DevTempNO = TempNO%MAX_BAT_NUM;
	if(DevTempNO == 0)
	{
		DevTempNO = MAX_BAT_NUM;
	}

	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	if(TempType == 0)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//根据温度探头编号，发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+DevTempNO-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+DevTempNO-1) & 0xFF;
	}
	else if(TempType == 1)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+19-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+19-1) & 0xFF;
	}
	else if(TempType == 2)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+20-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+20-1) & 0xFF;
	}
	else if(TempType == 3)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_EXID_ADDR+21-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+21-1) & 0xFF;
	}
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = 0;
	msg.MsgData[5] = 1;//读取温度探头厂家ID，总共2个字节，1个寄存器
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void ReadTempID(uint8 group)
**   功能:读取内阻校准系数
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void ReadTempID(uint8 group,uint8 TempType,uint8 TempNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//温度探头在每个模块内部的编码，从1到18，TempNO是系统温度探头编码，从1到240

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}

	DevTempNO = TempNO%MAX_BAT_NUM;
	if(DevTempNO == 0)
	{
		DevTempNO = MAX_BAT_NUM;
	}

	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	if(TempType == 0)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//根据温度探头编号，发送给相应的模块
		msg.MsgData[1] = ((TEMP_SET_ADDR+DevTempNO*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+DevTempNO*4-4) & 0xFF;
	}
	else if(TempType == 1)//发送的电池温度探头ID
	{
		msg.MsgDevAddr = TempNO;//根据环境温度探头编号发送给相应的模块
		msg.MsgData[1] = ((TEMP_SET_ADDR+19*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+19*4-4) & 0xFF;
	}
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = 0;
	msg.MsgData[5] = 4;//读取温度探头厂家ID，总共8个字节，4个寄存器
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void ReadRatio(uint8 group)
**   功能:读取内阻校准系数
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void ReadRatio(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = 1;//校准系数只发给巡检模块1
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (RATIO_ADDR>>8) & 0xFF;
	msg.MsgData[2] = RATIO_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(RATIO_NUM>>8) & 0xFF;
	msg.MsgData[5] = (uint8)RATIO_NUM & 0xFF;
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}

/***************************************************************************
**  void DevOneKeyRestore(uint8 group)
**   功能:巡检模块一键还原
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void DevOneKeyRestore(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	for(i=1;i<=LcdComm.DevNUM[group];i++)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = i;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = 0x08;
		msg.MsgData[2] = 0xB2;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = 0x00;
		msg.MsgData[5] = 0x00;
		if(group == GROUP1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(group == GROUP2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void GroupADValueRead(uint8 group)
**   功能:读取AD值和整组电压电流值
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void GroupADValueRead(uint8 group)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;

	if(group == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else
	{
		DEVCOM = BATCOM2;
	}
	
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr;
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (GROUP_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = GROUP_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(GROUP_DATA_NUM>>8) & 0xFF;
	msg.MsgData[5] = (uint8)GROUP_DATA_NUM & 0xFF;
	if(group == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void BatVRead(uint16 BATGROUP,uint8 DEVNO) 
**   功能:读取电池电压值，整数部分
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatVRead(uint16 BATGROUP,uint8 DEVNO) 
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(BATGROUP == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else if(BATGROUP == GROUP2)
	{
		DEVCOM = BATCOM2;
	}
	if((MbsSlave.StatusData.MoudleStat[BATGROUP][DEVNO].S_CellMeaStat & 0xFF00) == MEA_OVER)
	{
		msg.MsgType = MSG_RD_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = DEVNO+1;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (BAT_V_DATA_ADDR>>8) & 0xFF;
		msg.MsgData[2] = BAT_V_DATA_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(BAT_V_DATA_NUM>>8) & 0xFF;
		msg.MsgData[5] = (uint8)BAT_V_DATA_NUM & 0xFF;
		if(BATGROUP == GROUP1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(BATGROUP == GROUP2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void BatVDecRead(uint16 BATGROUP,uint8 DEVNO) 
**   功能:读取电池电压值，小数部分
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatVDecRead(uint16 BATGROUP,uint8 DEVNO) 
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(BATGROUP == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else if(BATGROUP == GROUP2)
	{
		DEVCOM = BATCOM2;
	}
	if((MbsSlave.StatusData.MoudleStat[BATGROUP][DEVNO].S_CellMeaStat & 0xFF00) == MEA_OVER)
	{
		msg.MsgType = MSG_RD_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = DEVNO+1;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (V_DATA_DEC_ADDR>>8) & 0xFF;
		msg.MsgData[2] = V_DATA_DEC_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(BAT_V_DATA_NUM>>8) & 0xFF;
		msg.MsgData[5] = (uint8)BAT_V_DATA_NUM & 0xFF;
		if(BATGROUP == GROUP1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(BATGROUP == GROUP2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}

/***************************************************************************
**  void BatRRead(uint8 DEVCOM)
**   功能:读取电池内阻值
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatRRead(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(BATGROUP == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else if(BATGROUP == GROUP2)
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (BAT_R_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = BAT_R_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(BAT_R_DATA_NUM>>8) & 0xFF;
	msg.MsgData[5] = (uint8)BAT_R_DATA_NUM & 0xFF;
	if(BATGROUP == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(BATGROUP == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void BatCRRead(uint16 BATGROUP,uint8 DEVNO)
**   功能:读取连接条阻值
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatCRRead(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(BATGROUP == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else if(BATGROUP == GROUP2)
	{
		DEVCOM = BATCOM2;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (CONNECT_R_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = CONNECT_R_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(CONNECT_R_DATA_NUM>>8) & 0xFF;
	msg.MsgData[5] = (uint8)CONNECT_R_DATA_NUM & 0xFF;
	if(BATGROUP == GROUP1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(BATGROUP == GROUP2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void BatTRead(uint16 BATGROUP,uint8 DEVNO)
**   功能:读取电池温度
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatTRead(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	if(BATGROUP == GROUP1)
	{
		DEVCOM = BATCOM1;
	}
	else if(BATGROUP == GROUP2)
	{
		DEVCOM = BATCOM2;
	}
	if((MbsSlave.StatusData.MoudleStat[BATGROUP][DEVNO].S_TempMeaStat & 0xFF00) == MEA_OVER)
	{
		msg.MsgType = MSG_RD_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = DEVNO + 1;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (BAT_TEMP_DATA_ADDR>>8) & 0xFF;
		msg.MsgData[2] = BAT_TEMP_DATA_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(BAT_TEMP_DATA_NUM>>8) & 0xFF;
		msg.MsgData[5] = (uint8)BAT_TEMP_DATA_NUM & 0xFF;
		if(BATGROUP == GROUP1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(BATGROUP == GROUP2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void MasterBatVRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取电池电压
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterBatVRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_BAT_V_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_BAT_V_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterBatVRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取电池电压
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterBatVDecRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (V_DATA_DEC_ADDR>>8) & 0xFF;
	msg.MsgData[2] = V_DATA_DEC_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterBatTRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取温度
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterBatTRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_BAT_TEMP_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_BAT_TEMP_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterBatRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取电池内阻
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterBatRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_BAT_R_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_BAT_R_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterBatCRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取连接条阻值
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterBatCRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_CONNECT_R_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_CONNECT_R_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterGroupDataRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取电池组信息，组电压，充放电电流等
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterGroupDataRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{

		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_GROUP_DATA_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_GROUP_DATA_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterDevStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:读取系统状态信息
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterDevStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{
		case COM1:
			piroflag = APP_TASK_COM1_PRIO;
			break;
		case COM2:
			piroflag = APP_TASK_COM2_PRIO;
			break;
		case COM3:
			piroflag = APP_TASK_COM3_PRIO;
			break;
		case COM4:
			piroflag = APP_TASK_COM4_PRIO;
			break;
		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_DEV_STAT_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_DEV_STAT_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterCfgRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取配置参数信息
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterCfgRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{

		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = ((S_MSYS_CFG_ADDR + 1)>>8) & 0xFF;
	msg.MsgData[2] = (S_MSYS_CFG_ADDR + 1) & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/***************************************************************************
**  void MasterErrStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
**   功能:后台主机读取配置参数信息
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MasterErrStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum)
{
	MSG_STRUCT msg;
	uint8 piroflag;
	switch(DevCom)
	{

		default:
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DevCom;
	msg.MsgDevAddr = HostAddr;//后台主机要与从机地址保持一致
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (S_BAT_ERR_STAT_ADDR>>8) & 0xFF;
	msg.MsgData[2] = S_BAT_ERR_STAT_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(ReadRegNum>>8) & 0xFF;
	msg.MsgData[5] = (uint8)ReadRegNum & 0xFF;
	SendMsg2Queue(&msg,piroflag);
}
/*************************************************************************************
**  uint8 SendComParaToS(void) 
**   功能: 发送配置参数到采集盒--分流器参数
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendParaToS(uint8 group) 
{ 
	MSG_STRUCT msg;
	if(group == GROUP1)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM1;
		msg.MsgDevAddr = MbsSlave.Group[GROUP1].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (uint8)((S_GROUP_PARA_ADDR + 2) >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)((S_GROUP_PARA_ADDR + 2)& 0xFF);
		msg.MsgData[3] = 1;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP1].SysPara.CurrentDiverterPara,2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM2;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (uint8)((S_GROUP_PARA_ADDR + 2) >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)((S_GROUP_PARA_ADDR + 2)& 0xFF);
		msg.MsgData[3] = 1;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP2].SysPara.CurrentDiverterPara,2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	return TRUE;
}
/*************************************************************************************
**  uint8 SendCliParaToS(uint8 group) 
**   功能: 发送校准参数到采集盒--分流器参数
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendCliParaToS(uint8 group) 
{ 
	MSG_STRUCT msg;
	if(group == GROUP1)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM1;
		msg.MsgDevAddr = MbsSlave.Group[GROUP1].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_MULT_REG;
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR& 0xFF);
		msg.MsgData[3] = S_GROUP_CAL_NUM;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP1].MeaCal.KU,S_GROUP_CAL_NUM*2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM2;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_MULT_REG;
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR & 0xFF);
		msg.MsgData[3] = S_GROUP_CAL_NUM;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP2].MeaCal.KU,S_GROUP_CAL_NUM*2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	return TRUE;
}
/*************************************************************************************
**  uint8 ReadCliParaToS(uint8 group) 
**   功能: 发送校准参数到采集盒--分流器参数
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 ReadCliParaToS(uint8 group) 
{ 
	MSG_STRUCT msg;
	if(group == GROUP1)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM1;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR& 0xFF);
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(GROUP_CAL_NUM>>8) & 0xFF;
		msg.MsgData[5] = (uint8)GROUP_CAL_NUM & 0xFF;
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM2;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_RD_HOLD_REG;
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR & 0xFF);
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)(GROUP_CAL_NUM>>8) & 0xFF;
		msg.MsgData[5] = (uint8)GROUP_CAL_NUM & 0xFF;
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	return TRUE;
}
/*************************************************************************************
**  uint8 SendCfgParaToDev(uint16 BATGROUP,uint8 DEVNO) 
**   功能: 发送配置参数到模块
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendCfgParaToDev(uint16 BATGROUP,uint8 DEVNO) 
{ 
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case GROUP1://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 6)>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
	msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 6)& 0xFF);
	msg.MsgData[3] = 13;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_SingleVoltageMax,13*2);//只需要发送13个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_SingleVoltageMax,13*2);//只需要发送13个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}

	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
	msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 1)& 0xFF);
	msg.MsgData[3] = 1;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum,2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum,2);//只需要发送1个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}

	return 1;
}
/*************************************************************************************
**  uint8 SendCfgParaToAllDev(void) 
**   功能: 发送配置参数到所有模块
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendCfgParaToAllDev(void) 
{ 
	MSG_STRUCT msg;
	uint8 i,j;
	for(j=0;j<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;j++)
	{
		for(i=1;i<=LcdComm.DevNUM[j];i++)
		{
			if((j == GROUP1)&&(DevTask.TaskBat1!=0))//发送到相应的串口
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM1;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
				msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 1)& 0xFF);
				msg.MsgData[3] = 18;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum,18*2);//只需要发送13个寄存器
				switch(DevTask.TaskBat1)
				{//当使用的是信瑞达通信协议时，不发送配置信息
					case APP_TASK_COM1_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM2_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM3_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM4_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					default:
						break;
				}
			}
			else if((j == GROUP2)&&(DevTask.TaskBat1!=0))
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM2;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
				msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 1)& 0xFF);
				msg.MsgData[3] = 18;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum,18*2);
				switch(DevTask.TaskBat2)
				{
					case APP_TASK_COM1_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM2_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM3_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM4_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					default:
						break;
				}
			}
		}
	}
	return 1;
}
/*************************************************************************************
**  uint8 SendShieldInfoToDev(uint8 DevNO) 
**   功能: 发送屏蔽信息到设定模块,将所有模块的屏蔽信息都发送过去
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendShieldInfoToDev(uint16 BATGROUP,uint8 DEVNO) 
{ 
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case GROUP1://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
	msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR & 0xFF);
	msg.MsgData[3] = 33;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//需要发送33个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP2].BatVShield[0],33*2);//需要发送33个寄存器	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}

	return 1;
}
/*************************************************************************************
**  uint8 SendShieldParaToAllDev(void) 
**   功能: 发送屏蔽信息到所有模块
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SendShieldInfoToAllDev(void) 
{ 
	MSG_STRUCT msg;
	uint8 i,j;
	for(j=0;j<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;j++)
	{
		for(i=1;i<=LcdComm.DevNUM[j];i++)
		{
			if((j == GROUP1)&&(DevTask.TaskBat1!=0))//发送到相应的串口
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM1;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
				msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR& 0xFF);
				msg.MsgData[3] = 33;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//需要发送33个寄存器
				switch(DevTask.TaskBat1)
				{//当使用的是信瑞达通信协议时，不发送配置信息
					case APP_TASK_COM1_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM2_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM3_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					case APP_TASK_COM4_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat1);
						}
						break;
					default:
						break;
				}
			}
			else if((j == GROUP2)&&(DevTask.TaskBat1!=0))
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM2;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
				msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR& 0xFF);
				msg.MsgData[3] = 33;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//需要发送33个寄存器
				switch(DevTask.TaskBat2)
				{
					case APP_TASK_COM1_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM1].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM2_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM2].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM3_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM3].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					case APP_TASK_COM4_PRIO:
						if((MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BAT5000_XRD)&&(MbsSlave.SysPara.ComPara.UsartPara[COM4].ComProtocal != BATM30BPROTOCAL))
						{
							SendMsg2Queue(&msg,DevTask.TaskBat2);
						}
						break;
					default:
						break;
				}
			}
		}
	}
	return 1;
}


/***************************************************************************
**  void StartInnerRTest(uint8 DEVCOM)
**   功能: 启动内阻测试
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void StartInnerRTest(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case 0://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case 1://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = (BAT_R_TEST_ADDR>>8) & 0xFF;
	msg.MsgData[2] = BAT_R_TEST_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)START & 0xFF;
	msg.MsgData[5] = (uint8)(START>>8) & 0xFF;
	if(DEVCOM == BATCOM1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void PauseInnerRTest(uint8 batgroup,uint8 DevNO)
**   功能: 暂停内阻测试
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void PauseInnerRTest(uint8 batgroup,uint8 DevNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(batgroup)
	{
		case GROUP1://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DevNO+1;
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = (BAT_R_TEST_ADDR>>8) & 0xFF;
	msg.MsgData[2] = BAT_R_TEST_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)STOP & 0xFF;
	msg.MsgData[5] = (uint8)(STOP>>8) & 0xFF;
	if(DEVCOM == BATCOM1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}

/***************************************************************************
**  void StopInnerRTest(uint8 DEVCOM)
**   功能: 停止内阻测试
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void StopInnerRTest(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case GROUP1://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	for(i=1;i<=LcdComm.DevNUM[BATGROUP];i++)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = i;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (BAT_R_TEST_ADDR>>8) & 0xFF;
		msg.MsgData[2] = BAT_R_TEST_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)STOP & 0xFF;
		msg.MsgData[5] = (uint8)(STOP>>8) & 0xFF;
		if(DEVCOM == BATCOM1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(DEVCOM == BATCOM2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void ReadRTestAddrDATA(uint16 BATGROUP,uint8 DEVNO)
**   功能: 读取内阻测试位的数据，判断是否写入成功，0Xaa表示启动内阻巡检，0X55表示停止内阻巡检
**   参数:  BATGROUP，蓄电池组号，DEVNO表示模块编号
**   输出:  
**   描述:  
****************************************************************************/
void ReadRTestAddrDATA(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case 0://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case 1://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (BAT_R_TEST_ADDR>>8) & 0xFF;
	msg.MsgData[2] = BAT_R_TEST_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = 0;//只读取R标识为一个寄存器
	msg.MsgData[5] = 1;
	if(DEVCOM == BATCOM1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
}
/***************************************************************************
**  void StartBatEQ(uint16 BATGROUP,uint8 DEVNO)
**   功能: 启动电池均衡
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void StartBatEQ(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case 0://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case 1://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	for(i=1;i<=LcdComm.DevNUM[BATGROUP];i++)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = i;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (BAT_EQ_ADDR>>8) & 0xFF;
		msg.MsgData[2] = BAT_EQ_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)START & 0xFF;
		msg.MsgData[5] = (uint8)(START>>8) & 0xFF;
		if(DEVCOM == BATCOM1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(DEVCOM == BATCOM2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void StopBatEQ(uint16 BATGROUP,uint8 DEVNO)
**   功能: 停止电池均衡
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void StopBatEQ(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case 0://蓄电池组1
			DEVCOM = BATCOM1;
			break;
		case 1://蓄电池组2
			DEVCOM = BATCOM2;
			break;
	}
	for(i=1;i<=LcdComm.DevNUM[BATGROUP];i++)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = DEVCOM;
		msg.MsgDevAddr = i;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (BAT_EQ_ADDR>>8) & 0xFF;
		msg.MsgData[2] = BAT_EQ_ADDR & 0xFF;
		msg.MsgData[3] = 1;
		msg.MsgData[4] = (uint8)STOP & 0xFF;
		msg.MsgData[5] = (uint8)(STOP>>8) & 0xFF;
		if(DEVCOM == BATCOM1)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		else if(DEVCOM == BATCOM2)
		{
			SendMsg2Queue(&msg,DevTask.TaskBat2);
		}
	}
}
/***************************************************************************
**  void DataErrCheck(void)
**   功能: 检测蜂鸣器是否具有报警条件
**   参数:  电池组号，检测项目标志，是否写入铁电，(后两个参数用来判断内阻超限是否报警)
**   输出:  输出TRUE表示系统正常，蜂鸣器不响，输出FALSE表示系统不正常，蜂鸣器响
**   描述: 
****************************************************************************/
uint8 CheckBeepCondition(uint8 batgroup,uint8 ErrCheckFlag,uint8 IsWrite)
{
	uint8 i;
	uint16 j;
	uint8 temp = OFF;
	uint8 tmp1 = OFF;//检查是否有阻值超限标志
	static uint8 tmpflag = OFF;//是否继续报警标志
	//tmp1 = on,tmpflag = on:表示故障状态中有内阻超限信息，需要报警，内阻测试完情况
	//tmp1 = on，tmpflag = off:表示故障状态中有内阻超限信息，不需要报警，开机后情况
	//tmp1 = OFF，tmpflag = off:表示故障状态中没有内阻超限信息，不需要报警，系统正常
	
	for(i=0;i<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;i++)//电池组数
	{
		for(j=0;j<MbsSlave.StatusData.BatErrStat[i].S_Quantity;j++)//故障数量
		{
			switch(MbsSlave.StatusData.BatErrStat[i].S_ErrInfo[j].S_ErrType)
			{
				case VOLTAGE_OVER://过压
				case VOLTAGE_LOW://欠压
				case TEMP_OVER://过温
				case TEMP_LOW://欠温
				case GROUP_V_OVER://组电压过高
				case GROUP_V_LOW://组电压过低
				case CURRENT_DISCHARGE_OVER://放电电流过高
				case CURRENT_CHARGE_OVER://充电电流过高
				case ENVIRMENT_TEMP_OVER://环境温度过高 
				case ENVIRMENT_TEMP_LOW://环境温度过低
				case DEV_TEMP_ERR://温度探头故障
				case ENVIR_TEMP_ERR://环境温度故障
				case DEV_COMM_ERR://通信异常
				case BAT5000_S_COM_ERR://采集盒通信异常
				case BKG_COM_ERR://巡检仪通信异常
				case DEV_INNER_R_ERR://内阻模块故障
				case DEV_CR_ERR://连接条模块故障
				case DEV_EQ_ERR://均衡模块故障
				case BAT_SAMPLE_LINE_DROP://采样线脱落
				case GROUP_SAMPLE_LINE_DROP://电池组采样线脱落
				case BAT_SAMPLE_LINE_RESERVE://反接
				case GROUP_SAMPLE_LINE_RESERVE:
					temp = ON;
					break;
				case INTER_R_OVER:
				case CR_OVER:
					tmp1 = ON;
					if(((ErrCheckFlag == INNER_R_CHECK)||(ErrCheckFlag == CONNECT_R_CHECK))&&(IsWrite == ON))
					{
						tmpflag = ON;
					}
					break;
				case VOLTAGE_RECOVER:
				case TEMP_RECOVER:
				case GROUP_V_RECOVER:
				case CURRENT_DISCHARGE_RECOVER:
				case CURRENT_CHARGE_RECOVER:
				case ENVIRMENT_TEMP_RECOVER:
				case DEV_TEMP_RECOVER:
				case ENVIR_TEMP_RECOVER:
				case DEV_COMM_RECOVER:
				case BAT5000_S_COM_RECOVER:
				case BKG_COM_RECOVER:
				case DEV_INNER_R_RECOVER:
				case SAMPLE_LINE_DROP_RECOVER:
				case GROUP_SAMPLE_LINE_DROP_RECOVER:
				case BAT_SAMPLE_LINE_RESERVE_RECOVER:
				case GROUP_SAMPLE_LINE_RESERVE_RECOVER:
				case DEV_TEST_FAIL1://内阻测试失败
				case DEV_TEST_FAIL2:
				case DEV_TEST_FAIL3:
				case DEV_TEST_FAIL4:
				case DEV_TEST_FAIL5:
				case DEV_TEST_FAIL6:
				case DEV_TEST_FAIL7:
				case DEV_TEST_FAIL8:
				case DEV_TEST_FAIL9:
				case DEV_TEST_FAIL10:
				case DEV_TEST_FAIL://内阻测试失败
					temp = OFF;
					break;
				default:
					temp = OFF;
					break;
			}
			if(temp == ON)
			{
				break;
			}
//			if(SysFlag.BeepFlag == ON)
//			{//如果有报警，就返回，不在检查
//				return FALSE;
//			}
		}
			if(temp == ON)
			{
				break;
			}
	}
	if(tmp1 == OFF)
	{
		tmpflag = OFF;//表明故障状态中没有内阻超限信息
	}
	if((tmpflag == ON)&&(tmp1 == ON))
	{
		SysFlag.BeepFlag = ON;
	}
	else
	{
		SysFlag.BeepFlag = temp;
	}
	
	return TRUE;
}
/***************************************************************************
**  void DataErrCheck(void)
**   功能: 电池系统故障检测
**   参数:  电池组号，模块编号，故障类型检测，是否写入历史记录
**   输出:  
**   描述: 
****************************************************************************/
ERR_FLAG ErrFlag;

void DataErrCheck(uint8 BatGroup,uint8 DevNO,uint8 ErrCheckFlag,uint8 IsWrite)
{
	uint8 i,j,k;
	uint8 tmpflag = OFF;
	uint8 BatNum;
	uint8 TempDevNO;
	uint32 TempRData;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif 
	if(DevNO+1 == LcdComm.DevNUM[BatGroup])//计算出需要检验的电池数量，当是最后一个模块时，电池数量可能不是18节
	{
		BatNum = MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_CellNum- DevNO*BAT_V_DATA_NUM;
	}
	else
	{
		BatNum = BAT_V_DATA_NUM;
	}
	switch(ErrCheckFlag)
	{
		case VOLTAGE_CHECK:
			for(i=0;i<BatNum;i++)
			{
				for(k=0;k<BAT_V_SHIELD_SUM;k++)
				{//检查是否有屏蔽的电池，屏蔽的电池不再进行电压报警
					if(DevNO*BAT_V_DATA_NUM+i+1 == FdyComm.ShieldInfo[BatGroup].BatShieldNO[k])
					{
						tmpflag = ON;
						break;
					}
				}
				if(tmpflag == ON)
				{
					ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = NORMAL;
					tmpflag = OFF;
					continue;
				}
				if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i] > 800)
				{//小于800mv算脱落
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_SingleVoltageMax)
					{//电压过高
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != VOLTAGE_OVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,VOLTAGE_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = VOLTAGE_OVER;
					}
					else if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i] < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_SingleVoltageMin)
					{//电压过低
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != VOLTAGE_LOW)
						{
							HistoryDataSave(BatGroup,DevNO,i,VOLTAGE_LOW,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = VOLTAGE_LOW;
					}
					else if((ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == VOLTAGE_OVER)||(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == VOLTAGE_LOW))
					{//电压恢复
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != VOLTAGE_RECOVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,VOLTAGE_RECOVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = VOLTAGE_RECOVER;
					}
					else if((ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == BAT_SAMPLE_LINE_DROP))
					{
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != SAMPLE_LINE_DROP_RECOVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,SAMPLE_LINE_DROP_RECOVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = SAMPLE_LINE_DROP_RECOVER;
					}
					else
					{
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = NORMAL;
					}
				}
				else
				{//采样线脱落
					if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != BAT_SAMPLE_LINE_DROP)
					{
						HistoryDataSave(BatGroup,DevNO,i,BAT_SAMPLE_LINE_DROP,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
					}
					ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = BAT_SAMPLE_LINE_DROP;
				}
			}
			break;
		case TEMP_CHECK:
			for(i=0;i<BatNum;i++)
			{
				if(MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i] != 0xFAFA)
				{//温度探头出现故障，赋值为0xFAFA
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMax)
					{//电池温度过高
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_OVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_OVER;
					}
					else if(MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i] < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMin)
					{//电池温度过低
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_LOW)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_LOW,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_LOW;
					}
					else if((ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == TEMP_OVER)||(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == TEMP_LOW))
					{//电池温度恢复
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_RECOVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_RECOVER;
					}
					else if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == DEV_TEMP_ERR)
					{//电池温感故障恢复
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != DEV_TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,DEV_TEMP_RECOVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = DEV_TEMP_RECOVER;
					}
					else
					{
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = NORMAL;
					}
				}
				else
				{//电池温感故障
					if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != DEV_TEMP_ERR)
					{
						HistoryDataSave(BatGroup,DevNO,i,DEV_TEMP_ERR,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
					}
					ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = DEV_TEMP_ERR;
				}
			}
			break;
		case INNER_R_CHECK:
			for(i=0;i<BatNum;i++)
			{
				if((MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i] & 0x8000) != 0)
				{
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i] == 0xFFFF)
					{
						TempRData = MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i];
					}
					else
					{
						TempRData = ((uint32)MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i]&0x7FFF)*10;
					}
				}
				else
				{
					TempRData = MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i];
				}
				if(TempRData > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_SingleCellRMax)
				{//内阻超限
					ErrFlag.InnerRErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = INTER_R_OVER;
					if(IsWrite == ON)
					{
						HistoryDataSave(BatGroup,DevNO,i,INTER_R_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_InterR[DevNO][i]);
					}
				}
				else
				{
					ErrFlag.InnerRErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = NORMAL;
				}
			}
			break;
		case CONNECT_R_CHECK:
			for(i=0;i<BatNum;i++)
			{
				if((MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i] & 0x8000) != 0)
				{
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i] == 0xFFFF)
					{
						TempRData = MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i];
					}
					else
					{
						TempRData = ((uint32)MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i]&0x7FFF)*10;
					}
				}
				else
				{
					TempRData = MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i];
				}
				if(TempRData > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_ConnectRMax)
				{//连接条阻值超限
					if(IsWrite == ON)
					{
						HistoryDataSave(BatGroup,DevNO,i,CR_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_ConnectR[DevNO][i]);
					}
					ErrFlag.ConnectRErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = CR_OVER;
				}
				else
				{
					ErrFlag.ConnectRErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = NORMAL;
				}
			}
			break;
		case GPOUP_VOLTAGE_CHECK://电池组总电压检查
			if(MbsSlave.Group[BatGroup].MeaData.VoltageValue > 1)
			{
				if(MbsSlave.Group[BatGroup].MeaData.VoltageValue > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_GroupVoltageMax)
				{//组电压过高
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_OVER)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_OVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_OVER;
				}
				else if(MbsSlave.Group[BatGroup].MeaData.VoltageValue < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_GroupVoltageMin)
				{//组电压过低
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_LOW)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_LOW,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_LOW;
				}
				else if((ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_V_OVER)||(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_V_LOW))
				{//组电压恢复
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_RECOVER)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_RECOVER;
				}
				else if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_DROP)
				{
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_DROP_RECOVER)
					{//电池组采样线脱落恢复
						HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_DROP_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_DROP_RECOVER;
				}
				else if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_RESERVE)
				{
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_RESERVE_RECOVER)
					{//采样线反接恢复
						HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_RESERVE_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_RESERVE_RECOVER;
				}
				else 
				{
					ErrFlag.GroupVoltageErrFlag[BatGroup] = NORMAL;
				}
			}
			else if((MbsSlave.Group[BatGroup].MeaData.VoltageValue >= -1)&&(MbsSlave.Group[BatGroup].MeaData.VoltageValue <= 1))
			{
				if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_SAMPLE_LINE_DROP)
				{//电池组采样线脱落
					HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_DROP,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
				}
				ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_DROP;
			}
			else if(MbsSlave.Group[BatGroup].MeaData.VoltageValue < -1)
			{
				if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_SAMPLE_LINE_RESERVE)
				{//电池组采样线反接
					HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_RESERVE,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
				}
				ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_RESERVE;
			}
			break;
		case CIRCUIT_CHECK:
			if(MbsSlave.Group[BatGroup].MeaData.VoltageValue == 0)
			{
				for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_CellNum;i++)
				{
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] != 0)
					{//总电压=0，而单体电池电压不等于0，则是电池组出现断路情况，系统开路
						if(ErrFlag.CircuitErrFlag[BatGroup] != SYS_OPEN_CIRCUIT)
						{
							HistoryDataSave(BatGroup,DevNO,i,SYS_OPEN_CIRCUIT,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
						}
						ErrFlag.CircuitErrFlag[BatGroup] = SYS_OPEN_CIRCUIT;
						break;
					}
				}
			}
			else if(ErrFlag.CircuitErrFlag[BatGroup] == SYS_OPEN_CIRCUIT)
			{
				if(ErrFlag.CircuitErrFlag[BatGroup] != SYS_OPEN_CIRCUIT_RECOVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,SYS_OPEN_CIRCUIT_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
				}
				ErrFlag.CircuitErrFlag[BatGroup] = SYS_OPEN_CIRCUIT_RECOVER;
			}
			else
			{
				ErrFlag.CircuitErrFlag[BatGroup] = NORMAL;
			}
			break;
		case CURRENT_CHECK://电池组充放电电流检查
			if(LcdComm.Current[BatGroup] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_ChargeCurrentMax)
			{//充电电流过高
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_CHARGE_OVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_CHARGE_OVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_CHARGE_OVER;
			}
			else if(LcdComm.Current[BatGroup] < (MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_DischargeCurrentMax * (-1)))
			{//放电电流过高
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_DISCHARGE_OVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_DISCHARGE_OVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_DISCHARGE_OVER;
			}
			else if(ErrFlag.CurrentErrFlag[BatGroup] == CURRENT_CHARGE_OVER)
			{//充电电流恢复
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_CHARGE_RECOVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_CHARGE_RECOVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_CHARGE_RECOVER;
			}
			else if(ErrFlag.CurrentErrFlag[BatGroup] == CURRENT_DISCHARGE_OVER)
			{//放电电流恢复
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_DISCHARGE_RECOVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_DISCHARGE_RECOVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_DISCHARGE_RECOVER;
			}
			else
			{
				ErrFlag.CurrentErrFlag[BatGroup] = NORMAL;
			}
			break;
		case ENVIRONMENT_TEMP_CHECK://电池环境温度检查
			for(i=0;i<LcdComm.DevNUM[BatGroup];i++)
			{
				if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp != 0xFAFA)
				{
					if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMax)
					{//温度过高
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_OVER)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_OVER,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_OVER;
					}
					else if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMin)
					{//温度过低
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_LOW)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_LOW,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_LOW;
					}
					else if((ErrFlag.EnvironmentTempErrFlag[BatGroup][i] == ENVIRMENT_TEMP_OVER)||(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == ENVIRMENT_TEMP_LOW))
					{//温度恢复
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_RECOVER,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_RECOVER;
					}
					else if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] == ENVIR_TEMP_ERR)
					{//环境温感故障恢复
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIR_TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIR_TEMP_RECOVER,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIR_TEMP_RECOVER;
					}
					else
					{
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = NORMAL;
					}
				}
				else
				{//环境温感异常
					if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIR_TEMP_ERR)
					{
						HistoryDataSave(BatGroup,0,DevNO,ENVIR_TEMP_ERR,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
					}
					ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIR_TEMP_ERR;
				}
			}
			break;
		case DEV_COMM_CHECK:
			if(ErrFlag.DevCommErrFlag[BatGroup][DevNO] == DEV_COMM_ERR)
			{//模块通信故障
				HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_COMM_ERR,0);
			}
			else if(ErrFlag.DevCommErrFlag[BatGroup][DevNO] == DEV_COMM_RECOVER)
			{//模块通信故障恢复
				HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_COMM_RECOVER,0);
			}
			break;
		case BAT5000_S_COMM_CHECK:
			if(ErrFlag.BAT5000SCommErrFlag[BatGroup] == BAT5000_S_COM_ERR)
			{//采集盒通信故障
				HistoryDataSave(BatGroup,0,BatGroup<<4,BAT5000_S_COM_ERR,0);
			}
			else if(ErrFlag.BAT5000SCommErrFlag[BatGroup] == BAT5000_S_COM_RECOVER)
			{//采集盒通信故障恢复
				HistoryDataSave(BatGroup,0,BatGroup<<4,BAT5000_S_COM_RECOVER,0);
			}			
			break;
		case INNER_R_DEV_STAT_CHECK://检测内阻模块状态
			if((MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].InnerRDevStat == ON)&&(MbsSlave.StatusData.MoudleStat[BatGroup][DevNO].S_InnerRDev == OFF))
			{
				if(ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] != DEV_INNER_R_ERR)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_INNER_R_ERR,0);
				}
				ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] = DEV_INNER_R_ERR;
			}
			else if(ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] == DEV_INNER_R_ERR)
			{
				if(ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] != DEV_INNER_R_RECOVER)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_INNER_R_RECOVER,0);
				}
				ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] = DEV_INNER_R_RECOVER;
			}
			else
			{
				ErrFlag.InnerRDevStatFlag[BatGroup][DevNO] = NORMAL;
			}
			break;
		case CR_DEV_STAT_CHECK://检测连接条模块状态
			if((MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].CRDevStat == ON)&&(MbsSlave.StatusData.MoudleStat[BatGroup][DevNO].S_CRDev == OFF))
			{
				if(ErrFlag.CRDevStatFlag[BatGroup][DevNO] != DEV_CR_ERR)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_CR_ERR,0);
				}
				ErrFlag.CRDevStatFlag[BatGroup][DevNO] = DEV_CR_ERR;
			}
			else if(ErrFlag.CRDevStatFlag[BatGroup][DevNO] == DEV_CR_ERR)
			{
				if(ErrFlag.CRDevStatFlag[BatGroup][DevNO] != DEV_CR_RECOVER)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_CR_RECOVER,0);
				}
				ErrFlag.CRDevStatFlag[BatGroup][DevNO] = DEV_CR_RECOVER;
			}
			else
			{
				ErrFlag.CRDevStatFlag[BatGroup][DevNO] = NORMAL;
			}
			
			break;
		case EQ_DEV_STAT_CHECK://检测均衡模块状态
			if((MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].EQDevStat == ON)&&(MbsSlave.StatusData.MoudleStat[BatGroup][DevNO].S_EQDev == OFF))
			{
				if(ErrFlag.EQDevStatFlag[BatGroup][DevNO] != DEV_EQ_ERR)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_EQ_ERR,0);
				}
				ErrFlag.EQDevStatFlag[BatGroup][DevNO] = DEV_EQ_ERR;
			}
			else if(ErrFlag.EQDevStatFlag[BatGroup][DevNO] == DEV_EQ_ERR)
			{
				if(ErrFlag.EQDevStatFlag[BatGroup][DevNO] != DEV_EQ_RECOVER)
				{
					HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_EQ_RECOVER,0);
				}
				ErrFlag.EQDevStatFlag[BatGroup][DevNO] = DEV_EQ_RECOVER;
			}
			else
			{
				ErrFlag.EQDevStatFlag[BatGroup][DevNO] = NORMAL;
			}
			break;
		case BKG_COMM_CHECK:
			if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)
			{//巡检仪主机后台通信故障
				HistoryDataSave(0,0,0,BKG_COM_ERR,0);
			}
			else if(ErrFlag.BkgCommErrFlag == BKG_COM_RECOVER)
			{//巡检仪主机后台通信故障恢复
				HistoryDataSave(0,0,0,BKG_COM_RECOVER,0);
			}
			break;
		case FDY_COMM_CHECK:
			if(ErrFlag.FdyCommErrFlag == FDY_COM_ERR)
			{//放电仪通信故障
				HistoryDataSave(0,0,0,FDY_COM_ERR,0);
			}
			else if(ErrFlag.FdyCommErrFlag == FDY_COM_RECOVER)
			{//放电仪通信故障恢复
				HistoryDataSave(0,0,0,FDY_COM_RECOVER,0);
			}
			break;
		default:
			break;
	}
	/*****************统计实时故障数据****************/
	for(j=0;j<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;j++)//电池组数
	{
		OS_ENTER_CRITICAL();//禁止任务切换，防止故障数据还没有统计完成，就被其它任务调用
		MbsSlave.StatusData.BatErrStat[j].S_Quantity = 0;
		if(LcdComm.InnerTest.TestFailFlag[j]!=OFF)//内阻模块状态
		{//内阻测试失败
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = (LcdComm.InnerTest.TestFailFlag[j] >>8)&0xFF ;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType = LcdComm.InnerTest.TestFailFlag[j]&0xFF;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData = 0;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		for(i=0;i<LcdComm.DevNUM[j];i++)
		{
			if(ErrFlag.DevCommErrFlag[j][i] == DEV_COMM_ERR)//模块通信检测
			{
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4)+(i);
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.DevCommErrFlag[j][i];
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	0;
				MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
				if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
				{
					MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
					break;
				}
			}
			if((ErrFlag.InnerRDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//内阻模块状态
			{//内阻模块故障
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4)+(i);
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType = ErrFlag.InnerRDevStatFlag[j][i];
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData = 0;
				MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
				if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
				{
					MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
					break;
				}
			}
			if((ErrFlag.CRDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//连接条模块状态
			{//连接条模块故障
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4)+(i);
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType = ErrFlag.CRDevStatFlag[j][i];
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData = 0;
				MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
				if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
				{
					MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
					break;
				}
			}
			if((ErrFlag.EQDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//均衡模块状态
			{
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4)+(i);
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType = ErrFlag.EQDevStatFlag[j][i];
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData = 0;
				MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
				if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
				{
					MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
					break;
				}
			}
		}
		if(ErrFlag.BAT5000SCommErrFlag[j] == BAT5000_S_COM_ERR)//采集盒通信检测
		{//统计采集盒通信故障
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4);
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.BAT5000SCommErrFlag[j];
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	0;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)//巡检仪主机后台通信检测
		{
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = 0;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.BkgCommErrFlag;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	0;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		if(ErrFlag.FdyCommErrFlag == FDY_COM_ERR)//放电仪通信检测
		{
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = 0;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.FdyCommErrFlag;
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	0;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		if(ErrFlag.CircuitErrFlag[j] != NORMAL)//是否有开路检测
		{
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4);
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.CircuitErrFlag[j];
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	0;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		if((ErrFlag.GroupVoltageErrFlag[j] != NORMAL)&&(ErrFlag.BAT5000SCommErrFlag[j] != BAT5000_S_COM_ERR))//总电压
		{//统计电压故障
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4);
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.GroupVoltageErrFlag[j];
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.Group[BatGroup].MeaData.VoltageValue;
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		if((ErrFlag.CurrentErrFlag[j] != NORMAL)&&(ErrFlag.BAT5000SCommErrFlag[j] != BAT5000_S_COM_ERR))//电流
		{//统计电流故障
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4);
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.CurrentErrFlag[j];
			MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	LcdComm.Current[j];
			MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
			if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
			{
				MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
				break;
			}
		}
		for(i=0;i<LcdComm.DevNUM[j];i++)
		{//统计环境温度故障
			if((ErrFlag.EnvironmentTempErrFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//环境温度
			{
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = ((j)<<4)+(i);
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.EnvironmentTempErrFlag[j][i];
				MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.StatusData.MoudleStat[j][i].S_Temp;
				MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
				if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
				{
					MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
					break;
				}
			}	
		}
		for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[j].S_CellNum;i++)//电池节数
		{//统计电池电压和温度故障
			TempDevNO = i/BAT_V_DATA_NUM;
			if(ErrFlag.DevCommErrFlag[j][TempDevNO] != DEV_COMM_ERR)
			{
				if(ErrFlag.VoltageErrFlag[j][i] != NORMAL)//单节电池电压
				{//模块通信正常，有故障计入故障状态
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = i;
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.VoltageErrFlag[j][i];
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.BatData[j].DevMeaData.S_BatV[i/BAT_V_DATA_NUM][i%BAT_V_DATA_NUM];
					MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
					if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
					{
						MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
						break;
					}
				}
				if((ErrFlag.TempErrFlag[j][i] != NORMAL)&&(MbsSlave.SysPara.MSysCfg.GroupCfg[j].TempDevStat != OFF))//单节电池温度
				{//温度功能开启，模块通信正常，有故障计入故障状态
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = i;
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.TempErrFlag[j][i];
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.BatData[j].DevMeaData.S_Temp[i/BAT_V_DATA_NUM][i%BAT_V_DATA_NUM];
					MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
					if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
					{
						MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
						break;
					}
				}
			}
		}
		for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[j].S_CellNum;i++)//电池节数
		{//统计电池内阻与连接条超限故障
			TempDevNO = i/BAT_V_DATA_NUM;
			if(ErrFlag.DevCommErrFlag[j][TempDevNO] != DEV_COMM_ERR)
			{
				if((ErrFlag.InnerRErrFlag[j][i] != NORMAL)&&(MbsSlave.SysPara.MSysCfg.GroupCfg[j].InnerRDevStat != OFF)&&(LcdComm.InnerTest.TestFailFlag[j]==OFF))//电池内阻
				{//内阻功能开启，模块通信正常，有故障计入故障状态
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = i;
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.InnerRErrFlag[j][i];
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.BatData[j].DevMeaData.S_InterR[i/BAT_V_DATA_NUM][i%BAT_V_DATA_NUM];
					MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
					if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
					{
						MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
						break;
					}
				}
				if((ErrFlag.ConnectRErrFlag[j][i] != NORMAL)&&(MbsSlave.SysPara.MSysCfg.GroupCfg[j].CRDevStat != OFF)&&(LcdComm.InnerTest.TestFailFlag[j]==OFF))
				{//连接条功能开启，模块通信正常，有故障计入故障状态
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrBatNum = i;
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrType =	ErrFlag.ConnectRErrFlag[j][i];
					MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[MbsSlave.StatusData.BatErrStat[j].S_Quantity].S_ErrData =	MbsSlave.BatData[j].DevMeaData.S_ConnectR[i/BAT_V_DATA_NUM][i%BAT_V_DATA_NUM];
					MbsSlave.StatusData.BatErrStat[j].S_Quantity++;
					if(MbsSlave.StatusData.BatErrStat[j].S_Quantity >= S_MAX_ERR_DATA)
					{
						MbsSlave.StatusData.BatErrStat[j].S_Quantity = S_MAX_ERR_DATA;
						break;
					}
				}
			}
		}
		OS_EXIT_CRITICAL();
	}
	CheckBeepCondition(BatGroup,ErrCheckFlag,IsWrite);
}

/***************************************************************************
**  void SysStatusRead(uint8 DEVCOM)
**   功能: 读取模块状态
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
uint8 SysStatusRead(uint8 DEVCOM,uint8 DEVNO)
{
	uint8 ret = FALSE;
	
	MSG_STRUCT msg;
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;//模块编号从1开始
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[1] = (DEV_STAT_ADDR>>8) & 0xFF;
	msg.MsgData[2] = DEV_STAT_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = (uint8)(DEV_STAT_NUM>>8) & 0xFF;
	msg.MsgData[5] = (uint8)DEV_STAT_NUM & 0xFF;
	if(DEVCOM == BATCOM1)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	ret = TRUE;
	return ret;
}
/***************************************************************************
**  void RTestFail(uint8 batgroup,uint8 devno)
**   功能: 内阻测试失败
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
void RTestFail(uint8 batgroup,uint8 devno)
{
	uint8 TempNO;
	LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_FAIL;//测试失败
	MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = STOP;				
	StopInnerRTest(batgroup);
	TempNO = (batgroup<<4)+LcdComm.InnerTest.InnerRTestDevNO-1;//高四位表示电池组号，低四位表示模块号
	switch(MbsSlave.StatusData.MoudleStat[batgroup][devno].S_MeaInterRStat)
	{
		case R_TEST_FAIL_I_LOW:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL1,0);
			break;
		case R_TEST_FAIL_I_OVER:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL2,0);
			break;
		case R_TEST_FAIL_STOP:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL3,0);
			break;
		case R_TEST_FAIL_V_LOW:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL4,0);
			break;
		case R_TEST_FAIL_V_OVER:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL5,0);
			break;
		case R_TEST_FAIL_BATV_LOW:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL6,0);
			break;
		case R_TEST_FAIL_BATV_OVER:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL7,0);
			break;
		case R_TEST_FAIL_TEMP_LOW:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL8,0);
			break;
		case R_TEST_FAIL_TEMP_OVER:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL9,0);
			break;
		default:
			HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL,0);
			break;
	}
}
/***************************************************************************
**  uint8 CheckVlow(void)
**   功能: 检测是否有欠压
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
uint8 CheckVlow(uint8 batgroup)
{
	uint8 i;
	uint8 ret = TRUE;
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{
		if(ErrFlag.VoltageErrFlag[batgroup][i] == VOLTAGE_LOW)
		{
			ret = FALSE;
			break;
		}
	}
	return ret;
}

/***************************************************************************
**  void DevProcess1(void)
**   功能: 作为MODBUS主机时的处理 
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/

BAT_COM_TIME BATComTime;

	uint32_t tempautotesttime;

void DevProcess(uint8 DEVCOM,uint8 GROUP)
{
	uint8 i;
	uint8 batgroup;
	uint8 tempdevno;
	uint8 TempNO;
	uint8 TempStat;
	uint32_t autoSaveTime;
	struct tm ttm;
	if(DEVCOM == BATCOM1)
	{
		batgroup = GROUP1;
	}
	else if(DEVCOM == BATCOM2)
	{
		batgroup = GROUP2;
	}

/*************************模块工作状态读取***************************************/		
	if((LcdComm.InnerTest.InnerRTestFlag[batgroup] != R_TEST_ING)&&(LcdComm.InnerTest.InnerRTestFlag[batgroup] != R_TEST_PAUSE))
	{//非内阻测试中，读取状态，电压，温度，采集盒信息
		if((g_TimerMs - BATComTime.SysStatusTimeMs[batgroup]) > BATComTime.DevStatusReadTime[batgroup])
		{
			LcdComm.RdDevFlag[batgroup]=OFF;//通信超时，则读取下一个模块的数据
		}
		if(LcdComm.RdDevFlag[batgroup]==OFF)
		{
			if((g_TimerMs - BATComTime.SysStatusTimeMs[batgroup])>(BATComTime.DevStatusReadTime[batgroup]/2))
			{//避免模块数量太少，导致读取过于频繁
				SysStatusRead(DEVCOM,LcdComm.RdDevNO[batgroup]);			/*模块工作状态读取*/
				BATComTime.SysStatusTimeMs[batgroup] = g_TimerMs;
				LcdComm.RdDevFlag[batgroup] = ON;//读取数据中
				LcdComm.RdDevNO[batgroup]++;
			}
			if(LcdComm.RdDevNO[batgroup] >= LcdComm.DevNUM[batgroup])
			{
				LcdComm.RdDevNO[batgroup]=0;
				GroupDataRead(DEVCOM,batgroup);//巡检模块读取完毕之后再读取采集盒数据
			}
		}
/************************************SOC估算*****************************/
		SOCCount(batgroup,LcdComm.BatSysStat.SingleBatMinV[batgroup][0]);

/************************************保存实时电压数据******************/
		autoSaveTime = BATComTime.BatDataSaveTime[batgroup]*60*1000;//常态数据保存，按设定时间保存
		if((g_TimerMs - BATComTime.BatDataSaveTimeMs[batgroup]) > autoSaveTime)//存储电池电压温度数据
		{
			BATComTime.BatDataSaveTimeMs[batgroup] = g_TimerMs;
			BatDataSave(batgroup);
		}		
	}//内阻测试中不许读取模块状态
/********************************超过设置的自动内阻测试时间，自动启动内阻测试*************************************/	
//自动内阻测量测量条件:1时间间隔达到，2上午九点，3浮充状态，4电池组未欠压，未过压，5电池未欠压，6内阻测量功能开启，7不能太频繁
	tempautotesttime = (uint32_t)MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_AutoMeaRTime*3600*1000*24;//单位为天,自动测量时间为0表示不允许进行自动测量
	if(((g_TimerMs- BATComTime.BatRLastTestTimeMs[batgroup])> tempautotesttime)&&(tempautotesttime != 0))
	{//超过自动测量时间间隔
		ttm = Time_GetCalendarTime();
		if((LcdComm.BatSysStat.BatStat[batgroup] == BATFLOATCHARGE)&&(ttm.tm_hour == AUTO_TEST_TIME))//自动测量内阻时，只许在浮充状态下
		{//浮充状态下，允许进行启动自动内阻测量
			if(CheckTestCondition(batgroup,ON) == TRUE)
			{
				MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = START;//标识为启动状态
				LcdComm.InnerTest.InnerRTestDevNO = 0;//初始化为0，从地址为0的模块开始进行巡检
				BatTestVTSave(batgroup);
				StartInnerRTest(batgroup,LcdComm.InnerTest.InnerRTestDevNO);//启动第一个模块进行内阻检测
				ReadRTestAddrDATA(batgroup,LcdComm.InnerTest.InnerRTestDevNO);//查询写入的数据，是否写入成功
				LcdComm.LcdSysCtrl[batgroup][LcdComm.InnerTest.InnerRTestDevNO].LCD_StartInterMea = RESET;//参数重置，方便查询模块是否接收到指令
				BATComTime.InnerRTestCtrlTimeMs[batgroup] = g_TimerMs;//记录启动时间
				BATComTime.BatRLastTestTimeMs[batgroup] = g_TimerMs;//记录为最近一次的内阻测量时间
				BATComTime.BatRTestTime[batgroup] = BAT_R_TEST_TIME;//刚开机时允许进行内阻测试，测试后设置再次测试时间间隔
				LcdComm.InnerTest.InnerRTestDevNO = 1;//下一个需要进行测试的模块编号
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_ING;
				LcdComm.itfNo = PICTURE132;//跳到内阻测试界面
				TempNO = (batgroup<<4);//高四位表示电池组号，
				HistoryDataSave(batgroup,0,TempNO,AUTO_TEST,0);//存储测量记录
			}
		}
	}
/**************************内阻测量过程******************************************/
/**************************内阻测量中，读取模块状态******************************************/
		if(LcdComm.InnerTest.InnerRTestFlag[batgroup] == R_TEST_ING)//检测一组电池的内阻测试状态
		{//测量过程中，不断的进行巡检状态查询
			tempdevno = LcdComm.InnerTest.InnerRTestDevNO;//必须要保存到另外的缓冲区里，防止因为任务调度改变该值
			if((g_TimerMs - BATComTime.InnerRTestCtrlTimeMs[batgroup]) > INNER_R_TEST_OVER_TIME)//内阻测试超时，停止测试，测试失败
			{//内阻测试超时，停止测试，测试失败，恢复到待启动状态
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_FAIL;//测试超时故障
				MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = STOP;				
				StopInnerRTest(batgroup);
				TempNO = (batgroup<<4)+LcdComm.InnerTest.InnerRTestDevNO-1;//高四位表示电池组号，低四位表示模块号
				HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL10,0);
				for(i=0;i<LcdComm.DevNUM[batgroup];i++)
				{//清除
					LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
				}
			}
			else if((((MbsSlave.StatusData.MoudleStat[batgroup][tempdevno-1].S_MeaInterRStat & 0xF0) >> 4) == R_TEST_FAIL)&&(LcdComm.LcdSysCtrl[batgroup][tempdevno-1].LCD_StartInterMea == START)&&(MbsSlave.StatusData.MoudleStat[batgroup][tempdevno-1].S_MeaInterRStat != 0x23))
			{//收到启动测试命令，内阻测试失败，则停止内阻巡检，恢复到待启动状态，
				RTestFail(batgroup,tempdevno-1);
				for(i=0;i<LcdComm.DevNUM[batgroup];i++)
				{//清除
					LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
				}
			}
			else if(tempdevno > 0)
			{
				if(((g_TimerMs - BATComTime.TesttingStatusReadTimeMs[batgroup])> 3000)&&(LcdComm.LcdSysCtrl[batgroup][tempdevno-1].LCD_StartInterMea == START))
				{//超过3秒，并且确认收到启动命令，读取测量状态
					SysStatusRead(DEVCOM,(tempdevno-1));//读取正在进行内阻测量模块的状态
					BATComTime.TesttingStatusReadTimeMs[batgroup] = g_TimerMs;
				}		
			}
		}
/**************************内阻测量完成，读取模块测量数据******************************************/
		if((LcdComm.InnerTest.InnerRTestFlag[batgroup] == R_TEST_SUCCESS)&&(MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea != STOP))//读取电池的内阻测试数据
		{//测量结束，读取测量数据，并存储
			LcdComm.InnerTest.TestFailFlag[batgroup]=OFF;
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;//测量成功，数据保存中
			for(i=0;i<LcdComm.DevNUM[batgroup];i++)
			{
				BATComTime.BatDataRReadTimeMs[batgroup] = g_TimerMs;//记录内阻数据读取时间
				memset(LcdComm.InnerTest.RDataReadFlag[batgroup],0,MAX_DEV_NUM);
				BatRRead(batgroup,i);//读取内阻测量数据
				if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].CRDevStat == ON)//有连接条模块，则读取连接条测量数据
				{
					memset(LcdComm.InnerTest.CRDataReadFlag[batgroup],0,MAX_DEV_NUM);
					BatCRRead(batgroup,i);//读取连接条测量数据
				}
			}
			for(i=0;i<LcdComm.DevNUM[batgroup];i++)
			{//清除
				LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
			}
		}
/**************************内阻数据读取完成，保存数据******************************************/
	if(LcdComm.InnerTest.InnerRTestFlag[batgroup] == DATA_SAVING)
	{
		for(i=0;i<LcdComm.DevNUM[batgroup];i++)
		{
			if(LcdComm.InnerTest.RDataReadFlag[batgroup][i] == OFF)
			{
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;
				TempStat = DATA_SAVING;//保存中
				break;
			}
			else
			{
				TempStat = DATA_SAVE_SUCCESS;//保存成功
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].CRDevStat == ON)
			{
				if(LcdComm.InnerTest.CRDataReadFlag[batgroup][i] == OFF)
				{
					LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;
					TempStat = DATA_SAVING;//保存中
					break;
				}
				else
				{
					TempStat = DATA_SAVE_SUCCESS;//保存成功
				}
			}
		}
		if(TempStat == DATA_SAVE_SUCCESS)
		{
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVE_SUCCESS;//保存成功
			CountBatRStat(batgroup);
			CountBatCRStat(batgroup);
			BatTestDataSave(batgroup);
		}
		else
		{
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;//保存中
			if((g_TimerMs - BATComTime.BatDataRReadTimeMs[batgroup]) > BATComTime.BatDataRReadTime[batgroup]*3)
			{//数据读取超时，则数据保存失败
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVE_FAIL;//没有把数据读上来，保存失败
			}
		}
	}
/**************************内阻测量过程******************************************/
}
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

