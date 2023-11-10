/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.h
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions:    Modbus后台通讯数据结构
****************************************************************************************/
#ifndef _MBSMASTER_H
#define _MBSMASTER_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "ModbusDef.h"

/*****************************BAT5K***********************************/
#define BAT_V_DATA_ADDR							0x0000				 	/*电压数据起始地址*/
#define BAT_R_DATA_ADDR							0x0100				 	/*内阻数据起始地址*/
#define CONNECT_R_DATA_ADDR						0x0200				/*连接电阻数据起始地址*/
#define BAT_TEMP_DATA_ADDR						0x0300				/*温度数据起始地址*/
#define BAT_ERR_STAT_ADDR						0x0400				/*电池故障状态起始地址*/
#define DEV_STAT_ADDR							0x0700				 	/*模块运行状态起始地址*/
#define MEA_DATA_ADDR							0x0800				 	/*测量数据起始地址*/
#define CAL_PARA_ADDR							0x0850					/*校准参数起始地址*/
#define SYS_WORK_STAT_ADDR						0x1000				/*系统工作状态起始地址*/
#define ASSIST_STAT_ADDR						0x1010				/*辅助状态起始地址*/
#define SYS_CTRL_ADDR							0x1020				 	/*系统控制起始地址*/
#define BAT_V_TEST_ADDR							0x1020
#define BAT_CR_TEST_ADDR						0x1021
#define BAT_T_TEST_ADDR							0x1022
#define BAT_R_TEST_ADDR							0x1023
#define BAT_EQ_ADDR								0x1024
#define MSYS_CFG_ADDR							0x1040				 	/*系统配置起始地址*/
#define SYS_TIME_ADDR							0x1060				 	/*系统时钟起始地址*/
#define COM_PARA_ADDR							0x1070				 	/*通讯参数起始地址*/
#define GROUP_DATA_ADDR							0x1100				/*电池整组数据起始地址*/
#define GROUP_CAL_ADDR							0x1180				/*整组采集盒校准数据*/
#define GROUP_PARA_ADDR							0x1200				/*整组采集盒参数地址*/
#define GROUP_STAT_ADDR							0x1280				/*整组采集盒状态地址*/
#define RATIO_ADDR								0x1300				/*内阻校准系数*/
#define V_DATA_DEC_ADDR							0x1350				/*电压小数*/
#define TEMP_SET_ADDR							0x1370				//温度探头ID信息地址
#define TEMP_EXID_ADDR							0x13D0				//温度探头自定义ID地址
#define M_SHIELD_INFO_ADDR						0x1400				//屏蔽信息地址
#define HISTORY_RECORD_ADDR						0x4000				/*历史记录起始地址*/
#define USART_TEST_ADDR							0x5000				 	/*串口测试起始地址*/


#define BAT_V_DATA_NUM							18						/*电压数据起始地址*/
#define BAT_R_DATA_NUM							18						/*内阻数据起始地址*/
#define CONNECT_R_DATA_NUM						18					/*连接电阻数据起始地址*/
#define BAT_TEMP_DATA_NUM						18					/*温度数据起始地址*/
#define BAT_ERR_STAT_NUM						500				 	/*电池故障状态起始地址*/
#define DEV_STAT_NUM							15				 		/*模块运行状态起始地址*/
#define MEA_DATA_NUM							3				 			/*测量数据起始地址*/
#define CAL_PARA_NUM							179 						/*校准参数起始地址,包含还原指令08B2*/
#define SYS_WORK_STAT_NUM						2				 		/*系统工作状态起始地址*/
#define ASSIST_STAT_NUM							4							/*辅助状态起始地址*/
#define SYS_CTRL_NUM							5				 			/*系统控制起始地址*/
#define MSYS_CFG_NUM							28				 		/*系统配置起始地址*/
#define SYS_TIME_NUM							6				 			/*系统时钟起始地址*/
#define COM_PARA_NUM							36				 		/*通讯参数起始地址*/
#define GROUP_DATA_NUM							16				/*电池整组数据起始地址*/
#define GROUP_CAL_NUM							16				/*整组采集盒校准数据*/
#define GROUP_PARA_NUM							16				/*整组采集盒参数地址*/
#define GROUP_STAT_NUM							16				/*整组采集盒状态地址*/
#define RATIO_NUM								4				/*内阻校准系数*/
#define V_DATA_DEC_SUM							18				/*电压数据小数起始地址*/
#define TEMP_SET_SUM							76				/*温度探头ID设置起始地址*/
#define TEMP_EXID_SUM							21				//温度探头自定义ID设置起始地址
#define SHIELD_INFO_NUM							33				//5节屏蔽电池信息和14个巡检模块温度探头屏蔽信息
#define HISTORY_RECORD_NUM						78				 	/*历史记录起始地址*/
#define USART_TEST_NUM							9				 		/*串口测试起始地址*/



