#include <includes.h>
/*************************************************************************************
**  void ResetFactoryPara(void)
**   功能:恢复出厂参数
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void ResetFactoryPara(void)
{
	uint8 i;
	uint8 SysSetLicense[] = SYS_LICENSE;
	uint8 FactorySetLicense[] = FACTORY_LICENSE;
	//密码恢复出厂设置
	Eeprom_Write_I2c((uint8 *)SysSetLicense, SYS_CFG_LICENSE_SAVE_OFFSET,BKP_SYS_CFG_LICENSE_SAVE_OFFSET,6);
	Eeprom_Write_I2c((uint8 *)FactorySetLicense, FACTORY_SET_LICENSE_SAVE_OFFSET,BKP_FACTORY_SET_LICENSE_SAVE_OFFSET,6);

	//历史记录参数恢复出厂设置
	MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1] = RESET;
	MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2] = RESET;
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1], HISTORY1_RECORD_QUANTITY_OFFSET,HISTORY1_RECORD_QUANTITY_NUM);
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2], HISTORY2_RECORD_QUANTITY_OFFSET,HISTORY2_RECORD_QUANTITY_NUM);
	MbsSlave.HistoryRecord.RecordOverFlag[GROUP1] = RESET;
	MbsSlave.HistoryRecord.RecordOverFlag[GROUP2] = RESET;
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);
	//通信参数恢复出厂设置
	InitComPara();
	//系统配置恢复出厂设置
	InitSysCFG();

	//电池温度探头屏蔽信息初始化
	memset(MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield,0,MAX_DEV_NUM*4);
	memset(MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield,0,MAX_DEV_NUM*4);
	Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);	
	Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);

	
}
/*************************************************************************************
**  void InitComPara(void)
**   功能:将故障数据写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void InitComPara(void)
{

}
/*************************************************************************************
**  void InitSysCFG(void)
**   功能:将故障数据写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void InitSysCFG(void)
{
	MbsSlave.SysPara.MSysCfg.S_CellGroupNum = CELLGROUPNUMVALUE;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum = CELLNUMVAULE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_GroupVoltageMax = GROUPVMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_GroupVoltageMin = GROUPVMINVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_ChargeCurrentMax = CHARGECURRENTMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_DischargeCurrentMax = DISCHARGECURRENTMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_ConnectRMax = CONNECTRMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_SingleCellRMax = SINGLECELLRMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_SingleVoltageMax = SINGEVMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_SingleVoltageMin = SINGLEVMINVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_TempMax = TEMPMAXVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_TempMin = TEMPMINVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_AutoMeaRTime = AUTOMEARTIMEVALUE1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].EqParaCfg = EQPARACFG1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].BatVoltageLevel = BATVLEVEL1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].InnerRDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].CRDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].EQDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].TempDevStat = OFF;	
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum = CELLNUMVAULE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_GroupVoltageMax = GROUPVMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_GroupVoltageMin = GROUPVMINVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_ChargeCurrentMax = CHARGECURRENTMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_DischargeCurrentMax = DISCHARGECURRENTMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_ConnectRMax = CONNECTRMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_SingleCellRMax = SINGLECELLRMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_SingleVoltageMax = SINGEVMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_SingleVoltageMin = SINGLEVMINVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_TempMax = TEMPMAXVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_TempMin = TEMPMINVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_AutoMeaRTime = AUTOMEARTIMEVALUE2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].EqParaCfg = EQPARACFG2;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].BatVoltageLevel = BATVLEVEL1;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].InnerRDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].CRDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].EQDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].TempDevStat = OFF;
	MbsSlave.SysPara.MSysCfg.CurrentChannel[GROUP1] = CURRENTCHANNEL1;
	MbsSlave.SysPara.MSysCfg.CurrentChannel[GROUP2] = CURRENTCHANNEL2;
	MbsSlave.SysPara.MSysCfg.S_GpsTimeCalEN = 0;
	MbsSlave.SysPara.MSysCfg.S_HostHWVersion = HARDWARE_VERSION;//硬件版本:主版本号，次版本号，修订号
	MbsSlave.SysPara.MSysCfg.S_HostSWVersion = SOFTWARE_VERSION;//软件版本:主版本号，次版本号，修订号

	if(Eeprom_Write_I2c((uint8 *)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, SYS_CFG_SAVE_OFFSET,BKP_SYS_CFG_SAVE_OFFSET,S_MSYS_CFG_NUM*2)==TRUE)
		{;}

}


void InitComPurpose(void)
{



}

/*************************************************************************************
**  void InitDataI2c(void)
**   功能:将故障数据写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void InitDataI2c(void)
{
	uint32 DataValidFlag = 0;
	uint8 SysSetLicense[] = SYS_LICENSE;
	uint8 FactorySetLicense[] = FACTORY_LICENSE;
	uint32 ReadAddr;
	uint8 i;



	
	#ifdef _RESTOREPARA__
		DataValidFlag = 0xE1E2E3E5;
		if(Eeprom_Write_I2c((uint8 *)&DataValidFlag, DATA_VALID_FLAG_OFFSET,BKP_DATA_VALID_FLAG_OFFSET, 4)==TRUE)
		{
				;
				;
		}
	#endif
	Eeprom_Read_I2c((uint8 *)&DataValidFlag, DATA_VALID_FLAG_OFFSET,BKP_DATA_VALID_FLAG_OFFSET, 4);
	if(DataValidFlag == DATA_VALID_FLAG)   
	{/* 非首次使用存储空间*/
		if(Eeprom_Read_I2c((uint8 *)&MbsSlave.SysPara.MSysCfg.GroupCfg[0].S_CellNum, SYS_CFG_SAVE_OFFSET+2,BKP_SYS_CFG_SAVE_OFFSET+2,(S_MSYS_CFG_NUM-2)*2) == TRUE)
		{//读取系统配置参数
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum == 0)
			{
				MbsSlave.SysPara.MSysCfg.S_CellGroupNum= 1;
			}
			else 
			{
				MbsSlave.SysPara.MSysCfg.S_CellGroupNum= 2;
			}
		}		
		//读取最近一次测量的内阻值
		TestTimePeriodRead();

		//读取历史记录总条数
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1],HISTORY1_RECORD_SAVE_OFFSET,2);
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2],HISTORY2_RECORD_SAVE_OFFSET,2);
		//读取历史记录是否已经超过500条标识符
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);
		//系统开机记录到历史记录
		HistoryDataSave(GROUP1,0,0,SYS_POWER_ON,0);
		HistoryDataSave(GROUP2,0,0,SYS_POWER_ON,0);
		//读取实时数据保存时间间隔
		Eeprom_Read((uint8 *)&BATComTime.BatDataSaveTime[GROUP1], SAVE1_TIME_PERIOD_ADDR_OFFSET,SAVE1_TIME_PERIOD_ADDR_NUM);
		Eeprom_Read((uint8 *)&BATComTime.BatDataSaveTime[GROUP2], SAVE2_TIME_PERIOD_ADDR_OFFSET,SAVE2_TIME_PERIOD_ADDR_NUM);
		//读取SOC值
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soc, SOC_SAVE1_OFFSET,SOC_SAVE1_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soc, SOC_SAVE2_OFFSET,SOC_SAVE2_NUM);
		//读取SOH值
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soh, SOH_SAVE1_OFFSET,SOH_SAVE1_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soh, SOH_SAVE2_OFFSET,SOH_SAVE2_NUM);
		//读取放电实时数据保存时间间隔
		Eeprom_Read((uint8 *)&BATComTime.DiscDataSaveTime[GROUP1], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);
		Eeprom_Read((uint8 *)&BATComTime.DiscDataSaveTime[GROUP2], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);

		
		//电池温度探头屏蔽信息读取
		Eeprom_Read((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2);	
		Eeprom_Read((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2);
		
	}
	else                                         /*首次使用存储空间 */
	{			
		DataValidFlag = DATA_VALID_FLAG;
		if(Eeprom_Write_I2c((uint8 *)&DataValidFlag, DATA_VALID_FLAG_OFFSET,BKP_DATA_VALID_FLAG_OFFSET, 4) == TRUE)
		{
			InitSysCFG();//预置系统配置参数
			InitComPara();//预制通通信端口配置参数
		}	
		//写入初始的系统设置密码和出厂设置密码
		Eeprom_Write_I2c((uint8 *)SysSetLicense, SYS_CFG_LICENSE_SAVE_OFFSET,BKP_SYS_CFG_LICENSE_SAVE_OFFSET,6);
		Eeprom_Write_I2c((uint8 *)FactorySetLicense, FACTORY_SET_LICENSE_SAVE_OFFSET,BKP_FACTORY_SET_LICENSE_SAVE_OFFSET,6);

		MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1] = 0;
		MbsSlave.HistoryRecord.RecordOverFlag[GROUP1] = 0;
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1],HISTORY1_RECORD_SAVE_OFFSET,2);
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
    
		MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2] = 0;
		MbsSlave.HistoryRecord.RecordOverFlag[GROUP2] = 0;
		//读取历史记录总条数
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2],HISTORY2_RECORD_SAVE_OFFSET,2);
		//读取历史记录是否已经超过500条标识符
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);


		HistoryDataSave(GROUP1,0,0,SYS_POWER_ON,0);
		HistoryDataSave(GROUP2,0,0,SYS_POWER_ON,0);

		TestTimePeriodSave();
      


    //设置存储时间间隔
		BATComTime.BatDataSaveTime[GROUP1] = 120;//单位为分钟，默认2小时保存一次数据
		BATComTime.BatDataSaveTime[GROUP2] = 120;
		Eeprom_Write((uint8 *)&BATComTime.BatDataSaveTime[GROUP1], SAVE1_TIME_PERIOD_ADDR_OFFSET,SAVE1_TIME_PERIOD_ADDR_NUM);
		Eeprom_Write((uint8 *)&BATComTime.BatDataSaveTime[GROUP2], SAVE2_TIME_PERIOD_ADDR_OFFSET,SAVE2_TIME_PERIOD_ADDR_NUM);

    //设置SOC初始值
		MbsSlave.Group[GROUP1].SysMeaData.Soc = 100;
		MbsSlave.Group[GROUP2].SysMeaData.Soc = 100;
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soc, SOC_SAVE1_OFFSET,SOC_SAVE1_NUM);
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soc, SOC_SAVE2_OFFSET,SOC_SAVE2_NUM);

    //设置SOH初始值,默认设置为200AH
		MbsSlave.Group[GROUP1].SysMeaData.Soh = 200;
		MbsSlave.Group[GROUP2].SysMeaData.Soh = 200;
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soh, SOH_SAVE1_OFFSET,SOH_SAVE1_NUM);
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soh, SOH_SAVE2_OFFSET,SOH_SAVE2_NUM);
    //放电实时数据保存时间间隔设置，默认15分钟保存一次
    		BATComTime.DiscDataSaveTime[GROUP1] = 30;
		BATComTime.DiscDataSaveTime[GROUP2] = 30;
		Eeprom_Write((uint8 *)&BATComTime.DiscDataSaveTime[GROUP1], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);
		Eeprom_Write((uint8 *)&BATComTime.DiscDataSaveTime[GROUP2], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);

		//电池温度探头屏蔽信息初始化
		memset(MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield,0,MAX_DEV_NUM*4);
		memset(MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield,0,MAX_DEV_NUM*4);
		Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);	
		Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);

	}   	
}
/*************************************************************************************
**  uint8 CountLastTestTime(uint8 com)
**   功能:将故障数据写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void CountLastTestTime(void)
{
//	struct
//	{
//		uint16 tempyear;
//		uint16 tempmon;
//		uint16 tempday;
//		uint16 temphour;
//		uint16 tempmin;
//		uint16 tempsec;
//	}TempTestTime[10];
//	uint32 TestTime[10];
//	uint8 i,lasttesttime1,lasttesttime2;
//	lasttesttime1 = 0;
//	lasttesttime2 = 0;
//	Eeprom_Read((uint8 *)LcdComm.LastTestTimesFlag1, LAST_TEST_SAVE_ADDR_FLAG1_OFFSET,LAST_TEST_ADDR_FLAG1_NUM);
//	Eeprom_Read((uint8 *)LcdComm.LastTestTimesFlag2, LAST_TEST_SAVE_ADDR_FLAG2_OFFSET,LAST_TEST_ADDR_FLAG2_NUM);

//	for(i=0;i<10;i++)//读取并计算出蓄电池组1最近一次的测量存储位置
//	{
//		Eeprom_Write((uint8 *)TempTestTime[i].tempyear, HIS1_MEA_TIME1_SAVE_OFFSET + HIS1_MEA_TIME1_NUM*i,12);
//		TestTime[i] = TempTestTime[i].tempmon*30*24*3600+TempTestTime[i].tempday*24*3600+TempTestTime[i].temphour*3600+TempTestTime[i].tempmin*60+TempTestTime[i].tempsec;
//	}
//	for(i=0;i<10;i++)
//	{
//		if(TempTestTime[lasttesttime1].tempyear > TempTestTime[i].tempyear)
//			{}
//		else if(TempTestTime[lasttesttime1].tempyear == TempTestTime[i].tempyear)
//		{
//			if(TestTime[lasttesttime1] <= TestTime[i])
//			{
//				lasttesttime1 = i;
//			}
//		}
//		else
//		{
//			lasttesttime1 = i;
//		}
//	}
//	LcdComm.LastTestTimesFlag1 = lasttesttime1;

//	for(i=0;i<10;i++)//读取并计算出蓄电池组2最近一次的测量存储位置
//	{
//		Eeprom_Write((uint8 *)TempTestTime[i].tempyear, HIS2_MEA_TIME1_SAVE_OFFSET + HIS2_MEA_TIME1_NUM*i,12);
//		TestTime[i] = TempTestTime[i].tempmon*30*24*3600+TempTestTime[i].tempday*24*3600+TempTestTime[i].temphour*3600+TempTestTime[i].tempmin*60+TempTestTime[i].tempsec;
//	}
//	for(i=0;i<10;i++)
//	{
//		if(TempTestTime[lasttesttime2].tempyear > TempTestTime[i].tempyear)
//			{}
//		else if(TempTestTime[lasttesttime2].tempyear == TempTestTime[i].tempyear)
//		{
//			if(TestTime[lasttesttime2] <= TestTime[i])
//			{
//				lasttesttime2 = i;
//			}
//		}
//		else
//		{
//			lasttesttime2 = i;
//		}
//	}
//	LcdComm.LastTestTimesFlag2 = lasttesttime2;
}
/*************************************************************************************
**  uint8 HistoryDataSave(uint8 com)
**   功能:将故障数据写入到铁电存储器
**   参数:   电池组号，模块号，模块内的电池编号，故障类型，故障数据
**   输出:  
**   描述: 当故障类型是电池故障时，故障源是电池编号，当故障类型是模块时，故障源
			高八位是电池组编号，第八位是模块编号
**************************************************************************************/
uint8 HistoryDataSave(uint8 batgroup,uint8 DevAddr,uint8 ErrSource,uint8 ErrType,int16 ErrData)
{
	uint16 tempRCDQ;
	struct tm ttm;
	uint32 TempAddr;
	uint8 ret = TRUE;
	

	return ret;
}
/*************************************************************************************
**  uint8 BatVDataSave(uint8 com)
**   功能:将测量内阻时的电压写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 先存储内阻测量时间(给20字节空间)，再存储216节电池内阻信息
(给480字节空间)，再存储216节电池连接条阻值信息(给480字节空间)，
再存储电池测量内阻时的电池电压信息(给480字节空间)，
再存储216节电池温度信息(给480字节空间)，空余20字节空间，进行下一组存储
**************************************************************************************/
uint8 BatVDataSave(uint8 groupnum,uint8 DevAddr,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 BatTempDataSave(uint8 com)
**   功能:将测量内阻时的温度写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 先存储内阻测量时间(给20字节空间)，再存储216节电池内阻信息
(给480字节空间)，再存储216节电池连接条阻值信息(给480字节空间)，
再存储电池测量内阻时的电池电压信息(给240字节空间)，
再存储216节电池温度信息(给480字节空间)，空余20字节空间，进行下一组存储
//温度存储规则，数据小于3276，大于-3276时，乘10存储，否则强制转换成整数存储
**************************************************************************************/
uint8 BatTempDataSave(uint8 groupnum,uint8 DevAddr,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 InnerRDataSave(uint8 com)
**   功能:将故障数据写入到铁电存储器
**   参数:   
**   输出:  
**   描述: 先存储内阻测量时间(给20字节空间)，再存储120节电池内阻信息
(给240字节空间)，再存储120节电池连接条阻值信息(给240字节空间)，
再存储电池测量内阻时的电池电压信息(给240字节空间)，
再存储120节电池温度信息(给240字节空间)，空余20字节空间，进行下一组存储
**************************************************************************************/
uint8 InnerRDataSave(uint8 groupnum,uint8 DevAddr)
{

	return 1;
}
/*************************************************************************************
**  uint8 BatStatDataSave(uint8 batgroup)
**   功能:电池组运行状态信息存储
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 BatStatDataSave(uint8 batgroup,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 CRDataSave(uint8 groupnum,uint8 DevAddr)
**   功能:将连接条阻值存储到铁电里面
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 CRDataSave(uint8 groupnum,uint8 DevAddr)
{

	return 1;
}
/***************************************************************************
**  void BatTestVTSave(uint8 batgroup)
**   功能:保存电池相关测量数据到铁电中，需要在蓄电池启动内阻测试之前存储
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatTestVTSave(uint8 batgroup)
{

}
/***************************************************************************
**  void BatTestDataSave(uint8 DEVCOM,uint8 batgroup)
**   功能:内阻测量数据保存
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void BatTestDataSave(uint8 batgroup)
{


}

/***************************************************************************
**  void TestTimePeriodSave(uint8 DEVCOM,uint8 batgroup)
**   功能:模块间测量时间间隔存储
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void TestTimePeriodSave(void)
{
}
/***************************************************************************
**  void DiscDataPeriodSave(void)
**   功能:放电实时数据存储时间间隔
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void DiscDataPeriodSave(void)
{
}
/***************************************************************************
**  void TestTimePeriodRead(void)
**   功能:模块间测量时间间隔读取
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void TestTimePeriodRead(void)
{
}

/***************************************************************************
**  void FdyPreDiscDataSave(void)
**   功能:放电仪放电前数据存储
**   参数:  
**   输出:  
**   描述: 存储放电前的电池电压，组电压， 温度，放电参数，放电起始时间
****************************************************************************/
void FdyPreDiscDataSave(uint8 batgroup)
{

}

/***************************************************************************
**  void FdyAfterDiscDataSave(void)
**   功能:放电仪放电后数据存储
**   参数:  
**   输出:  
**   描述: 存储放电后的电池电压，组电压，温度，放电停止时间
****************************************************************************/
void FdyAfterDiscDataSave(uint8 batgroup)
{


}

/***************************************************************************
**  void BatDataSave(void)
**   功能:电池数据存储
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
uint8 BatDataSave(uint8 batgroup)
{


	return TRUE;
}

/***************************************************************************
**  void DiscBatDataSave(void)
**   功能:放电时电池数据存储
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
uint8 DiscBatDataSave(uint8 batgroup)
{



	return TRUE;
}

