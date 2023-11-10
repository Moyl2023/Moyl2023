/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
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



#define BAT_DATA_R_RD_TIME 			10000	//内阻读取超时重发时间
#define BAT_DATA_T_RD_TIME 			6000	//温度数据读取，6S读一次
#define SYS_STATUS_TIME 			6000	//系统状态读取，6S读一次
#define INNER_R_TEST_OVER_TIME 		2000000	//内阻测量超过900S，则认为超时
#define BAT_R_TEST_TIME 			600000	//10分钟内只允许测量一次
#define GROUP_DATA_RD_TIME			6000	//整组数据读取时间





/*内阻测试时相关变量注释
InnerRTestDevNO:正在测试的模块编号，第一个模块测量时，值为1；
S_StartInterMea:启动停止标识；
InnerRTestFlag:内阻测量状态；
LCD_StartInterMea:每个模块测量状态，用来查询是否收到启动命令；
InnerRTestCtrlTimeMs:记录启动时间；
BatRLastTestTimeMs:最近一次的内阻测量时间；
*/









/*主机显示通讯任务外部调用*/

extern void DevProcess(uint8 DEVCOM,uint8 GROUP);
extern int fputc(int ch, FILE *f);
BOOL UartMessageDeal(MSG_STRUCT* pMsg);


uint8 SysStatusRead(uint8 DEVCOM,uint8 DEVNO);			/*模块工作状态读取*/


uint8 CheckSysInnerRStat(uint8 groupnum,uint8 devaddr);
void ReadRTestAddrDATA(uint16 BATGROUP,uint8 DEVNO);
void DataErrCheck(uint8 BatGroup,uint8 DevNO,uint8 ErrCheckFlag,uint8 IsWrite);
uint8 SendCfgParaToDev(uint16 BATGROUP,uint8 DEVNO);
uint8 SendCfgParaToAllDev(void);
uint8 SendShieldInfoToDev(uint16 BATGROUP,uint8 DEVNO);//发送屏蔽信息到指定模块
uint8 SendShieldInfoToAllDev(void);//发送屏蔽信息
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