typedef enum
{
	NON_SECT = 0,											/*不存在数据段*/
	BAT_V_DATA_SECT,									/*电池电压数据段*/
	BAT_R_DATA_SECT,									/*电池内阻数段*/
	CONNECT_R_DATA_SECT,							/*连接电阻数据段*/
	BAT_TEMP_DATA_SECT,								/*电池温度数据段*/
	BAT_ERR_STAT_SECT,								/*电池故障状态段*/
	DEV_STAT_SECT,										/*模块运行状态段*/
	MEA_DATA_SECT,										/*测量数据段*/
	CAL_PARA_SECT,										/*校准参数段*/
	SYS_WORK_STAT_SECT,								/*系统工作状态段*/
	ASSIST_STAT_SECT,									/*辅助状态段*/
	SYS_CTRL_SECT,										/*系统控制段*/
	MSYS_CFG_SECT,										/*系统配置段*/
	SYS_TIME_SECT,										/*系统时钟段*/
	COM_PARA_SECT,										/*通讯参数段*/
	GROUP_DATA_SECT,									/*采集盒整组数据段*/
	GROUP_CAL_SECT,									/*采集盒校准数据段*/
	GROUP_PARA_SECT,									/*采集盒参数数据段*/
	GROUP_STAT_SECT,									/*采集盒状态数据段*/
	R_RATIO_SECT,									/*采集盒状态数据段*/
	V_DATA_DEC_SECT,								/*电压数据小数数据段*/
	TEMP_SET_SECT,									//温度探头设置数据段
	TEMP_EXID_SECT,									//温度探头自定义ID数据段
	SHIELD_INFO_SECT,								//屏蔽信息数据段
	HISTORY_RECORD_SECT,							/*历史记录段*/
	USART_TEST_SECT,									/*串口测试段*/
}SECT_ADDR_DEF;

#define MAX_SECT USART_TEST_SECT
/*********************电池数据**************************/
#define MAX_BAT_NUM								18//每个模块最多18节电池
#define MAX_DEV_NUM								14//最多支持240节电池，七个模块
#define MAX_GROUP_NUM							2//总电池组数
#define MAX_GROUP_BAT_NUM						240//一组电池中电池最大数量，形式实验要求

//typedef struct
//{
//	uint16_t BatV[MAX_BAT_NUM];			/*电池电压(uv)*/
//	uint16_t InterR[MAX_BAT_NUM];		/*电池内阻(uΩ)*/
//	uint16_t ConnectR[MAX_BAT_NUM];	/*连接电阻(uΩ)*/
//	int16_t Temp[MAX_BAT_NUM];			/*温度*/
//}MOUDLE_MEA_DATA;

///*********************巡检模块运行状态**************************/
//typedef struct
//{
//	int16_t Temp;													/*巡检模块环境温度*/
//	uint16_t MeaInterRStat;									/*内阻测量状态*/
//	uint16_t CellMeaStat;										/*电池巡检状态*/
//	uint16_t ConnectRMeaStat;								/*连接条巡检状态*/
//	uint16_t TempMeaStat;										/*温度巡检状态*/
//	uint16_t DevInputStat;								/*巡检模块输入状态*/
//	uint16_t DevOutputStat;							/*巡检模块输出状态*/
//	uint16_t DevERR;											/*巡检模块故障*/
//	uint16_t DevHWVersion;								/*模块硬件版本号*/
//	uint16_t DevSWVersion;								/*模块软件版本号*/
//	uint16_t DischargeMoudle;							/*放电模块状态*/
//	uint16_t ConnectMoudle;								/*连接条模块状态*/
//	uint16_t EQMoudle;									/*均衡模块状态*/
//}MOUDLE_STAT;
///*********************测量原始数据**************************/
//typedef struct
//{
//	uint16_t CellVoltageAD;									/*蓄电池电压采集通道AD值*/
//	uint16_t ConnectVoltageAD;							/*连接条电压采集通道AD值*/
//	uint16_t CurrentAD;											/*放点电流采集通道AD值*/
//}MEA_DATA;
///*********************校准参数**************************/
//typedef struct
//{
//	fp32 CellVoltageB;									/*蓄电池电压采集通道B值*/
//	fp32 CellVoltageK;									/*蓄电池电压采集通道K值*/
//	fp32 ConnectVoltageB;								/*连接条电压采集通道B值*/
//	fp32 ConnectVoltageK;								/*连接条电压采集通道K值*/
//	fp32 CurrentB;											/*放电电流电压采集通道B值*/
//	fp32 CurrentK;											/*放电电流电压采集通道K值*/
//}CAL_PARA;

