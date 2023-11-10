/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsSlave.h
** Created By: 
** Last modified Date: 
** Last Version: 1.0
** Descriptions: 基RVMDK 4.72软件版本
****************************************************************************************/
#ifndef _MBSSLAVE_H
#define _MBSSLAVE_H

#ifdef  __cplusplus
extern  "C" {
#endif
	
#include "MbsMaster.h"
#include "ModbusDef.h"

/*定义数据段的起始地址*/
#define S_BAT_V_DATA_ADDR							0x0000				 /*电压数据起始地址*/
#define S_BAT_R_DATA_ADDR							0x0200				 /*内阻数据起始地址*/
#define S_CONNECT_R_DATA_ADDR						0x0400				 /*连接电阻数据起始地址*/
#define S_BAT_TEMP_DATA_ADDR						0x0600				 /*温度数据起始地址*/
#define S_BAT_ERR_STAT_ADDR							0x0800				 /*电池故障状态起始地址*/
#define S_MBS_SINGNAL_ADDR							0x0A50				/*MODBUS后台遥信起始地址*/
#define S_DEV_STAT_ADDR								0x0B00				 /*模块运行状态起始地址*/
#define S_MEA_DATA_ADDR								0x0C00				 /*测量数据起始地址*/
#define S_CAL_PARA_ADDR								0x0C50				/*校准参数起始地址*/
#define S_SYS_WORK_STAT_ADDR						0x1000				 /*系统工作状态起始地址*/
#define S_ASSIST_STAT_ADDR							0x1010				/*辅助状态起始地址*/
#define S_SYS_CTRL_ADDR								0x1020				 /*系统控制起始地址*/
#define S_MSYS_CFG_ADDR								0x1040				 /*系统配置起始地址*/
#define S_SYS_TIME_ADDR								0x10A0				 /*系统时钟起始地址*/
#define S_COM_PARA_ADDR								0x10B0				 /*通讯参数起始地址*/
#define S_GROUP_DATA_ADDR							0x1100				/*电池整组数据起始地址*/
#define S_GROUP_CAL_ADDR							0x1180				/*整组采集盒校准数据*/
#define S_GROUP_PARA_ADDR							0x1200				/*整组采集盒参数地址*/
#define S_GROUP_STAT_ADDR							0x1280				/*整组采集盒状态地址*/
#define S_R_RATIO_ADDR								0x1300				/*内阻校准系统*/
#define S_V_DATA_DEC_ADDR							0x1350				//电压数据小数地址
#define S_TEMP_SET_ADDR								0x1370				//温度探头ID信息地址
#define S_TEMP_EXID_ADDR							0x13D0				//温度探头自定义ID地址
#define S_SHIELD_INFO_ADDR							0x1400				//屏蔽信息起始地址
#define S_HISTORY_RECORD_ADDR						0x4000				 /*历史记录起始地址*/
#define S_USART_TEST_ADDR							0x5000				 /*串口测试起始地址*/
#define LCD_CMD_ADDR								0x2000				 /*串口测试起始地址*/


#define S_BAT_V_DATA_NUM							400					/*电压数据起始地址*/
#define S_BAT_R_DATA_NUM							400					/*内阻数据起始地址*/
#define S_CONNECT_R_DATA_NUM						400					/*连接电阻数据起始地址*/
#define S_BAT_TEMP_DATA_NUM							400					/*温度数据起始地址*/
#define S_BAT_ERR_STAT_NUM							515				 	/*电池故障状态起始地址*/
#define S_MBS_SINGNAL_NUM							4					/*MODBUS后台遥信寄存器数量*/
#define S_DEV_STAT_NUM								220				 	/*模块运行状态起始地址*/
#define S_MEA_DATA_NUM								3				 		/*测量数据起始地址*/
#define S_CAL_PARA_NUM								6 					/*校准参数起始地址*/
#define S_SYS_WORK_STAT_NUM							2				 		/*系统工作状态起始地址*/
#define S_ASSIST_STAT_NUM							4						/*辅助状态起始地址*/
#define S_SYS_CTRL_NUM								5				 		/*系统控制起始地址*/
#define S_MSYS_CFG_NUM								42				 	/*系统配置起始地址*/
#define S_SYS_TIME_NUM								6				 		/*系统时钟起始地址*/
#define S_COM_PARA_NUM								36				 	/*通讯参数起始地址*/
#define S_GROUP_DATA_NUM							16				/*电池整组数据起始地址*/
#define S_GROUP_CAL_NUM								16				/*整组采集盒校准数据*/
#define S_GROUP_PARA_NUM							16				/*整组采集盒参数地址*/
#define S_GROUP_STAT_NUM							16				/*整组采集盒状态地址*/
#define S_RATIO_NUM									4				/*内阻校准系统*/
#define S_V_DATA_DEC_SUM							400				//电池电压数据小数地址
#define S_TEMP_SET_SUM								76				//温度探头ID信息地址
#define S_TEMP_EXID_SUM								21				//温度探头自定义ID
#define S_SHIELD_INFO_NUM							33				//5节屏蔽电池信息和14个巡检模块温度探头屏蔽信息
#define S_HISTORY_RECORD_NUM						78				 	/*历史记录起始地址*/
#define S_USART_TEST_NUM							9				 		/*串口测试起始地址*/
#define LCD_CMD_NUM									0x0900					

typedef enum
{
	S_NON_SECT = 0,												/*不存在数据段*/
	S_BAT_V_DATA_SECT,											/*电池电压数据段*/
	S_BAT_R_DATA_SECT,											/*电池内阻数段*/
	S_CONNECT_R_DATA_SECT,										/*连接电阻数据段*/
	S_BAT_TEMP_DATA_SECT,										/*电池温度数据段*/
	S_BAT_ERR_STAT_SECT,										/*电池故障状态段*/
	S_MBS_SINGNAL_SECT,											/*MODBUS遥信数据段*/
	S_DEV_STAT_SECT,											/*模块运行状态段*/
	S_MEA_DATA_SECT,											/*测量数据段*/
	S_CAL_PARA_SECT,											/*校准参数段*/
	S_SYS_WORK_STAT_SECT,										/*系统工作状态段*/
	S_ASSIST_STAT_SECT,											/*辅助状态段*/
	S_SYS_CTRL_SECT,											/*系统控制段*/
	S_MSYS_CFG_SECT,											/*系统配置段*/
	S_SYS_TIME_SECT,											/*系统时钟段*/
	S_COM_PARA_SECT,											/*通讯参数段*/
	S_GROUP_DATA_SECT,											/*采集盒整组数据段*/
	S_GROUP_CAL_SECT,											/*采集盒校准数据段*/
	S_GROUP_PARA_SECT,											/*采集盒参数数据段*/
	S_GROUP_STAT_SECT,											/*采集盒状态数据段*/
	S_R_RATIO_SECT,												/*内阻校准系统*/
	S_V_DATA_DEC_SECT,											/*电压数据小数数据段*/
	S_TEMP_SET_SECT,											/*温度探头ID设置段*/
	S_TEMP_EXID_SECT,											//温度探头自定义ID设置段
	S_SHIELD_INFO_SECT,										//屏蔽信息数据段
	S_HISTORY_RECORD_SECT,										/*历史记录段*/
	S_USART_TEST_SECT,											/*串口测试段*/
	LCD_CMD_SECT,
}S_SECT_ADDR_DEF;

#define S_MAX_SECT LCD_CMD_SECT
/*********************电池数据**************************/
typedef struct
{
	uint16_t S_BatV[MAX_DEV_NUM][MAX_BAT_NUM];			/*电池电压(1mv)*/
	uint16_t S_VDec[MAX_DEV_NUM][MAX_BAT_NUM];			/*电池电压小数(0.1mv)*/
	uint16_t S_InterR[MAX_DEV_NUM][MAX_BAT_NUM];		/*电池内阻(uΩ)，超过10毫欧的最高位置1，存储单位为10微欧，未超过10毫欧的存储单位为1微欧*/
	uint16_t S_ConnectR[MAX_DEV_NUM][MAX_BAT_NUM];	/*连接电阻(uΩ)，存储方式和内阻一样*/
	float S_Temp[MAX_DEV_NUM][MAX_BAT_NUM];			/*温度*/
}S_MOUDLE_MEA_DATA;

/*********************蓄电池故障状态数据**************************/
#define S_MAX_ERR_DATA								500
typedef enum
{
	NORMAL = 0,
	VOLTAGE_OVER,						// 1电池过压
	VOLTAGE_LOW,						// 2电池欠压
	VOLTAGE_RECOVER,					// 3电压恢复
	TEMP_OVER,							// 4温度过高
	TEMP_LOW,							// 5温度过低
	TEMP_RECOVER,						// 6温度恢复
	INTER_R_OVER,						// 7内阻超限
	CR_OVER,							// 8连接条阻值超限
	GROUP_V_OVER,						// 9组电压过高
	GROUP_V_LOW,						// 10组电压过低
	GROUP_V_RECOVER,					// 11组电压恢复
	CURRENT_DISCHARGE_OVER,			// 12放电电流过高
	CURRENT_DISCHARGE_RECOVER,		// 13放电电流恢复
	CURRENT_CHARGE_OVER,				// 14充电电流过高
	CURRENT_CHARGE_RECOVER,			// 15充电电流恢复
	ENVIRMENT_TEMP_OVER,				// 16环境温度过高
	ENVIRMENT_TEMP_LOW,				// 17环境温度过低
	ENVIRMENT_TEMP_RECOVER,			// 18环境温度恢复
	DEV_TEMP_ERR,						// 19温度传感器故障
	DEV_TEMP_RECOVER,					// 20温度传感器故障恢复
	ENVIR_TEMP_ERR,					// 21环境温感故障
	ENVIR_TEMP_RECOVER,				// 22环境温感故障恢复
	DEV_TEST_FAIL,						//23内阻测试失败	
	DEV_COMM_ERR,						// 24通信异常
	DEV_COMM_RECOVER,				// 25通信异常恢复	
	FDY_COM_ERR,						// 26放电仪通信异常
	FDY_COM_RECOVER,					// 27放电仪通信恢复
	GPS_COM_ERR,						// 28GPS通信异常
	GPS_COM_RECOVER,					// 29GPS通信恢复
	BAT5000_S_COM_ERR,				// 30采集盒通信异常
	BAT5000_S_COM_RECOVER,			// 31采集盒通信恢复
	BKG_COM_ERR,						// 32巡检仪主机后台通信异常
	BKG_COM_RECOVER,					// 33巡检仪主机后台通信恢复	
	SYS_OPEN_CIRCUIT,					//34回路开路
	SYS_OPEN_CIRCUIT_RECOVER,		// 35回路开路恢复
	SYS_POWER_ON,						//36系统开机
	DEV_INNER_R_ERR,					// 37内阻模块故障
	DEV_INNER_R_RECOVER,				// 38内阻模块故障恢复
	DEV_CR_ERR,						// 39连接条模块故障
	DEV_CR_RECOVER,					// 40连接条模块故障恢复
	DEV_EQ_ERR,						// 41均衡模块故障
	DEV_EQ_RECOVER,					// 42均衡模块故障恢复
	START_TEST,							// 43手动内阻测试
	AUTO_TEST,							// 44自动内阻测试
	JKQ_TEST,							// 45后台内阻测试
	DEV_TEST_FAIL1,					//46内阻测试失败原因 :放电电流小
	DEV_TEST_FAIL2,					//47内阻测试失败	:放电电流过大
	DEV_TEST_FAIL3,					//48内阻测试失败原因 :停止内阻测试
	DEV_TEST_FAIL4,					//49内阻测试失败	:放电组组电压过小
	DEV_TEST_FAIL5,					//50内阻测试失败原因 :放电组组电压过大
	DEV_TEST_FAIL6,					//51内阻测试失败	:单体电压过小
	DEV_TEST_FAIL7,					//52内阻测试失败原因 :单体电压过大	
	DEV_TEST_FAIL8,					//53内阻测试失败	:单体温度过低或温度探头异常
	DEV_TEST_FAIL9,					//54内阻测试失败原因 :单体温度过高	
	DEV_TEST_FAIL10,					//55内阻测试失败原因 :测试超时
	BAT_SAMPLE_LINE_DROP,				//56	电池采样线脱落
	GROUP_SAMPLE_LINE_DROP,			//57	电池组采样线脱落
	SAMPLE_LINE_DROP_RECOVER,		//58采样线脱落恢复
	START_BAT_DISCHARGE,				//59 启动蓄电池组放电
	STOP_BAT_DISCHARGE,				//60 停止蓄电池组放电
	GROUP_SAMPLE_LINE_DROP_RECOVER,	//61电池组采样线脱落恢复
	BAT_SAMPLE_LINE_RESERVE,			//62电池采样线反接
	GROUP_SAMPLE_LINE_RESERVE,		//63电池组采样线反接
	BAT_SAMPLE_LINE_RESERVE_RECOVER,//64电池采样线反接恢复
	GROUP_SAMPLE_LINE_RESERVE_RECOVER,//65电池组采样线反接恢复
	PAUSE_R_TEST,						//66暂停内阻测试
	CONTINUE_R_TEST,					//67继续进行内阻测试
	GV_LOW_DISC_STOP,					//68组电压欠压停止放电
	TIME_OVER_DISC_STOP,				//69放电时间到
	DISC_START,						//70启动放电		
	BATV_LOW_DISC_STOP,				//71单节电池欠压停止放电
	STOP_DISC_STOP,					//72手动停止放电
	DISC_PAUSE,						//73暂停放电
	RESTART_DISC,						//74继续放电


	


	DEV_DISCHARGE_VOLTAGE_ERR,			// 23放电组组电压异常
	DEV_DISCHARGE_VOLTAGE_RECOVER,		// 24放电组组电压恢复
	DEV_R_PATROL_ERR,					// 25内阻巡检异常
	DEV_R_PATROL_RECOVER,				// 26内阻巡检恢复
	DEV_DISCHARGE_DEV_ERR,				// 27放电板异常
	DEV_DISCHARGE_DEV_RECOVER,			// 28放电板异常恢复
	DEV_DISCHARGE_CURRENT_ERR,			// 29放电电流异常
	DEV_DISCHARGE_CURRENT_RECOVER,		// 30放电电流异常恢复
	DEV_EQ_OPEN_ERR,					// 31均衡异常(开)
	DEV_EQ_OPEN_RECOVER,				// 32均衡异常(开)恢复
	DEV_EQ_CLOSE_ERR,					// 33均衡异常(关闭)
	DEV_EQ_CLOSE_RECOVER,				// 34均衡异常(关闭)恢复

}ERR_TYPE;
typedef struct
{
		uint8_t S_ErrBatNum;/*异常蓄电池编号及故障类型*/
		uint8_t S_ErrType;																/*0x01-过压，0x02-欠压，0x03-电压恢复，0x04-高温，0x05-低温，0x06-温度恢复
																		0x07-电池内阻超限，0x08-连接电阻超限*/
		int16_t S_ErrData;/*异常蓄电池故障数据*/	
}S_ERR_INFO;

typedef struct
{
	uint16_t S_Quantity;							/*异常蓄电池总数*/
	S_ERR_INFO S_ErrInfo[S_MAX_ERR_DATA];
}S_BAT_ERR_STAT;
/*********************巡检模块运行状态**************************/
typedef enum
{
	NOERR = 0,
	DEV_TEMP_CONVERT_ERR1,//温度转换故障
	DEV_TEMP_READ_ERR1,//温度读取故障
	DEV_DISCHARGE_VOLTAGE_ERR1,//放电组组电压异常
	DEV_R_PATROL_ERR1,//内阻巡检异常
	DEV_DISCHARGE_DEV_ERR1,//放电板异常
	DEV_DISCHARGE_CURRENT_ERR1,//放电电流过低
	DEV_EQ_OPEN_ERR1,//均衡异常(打开)
	DEV_EQ_CLOSE_ERR1,//均衡异常(关闭)
	SINGLE_V_OVER,//单体电压过高
	SINGLE_V_LOW,//单体电压过低
	SINGLE_TEMP_OVER,//单体温度过高
	SINGLE_TEMP_LOW,//单体温度过低
	DISCHARGE_CURRENT_OVER,//放电电流过大
	COMM_ERR,		//通信异常
}DEV_ERR;

typedef enum
{
	NO_MEA = 0,
	MEA_OVER = 0x0100,
}MEA_STAT;

typedef struct
{
	int16_t InnerRRatio;//(放大1000倍)
	int16_t CRRatio;//(放大1000倍)
	int16_t TempCliPara;//(放大十倍)
	int16_t VCliPara;//(放大十倍)
}R_RATIO_INFO;//内阻校准系数

typedef struct
{
	float S_Temp;										/*巡检模块环境温度*/
	uint16_t S_MeaInterRStat;							/*内阻测量状态*/
	uint16_t S_CellMeaStat;								/*电池巡检状态*/
	uint16_t S_ConnectRMeaStat;							/*连接条巡检状态*/
	uint16_t S_TempMeaStat;								/*温度巡检状态*/
	uint16_t S_DevInputStat;							/*巡检模块输入状态*/
	uint16_t S_DevOutputStat;							/*巡检模块输出状态*/
	uint16_t S_DevERR;									/*巡检模块故障*/
	uint16_t S_DevHWVersion;							/*模块硬件版本号*/
	uint16_t S_DevSWVersion;							/*模块软件版本号*/
	uint16_t S_InnerRDev;								/*放电模块注册信息*/
	uint16_t S_CRDev;									/*连接条模块注册信息*/
	uint16_t S_EQDev;									/*均衡模块注册信息*/
	uint16_t S_CalStat;									/*校准状态信息*/
	uint16_t S_Handshake;								/*与模块握手信息*/	
}S_MOUDLE_STAT;
/*********************测量原始数据**************************/
typedef struct
{
	uint16 S_CellVoltageAD;									/*蓄电池电压采集通道AD值*/
	uint16 S_ConnectVoltageAD;							/*连接条电压采集通道AD值*/
	uint16 S_CurrentAD;											/*放点电流采集通道AD值*/
}S_MEA_DATA;
/*********************校准参数**************************/
typedef struct
{
	int16 S_CellVoltageB;									/*蓄电池电压采集通道B值*/
	uint16 S_CellVoltageK;									/*蓄电池电压采集通道K值*/
	int16 S_ConnectVoltageB;								/*连接条电压采集通道B值*/
	uint16 S_ConnectVoltageK;								/*连接条电压采集通道K值*/
	int16 S_CurrentB;											/*放电电流电压采集通道B值*/
	uint16 S_CurrentK;											/*放电电流电压采集通道K值*/
}S_CAL_PARA;
/*********************温度探头ID信息**************************/
typedef struct
{
	uint8 TempType;//0表示电池温度探头，1表示环境温度探头1；2表示环境温度探头2；3表示环境温度探头3
	uint8 TempNO;
	uint16 WrExid;//写入的自定义ID
	uint16 RdExid;//读取的自定义ID
	uint8 WrID[8];//写入的厂家ID
	uint8 RdID[8];//读取的厂家ID
}TEMP_SET;//温度探头相关信息变量
/*********************电池屏蔽信息**************************/
typedef struct
{
	uint16 BatVShield[5];
	uint16 TempShield[MAX_DEV_NUM*2];
}SHIELD_INFO;
/*********************系统工作状态**************************/
typedef struct
{
	uint16 S_PresentMode;										/*当前工作模式*/
	uint16 S_ExecutMode;										/*正在执行的模式*/
}S_SYS_WORK_STAT;

/*********************辅助状态**************************/
typedef struct
{
	uint16 S_OutputStat;									/*IO输出状态*/
	uint16 S_InputStat;										/*IO输入状态*/
	uint16 S_AlarmOutputStat;							/*报警输出状态*/
	uint16 S_SysErrStat;									/*系统故障状态*/
}S_ASSIST_STAT;
/*********************系统控制寄存器**************************/
#define START		0xaa
#define STOP		0x55

typedef struct
{
	uint16 S_StartCellMea;								/*启动电池巡检*/
	uint16 S_StartConnectMea;							/*启动连接条巡检*/
	uint16 S_StartTempMea;								/*启动温度巡检*/
	uint16 S_StartInterMea;								/*启动内阻测量*/
	uint16 S_StartBatEQ;								/*启动电池均衡*/
	uint16 S_DevCliCmd;									/*巡检模块校准命令*/
	uint16 S_BatDisCmd;									/*(0x1026)蓄电池组放电控制命令*/
}S_SYS_CTRL;

/*********************系统配置寄存器**************************/
typedef struct
{
	uint16 S_CellNum;									/*组蓄电池数量*/
	uint16 S_GroupVoltageMax;							/*整组电压上限*/
	uint16 S_GroupVoltageMin;							/*整组电压下限*/
	int16 S_ChargeCurrentMax;							/*充电电流上限*/
	int16 S_DischargeCurrentMax;						/*放电电流上限*/
	uint16 S_SingleVoltageMax;							/*单体电压上限*/
	uint16 S_SingleVoltageMin;							/*单体电压下限*/
	uint16 S_SingleCellRMax;							/*单体电池内阻上限*/
	uint16 S_ConnectRMax;								/*连接条内阻上限*/
	int16 S_TempMax;									/*温度上限*/
	int16 S_TempMin;									/*温度下限*/
	uint16 S_AutoMeaRTime;								/*自动测量内阻间隔，单位为"天"*/
	uint16 EqParaCfg;									//均衡参数配置，均衡百分比
	uint16	BatVoltageLevel;						//电池电压等级
	uint16	InnerRDevStat;							//内阻巡检功能启用关闭
	uint16	CRDevStat;								//连接条功能启用关闭
	uint16	EQDevStat;								//均衡功能启用关闭
	uint16  TempDevStat;							//温度采集功能启用关闭
}GROUP_CFG;


typedef struct
{
	uint16 S_CellGroupNum;								/*蓄电池组数量*/
	GROUP_CFG GroupCfg[MAX_GROUP_NUM];	
	uint16 CurrentChannel[MAX_GROUP_NUM];
	uint16 S_GpsTimeCalEN;								/*使能GPS对时*/
	uint16 S_HostHWVersion;							/*主板硬件版本号*/
	uint16 S_HostSWVersion;							/*主板软件版本号*/
	
}S_MSYS_CFG;

typedef enum
{
	NO_SENSOR = 0,
	HALL_SENSOR1 = 1,
	HALL_SENSOR2 = 2,
	SHUNT = 3,//分流器
}I_CHANNEL;

/*********************系统时钟寄存器**************************/
typedef struct
{
	uint16 Year;												/*年*/
	uint16 Month;												/*月*/
	uint16 Day;													/*日*/
	uint16 Hour;												/*时*/
	uint16 Minute;											/*分*/
	uint16 Second;											/*秒*/
}S_SYS_TIME;




/*********************通讯参数寄存器**************************/
#define S_MAX_USART_NUM 5

typedef enum
{
	RATE2400 = 0,
	RATE4800,
	RATE9600,
	RATE19200,
	RATE38400,
	RATE115200,
}BAUD_RATE;		//波特率

typedef enum
{
	NOPROTOCAL = 0,
	BAT5000_MABC,
	BAT5000_MBS,
	BATM30BPROTOCAL,
	BAT5000_XRD,
	EB90PROTOCAL,		
	A55APROTOCAL,
	BAT5000_WZCK,
	BAT5K_3000B,
}COM_PROTOCAL;/*通信端口通信协议*/

typedef enum
{
	NOUSE = 0,
	BATGROUP1,		//蓄电池组1
	BATGROUP2,		//蓄电池组2
	JKQ,			//监控器
	MODBUS,			//mbs后台从机
	MODBUSHOST,		//mbs后台主机
	BCODETIME,
	FDY,			//放电仪
}COM_PURPOSE;/*通信端口用途*/

typedef	struct
{
	uint16 BaudRate;									/*波特率*/
	uint16 ComAddr;										/*本端口地址*/
	uint16 SlaveAddr;									/*从机地址*/
	uint16 Purpose;										/*用途*/
	uint16 Reserve1;
	uint16 Reserve2;	
}CAN_PARA;

typedef	struct
{
	uint16 BaudRate;									/*波特率*/
	uint16 ComAddr;										/*本端口地址*/
	uint16 ComProtocal;								/*通讯协议*/
	uint16 Purpose;										/*用途*/
	uint16 Reserve1;
	uint16 Reserve2;
}USART_PART;	

typedef struct
{
	CAN_PARA CanPara;
	USART_PART UsartPara[S_MAX_USART_NUM];
}S_COM_PARA;

/*********************历史记录**************************/
#define S_MAX_HISTORY_RECORD_NUM   500 //最多记录500条历史记录
typedef struct
{
	uint16 HistoryRecordQuantity[MAX_GROUP_NUM];		/*历史记录最后一条存储位置，0表示没有历史记录，非0表示存储位置*/
	uint8 RecordOverFlag[MAX_GROUP_NUM];				//历史记录超过最高记录条数标识符
	uint16 HistoryRecordNum;							/*历史记录序号*/
	uint16 ReadQuantity;								/*读取的条数*/

	uint16 Year;
	uint16 MonthDay;										/*Month/Day*/
	uint16 HourMinute;									/*Hour/Minute*/
	uint16 Sec;
	uint16 ErrSource;			//故障来源是单节电池时，该变量表示电池编号，如果来源是模块或电池组时，高4位表示组号，低4位表示设备编号
	uint16 ErrType;											/*故障类型*/
	int16 ErrData;											/*故障数据*/
	uint16 ReserveData;									/**/
}S_HISTORY_RECORD;
/*********************串口测试**************************/
typedef struct
{
	uint16 TestChannel;										/*测试通道*/
	uint16 TestType;											/*测试类型*/
	uint16 TestDataWordQuantity;					/*测量数据字节数*/
	uint16 RcvBuffer[6];									/*接收缓存*/
}S_COM_TEST;

typedef enum
{
	VOVER = 0,	//单体过压
	VLOW = 1,		//单体欠压
	GROUPVOVERSIG = 2,		//组电压过压
	GROUPVLOWSIG = 3,			//组电压欠压
	TOVER = 4,			//温度过高
	TLOW = 5,			//温度过低
	INNERROVER = 6,		//内阻过高
	CROVER = 7,			//连接条阻值过高
	IOVER = 8,			//充电电流过高
	DISIOVER = 9,		//放电电流过高
}BAT_SIGNAL;/*通信端口用途*/

typedef enum
{
	S_COMM_ERR = 0,	//单体过压
	SMM1_COMM_ERR = 1,//模块1通信故障
	SMM2_COMM_ERR = 2,//模块2通信故障
	SMM3_COMM_ERR = 3,//模块3通信故障
	SMM4_COMM_ERR = 4,//模块4通信故障
	SMM5_COMM_ERR = 5,//模块5通信故障
	SMM6_COMM_ERR = 6,//模块6通信故障
	SMM7_COMM_ERR = 7,//模块7通信故障
	SMM8_COMM_ERR = 8,//模块8通信故障
	SMM9_COMM_ERR = 9,//模块9通信故障
	SMM10_COMM_ERR = 10,//模块10通信故障
	SMM11_COMM_ERR = 11,//模块11通信故障
	SMM12_COMM_ERR = 12,//模块12通信故障
	SMM13_COMM_ERR = 13,//模块13通信故障
	SMM14_COMM_ERR = 14,//模块14通信故障
	GROUP_SMP_LINE_ERR = 15,//电池电压采样线异常
	BAT_SMP_LINE_ERR = 16,//电池电压采样线异常
	BAT_DEVT_ERR = 17,//电池温度探头异常
	TEMP_DEVT_ERR = 18,//环境温度探头异常
}DEV_SIGNAL;/*通信端口用途*/

typedef struct
{
	uint16 MbsSignal1;//电池遥信
	uint16 MbsSignal2;//电池遥信
	uint16 MbsSignal3;//设备遥信
	uint16 MbsSignal4;//设备遥信

}MBS_SIGNAL;

typedef struct
{
	S_MOUDLE_MEA_DATA 	DevMeaData;//电池电压，温度，内阻，连接条阻值
	S_MEA_DATA 			MeaData[MAX_DEV_NUM];//校准AD值
}S_BAT_DATA;

typedef struct
{
	S_CAL_PARA CalPara[MAX_GROUP_NUM][MAX_DEV_NUM];
	S_COM_PARA ComPara;
	S_SYS_CTRL  SysCtrl[MAX_GROUP_NUM];
	S_MSYS_CFG MSysCfg;
	S_SYS_TIME CalTime;
	R_RATIO_INFO RatioInfo;
	TEMP_SET	TempInfo;//
	SHIELD_INFO ShieldInfo[MAX_GROUP_NUM];
}S_SYSTEM_PARA;

typedef struct
{
	S_BAT_ERR_STAT BatErrStat[MAX_GROUP_NUM];
	S_MOUDLE_STAT  MoudleStat[MAX_GROUP_NUM][MAX_DEV_NUM];
	S_SYS_WORK_STAT SysWorkStat;
	S_ASSIST_STAT		AssistStat;
	MBS_SIGNAL MbsSingal[MAX_GROUP_NUM];
}S_STATUS_DATA;

typedef  __packed  struct
{
	int16 GroupV;
	int16 GroupI;
	int16 EnvirTemp;
	float Soc;
	float Soh;
	float Sof;
	
}SYS_MEA_DATA;

typedef struct
{
	float VoltageValue;
	float Current1Value;
	float Current2Value;
	float Current3Value;
	int32_t VoltageAD;
	int32_t Current1AD;
	int32_t Current2AD;
	int32_t Current3AD;
}GROUP_MEA_DATA;



typedef struct
{
	float KU;
	float BU;
	float KI1;
	float BI1;
	float KI2;
	float BI2;
	float KI3;
	float BI3;
}GROUP_MEA_CAL;


typedef struct
{
	uint16_t DevAddr;
	uint16_t UsartBaudRate;
	uint16_t CurrentDiverterPara;
}GROUP_SYS_PARA;


typedef struct
{
	uint16_t DevStat;
}GROUP_SYS_STAT;


typedef struct
{
	GROUP_MEA_DATA MeaData;
	SYS_MEA_DATA SysMeaData;
	GROUP_MEA_CAL	MeaCal;
	GROUP_SYS_PARA	SysPara;
	GROUP_SYS_STAT	SysStat;
}S_GROUP;


typedef  struct 
{
	uint8  CommErrStatus;  
	uint8  CommErrCount;
	uint32 CommErrStartTime;

	S_BAT_DATA BatData[MAX_GROUP_NUM];         /*两组电池数据*/
	S_STATUS_DATA StatusData;	        /*两组电池状态数据*/
	S_SYSTEM_PARA SysPara;	        /*系统参数*/
	S_HISTORY_RECORD HistoryRecord;   /*历史记录*/
	S_GROUP	Group[MAX_GROUP_NUM];
	S_COM_TEST 		ComTest;
}MBS_SLAVE;

/*读取的数据段地址定义 */
typedef enum
{
	DEV_NON_SECT = 0,                                /*无效数据段*/
	
}DEV_SECT_ADDR_DEF;

extern MBS_SLAVE MbsSlave;

/********************分机串口处理使用数据结构****************/
#define  DEVCOMM_FAIL_NUM        251                             /*通讯失败最大次数*/
#define  BROADCAST_ADDR          0xFF                           /*广播地址*/
#define  RCV_QUEUE_SIZE          256
typedef struct
{
	uint8 	*p_head;                                            /* 串口数据接收地址 */
	uint8 	*p_tail;                                            /* 串口数据已处理地址 */	
	uint8   FailCount;                                          /* 通讯失败计数 */
	uint8 	rcvbuf[RCV_QUEUE_SIZE];                             /* 串口数据缓存,环形存储 */
}UART_RCV_CFG;

extern UART_RCV_CFG     UartRcvCfg[5];                          /*用于处理已接收串口数据*/

extern void MbsSlaveProcess(uint8 com);



extern const SECT_ADDR_AND_DATA SSectAddrAndData[S_MAX_SECT + 1];

/*************************************************************/
/*                functions def                              */
/*************************************************************/


#ifdef  __cplusplus
}
#endif
#endif

/**************************************************************************************************
**                            End Of File
****************************************************************************************************/

