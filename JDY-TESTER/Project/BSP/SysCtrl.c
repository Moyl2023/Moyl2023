/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppCom1.c
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions: �������߼��ϵͳ
**               ������ʾͨ������
****************************************************************************************/
#include <includes.h>




/***************************************************************************
**  void LedON(void)
**   ����: �ƿ��Ƴ���
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void LedON(void)
{	

}

/***************************************************************************
**  void LedOFF(void)
**   ����: �ƿ��Ƴ���
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void LedOFF(void)
{	

}


/***************************************************************************
**  void FeedDog(uint32 WdtTimeFlag)
**   ����: ι�����Ƴ���
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void FeedDog(uint32 WdtTimeFlag)
{
	if((JdyTest.g_TimerMs - WdtTimeFlag)> 500)
	{//ι��
		WdtTimeFlag = JdyTest.g_TimerMs;
		ToggleGPIORev(&WDT);
	}
}
/***************************************************************************
**  void BeepCtrl(uint32 BeepTimeTemp,uint8 flag)
**   ����: ���������Ƴ���
**   ����:  
**   ���:  
**   ����: //�й���ʱSysFlag.BeepFlag��ON���й���д�뵽����ʱLcdComm.SysErrBeepFlag��ON������ʱ����LcdComm.BeepFlag��OFF
****************************************************************************/
uint8 BeepCtrl(uint32 *p,uint8 flag)
{

	return 1;
}
/***************************************************************************
**  void ErrOutputCheck(uint8 BatGroup)
**   ����: Ӳ�ڵ㱨��������
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/ 
void ErrOutputCheck(void)
{
	uint8 BatErrFlag,DevErrFlag;
	uint16 i,j;
	uint8 TmpFlag = OFF;//ͳ���Ƿ��е��Ƿѹ���ϱ�ʶ��
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
					BatErrFlag = ON;//��ʱ�����ֵ�ع��Ϻ��豸����
					break;
				case TEMP_OVER:
				case TEMP_LOW:			
					TmpFlag = ON;//������Ͻ���
					//BatErrFlag = ON;//��ʱ�����ֵ�ع��Ϻ��豸����
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
					//BatErrFlag = ON;//��ʱ�����ֵ�ع��Ϻ��豸����
					break;
				default:
					break;
			}
		}
	}

	
	if(DevErrFlag == ON)
	{//�豸���ϸɽӵ㣬���������2���豸������ʱ�����ڶϿ�״̬��û����ʱ���պϣ������ϵ磬�Ͽ�
		OutPutOFF(JOUTPUT1);
	}
	else
	{
		OutPutON(JOUTPUT1);
	}	
	
	if(BatErrFlag == ON)
	{//�����쳣�澯�����գ����1
		OutPutON(JOUTPUT0);
	}
	else
	{
		OutPutOFF(JOUTPUT0);
	}
	
}

/***************************************************************************
**  void DisChargeCtrl(void)
**   ����: �ŵ����
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/ 
void DisChargeCtrl(void)
{
	uint8 batgroup,i;
	uint8 tmpdisflag = DIS_NORMAL;
	
	if(MbsSlave.Group[batgroup].MeaData.VoltageValue < MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMin)
	{//���ѹǷѹ��ֹͣ�ŵ�
		MbsSlave.SysPara.SysCtrl[batgroup].S_BatDisCmd = OFF;
		tmpdisflag = GROUPVLOW;
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;i++)
	{//�����Ƿѹ����������зŵ�
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
**   ����: ���������״̬�����������ع�Ƿѹ��������Ƿѹ���¶��쳣
**   ����:  
**   ���:  
**   ����: 
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
/**********************�����1****************************/
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{//�����ص�ѹ��ѹ
		if(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] > MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleVoltageMax)
		{
			SysSignal.BatVOverFlag = ON;
			break;
		}
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{//�����ص�ѹǷѹ
		if((MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM]<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleVoltageMin)
			&&(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] > 800))
		{
			SysSignal.BatVLowFlag = ON;
		}
		else if(MbsSlave.BatData[batgroup].DevMeaData.S_BatV[i/MAX_BAT_NUM][i%MAX_BAT_NUM] < 800)
		{//�������쳣
			SysSignal.BatSmpLineErrFlag = ON;
		}
		if((SysSignal.BatSmpLineErrFlag != OFF)&&(SysSignal.BatVLowFlag != OFF))
		{//Ƿѹ�Ͳ������쳣ͬʱ�����ˣ��Ͳ��ټ��
			break;
		}
	}
	for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_CellNum;i++)
	{
		if(MbsSlave.BatData[batgroup].DevMeaData.S_InterR[i/MAX_BAT_NUM][i%MAX_BAT_NUM]>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_SingleCellRMax)
		{//������ֵ����
			SysSignal.InnerROverFlag = ON;
		}
		if(MbsSlave.BatData[batgroup].DevMeaData.S_ConnectR[i/MAX_BAT_NUM][i%MAX_BAT_NUM]>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_ConnectRMax)
		{//��������ֵ����
			SysSignal.CROverFlag = ON;
		}
	}

	if(MbsSlave.Group[batgroup].MeaData.VoltageValue>MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMax)
	{//���ѹ��ѹ
		SysSignal.GroupVOverFlag = ON;
	}
	if((MbsSlave.Group[batgroup].MeaData.VoltageValue<MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_GroupVoltageMin)
		&&(MbsSlave.Group[batgroup].MeaData.VoltageValue > 3))
	{//���ѹǷѹ
		SysSignal.GroupVLowFlag = ON;
	}
	if(MbsSlave.Group[batgroup].MeaData.VoltageValue <= 3)
	{//���ѹ�������쳣
		SysSignal.GroupSmpLineErrFlag = ON;
	}

	if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].TempDevStat == ON)
	{//����¶�̽ͷ
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
	{//��ع�ѹ
		SigFlag1 |=(1<<VOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<VOVER);
	}
	if(SysSignal.BatVLowFlag != OFF)
	{//���Ƿѹ
		SigFlag1 |=(1<<VLOW);
	}
	else
	{
		SigFlag1 &= ~(1<<VLOW);
	}
	if(SysSignal.BatVOverFlag != OFF)
	{//������ѹ��ѹ
		SigFlag1 |=(1<<GROUPVOVERSIG);
	}
	else
	{
		SigFlag1 &= ~(1<<GROUPVOVERSIG);
	}
	if(SysSignal.BatVLowFlag != OFF)
	{//������ѹǷѹ
		SigFlag1 |=(1<<GROUPVLOWSIG);
	}
	else
	{
		SigFlag1 &= ~(1<<GROUPVLOWSIG);
	}
	if(SysSignal.TempOverFlag != OFF)
	{//����¶ȹ���
		SigFlag1 |=(1<<TOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<TOVER);
	}
	if(SysSignal.TempLowFlag != OFF)
	{//����¶ȹ���
		SigFlag1 |=(1<<TLOW);
	}
	else
	{
		SigFlag1 &= ~(1<<TLOW);
	}	
	if(SysSignal.InnerROverFlag != OFF)
	{//����������
		SigFlag1 |=(1<<INNERROVER);
	}
	else
	{
		SigFlag1 &= ~(1<<INNERROVER);
	}	
	if(SysSignal.CROverFlag != OFF)
	{//��������ֵ����
		SigFlag1 |=(1<<CROVER);
	}
	else
	{
		SigFlag1 &= ~(1<<CROVER);
	}
	if(SysSignal.IOverFlag != OFF)
	{//��س���������
		SigFlag1 |=(1<<IOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<IOVER);
	}	
	if(SysSignal.DisIOverFlag != OFF)
	{//��طŵ��������
		SigFlag1 |=(1<<DISIOVER);
	}
	else
	{
		SigFlag1 &= ~(1<<DISIOVER);
	}	
	//�豸ͨ���쳣ң��
	SigFlag3 &= 0x8000;
	SigFlag3 |= SysSignal.CommErrFlag;
	if(SysSignal.GroupSmpLineErrFlag != OFF)
	{//��������쳣
		SigFlag3 |=(1<<GROUP_SMP_LINE_ERR);
	}
	else
	{
		SigFlag3 &= ~(1<<GROUP_SMP_LINE_ERR);
	}	
	if(SysSignal.BatSmpLineErrFlag != OFF)
	{//��ز������쳣
		SigFlag4 |=(1<<0);
	}
	else
	{
		SigFlag4 &= ~(1<<0);
	}	
	if(SysSignal.TempErrFlag != OFF)
	{//�¶�̽ͷ�쳣
		SigFlag4 |=(1<<1);
	}
	else
	{
		SigFlag4 &= ~(1<<1);
	}	
	if(SysSignal.DevTempErrFlag != OFF)
	{//�����¶�̽ͷ�쳣
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