///*********************系统工作状态**************************/
//typedef struct
//{
//	uint16_t PresentMode;										/*当前工作模式*/
//	uint16_t ExecutMode;										/*正在执行的模式*/
//}SYS_WORK_STAT;

///*********************辅助状态**************************/
//typedef struct
//{
//	uint16_t OutputStat;									/*IO输出状态*/
//	uint16_t InputStat;										/*IO输入状态*/
//	uint16_t AlarmOutputStat;							/*报警输出状态*/
//	uint16_t SysErrStat;									/*系统故障状态*/
//}ASSIST_STAT;
///*********************系统控制寄存器**************************/

//#define START 0xaa
//#define STOP   0x55

//typedef struct
//{
//	uint16_t StartCellMea;								/*启动电池巡检*/
//	uint16_t StartConnectMea;							/*启动连接条巡检*/
//	uint16_t StartTempMea;								/*启动温度巡检*/
//	uint16_t StartInterMea;								/*启动内阻测量*/
//}SYS_CTRL;

///*********************串口测试**************************/
//typedef struct
//{
//	uint16_t TestChannel;										/*测试通道*/
//	uint16_t TestType;											/*测试类型*/
//	uint16_t TestDataWordQuantity;					/*测量数据字节数*/
//	uint16_t RcvBuffer[6];									/*接收缓存*/
//}COM_TEST;



//typedef struct
//{
//	MOUDLE_MEA_DATA 	DevMeaData;
//	//MEA_DATA 					MeaData;
//}BAT_DATA;

//typedef struct
//{
//	CAL_PARA CalPara;
//	//COM_PARA ComPara;
//	SYS_CTRL  SysCtrl;
//	//MSYS_CFG MSysCfg;
//	//SYS_TIME CalTime;
//}SYSTEM_PARA;

//typedef struct
//{
//	//BAT_ERR_STAT BatErrStat;
//	MOUDLE_STAT  MoudleStat;
//	SYS_WORK_STAT SysWorkStat;
//	ASSIST_STAT		AssistStat;
//}STATUS_DATA;

typedef  struct 
{
	uint8  CommErrStatus[MAX_GROUP_NUM][30];  
	uint8  CommErrCount[MAX_GROUP_NUM][30];
	uint32 CommErrStartTime[MAX_GROUP_NUM][30];
	uint8  Bat5kCommErrStatus[MAX_GROUP_NUM]; //采集盒 
	uint8  Bat5kCommErrCount[MAX_GROUP_NUM];
	uint32 Bat5kCommErrStartTime[MAX_GROUP_NUM];
	uint8  BkgCommErrStatus; //后台主机
	uint8  BkgCommErrCount;
	uint32 BkgCommErrStartTime;
	//BAT_DATA BatData;         /*数据*/
	//STATUS_DATA StatusData;	        /*状态数据*/
	//SYSTEM_PARA SysPara;	        /*系统参数*/
	//HISTORY_RECORD HistoryRecord;   /*历史记录*/
	//COM_TEST 		ComTest;
}MBS_MASTER;   

extern MBS_MASTER MbsMaster;

extern BOOL MbsMasterLoop(uint8 com, uint8 DecAddr, uint8 cmd, uint16 startAddr, uint16 *buf, uint16 length);	
//extern BOOL SavePara(uint8 PageId);
/*********************BAT5K**************************/



#ifdef  __cplusplus
}
#endif
#endif

