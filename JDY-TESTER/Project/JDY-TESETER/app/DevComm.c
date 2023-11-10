/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѹ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppUART.c
** Created By: abe
** Last modified Date:  2016-03-16
** Last Version: 1.0
** Descriptions: �������߼��ϵͳ
**               ������ʾͨ������
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
* ������������Ϣ
* ������pMsg ��Ϣ����
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
**   ����: SOC����
**   ����: 
**   ���:  SOC����ֵ����λ�ٷֺ�
**   ����: 
****************************************************************************/
void SOCCount(uint8 batgroup,uint16 Vmin)
{
	float OldSocValue;
	uint32 DeltTime;
	float TempCur = 0;
	OldSocValue = MbsSlave.Group[batgroup].SysMeaData.Soc;
/**********************��·��ѹ�� + ��ʱ���ַ�����SOC******************************/
	if((g_TimerMs - BATComTime.SocCountTimeMs[batgroup])>1000)
	{
		if(MbsSlave.SysPara.MSysCfg.CurrentChannel[batgroup] != NO_SENSOR)//�е����ɼ�
		{//���
			DeltTime = g_TimerMs - BATComTime.SocCountTimeMs[batgroup];
			MbsSlave.Group[batgroup].SysMeaData.Soc = (MbsSlave.Group[batgroup].SysMeaData.Soh*MbsSlave.Group[batgroup].SysMeaData.Soc + LcdComm.Current[batgroup]*DeltTime/36000)/MbsSlave.Group[batgroup].SysMeaData.Soh;
			BATComTime.SocCountTimeMs[batgroup] = g_TimerMs;
		}
		else if(MbsSlave.SysPara.MSysCfg.CurrentChannel[batgroup] == NO_SENSOR)//�޵����ɼ�
		{
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 0)
			{// 2V���
				MbsSlave.Group[batgroup].SysMeaData.Soc = (float)Vmin*100/2250;
			}
			else if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 1)
			{// 6V���
				MbsSlave.Group[batgroup].SysMeaData.Soc = (float)Vmin*100/67500;
			}
			else if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].BatVoltageLevel == 2)
			{// 12V���
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
	{//������ڷŵ�״̬�������SOF������״̬,С��-3A��ʱ��ż���
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
	{//ƫ��5%����һ��
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
**   ����: ���������������Ƿ�����
**   ����: timecheckflag:�Ƿ���Ҫ����ʱ�������
**   ���:  ������������������TRUE,����������������FALSE��
**   ����: 
****************************************************************************/
uint8 CheckTestCondition(uint8 batgroup,uint8 timecheckflag)
{
	uint8 ret = TRUE;
	uint8 TempFlag = TEST_IDLE;
	if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].InnerRDevStat != ON)
	{//�������������
		TempFlag = TEST_NO_FUNCTION;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}

	if(LcdComm.BatSysStat.BatStat[batgroup] == BATDISCHARGE)
	{//�ŵ���
		TempFlag = TEST_DISCHARGE;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(CheckVlow(batgroup) == FALSE)
	{//���Ƿѹ
		TempFlag = TEST_V_LOW;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(ErrFlag.GroupVoltageErrFlag[batgroup] == GROUP_V_OVER)
	{//���ѹ��ѹ
		TempFlag = TEST_GROUP_V_OVER;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
	if(ErrFlag.GroupVoltageErrFlag[batgroup] == GROUP_V_LOW)
	{//���ѹǷѹ
		TempFlag = TEST_GROUP_V_LOW;
        LcdComm.InnerTest.TestConditionFlag[batgroup] = TempFlag;
		ret = FALSE;
		return ret;
	}
    if(((g_TimerMs - BATComTime.BatRLastTestTimeMs[batgroup])<BATComTime.BatRTestTime[batgroup])&&(timecheckflag==ON))
	{//����Ƶ��,//�������������ʱ����Ҫ���м��ʱ���жϣ�
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
**   ����: ���ģ������Ѳ��״̬�����Ƿ�����ģ�鶼�Ѿ��������
**   ����: 
**   ���:  ����ģ�鶼������ɲ����TRUE��
**   ����: ���ģ�����趼������ɣ���׼����ȡ��������
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
		{//û�в��Գɹ������أ��ȴ����Ի����ʧ�ܶ�ֹͣ����
			if(LcdComm.InnerTest.InnerRTestDevNO < LcdComm.DevNUM[groupnum])
			{//��һ��ģ����Գɹ����ж��Ƿ�����һ��ģ����Ҫ����������ԣ��������һ��ģ����Ҫ���ԣ�����Ҫ���һ��ʱ�������������
				if(g_TimerMs - BATComTime.InnerRTestCtrlTimeMs[groupnum]>(LcdComm.InnerTest.TestTimePeriod[groupnum]+12)*1000)
				{
					StartInnerRTest(groupnum,DevNO+1);
					ReadRTestAddrDATA(groupnum,DevNO+1);//��ȡ1023λ�õ����ݣ���ģ���Ƿ���յ�����ָ��
					LcdComm.LcdSysCtrl[groupnum][DevNO].LCD_StartInterMea = RESET;//�����һ��ģ��Ĳ���
					LcdComm.LcdSysCtrl[groupnum][LcdComm.InnerTest.InnerRTestDevNO].LCD_StartInterMea = RESET;//�������ã������ѯģ���Ƿ���յ�ָ��
					LcdComm.InnerTest.InnerRTestDevNO++;//��Ҫ������ģ���ż�1
					BATComTime.InnerRTestCtrlTimeMs[groupnum] = g_TimerMs;//��¼������ģ��Ĳ���ʱ�䣬�����Ƿ������ʱ
					BATComTime.TesttingStatusReadTimeMs[groupnum] = g_TimerMs;//�������Ժ󣬹�3S��ȥ��ȡ״̬
				}
				return FALSE;
			}
			if((devaddr == (DevNO))&&(MbsSlave.StatusData.MoudleStat[groupnum][DevNO].S_MeaInterRStat == R_TEST_SUCCESS))
			{//������һ��ģ�����������ԣ������������ɣ��ɹ�
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
//		{//����ʧ�ܣ��ٴ�����
//				StartInnerRTest(groupnum,DevNO);
//				ReadRTestAddrDATA(groupnum,DevNO);//��ȡ1023λ�õ����ݣ���ģ���Ƿ���յ�����ָ��
//				BATComTime.InnerRTestCtrlTimeMs[groupnum] = g_TimerMs;//��¼������ģ��Ĳ���ʱ�䣬�����Ƿ������ʱ
//				BATComTime.TesttingStatusReadTimeMs[groupnum] = g_TimerMs;//�������Ժ󣬹�3S��ȥ��ȡ״̬
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
**   ����:ģ��У׼������ȡ,�����ݳ�ʼ��ʱ����ȡ����ģ���У׼����
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void AllCalDataRead(void)
{


}
/***************************************************************************
**  void GroupDataRead(uint8 DEVCOM)
**   ����:��ȡ��ص�ѹֵ
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void GroupDataRead(uint8 DEVCOM,uint8 group)
{
	MSG_STRUCT msg;
	int32 tmptime;
	tmptime = (int32)g_TimerMs - BATComTime.GroupDataReadTimeMs[group];
	if(tmptime > BATComTime.GroupDataReadTime[group])		/****��ȡ��ص�ѹ*****/
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
**   ����:��ȡУ׼����
**   ����:  
**   ���:  
**   ����: 
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
**   ����:����У׼ϵ����Ѳ��ģ��
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = 1;//У׼ϵ��ֻ����Ѳ��ģ��1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = (RATIO_ADDR>>8) & 0xFF;
	msg.MsgData[2] = RATIO_ADDR & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.InnerRRatio,2);//ֻ��Ҫ����1���Ĵ���	
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
**   ����:����У׼ϵ����Ѳ��ģ��
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = 1;//У׼ϵ��ֻ����Ѳ��ģ��1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+1)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+1) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.CRRatio,2);//ֻ��Ҫ����1���Ĵ���	
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
**   ����:����У׼ϵ����Ѳ��ģ��
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = 1;//У׼ϵ��ֻ����Ѳ��ģ��1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+2)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+2) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.TempCliPara,2);//ֻ��Ҫ����1���Ĵ���	
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
**   ����:����У׼ϵ����Ѳ��ģ��
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = 1;//У׼ϵ��ֻ����Ѳ��ģ��1
	msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
	msg.MsgData[1] = ((RATIO_ADDR+3)>>8) & 0xFF;
	msg.MsgData[2] = (RATIO_ADDR+3) & 0xFF;
	msg.MsgData[3] = 1;
	memcpy(&msg.MsgData[4],&MbsSlave.SysPara.RatioInfo.VCliPara,2);//ֻ��Ҫ����1���Ĵ���	
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
**   ����:�����¶�̽ͷ�Զ���ID���豸
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void SendTempEXIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//�¶�̽ͷ��ÿ��ģ���ڲ��ı��룬��1��18��TempNO��ϵͳ�¶�̽ͷ���룬��1��240
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
	if(TempType == 0)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//�����¶�̽ͷ��ţ����͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+DevTempNO-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+DevTempNO-1) & 0xFF;
	}
	else if(TempType == 1)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+19-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+19-1) & 0xFF;
	}
	else if(TempType == 2)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+20-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+20-1) & 0xFF;
	}
	else if(TempType == 3)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+21-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+21-1) & 0xFF;
	}
	msg.MsgData[3] = 1;// 2���ֽڣ�1���Ĵ���
	memcpy(&msg.MsgData[4],(uint8 *)&MbsSlave.SysPara.TempInfo.WrExid,2);// 2���ֽڣ�1���Ĵ���
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
**   ����:�����¶�̽ͷID���豸
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void SendTempIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//�¶�̽ͷ��ÿ��ģ���ڲ��ı��룬��1��18��TempNO��ϵͳ�¶�̽ͷ���룬��1��240
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
	if(TempType == 0)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//�����¶�̽ͷ��ţ����͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_SET_ADDR+DevTempNO*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+DevTempNO*4-4) & 0xFF;
	}
	else if(TempType == 1)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_SET_ADDR+19*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+19*4-4) & 0xFF;
	}
	msg.MsgData[3] = 4;//10���ֽڣ�5���Ĵ���
	memcpy(&msg.MsgData[4],MbsSlave.SysPara.TempInfo.WrID,8);//10���ֽڣ�5���Ĵ���
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
**   ����:��ȡ����У׼ϵ��
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void ReadTempEXID(uint8 group,uint8 TempType,uint8 TempNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//�¶�̽ͷ��ÿ��ģ���ڲ��ı��룬��1��18��TempNO��ϵͳ�¶�̽ͷ���룬��1��240

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
	if(TempType == 0)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//�����¶�̽ͷ��ţ����͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+DevTempNO-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+DevTempNO-1) & 0xFF;
	}
	else if(TempType == 1)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+19-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+19-1) & 0xFF;
	}
	else if(TempType == 2)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+20-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+20-1) & 0xFF;
	}
	else if(TempType == 3)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_EXID_ADDR+21-1)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_EXID_ADDR+21-1) & 0xFF;
	}
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = 0;
	msg.MsgData[5] = 1;//��ȡ�¶�̽ͷ����ID���ܹ�2���ֽڣ�1���Ĵ���
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
**   ����:��ȡ����У׼ϵ��
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void ReadTempID(uint8 group,uint8 TempType,uint8 TempNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 DevTempNO;//�¶�̽ͷ��ÿ��ģ���ڲ��ı��룬��1��18��TempNO��ϵͳ�¶�̽ͷ���룬��1��240

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
	if(TempType == 0)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = (TempNO+MAX_BAT_NUM-1)/MAX_BAT_NUM;//�����¶�̽ͷ��ţ����͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_SET_ADDR+DevTempNO*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+DevTempNO*4-4) & 0xFF;
	}
	else if(TempType == 1)//���͵ĵ���¶�̽ͷID
	{
		msg.MsgDevAddr = TempNO;//���ݻ����¶�̽ͷ��ŷ��͸���Ӧ��ģ��
		msg.MsgData[1] = ((TEMP_SET_ADDR+19*4-4)>>8) & 0xFF;
		msg.MsgData[2] = (TEMP_SET_ADDR+19*4-4) & 0xFF;
	}
	msg.MsgData[0] = MODBUS_RD_HOLD_REG;
	msg.MsgData[3] = 1;
	msg.MsgData[4] = 0;
	msg.MsgData[5] = 4;//��ȡ�¶�̽ͷ����ID���ܹ�8���ֽڣ�4���Ĵ���
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
**   ����:��ȡ����У׼ϵ��
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = 1;//У׼ϵ��ֻ����Ѳ��ģ��1
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
**   ����:Ѳ��ģ��һ����ԭ
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡADֵ�������ѹ����ֵ
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡ��ص�ѹֵ����������
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡ��ص�ѹֵ��С������
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡ�������ֵ
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡ��������ֵ
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��ȡ����¶�
**   ����:  
**   ���:  
**   ����: 
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
**   ����:��̨������ȡ��ص�ѹ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ��ص�ѹ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ�¶�
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ�������
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ��������ֵ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ�������Ϣ�����ѹ����ŵ������
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��ȡϵͳ״̬��Ϣ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ���ò�����Ϣ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����:��̨������ȡ���ò�����Ϣ
**   ����:  
**   ���:  
**   ����: 
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
	msg.MsgDevAddr = HostAddr;//��̨����Ҫ��ӻ���ַ����һ��
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
**   ����: �������ò������ɼ���--����������
**   ����:   
**   ���:  
**   ����: 
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
		msg.MsgData[1] = (uint8)((S_GROUP_PARA_ADDR + 2) >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
		msg.MsgData[2] = (uint8)((S_GROUP_PARA_ADDR + 2)& 0xFF);
		msg.MsgData[3] = 1;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP1].SysPara.CurrentDiverterPara,2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM2;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
		msg.MsgData[1] = (uint8)((S_GROUP_PARA_ADDR + 2) >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
		msg.MsgData[2] = (uint8)((S_GROUP_PARA_ADDR + 2)& 0xFF);
		msg.MsgData[3] = 1;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP2].SysPara.CurrentDiverterPara,2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	return TRUE;
}
/*************************************************************************************
**  uint8 SendCliParaToS(uint8 group) 
**   ����: ����У׼�������ɼ���--����������
**   ����:   
**   ���:  
**   ����: 
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
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR& 0xFF);
		msg.MsgData[3] = S_GROUP_CAL_NUM;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP1].MeaCal.KU,S_GROUP_CAL_NUM*2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(group == GROUP2)
	{
		msg.MsgType = MSG_WR_REG;
		msg.MsgLenth = 1;
		msg.MsgCOM = BATCOM2;
		msg.MsgDevAddr = MbsSlave.Group[group].SysPara.DevAddr & 0xFF;
		msg.MsgData[0] = MODBUS_WR_MULT_REG;
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
		msg.MsgData[2] = (uint8)(S_GROUP_CAL_ADDR & 0xFF);
		msg.MsgData[3] = S_GROUP_CAL_NUM;
		memcpy(&msg.MsgData[4],&MbsSlave.Group[GROUP2].MeaCal.KU,S_GROUP_CAL_NUM*2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}
	return TRUE;
}
/*************************************************************************************
**  uint8 ReadCliParaToS(uint8 group) 
**   ����: ����У׼�������ɼ���--����������
**   ����:   
**   ���:  
**   ����: 
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
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
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
		msg.MsgData[1] = (uint8)(S_GROUP_CAL_ADDR >> 8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
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
**   ����: �������ò�����ģ��
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
/***********************************************************/
uint8 SendCfgParaToDev(uint16 BATGROUP,uint8 DEVNO) 
{ 
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case GROUP1://������1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://������2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 6)>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
	msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 6)& 0xFF);
	msg.MsgData[3] = 13;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_SingleVoltageMax,13*2);//ֻ��Ҫ����13���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_SingleVoltageMax,13*2);//ֻ��Ҫ����13���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}

	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
	msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 1)& 0xFF);
	msg.MsgData[3] = 1;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum,2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP2].S_CellNum,2);//ֻ��Ҫ����1���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}

	return 1;
}
/*************************************************************************************
**  uint8 SendCfgParaToAllDev(void) 
**   ����: �������ò���������ģ��
**   ����:   
**   ���:  
**   ����: 
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
			if((j == GROUP1)&&(DevTask.TaskBat1!=0))//���͵���Ӧ�Ĵ���
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM1;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
				msg.MsgData[2] = (uint8)((S_MSYS_CFG_ADDR + 1)& 0xFF);
				msg.MsgData[3] = 18;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.MSysCfg.GroupCfg[GROUP1].S_CellNum,18*2);//ֻ��Ҫ����13���Ĵ���
				switch(DevTask.TaskBat1)
				{//��ʹ�õ��������ͨ��Э��ʱ��������������Ϣ
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
				msg.MsgData[1] = (uint8)((S_MSYS_CFG_ADDR + 1)>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
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
**   ����: ����������Ϣ���趨ģ��,������ģ���������Ϣ�����͹�ȥ
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
/***********************************************************/
uint8 SendShieldInfoToDev(uint16 BATGROUP,uint8 DEVNO) 
{ 
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case GROUP1://������1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://������2
			DEVCOM = BATCOM2;
			break;
	}
	msg.MsgType = MSG_WR_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;
	msg.MsgData[0] = MODBUS_WR_MULT_REG;
	msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
	msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR & 0xFF);
	msg.MsgData[3] = 33;
	if(DEVCOM == BATCOM1)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//��Ҫ����33���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat1);
	}
	else if(DEVCOM == BATCOM2)
	{
		memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP2].BatVShield[0],33*2);//��Ҫ����33���Ĵ���	
		SendMsg2Queue(&msg,DevTask.TaskBat2);
	}

	return 1;
}
/*************************************************************************************
**  uint8 SendShieldParaToAllDev(void) 
**   ����: ����������Ϣ������ģ��
**   ����:   
**   ���:  
**   ����: 
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
			if((j == GROUP1)&&(DevTask.TaskBat1!=0))//���͵���Ӧ�Ĵ���
			{
				msg.MsgType = MSG_WR_REG;
				msg.MsgLenth = 1;
				msg.MsgCOM = BATCOM1;
				msg.MsgDevAddr = i;
				msg.MsgData[0] = MODBUS_WR_MULT_REG;
				msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
				msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR& 0xFF);
				msg.MsgData[3] = 33;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//��Ҫ����33���Ĵ���
				switch(DevTask.TaskBat1)
				{//��ʹ�õ��������ͨ��Э��ʱ��������������Ϣ
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
				msg.MsgData[1] = (uint8)(S_SHIELD_INFO_ADDR>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
				msg.MsgData[2] = (uint8)(S_SHIELD_INFO_ADDR& 0xFF);
				msg.MsgData[3] = 33;
				memcpy(&msg.MsgData[4],&MbsSlave.SysPara.ShieldInfo[GROUP1].BatVShield[0],33*2);//��Ҫ����33���Ĵ���
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
**   ����: �����������
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void StartInnerRTest(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case 0://������1
			DEVCOM = BATCOM1;
			break;
		case 1://������2
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
**   ����: ��ͣ�������
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void PauseInnerRTest(uint8 batgroup,uint8 DevNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(batgroup)
	{
		case GROUP1://������1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://������2
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
**   ����: ֹͣ�������
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void StopInnerRTest(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case GROUP1://������1
			DEVCOM = BATCOM1;
			break;
		case GROUP2://������2
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
**   ����: ��ȡ�������λ�����ݣ��ж��Ƿ�д��ɹ���0Xaa��ʾ��������Ѳ�죬0X55��ʾֹͣ����Ѳ��
**   ����:  BATGROUP��������ţ�DEVNO��ʾģ����
**   ���:  
**   ����:  
****************************************************************************/
void ReadRTestAddrDATA(uint16 BATGROUP,uint8 DEVNO)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	switch(BATGROUP)
	{
		case 0://������1
			DEVCOM = BATCOM1;
			break;
		case 1://������2
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
	msg.MsgData[4] = 0;//ֻ��ȡR��ʶΪһ���Ĵ���
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
**   ����: ������ؾ���
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void StartBatEQ(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case 0://������1
			DEVCOM = BATCOM1;
			break;
		case 1://������2
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
**   ����: ֹͣ��ؾ���
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void StopBatEQ(uint16 BATGROUP)
{
	MSG_STRUCT msg;
	uint8 DEVCOM;
	uint8 i;
	switch(BATGROUP)
	{
		case 0://������1
			DEVCOM = BATCOM1;
			break;
		case 1://������2
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
**   ����: ���������Ƿ���б�������
**   ����:  �����ţ������Ŀ��־���Ƿ�д�����磬(���������������ж����賬���Ƿ񱨾�)
**   ���:  ���TRUE��ʾϵͳ���������������죬���FALSE��ʾϵͳ����������������
**   ����: 
****************************************************************************/
uint8 CheckBeepCondition(uint8 batgroup,uint8 ErrCheckFlag,uint8 IsWrite)
{
	uint8 i;
	uint16 j;
	uint8 temp = OFF;
	uint8 tmp1 = OFF;//����Ƿ�����ֵ���ޱ�־
	static uint8 tmpflag = OFF;//�Ƿ����������־
	//tmp1 = on,tmpflag = on:��ʾ����״̬�������賬����Ϣ����Ҫ������������������
	//tmp1 = on��tmpflag = off:��ʾ����״̬�������賬����Ϣ������Ҫ���������������
	//tmp1 = OFF��tmpflag = off:��ʾ����״̬��û�����賬����Ϣ������Ҫ������ϵͳ����
	
	for(i=0;i<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;i++)//�������
	{
		for(j=0;j<MbsSlave.StatusData.BatErrStat[i].S_Quantity;j++)//��������
		{
			switch(MbsSlave.StatusData.BatErrStat[i].S_ErrInfo[j].S_ErrType)
			{
				case VOLTAGE_OVER://��ѹ
				case VOLTAGE_LOW://Ƿѹ
				case TEMP_OVER://����
				case TEMP_LOW://Ƿ��
				case GROUP_V_OVER://���ѹ����
				case GROUP_V_LOW://���ѹ����
				case CURRENT_DISCHARGE_OVER://�ŵ��������
				case CURRENT_CHARGE_OVER://����������
				case ENVIRMENT_TEMP_OVER://�����¶ȹ��� 
				case ENVIRMENT_TEMP_LOW://�����¶ȹ���
				case DEV_TEMP_ERR://�¶�̽ͷ����
				case ENVIR_TEMP_ERR://�����¶ȹ���
				case DEV_COMM_ERR://ͨ���쳣
				case BAT5000_S_COM_ERR://�ɼ���ͨ���쳣
				case BKG_COM_ERR://Ѳ����ͨ���쳣
				case DEV_INNER_R_ERR://����ģ�����
				case DEV_CR_ERR://������ģ�����
				case DEV_EQ_ERR://����ģ�����
				case BAT_SAMPLE_LINE_DROP://����������
				case GROUP_SAMPLE_LINE_DROP://��������������
				case BAT_SAMPLE_LINE_RESERVE://����
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
				case DEV_TEST_FAIL1://�������ʧ��
				case DEV_TEST_FAIL2:
				case DEV_TEST_FAIL3:
				case DEV_TEST_FAIL4:
				case DEV_TEST_FAIL5:
				case DEV_TEST_FAIL6:
				case DEV_TEST_FAIL7:
				case DEV_TEST_FAIL8:
				case DEV_TEST_FAIL9:
				case DEV_TEST_FAIL10:
				case DEV_TEST_FAIL://�������ʧ��
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
//			{//����б������ͷ��أ����ڼ��
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
		tmpflag = OFF;//��������״̬��û�����賬����Ϣ
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
**   ����: ���ϵͳ���ϼ��
**   ����:  �����ţ�ģ���ţ��������ͼ�⣬�Ƿ�д����ʷ��¼
**   ���:  
**   ����: 
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
	if(DevNO+1 == LcdComm.DevNUM[BatGroup])//�������Ҫ����ĵ���������������һ��ģ��ʱ������������ܲ���18��
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
				{//����Ƿ������εĵ�أ����εĵ�ز��ٽ��е�ѹ����
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
				{//С��800mv������
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_SingleVoltageMax)
					{//��ѹ����
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != VOLTAGE_OVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,VOLTAGE_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = VOLTAGE_OVER;
					}
					else if(MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i] < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_SingleVoltageMin)
					{//��ѹ����
						if(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] != VOLTAGE_LOW)
						{
							HistoryDataSave(BatGroup,DevNO,i,VOLTAGE_LOW,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_BatV[DevNO][i]);
						}
						ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] = VOLTAGE_LOW;
					}
					else if((ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == VOLTAGE_OVER)||(ErrFlag.VoltageErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == VOLTAGE_LOW))
					{//��ѹ�ָ�
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
				{//����������
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
				{//�¶�̽ͷ���ֹ��ϣ���ֵΪ0xFAFA
					if(MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMax)
					{//����¶ȹ���
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_OVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_OVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_OVER;
					}
					else if(MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i] < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMin)
					{//����¶ȹ���
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_LOW)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_LOW,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_LOW;
					}
					else if((ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == TEMP_OVER)||(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == TEMP_LOW))
					{//����¶Ȼָ�
						if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] != TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,DevNO,i,TEMP_RECOVER,(int16)MbsSlave.BatData[BatGroup].DevMeaData.S_Temp[DevNO][i]);
						}
						ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_TEMP_DATA_NUM+i] = TEMP_RECOVER;
					}
					else if(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == DEV_TEMP_ERR)
					{//����¸й��ϻָ�
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
				{//����¸й���
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
				{//���賬��
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
				{//��������ֵ����
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
		case GPOUP_VOLTAGE_CHECK://������ܵ�ѹ���
			if(MbsSlave.Group[BatGroup].MeaData.VoltageValue > 1)
			{
				if(MbsSlave.Group[BatGroup].MeaData.VoltageValue > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_GroupVoltageMax)
				{//���ѹ����
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_OVER)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_OVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_OVER;
				}
				else if(MbsSlave.Group[BatGroup].MeaData.VoltageValue < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_GroupVoltageMin)
				{//���ѹ����
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_LOW)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_LOW,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_LOW;
				}
				else if((ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_V_OVER)||(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_V_LOW))
				{//���ѹ�ָ�
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_V_RECOVER)
					{
						HistoryDataSave(BatGroup,DevNO,i,GROUP_V_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_V_RECOVER;
				}
				else if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_DROP)
				{
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_DROP_RECOVER)
					{//��������������ָ�
						HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_DROP_RECOVER,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
					}
					ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_DROP_RECOVER;
				}
				else if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_RESERVE)
				{
					if(ErrFlag.GroupVoltageErrFlag[BatGroup] == GROUP_SAMPLE_LINE_RESERVE_RECOVER)
					{//�����߷��ӻָ�
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
				{//��������������
					HistoryDataSave(BatGroup,DevNO,i,GROUP_SAMPLE_LINE_DROP,(int16)MbsSlave.Group[BatGroup].MeaData.VoltageValue);
				}
				ErrFlag.GroupVoltageErrFlag[BatGroup] = GROUP_SAMPLE_LINE_DROP;
			}
			else if(MbsSlave.Group[BatGroup].MeaData.VoltageValue < -1)
			{
				if(ErrFlag.GroupVoltageErrFlag[BatGroup] != GROUP_SAMPLE_LINE_RESERVE)
				{//���������߷���
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
					{//�ܵ�ѹ=0���������ص�ѹ������0�����ǵ������ֶ�·�����ϵͳ��·
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
		case CURRENT_CHECK://������ŵ�������
			if(LcdComm.Current[BatGroup] > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_ChargeCurrentMax)
			{//����������
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_CHARGE_OVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_CHARGE_OVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_CHARGE_OVER;
			}
			else if(LcdComm.Current[BatGroup] < (MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_DischargeCurrentMax * (-1)))
			{//�ŵ��������
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_DISCHARGE_OVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_DISCHARGE_OVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_DISCHARGE_OVER;
			}
			else if(ErrFlag.CurrentErrFlag[BatGroup] == CURRENT_CHARGE_OVER)
			{//�������ָ�
				if(ErrFlag.CurrentErrFlag[BatGroup] != CURRENT_CHARGE_RECOVER)
				{
					HistoryDataSave(BatGroup,DevNO,i,CURRENT_CHARGE_RECOVER,(int16)LcdComm.Current[BatGroup]);
				}
				ErrFlag.CurrentErrFlag[BatGroup] = CURRENT_CHARGE_RECOVER;
			}
			else if(ErrFlag.CurrentErrFlag[BatGroup] == CURRENT_DISCHARGE_OVER)
			{//�ŵ�����ָ�
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
		case ENVIRONMENT_TEMP_CHECK://��ػ����¶ȼ��
			for(i=0;i<LcdComm.DevNUM[BatGroup];i++)
			{
				if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp != 0xFAFA)
				{
					if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp > MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMax)
					{//�¶ȹ���
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_OVER)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_OVER,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_OVER;
					}
					else if(MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp < MbsSlave.SysPara.MSysCfg.GroupCfg[BatGroup].S_TempMin)
					{//�¶ȹ���
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_LOW)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_LOW,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_LOW;
					}
					else if((ErrFlag.EnvironmentTempErrFlag[BatGroup][i] == ENVIRMENT_TEMP_OVER)||(ErrFlag.TempErrFlag[BatGroup][DevNO*BAT_V_DATA_NUM+i] == ENVIRMENT_TEMP_LOW))
					{//�¶Ȼָ�
						if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] != ENVIRMENT_TEMP_RECOVER)
						{
							HistoryDataSave(BatGroup,0,DevNO,ENVIRMENT_TEMP_RECOVER,(int16)MbsSlave.StatusData.MoudleStat[BatGroup][i].S_Temp);
						}
						ErrFlag.EnvironmentTempErrFlag[BatGroup][i] = ENVIRMENT_TEMP_RECOVER;
					}
					else if(ErrFlag.EnvironmentTempErrFlag[BatGroup][i] == ENVIR_TEMP_ERR)
					{//�����¸й��ϻָ�
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
				{//�����¸��쳣
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
			{//ģ��ͨ�Ź���
				HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_COMM_ERR,0);
			}
			else if(ErrFlag.DevCommErrFlag[BatGroup][DevNO] == DEV_COMM_RECOVER)
			{//ģ��ͨ�Ź��ϻָ�
				HistoryDataSave(BatGroup,0,(BatGroup<<4)+DevNO,DEV_COMM_RECOVER,0);
			}
			break;
		case BAT5000_S_COMM_CHECK:
			if(ErrFlag.BAT5000SCommErrFlag[BatGroup] == BAT5000_S_COM_ERR)
			{//�ɼ���ͨ�Ź���
				HistoryDataSave(BatGroup,0,BatGroup<<4,BAT5000_S_COM_ERR,0);
			}
			else if(ErrFlag.BAT5000SCommErrFlag[BatGroup] == BAT5000_S_COM_RECOVER)
			{//�ɼ���ͨ�Ź��ϻָ�
				HistoryDataSave(BatGroup,0,BatGroup<<4,BAT5000_S_COM_RECOVER,0);
			}			
			break;
		case INNER_R_DEV_STAT_CHECK://�������ģ��״̬
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
		case CR_DEV_STAT_CHECK://���������ģ��״̬
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
		case EQ_DEV_STAT_CHECK://������ģ��״̬
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
			{//Ѳ����������̨ͨ�Ź���
				HistoryDataSave(0,0,0,BKG_COM_ERR,0);
			}
			else if(ErrFlag.BkgCommErrFlag == BKG_COM_RECOVER)
			{//Ѳ����������̨ͨ�Ź��ϻָ�
				HistoryDataSave(0,0,0,BKG_COM_RECOVER,0);
			}
			break;
		case FDY_COMM_CHECK:
			if(ErrFlag.FdyCommErrFlag == FDY_COM_ERR)
			{//�ŵ���ͨ�Ź���
				HistoryDataSave(0,0,0,FDY_COM_ERR,0);
			}
			else if(ErrFlag.FdyCommErrFlag == FDY_COM_RECOVER)
			{//�ŵ���ͨ�Ź��ϻָ�
				HistoryDataSave(0,0,0,FDY_COM_RECOVER,0);
			}
			break;
		default:
			break;
	}
	/*****************ͳ��ʵʱ��������****************/
	for(j=0;j<MbsSlave.SysPara.MSysCfg.S_CellGroupNum;j++)//�������
	{
		OS_ENTER_CRITICAL();//��ֹ�����л�����ֹ�������ݻ�û��ͳ����ɣ��ͱ������������
		MbsSlave.StatusData.BatErrStat[j].S_Quantity = 0;
		if(LcdComm.InnerTest.TestFailFlag[j]!=OFF)//����ģ��״̬
		{//�������ʧ��
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
			if(ErrFlag.DevCommErrFlag[j][i] == DEV_COMM_ERR)//ģ��ͨ�ż��
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
			if((ErrFlag.InnerRDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//����ģ��״̬
			{//����ģ�����
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
			if((ErrFlag.CRDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//������ģ��״̬
			{//������ģ�����
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
			if((ErrFlag.EQDevStatFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//����ģ��״̬
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
		if(ErrFlag.BAT5000SCommErrFlag[j] == BAT5000_S_COM_ERR)//�ɼ���ͨ�ż��
		{//ͳ�Ʋɼ���ͨ�Ź���
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
		if(ErrFlag.BkgCommErrFlag == BKG_COM_ERR)//Ѳ����������̨ͨ�ż��
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
		if(ErrFlag.FdyCommErrFlag == FDY_COM_ERR)//�ŵ���ͨ�ż��
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
		if(ErrFlag.CircuitErrFlag[j] != NORMAL)//�Ƿ��п�·���
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
		if((ErrFlag.GroupVoltageErrFlag[j] != NORMAL)&&(ErrFlag.BAT5000SCommErrFlag[j] != BAT5000_S_COM_ERR))//�ܵ�ѹ
		{//ͳ�Ƶ�ѹ����
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
		if((ErrFlag.CurrentErrFlag[j] != NORMAL)&&(ErrFlag.BAT5000SCommErrFlag[j] != BAT5000_S_COM_ERR))//����
		{//ͳ�Ƶ�������
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
		{//ͳ�ƻ����¶ȹ���
			if((ErrFlag.EnvironmentTempErrFlag[j][i] != NORMAL)&&(ErrFlag.DevCommErrFlag[j][i] != DEV_COMM_ERR))//�����¶�
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
		for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[j].S_CellNum;i++)//��ؽ���
		{//ͳ�Ƶ�ص�ѹ���¶ȹ���
			TempDevNO = i/BAT_V_DATA_NUM;
			if(ErrFlag.DevCommErrFlag[j][TempDevNO] != DEV_COMM_ERR)
			{
				if(ErrFlag.VoltageErrFlag[j][i] != NORMAL)//���ڵ�ص�ѹ
				{//ģ��ͨ���������й��ϼ������״̬
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
				if((ErrFlag.TempErrFlag[j][i] != NORMAL)&&(MbsSlave.SysPara.MSysCfg.GroupCfg[j].TempDevStat != OFF))//���ڵ���¶�
				{//�¶ȹ��ܿ�����ģ��ͨ���������й��ϼ������״̬
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
		for(i=0;i<MbsSlave.SysPara.MSysCfg.GroupCfg[j].S_CellNum;i++)//��ؽ���
		{//ͳ�Ƶ�����������������޹���
			TempDevNO = i/BAT_V_DATA_NUM;
			if(ErrFlag.DevCommErrFlag[j][TempDevNO] != DEV_COMM_ERR)
			{
				if((ErrFlag.InnerRErrFlag[j][i] != NORMAL)&&(MbsSlave.SysPara.MSysCfg.GroupCfg[j].InnerRDevStat != OFF)&&(LcdComm.InnerTest.TestFailFlag[j]==OFF))//�������
				{//���蹦�ܿ�����ģ��ͨ���������й��ϼ������״̬
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
				{//���������ܿ�����ģ��ͨ���������й��ϼ������״̬
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
**   ����: ��ȡģ��״̬
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
uint8 SysStatusRead(uint8 DEVCOM,uint8 DEVNO)
{
	uint8 ret = FALSE;
	
	MSG_STRUCT msg;
	msg.MsgType = MSG_RD_REG;
	msg.MsgLenth = 1;
	msg.MsgCOM = DEVCOM;
	msg.MsgDevAddr = DEVNO + 1;//ģ���Ŵ�1��ʼ
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
**   ����: �������ʧ��
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
void RTestFail(uint8 batgroup,uint8 devno)
{
	uint8 TempNO;
	LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_FAIL;//����ʧ��
	MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = STOP;				
	StopInnerRTest(batgroup);
	TempNO = (batgroup<<4)+LcdComm.InnerTest.InnerRTestDevNO-1;//����λ��ʾ�����ţ�����λ��ʾģ���
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
**   ����: ����Ƿ���Ƿѹ
**   ����:  
**   ���:  
**   ����:  
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
**   ����: ��ΪMODBUS����ʱ�Ĵ��� 
**   ����:  
**   ���:  
**   ����:  
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

/*************************ģ�鹤��״̬��ȡ***************************************/		
	if((LcdComm.InnerTest.InnerRTestFlag[batgroup] != R_TEST_ING)&&(LcdComm.InnerTest.InnerRTestFlag[batgroup] != R_TEST_PAUSE))
	{//����������У���ȡ״̬����ѹ���¶ȣ��ɼ�����Ϣ
		if((g_TimerMs - BATComTime.SysStatusTimeMs[batgroup]) > BATComTime.DevStatusReadTime[batgroup])
		{
			LcdComm.RdDevFlag[batgroup]=OFF;//ͨ�ų�ʱ�����ȡ��һ��ģ�������
		}
		if(LcdComm.RdDevFlag[batgroup]==OFF)
		{
			if((g_TimerMs - BATComTime.SysStatusTimeMs[batgroup])>(BATComTime.DevStatusReadTime[batgroup]/2))
			{//����ģ������̫�٣����¶�ȡ����Ƶ��
				SysStatusRead(DEVCOM,LcdComm.RdDevNO[batgroup]);			/*ģ�鹤��״̬��ȡ*/
				BATComTime.SysStatusTimeMs[batgroup] = g_TimerMs;
				LcdComm.RdDevFlag[batgroup] = ON;//��ȡ������
				LcdComm.RdDevNO[batgroup]++;
			}
			if(LcdComm.RdDevNO[batgroup] >= LcdComm.DevNUM[batgroup])
			{
				LcdComm.RdDevNO[batgroup]=0;
				GroupDataRead(DEVCOM,batgroup);//Ѳ��ģ���ȡ���֮���ٶ�ȡ�ɼ�������
			}
		}
/************************************SOC����*****************************/
		SOCCount(batgroup,LcdComm.BatSysStat.SingleBatMinV[batgroup][0]);

/************************************����ʵʱ��ѹ����******************/
		autoSaveTime = BATComTime.BatDataSaveTime[batgroup]*60*1000;//��̬���ݱ��棬���趨ʱ�䱣��
		if((g_TimerMs - BATComTime.BatDataSaveTimeMs[batgroup]) > autoSaveTime)//�洢��ص�ѹ�¶�����
		{
			BATComTime.BatDataSaveTimeMs[batgroup] = g_TimerMs;
			BatDataSave(batgroup);
		}		
	}//��������в����ȡģ��״̬
/********************************�������õ��Զ��������ʱ�䣬�Զ������������*************************************/	
//�Զ����������������:1ʱ�����ﵽ��2����ŵ㣬3����״̬��4�����δǷѹ��δ��ѹ��5���δǷѹ��6����������ܿ�����7����̫Ƶ��
	tempautotesttime = (uint32_t)MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].S_AutoMeaRTime*3600*1000*24;//��λΪ��,�Զ�����ʱ��Ϊ0��ʾ����������Զ�����
	if(((g_TimerMs- BATComTime.BatRLastTestTimeMs[batgroup])> tempautotesttime)&&(tempautotesttime != 0))
	{//�����Զ�����ʱ����
		ttm = Time_GetCalendarTime();
		if((LcdComm.BatSysStat.BatStat[batgroup] == BATFLOATCHARGE)&&(ttm.tm_hour == AUTO_TEST_TIME))//�Զ���������ʱ��ֻ���ڸ���״̬��
		{//����״̬�£�������������Զ��������
			if(CheckTestCondition(batgroup,ON) == TRUE)
			{
				MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = START;//��ʶΪ����״̬
				LcdComm.InnerTest.InnerRTestDevNO = 0;//��ʼ��Ϊ0���ӵ�ַΪ0��ģ�鿪ʼ����Ѳ��
				BatTestVTSave(batgroup);
				StartInnerRTest(batgroup,LcdComm.InnerTest.InnerRTestDevNO);//������һ��ģ�����������
				ReadRTestAddrDATA(batgroup,LcdComm.InnerTest.InnerRTestDevNO);//��ѯд������ݣ��Ƿ�д��ɹ�
				LcdComm.LcdSysCtrl[batgroup][LcdComm.InnerTest.InnerRTestDevNO].LCD_StartInterMea = RESET;//�������ã������ѯģ���Ƿ���յ�ָ��
				BATComTime.InnerRTestCtrlTimeMs[batgroup] = g_TimerMs;//��¼����ʱ��
				BATComTime.BatRLastTestTimeMs[batgroup] = g_TimerMs;//��¼Ϊ���һ�ε��������ʱ��
				BATComTime.BatRTestTime[batgroup] = BAT_R_TEST_TIME;//�տ���ʱ�������������ԣ����Ժ������ٴβ���ʱ����
				LcdComm.InnerTest.InnerRTestDevNO = 1;//��һ����Ҫ���в��Ե�ģ����
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_ING;
				LcdComm.itfNo = PICTURE132;//����������Խ���
				TempNO = (batgroup<<4);//����λ��ʾ�����ţ�
				HistoryDataSave(batgroup,0,TempNO,AUTO_TEST,0);//�洢������¼
			}
		}
	}
/**************************�����������******************************************/
/**************************��������У���ȡģ��״̬******************************************/
		if(LcdComm.InnerTest.InnerRTestFlag[batgroup] == R_TEST_ING)//���һ���ص��������״̬
		{//���������У����ϵĽ���Ѳ��״̬��ѯ
			tempdevno = LcdComm.InnerTest.InnerRTestDevNO;//����Ҫ���浽����Ļ��������ֹ��Ϊ������ȸı��ֵ
			if((g_TimerMs - BATComTime.InnerRTestCtrlTimeMs[batgroup]) > INNER_R_TEST_OVER_TIME)//������Գ�ʱ��ֹͣ���ԣ�����ʧ��
			{//������Գ�ʱ��ֹͣ���ԣ�����ʧ�ܣ��ָ���������״̬
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = R_TEST_FAIL;//���Գ�ʱ����
				MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea = STOP;				
				StopInnerRTest(batgroup);
				TempNO = (batgroup<<4)+LcdComm.InnerTest.InnerRTestDevNO-1;//����λ��ʾ�����ţ�����λ��ʾģ���
				HistoryDataSave(batgroup,0,TempNO,DEV_TEST_FAIL10,0);
				for(i=0;i<LcdComm.DevNUM[batgroup];i++)
				{//���
					LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
				}
			}
			else if((((MbsSlave.StatusData.MoudleStat[batgroup][tempdevno-1].S_MeaInterRStat & 0xF0) >> 4) == R_TEST_FAIL)&&(LcdComm.LcdSysCtrl[batgroup][tempdevno-1].LCD_StartInterMea == START)&&(MbsSlave.StatusData.MoudleStat[batgroup][tempdevno-1].S_MeaInterRStat != 0x23))
			{//�յ�������������������ʧ�ܣ���ֹͣ����Ѳ�죬�ָ���������״̬��
				RTestFail(batgroup,tempdevno-1);
				for(i=0;i<LcdComm.DevNUM[batgroup];i++)
				{//���
					LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
				}
			}
			else if(tempdevno > 0)
			{
				if(((g_TimerMs - BATComTime.TesttingStatusReadTimeMs[batgroup])> 3000)&&(LcdComm.LcdSysCtrl[batgroup][tempdevno-1].LCD_StartInterMea == START))
				{//����3�룬����ȷ���յ����������ȡ����״̬
					SysStatusRead(DEVCOM,(tempdevno-1));//��ȡ���ڽ����������ģ���״̬
					BATComTime.TesttingStatusReadTimeMs[batgroup] = g_TimerMs;
				}		
			}
		}
/**************************���������ɣ���ȡģ���������******************************************/
		if((LcdComm.InnerTest.InnerRTestFlag[batgroup] == R_TEST_SUCCESS)&&(MbsSlave.SysPara.SysCtrl[batgroup].S_StartInterMea != STOP))//��ȡ��ص������������
		{//������������ȡ�������ݣ����洢
			LcdComm.InnerTest.TestFailFlag[batgroup]=OFF;
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;//�����ɹ������ݱ�����
			for(i=0;i<LcdComm.DevNUM[batgroup];i++)
			{
				BATComTime.BatDataRReadTimeMs[batgroup] = g_TimerMs;//��¼�������ݶ�ȡʱ��
				memset(LcdComm.InnerTest.RDataReadFlag[batgroup],0,MAX_DEV_NUM);
				BatRRead(batgroup,i);//��ȡ�����������
				if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].CRDevStat == ON)//��������ģ�飬���ȡ��������������
				{
					memset(LcdComm.InnerTest.CRDataReadFlag[batgroup],0,MAX_DEV_NUM);
					BatCRRead(batgroup,i);//��ȡ��������������
				}
			}
			for(i=0;i<LcdComm.DevNUM[batgroup];i++)
			{//���
				LcdComm.LcdSysCtrl[batgroup][i].LCD_StartInterMea = RESET;
			}
		}
/**************************�������ݶ�ȡ��ɣ���������******************************************/
	if(LcdComm.InnerTest.InnerRTestFlag[batgroup] == DATA_SAVING)
	{
		for(i=0;i<LcdComm.DevNUM[batgroup];i++)
		{
			if(LcdComm.InnerTest.RDataReadFlag[batgroup][i] == OFF)
			{
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;
				TempStat = DATA_SAVING;//������
				break;
			}
			else
			{
				TempStat = DATA_SAVE_SUCCESS;//����ɹ�
			}
			if(MbsSlave.SysPara.MSysCfg.GroupCfg[batgroup].CRDevStat == ON)
			{
				if(LcdComm.InnerTest.CRDataReadFlag[batgroup][i] == OFF)
				{
					LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;
					TempStat = DATA_SAVING;//������
					break;
				}
				else
				{
					TempStat = DATA_SAVE_SUCCESS;//����ɹ�
				}
			}
		}
		if(TempStat == DATA_SAVE_SUCCESS)
		{
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVE_SUCCESS;//����ɹ�
			CountBatRStat(batgroup);
			CountBatCRStat(batgroup);
			BatTestDataSave(batgroup);
		}
		else
		{
			LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVING;//������
			if((g_TimerMs - BATComTime.BatDataRReadTimeMs[batgroup]) > BATComTime.BatDataRReadTime[batgroup]*3)
			{//���ݶ�ȡ��ʱ�������ݱ���ʧ��
				LcdComm.InnerTest.InnerRTestFlag[batgroup] = DATA_SAVE_FAIL;//û�а����ݶ�����������ʧ��
			}
		}
	}
/**************************�����������******************************************/
}
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

