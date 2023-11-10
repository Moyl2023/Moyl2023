/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppCom1.c
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               主机显示通信任务
****************************************************************************************/
#include <includes.h>




/***************************************************************************
**  void LedON(void)
**   功能: 灯控制程序
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void LedON(void)
{	

}

/***************************************************************************
**  void LedOFF(void)
**   功能: 灯控制程序
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void LedOFF(void)
{	

}


/***************************************************************************
**  void FeedDog(uint32 WdtTimeFlag)
**   功能: 喂狗控制程序
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void FeedDog(uint32 WdtTimeFlag)
{
	if((JdyTest.g_TimerMs - WdtTimeFlag)> 500)
	{//喂狗
		WdtTimeFlag = JdyTest.g_TimerMs;
		ToggleGPIORev(&WDT);
	}
}
/***************************************************************************
**  void BeepCtrl(uint32 BeepTimeTemp,uint8 flag)
**   功能: 蜂鸣器控制程序
**   参数:  
**   输出:  
**   描述: //有故障时SysFlag.BeepFlag置ON，有故障写入到铁电时LcdComm.SysErrBeepFlag置ON，调试时静音LcdComm.BeepFlag置OFF
****************************************************************************/
uint8 BeepCtrl(uint32 *p,uint8 flag)
{

	return 1;
}
/***************************************************************************
**  void ErrOutputCheck(uint8 BatGroup)
**   功能: 硬节点报警输出检查
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/ 
void ErrOutputCheck(void)
{
	uint8 BatErrFlag,DevErrFlag;
	uint16 i,j;
	uint8 TmpFlag = OFF;//统计是否有电池欠压故障标识符
	DevErrFlag = OFF;
	BatErrFlag = OFF;
	for(j=0;j<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;j++)
	{
		for(i=0;i<MbsSlave.StatusData.BatErrStat[j].S_Quantity;i++)
		{
			switch(MbsSlave.StatusData.BatErrStat[j].S_ErrInfo[i].S_ErrType)
			{
				case VOLTAGE_LOW:
				case VOLTAGE_OVER:	
				case GROUP_V_OVER:
				case GROUP_V_LOW:
				case BAT_SAMPLE_LINE_DROP:
				case GROUP_SAMPLE_LINE_DROP:
				case BAT_SAMPLE_LINE_RESERVE:
				case GROUP_SAMPLE_LINE_RESERVE:					
					TmpFlag = ON;
					BatErrFlag = ON;//暂时不区分电池故障和设备故障
					break;
				case TEMP_OVER:
				case TEMP_LOW:			
					TmpFlag = ON;//进入故障界面
					//BatErrFlag = ON;//暂时不区分电池故障和设备故障
					break;
				case CURRENT_DISCHARGE_OVER:
				case CURRENT_CHARGE_OVER:
					break;
				case ENVIRMENT_TEMP_OVER:
				case ENVIRMENT_TEMP_LOW:
					BatErrFlag = ON;
					break;
				case DEV_COMM_ERR:
				case BAT5000_S_COM_ERR:
				case FDY_COM_ERR:
					DevErrFlag = ON;
					break;
				case DEV_TEMP_ERR:
				case ENVIR_TEMP_ERR:
				case GPS_COM_ERR:
				case SYS_OPEN_CIRCUIT:
				case DEV_INNER_R_ERR:
				case DEV_CR_ERR:
				case DEV_EQ_ERR:
					//BatErrFlag = ON;//暂时不区分电池故障和设备故障
					break;
				default:
					break;
			}
		}
	}

	
	if(DevErrFlag == ON)
	{//设备故障干接点，常开，输出2，设备有问题时，处于断开状态，没问题时，闭合，主机断电，断开
		OutPutOFF(JOUTPUT1);
	}
	else
	{
		OutPutON(JOUTPUT1);
	}	
	
	if(BatErrFlag == ON)
	{//蓄电池异常告警，常闭，输出1
		OutPutON(JOUTPUT0);
	}
	else
	{
		OutPutOFF(JOUTPUT0);
	}
	
}

/***************************************************************************
**  void DisChargeCtrl(void)
**   功能: 放电控制
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/ 
void DisChargeCtrl(void)
{
	uint8 batgroup,i;
	uint8 tmpdisflag = DIS_NORMAL;
	
	if(MbsSlave.Group[batgroup].MeaData.VoltageValue < MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMin)
	{//组电压欠压，停止放电
		MbsSlave.SysPara.SysCtrl[batgroup].S_BatDisCmd = OFF;
		tmpdisflag = GROUPVLOW;
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;i++)
	{//如果有欠压，不允许进行放电
		if(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM]<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleVoltageMin)
		{
			MbsSlave.SysPara.SysCtrl[batgroup].S_BatDisCmd = OFF;
			tmpdisflag = BATVLOW;
			break;
		}
	}

	if(tmpdisflag != DIS_NORMAL)
	{
		//LcdComm.DisStat.DisChargeFlag = tmpdisflag;
	}
	else
	{
		//LcdComm.DisStat.DisChargeFlag = DIS_NORMAL;
	}
}


/***************************************************************************
**  void CheckBatStat(void)
**   功能: 检查蓄电池组状态，包括单体电池过欠压，电池组过欠压，温度异常
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/ 
void CheckBatStat(uint8 batgroup)
{
	uint8 i;
	uint16 SigFlag1=0,SigFlag2=0,SigFlag3=0,SigFlag4=0;
	struct{
		uint8 BatVOverFlag;
		uint8 BatVLowFlag;
		uint8 GroupVOverFlag;
		uint8 GroupVLowFlag;
		uint8 TempOverFlag;
		uint8 TempLowFlag;
		uint8 InnerROverFlag;
		uint8 CROverFlag;
		uint8 IOverFlag;
		uint8 DisIOverFlag;
		uint8 TempErrFlag;
		uint8 DevTempErrFlag;
		uint8 BatSmpLineErrFlag;
		uint8 GroupSmpLineErrFlag;
		uint16 CommErrFlag;
	}SysSignal;
	if(JdyTest.g_TimerMs < 15000)
	{
		return;
	}
	memset(&SysSignal.BatVOverFlag,0,sizeof(SysSignal));
/**********************电池组1****************************/
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{//单体电池电压过压
		if(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] > MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleVoltageMax)
		{
			SysSignal.BatVOverFlag = ON;
			break;
		}
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{//单体电池电压欠压
		if((MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM]<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleVoltageMin)
			&&(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] > 800))
		{
			SysSignal.BatVLowFlag = ON;
		}
		else if(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] < 800)
		{//采样线异常
			SysSignal.BatSmpLineErrFlag = ON;
		}
		if((SysSignal.BatSmpLineErrFlag != OFF)&&(SysSignal.BatVLowFlag != OFF))
		{//欠压和采样线异常同时出现了，就不再检查
			break;
		}
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{
		if(MbsSlave.BatData[batgroup].DevMeaData.S_InterR[i/MAX_BAT_NUM][i%MAX_BAT_NUM]>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleCellRMax)
		{//内阻阻值过高
			SysSignal.InnerROverFlag = ON;
		}
		if(MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[i/MAX_BAT_NUM][i%MAX_BAT_NUM]>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_ConnectRMax)
		{//连接条阻值过高
			SysSignal.CROverFlag = ON;
		}
	}

	if(MbsSlave.Group[batgroup].MeaData.VoltageValue>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMax)
	{//组电压过压
		SysSignal.GroupVOverFlag = ON;
	}
	if((MbsSlave.Group[batgroup].MeaData.VoltageValue<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMin)
		&&(MbsSlave.Group[batgroup].MeaData.VoltageValue > 3))
	{//组电压欠压
		SysSignal.GroupVLowFlag = ON;
	}
	if(MbsSlave.Group[batgroup].MeaData.VoltageValue <= 3)
	{//组电压采样线异常
		SysSignal.GroupSmpLineErrFlag = ON;
	}

	if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].TempDevStat == ON)
	{//电池温度探头
		for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
		{
			if((MbsSlave.BatData[batgroup].DevMeaData.S_Temp[i/MAX_BAT_NUM][i%MAX_BAT_NUM]>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_TempMax)
			&&(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[i/MAX_BAT_NUM][i%MAX_BAT_NUM] != 0xFAFA))
			{
				SysSignal.TempOverFlag = ON;
			}
			else if((MbsSlave.BatData[batgroup].DevMeaData.S_Temp[i/MAX_BAT_NUM][i%MAX_BAT_NUM]<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_TempMin)
			&&(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[i/MAX_BAT_NUM][i%MAX_BAT_NUM] != 0xFAFA))
			{
				SysSignal.TempLowFlag = ON;
			}
			else if(MbsSlave.BatData[batgroup].DevMeaData.S_Temp[i/MAX_BAT_NUM][i%MAX_BAT_NUM] == 0xFAFA)
			{
				SysSignal.TempErrFlag = ON;
			}
		}	
	}
	if(SysSignal.BatVOverFlag != OFF)
	{//电池过压
		SigFlag1 |=(1<<VOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<VOVER);
	}
	if(SysSignal.BatVLowFlag != OFF)
	{//电池欠压
		SigFlag1 |=(1<<VLOW);
	}
	else
	{
		SigFlag1 &= ~(1<<VLOW);
	}
	if(SysSignal.BatVOverFlag != OFF)
	{//电池组电压过压
		SigFlag1 |=(1<<GROUPVOVERSIG);
	}
	else
	{
		SigFlag1 &= ~(1<<GROUPVOVERSIG);
	}
	if(SysSignal.BatVLowFlag != OFF)
	{//电池组电压欠压
		SigFlag1 |=(1<<GROUPVLOWSIG);
	}
	else
	{
		SigFlag1 &= ~(1<<GROUPVLOWSIG);
	}
	if(SysSignal.TempOverFlag != OFF)
	{//电池温度过高
		SigFlag1 |=(1<<TOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<TOVER);
	}
	if(SysSignal.TempLowFlag != OFF)
	{//电池温度过低
		SigFlag1 |=(1<<TLOW);
	}
	else
	{
		SigFlag1 &= ~(1<<TLOW);
	}	
	if(SysSignal.InnerROverFlag != OFF)
	{//电池内阻过高
		SigFlag1 |=(1<<INNERROVER);
	}
	else
	{
		SigFlag1 &= ~(1<<INNERROVER);
	}	
	if(SysSignal.CROverFlag != OFF)
	{//连接条阻值过高
		SigFlag1 |=(1<<CROVER);
	}
	else
	{
		SigFlag1 &= ~(1<<CROVER);
	}
	if(SysSignal.IOverFlag != OFF)
	{//电池充电电流过高
		SigFlag1 |=(1<<IOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<IOVER);
	}	
	if(SysSignal.DisIOverFlag != OFF)
	{//电池放电电流过高
		SigFlag1 |=(1<<DISIOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<DISIOVER);
	}	
	//设备通信异常遥信
	SigFlag3 &= 0x8000;
	SigFlag3 |= SysSignal.CommErrFlag;
	if(SysSignal.GroupSmpLineErrFlag != OFF)
	{//组采样线异常
		SigFlag3 |=(1<<GROUP_SMP_LINE_ERR);
	}
	else
	{
		SigFlag3 &= ~(1<<GROUP_SMP_LINE_ERR);
	}	
	if(SysSignal.BatSmpLineErrFlag != OFF)
	{//电池采样线异常
		SigFlag4 |=(1<<0);
	}
	else
	{
		SigFlag4 &= ~(1<<0);
	}	
	if(SysSignal.TempErrFlag != OFF)
	{//温度探头异常
		SigFlag4 |=(1<<1);
	}
	else
	{
		SigFlag4 &= ~(1<<1);
	}	
	if(SysSignal.DevTempErrFlag != OFF)
	{//环境温度探头异常
		SigFlag4 |=(1<<2);
	}
	else
	{
		SigFlag4 &= ~(1<<2);
	}	
	MbsSlave.StatusData.MbsSingal[batgroup].MbsSignal1 = SigFlag1;
	MbsSlave.StatusData.MbsSingal[batgroup].MbsSignal2 = SigFlag2;
	MbsSlave.StatusData.MbsSingal[batgroup].MbsSignal3 = SigFlag3;
	MbsSlave.StatusData.MbsSingal[batgroup].MbsSignal4 = SigFlag4;
}


