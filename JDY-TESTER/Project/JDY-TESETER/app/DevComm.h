/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: AppUART.h
** Created By: xu
** Last modified Date:  2012-05-29
** Last Version: 1.0
** Descriptions:  
****************************************************************************************/
#ifndef __DEVCOMM_H 
#define __DEVCOMM_H

#include "Def.h"
#include "stdio.h"
#include "config.h"



#define BAT_DATA_R_RD_TIME 			10000	//�����ȡ��ʱ�ط�ʱ��
#define BAT_DATA_T_RD_TIME 			6000	//�¶����ݶ�ȡ��6S��һ��
#define SYS_STATUS_TIME 			6000	//ϵͳ״̬��ȡ��6S��һ��
#define INNER_R_TEST_OVER_TIME 		2000000	//�����������900S������Ϊ��ʱ
#define BAT_R_TEST_TIME 			600000	//10������ֻ�������һ��
#define GROUP_DATA_RD_TIME			6000	//�������ݶ�ȡʱ��





/*�������ʱ��ر���ע��
InnerRTestDevNO:���ڲ��Ե�ģ���ţ���һ��ģ�����ʱ��ֵΪ1��
S_StartInterMea:����ֹͣ��ʶ��
InnerRTestFlag:�������״̬��
LCD_StartInterMea:ÿ��ģ�����״̬��������ѯ�Ƿ��յ��������
InnerRTestCtrlTimeMs:��¼����ʱ�䣻
BatRLastTestTimeMs:���һ�ε��������ʱ�䣻
*/









/*������ʾͨѶ�����ⲿ����*/

extern void DevProcess(uint8 DEVCOM,uint8 GROUP);
extern int fputc(int ch, FILE *f);
BOOL UartMessageDeal(MSG_STRUCT* pMsg);


uint8 SysStatusRead(uint8 DEVCOM,uint8 DEVNO);			/*ģ�鹤��״̬��ȡ*/


uint8 CheckSysInnerRStat(uint8 groupnum,uint8 devaddr);
void ReadRTestAddrDATA(uint16 BATGROUP,uint8 DEVNO);
void DataErrCheck(uint8 BatGroup,uint8 DevNO,uint8 ErrCheckFlag,uint8 IsWrite);
uint8 SendCfgParaToDev(uint16 BATGROUP,uint8 DEVNO);
uint8 SendCfgParaToAllDev(void);
uint8 SendShieldInfoToDev(uint16 BATGROUP,uint8 DEVNO);//����������Ϣ��ָ��ģ��
uint8 SendShieldInfoToAllDev(void);//����������Ϣ
void GroupDataRead(uint8 DEVCOM,uint8 group);
uint8 SendParaToS(uint8 group);
void GroupADValueRead(uint8 group);
uint8 ReadCliParaToS(uint8 group);
uint8 SendCliParaToS(uint8 group);
void GroupCliParaRead(uint8 group);
void DevOneKeyRestore(uint8 group);
uint8 CheckVlow(uint8 batgroup);
uint8 CheckTestCondition(uint8 batgroup,uint8 timecheckflag);
void PauseInnerRTest(uint8 batgroup,uint8 DevNO);
void SendRRatioToDev(uint8 group);
void SendCRRatioToDev(uint8 group);
void SendTRatioToDev(uint8 group);
void SendVRatioToDev(uint8 group);
void ReadRatio(uint8 group);
void SendTempIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf);
void ReadTempID(uint8 group,uint8 TempType,uint8 TempNO);
void SendTempEXIDToDev(uint8 group,uint8 TempType,uint8 TempNO,uint8 *buf);
void ReadTempEXID(uint8 group,uint8 TempType,uint8 TempNO);
void MasterProcess(uint8 DEVCOM,uint8 GROUP);
void MasterBatVRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterBatVDecRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterBatTRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterBatRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterBatCRRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterDevStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterCfgRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterGroupDataRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);
void MasterErrStatRead(uint16 BATGROUP,uint8 DevCom,uint16 ReadRegNum);

#endif
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

