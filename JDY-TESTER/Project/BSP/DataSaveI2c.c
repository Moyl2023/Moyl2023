#include <includes.h>
/*************************************************************************************
**  void ResetFactoryPara(void)
**   ����:�ָ���������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
void ResetFactoryPara(void)
{
	uint8 i;
	uint8 SysSetLicense[] = SYS_LICENSE;
	uint8 FactorySetLicense[] = FACTORY_LICENSE;
	//����ָ���������
	Eeprom_Write_I2c((uint8 *)SysSetLicense, SYS_CFG_LICENSE_SAVE_OFFSET,BKP_SYS_CFG_LICENSE_SAVE_OFFSET,6);
	Eeprom_Write_I2c((uint8 *)FactorySetLicense, FACTORY_SET_LICENSE_SAVE_OFFSET,BKP_FACTORY_SET_LICENSE_SAVE_OFFSET,6);

	//��ʷ��¼�����ָ���������
	MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1] = RESET;
	MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2] = RESET;
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1], HISTORY1_RECORD_QUANTITY_OFFSET,HISTORY1_RECORD_QUANTITY_NUM);
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2], HISTORY2_RECORD_QUANTITY_OFFSET,HISTORY2_RECORD_QUANTITY_NUM);
	MbsSlave.HistoryRecord.RecordOverFlag[GROUP1] = RESET;
	MbsSlave.HistoryRecord.RecordOverFlag[GROUP2] = RESET;
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
	Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);
	//ͨ�Ų����ָ���������
	InitComPara();
	//ϵͳ���ûָ���������
	InitSysCFG();

	//����¶�̽ͷ������Ϣ��ʼ��
	memset(MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield,0,MAX_DEV_NUM*4);
	memset(MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield,0,MAX_DEV_NUM*4);
	Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);	
	Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);

	
}
/*************************************************************************************
**  void InitComPara(void)
**   ����:����������д�뵽����洢��
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
void InitComPara(void)
{

}
/*************************************************************************************
**  void InitSysCFG(void)
**   ����:����������д�뵽����洢��
**   ����:   
**   ���:  
**   ����: 
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
	MbsSlave.SysPara.MSysCfg.S_HostHWVersion = HARDWARE_VERSION;//Ӳ���汾:���汾�ţ��ΰ汾�ţ��޶���
	MbsSlave.SysPara.MSysCfg.S_HostSWVersion = SOFTWARE_VERSION;//����汾:���汾�ţ��ΰ汾�ţ��޶���

	if(Eeprom_Write_I2c((uint8 *)&MbsSlave.SysPara.MSysCfg.S_CellGroupNum, SYS_CFG_SAVE_OFFSET,BKP_SYS_CFG_SAVE_OFFSET,S_MSYS_CFG_NUM*2)==TRUE)
		{;}

}


void InitComPurpose(void)
{



}

/*************************************************************************************
**  void InitDataI2c(void)
**   ����:����������д�뵽����洢��
**   ����:   
**   ���:  
**   ����: 
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
	{/* ���״�ʹ�ô洢�ռ�*/
		if(Eeprom_Read_I2c((uint8 *)&MbsSlave.SysPara.MSysCfg.GroupCfg[0].S_CellNum, SYS_CFG_SAVE_OFFSET+2,BKP_SYS_CFG_SAVE_OFFSET+2,(S_MSYS_CFG_NUM-2)*2) == TRUE)
		{//��ȡϵͳ���ò���
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum == 0)
			{
				MbsSlave.SysPara.MSysCfg.S_CellGroupNum= 1;
			}
			else 
			{
				MbsSlave.SysPara.MSysCfg.S_CellGroupNum= 2;
			}
		}		
		//��ȡ���һ�β���������ֵ
		TestTimePeriodRead();

		//��ȡ��ʷ��¼������
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1],HISTORY1_RECORD_SAVE_OFFSET,2);
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2],HISTORY2_RECORD_SAVE_OFFSET,2);
		//��ȡ��ʷ��¼�Ƿ��Ѿ�����500����ʶ��
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);
		//ϵͳ������¼����ʷ��¼
		HistoryDataSave(GROUP1,0,0,SYS_POWER_ON,0);
		HistoryDataSave(GROUP2,0,0,SYS_POWER_ON,0);
		//��ȡʵʱ���ݱ���ʱ����
		Eeprom_Read((uint8 *)&BATComTime.BatDataSaveTime[GROUP1], SAVE1_TIME_PERIOD_ADDR_OFFSET,SAVE1_TIME_PERIOD_ADDR_NUM);
		Eeprom_Read((uint8 *)&BATComTime.BatDataSaveTime[GROUP2], SAVE2_TIME_PERIOD_ADDR_OFFSET,SAVE2_TIME_PERIOD_ADDR_NUM);
		//��ȡSOCֵ
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soc, SOC_SAVE1_OFFSET,SOC_SAVE1_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soc, SOC_SAVE2_OFFSET,SOC_SAVE2_NUM);
		//��ȡSOHֵ
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soh, SOH_SAVE1_OFFSET,SOH_SAVE1_NUM);
		Eeprom_Read((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soh, SOH_SAVE2_OFFSET,SOH_SAVE2_NUM);
		//��ȡ�ŵ�ʵʱ���ݱ���ʱ����
		Eeprom_Read((uint8 *)&BATComTime.DiscDataSaveTime[GROUP1], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);
		Eeprom_Read((uint8 *)&BATComTime.DiscDataSaveTime[GROUP2], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);

		
		//����¶�̽ͷ������Ϣ��ȡ
		Eeprom_Read((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2);	
		Eeprom_Read((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2);
		
	}
	else                                         /*�״�ʹ�ô洢�ռ� */
	{			
		DataValidFlag = DATA_VALID_FLAG;
		if(Eeprom_Write_I2c((uint8 *)&DataValidFlag, DATA_VALID_FLAG_OFFSET,BKP_DATA_VALID_FLAG_OFFSET, 4) == TRUE)
		{
			InitSysCFG();//Ԥ��ϵͳ���ò���
			InitComPara();//Ԥ��ͨͨ�Ŷ˿����ò���
		}	
		//д���ʼ��ϵͳ��������ͳ�����������
		Eeprom_Write_I2c((uint8 *)SysSetLicense, SYS_CFG_LICENSE_SAVE_OFFSET,BKP_SYS_CFG_LICENSE_SAVE_OFFSET,6);
		Eeprom_Write_I2c((uint8 *)FactorySetLicense, FACTORY_SET_LICENSE_SAVE_OFFSET,BKP_FACTORY_SET_LICENSE_SAVE_OFFSET,6);

		MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1] = 0;
		MbsSlave.HistoryRecord.RecordOverFlag[GROUP1] = 0;
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP1],HISTORY1_RECORD_SAVE_OFFSET,2);
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP1], QUANTITY1_OVER_MAX_NUM_OFFSET,QUANTITY1_FLAG_NUM);
    
		MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2] = 0;
		MbsSlave.HistoryRecord.RecordOverFlag[GROUP2] = 0;
		//��ȡ��ʷ��¼������
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.HistoryRecordQuantity[GROUP2],HISTORY2_RECORD_SAVE_OFFSET,2);
		//��ȡ��ʷ��¼�Ƿ��Ѿ�����500����ʶ��
		Eeprom_Write((uint8 *)&MbsSlave.HistoryRecord.RecordOverFlag[GROUP2], QUANTITY2_OVER_MAX_NUM_OFFSET,QUANTITY2_FLAG_NUM);


		HistoryDataSave(GROUP1,0,0,SYS_POWER_ON,0);
		HistoryDataSave(GROUP2,0,0,SYS_POWER_ON,0);

		TestTimePeriodSave();
      


    //���ô洢ʱ����
		BATComTime.BatDataSaveTime[GROUP1] = 120;//��λΪ���ӣ�Ĭ��2Сʱ����һ������
		BATComTime.BatDataSaveTime[GROUP2] = 120;
		Eeprom_Write((uint8 *)&BATComTime.BatDataSaveTime[GROUP1], SAVE1_TIME_PERIOD_ADDR_OFFSET,SAVE1_TIME_PERIOD_ADDR_NUM);
		Eeprom_Write((uint8 *)&BATComTime.BatDataSaveTime[GROUP2], SAVE2_TIME_PERIOD_ADDR_OFFSET,SAVE2_TIME_PERIOD_ADDR_NUM);

    //����SOC��ʼֵ
		MbsSlave.Group[GROUP1].SysMeaData.Soc = 100;
		MbsSlave.Group[GROUP2].SysMeaData.Soc = 100;
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soc, SOC_SAVE1_OFFSET,SOC_SAVE1_NUM);
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soc, SOC_SAVE2_OFFSET,SOC_SAVE2_NUM);

    //����SOH��ʼֵ,Ĭ������Ϊ200AH
		MbsSlave.Group[GROUP1].SysMeaData.Soh = 200;
		MbsSlave.Group[GROUP2].SysMeaData.Soh = 200;
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP1].SysMeaData.Soh, SOH_SAVE1_OFFSET,SOH_SAVE1_NUM);
		Eeprom_Write((uint8 *)&MbsSlave.Group[GROUP2].SysMeaData.Soh, SOH_SAVE2_OFFSET,SOH_SAVE2_NUM);
    //�ŵ�ʵʱ���ݱ���ʱ�������ã�Ĭ��15���ӱ���һ��
    		BATComTime.DiscDataSaveTime[GROUP1] = 30;
		BATComTime.DiscDataSaveTime[GROUP2] = 30;
		Eeprom_Write((uint8 *)&BATComTime.DiscDataSaveTime[GROUP1], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);
		Eeprom_Write((uint8 *)&BATComTime.DiscDataSaveTime[GROUP2], DISC1_SAVE_TIME_PERIOD_OFFSET,DISC1_SAVE_TIME_PERIOD_NUM);

		//����¶�̽ͷ������Ϣ��ʼ��
		memset(MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield,0,MAX_DEV_NUM*4);
		memset(MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield,0,MAX_DEV_NUM*4);
		Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP1].TempShield[0], TEMP1_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);	
		Eeprom_Write((uint8 *)&MbsSlave.SysPara.ShieldInfo[GROUP2].TempShield[0], TEMP2_SHIELD_ADDR_OFFSET,MAX_DEV_NUM*2*2);

	}   	
}
/*************************************************************************************
**  uint8 CountLastTestTime(uint8 com)
**   ����:����������д�뵽����洢��
**   ����:   
**   ���:  
**   ����: 
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

//	for(i=0;i<10;i++)//��ȡ�������������1���һ�εĲ����洢λ��
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

//	for(i=0;i<10;i++)//��ȡ�������������2���һ�εĲ����洢λ��
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
**   ����:����������д�뵽����洢��
**   ����:   �����ţ�ģ��ţ�ģ���ڵĵ�ر�ţ��������ͣ���������
**   ���:  
**   ����: �����������ǵ�ع���ʱ������Դ�ǵ�ر�ţ�������������ģ��ʱ������Դ
			�߰�λ�ǵ�����ţ��ڰ�λ��ģ����
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
**   ����:����������ʱ�ĵ�ѹд�뵽����洢��
**   ����:   
**   ���:  
**   ����: �ȴ洢�������ʱ��(��20�ֽڿռ�)���ٴ洢216�ڵ��������Ϣ
(��480�ֽڿռ�)���ٴ洢216�ڵ����������ֵ��Ϣ(��480�ֽڿռ�)��
�ٴ洢��ز�������ʱ�ĵ�ص�ѹ��Ϣ(��480�ֽڿռ�)��
�ٴ洢216�ڵ���¶���Ϣ(��480�ֽڿռ�)������20�ֽڿռ䣬������һ��洢
**************************************************************************************/
uint8 BatVDataSave(uint8 groupnum,uint8 DevAddr,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 BatTempDataSave(uint8 com)
**   ����:����������ʱ���¶�д�뵽����洢��
**   ����:   
**   ���:  
**   ����: �ȴ洢�������ʱ��(��20�ֽڿռ�)���ٴ洢216�ڵ��������Ϣ
(��480�ֽڿռ�)���ٴ洢216�ڵ����������ֵ��Ϣ(��480�ֽڿռ�)��
�ٴ洢��ز�������ʱ�ĵ�ص�ѹ��Ϣ(��240�ֽڿռ�)��
�ٴ洢216�ڵ���¶���Ϣ(��480�ֽڿռ�)������20�ֽڿռ䣬������һ��洢
//�¶ȴ洢��������С��3276������-3276ʱ����10�洢������ǿ��ת���������洢
**************************************************************************************/
uint8 BatTempDataSave(uint8 groupnum,uint8 DevAddr,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 InnerRDataSave(uint8 com)
**   ����:����������д�뵽����洢��
**   ����:   
**   ���:  
**   ����: �ȴ洢�������ʱ��(��20�ֽڿռ�)���ٴ洢120�ڵ��������Ϣ
(��240�ֽڿռ�)���ٴ洢120�ڵ����������ֵ��Ϣ(��240�ֽڿռ�)��
�ٴ洢��ز�������ʱ�ĵ�ص�ѹ��Ϣ(��240�ֽڿռ�)��
�ٴ洢120�ڵ���¶���Ϣ(��240�ֽڿռ�)������20�ֽڿռ䣬������һ��洢
**************************************************************************************/
uint8 InnerRDataSave(uint8 groupnum,uint8 DevAddr)
{

	return 1;
}
/*************************************************************************************
**  uint8 BatStatDataSave(uint8 batgroup)
**   ����:���������״̬��Ϣ�洢
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 BatStatDataSave(uint8 batgroup,uint8 TimesFlag)
{

	return 1;
}
/*************************************************************************************
**  uint8 CRDataSave(uint8 groupnum,uint8 DevAddr)
**   ����:����������ֵ�洢����������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 CRDataSave(uint8 groupnum,uint8 DevAddr)
{

	return 1;
}
/***************************************************************************
**  void BatTestVTSave(uint8 batgroup)
**   ����:��������ز������ݵ������У���Ҫ�����������������֮ǰ�洢
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void BatTestVTSave(uint8 batgroup)
{

}
/***************************************************************************
**  void BatTestDataSave(uint8 DEVCOM,uint8 batgroup)
**   ����:����������ݱ���
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void BatTestDataSave(uint8 batgroup)
{


}

/***************************************************************************
**  void TestTimePeriodSave(uint8 DEVCOM,uint8 batgroup)
**   ����:ģ������ʱ�����洢
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void TestTimePeriodSave(void)
{
}
/***************************************************************************
**  void DiscDataPeriodSave(void)
**   ����:�ŵ�ʵʱ���ݴ洢ʱ����
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void DiscDataPeriodSave(void)
{
}
/***************************************************************************
**  void TestTimePeriodRead(void)
**   ����:ģ������ʱ������ȡ
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void TestTimePeriodRead(void)
{
}

/***************************************************************************
**  void FdyPreDiscDataSave(void)
**   ����:�ŵ��Ƿŵ�ǰ���ݴ洢
**   ����:  
**   ���:  
**   ����: �洢�ŵ�ǰ�ĵ�ص�ѹ�����ѹ�� �¶ȣ��ŵ�������ŵ���ʼʱ��
****************************************************************************/
void FdyPreDiscDataSave(uint8 batgroup)
{

}

/***************************************************************************
**  void FdyAfterDiscDataSave(void)
**   ����:�ŵ��Ƿŵ�����ݴ洢
**   ����:  
**   ���:  
**   ����: �洢�ŵ��ĵ�ص�ѹ�����ѹ���¶ȣ��ŵ�ֹͣʱ��
****************************************************************************/
void FdyAfterDiscDataSave(uint8 batgroup)
{


}

/***************************************************************************
**  void BatDataSave(void)
**   ����:������ݴ洢
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
uint8 BatDataSave(uint8 batgroup)
{


	return TRUE;
}

/***************************************************************************
**  void DiscBatDataSave(void)
**   ����:�ŵ�ʱ������ݴ洢
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
uint8 DiscBatDataSave(uint8 batgroup)
{



	return TRUE;
}

