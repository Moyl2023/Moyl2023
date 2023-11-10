#ifndef __LCDCOMM_H
#define __LCDCOMM_H
#include "config.h"

/////////////图片区地址定义/////////////////
typedef enum
{
   PICTURE0 = 0,  // 启动界面
   PICTURE1,
   PICTURE2,


}PICTURE;


typedef enum
{
	FIRST_BUTTON = 0,

	FINAL_BUTTON,
}LCD_BUTTON;

/////////////屏变量首地址/////////////////
#define LCD_BAT_VOLTAGE_DATA_ADDR	0x0000
#define LCD_BAT_R_DATA_ADDR			0x0100
#define LCD_TREND_R_DISPLAY_ADDR		0x0124
#define LCD_TREND_R_DATA_ADDR			0x0138
#define LCD_BAT_CR_DATA_ADDR			0x0200
#define LCD_BAT_TEMP_DATA_ADDR		0x0300
#define LCD_BAT_ERR_STAT_ADDR			0x0800
#define LCD_DEV_STAT_ADDR				0x0B00
#define LCD_MEA_DATA_ADDR				0x0C00
#define LCD_CAL_PARA_ADDR				0x0C50
#define LCD_SYS_WORK_STAT_ADDR		0x1000
#define LCD_ASSIST_STAT_ADDR			0x1010
#define LCD_SYS_CTRL_ADDR				0x1020
#define LCD_SYS_CFG_ADDR				0x1040
#define LCD_SYS_TIME_ADDR				0x10A0
#define LCD_COM_PARA_ADDR				0x10B0
#define LCD_HISTORY_RECORD_ADDR		0x4000
#define LCD_USART_TEST_ADDR			0x5000


#define LCD_R_PATROL_ADDR				0x1023		//内阻巡检
#define LCD_EQ_PATROL_ADDR			0x1024		//电池均衡
#define LCD_PARA1_SET_ADDR				0x104E		//出厂前系统配置的起始地址
#define LCD_PARA2_SET_ADDR				0x1060		//出厂前系统配置的起始地址
#define LCD_PARA_SET_NUM				5

#define LCD_DEV_ERR_ADDR				LCD_DEV_STAT_ADDR+7
#define LCD_CAN_PARA_ADDR				LCD_COM_PARA_ADDR
#define LCD_COM1_PARA_ADDR			LCD_CAN_PARA_ADDR+6
#define LCD_COM2_PARA_ADDR			LCD_CAN_PARA_ADDR+12
#define LCD_COM3_PARA_ADDR			LCD_CAN_PARA_ADDR+18
#define LCD_COM4_PARA_ADDR			LCD_CAN_PARA_ADDR+24


#define BAT_GROUP_NO_ADDR					0x201C		//当前显示的电池组编号
#define DEV_NO_ADDR						0x201D		//当前页面上显示的模块编号


#define ERR_TOTAL_NUM_ADDR				0x0800		//故障数据总条数在LCD上显示地址
#define ERR_PAGE_NUM_ADDR					0x0801		//故障数据当前页数LCD上显示地址
#define ERR_INFO_NUM1_ADDR				0x0810		//故障数据当前页数LCD上显示地址

#define SYS_TIME_DISPLAY_ADDR				0x2500		//系统时间在LCD上显示的地址


#define HISTORY_TOTAL_NUM_ADDR			0x4000		//历史记录总条数显示地址
#define HISTORY_PAGE_NUM_ADDR				0x4001		//历史记录当前显示的页数地址
/////////////寄存器数量/////////////////


#define LCD_COM_PARA_NUM					0x04	//每个通信通道的寄存器数量


/////////////按键区地址定义/////////////////
#define	FIRST_BUTTON						0x2000			//LCD上的按键第一个的起始地址，
#define LCD_BUTTON_MAX_ADDR				0x2900			//LCD上的按键最大地址，方便计算段空间
#define BUTTON_RETU_VALUE					0x0200			//按键返回值



/*************************************************************/
/***********保留一些寄存器位置，留给系统配置进行扩展***********/
/*************************************************************/
#define LCD_COLOUR	0x4500
typedef enum
{
	DISP_GREEN = 0x8010,
	DISP_RED = 0xF800,
	DISP_BLUE = 0x001F,
	DISP_YELLOW = 0xFFE0,
	DISP_BLACK = 0x0000,
}DISP_COLOUR;
typedef struct
{
	uint16 VarAddr;
	uint16 DispPosition;
	uint16 Colour;
}DATA_VAR_DISP;//数据变量显示

typedef struct
{
	uint16 VarAddr;
	uint16 Range;
	uint16 BorderColour;//边框色
	uint16 ForeColour;//前景色
}PROGRESS_BAR_DISP;//数据变量显示

#define V_DATA_VAR_DISP1	LCD_COLOUR
#define V_DATA_VAR_DISP2	(LCD_COLOUR + 0x10)
#define V_DATA_VAR_DISP3	(LCD_COLOUR + 0x20)
#define V_DATA_VAR_DISP4	(LCD_COLOUR + 0x30)
#define V_DATA_VAR_DISP5	(LCD_COLOUR + 0x40)
#define V_DATA_VAR_DISP6	(LCD_COLOUR + 0x50)
#define V_DATA_VAR_DISP7	(LCD_COLOUR + 0x60)
#define V_DATA_VAR_DISP8	(LCD_COLOUR + 0x70)
#define V_DATA_VAR_DISP9	(LCD_COLOUR + 0x80)
#define V_DATA_VAR_DISP10	(LCD_COLOUR + 0x90)
#define V_DATA_VAR_DISP11	(LCD_COLOUR + 0xA0)
#define V_DATA_VAR_DISP12	(LCD_COLOUR + 0xB0)
#define V_DATA_VAR_DISP13	(LCD_COLOUR + 0xC0)
#define V_DATA_VAR_DISP14	(LCD_COLOUR + 0xD0)
#define V_DATA_VAR_DISP15	(LCD_COLOUR + 0xE0)
#define V_DATA_VAR_DISP16	(LCD_COLOUR + 0xF0)
#define V_DATA_VAR_DISP17	(LCD_COLOUR + 0x100)
#define V_DATA_VAR_DISP18	(LCD_COLOUR + 0x110)//0x4610

#define T_DATA_VAR_DISP1	(V_DATA_VAR_DISP18 + 0x10)
#define T_DATA_VAR_DISP2	(T_DATA_VAR_DISP1 + 0x10)
#define T_DATA_VAR_DISP3	(T_DATA_VAR_DISP1 + 0x20)
#define T_DATA_VAR_DISP4	(T_DATA_VAR_DISP1 + 0x30)
#define T_DATA_VAR_DISP5	(T_DATA_VAR_DISP1 + 0x40)
#define T_DATA_VAR_DISP6	(T_DATA_VAR_DISP1 + 0x50)
#define T_DATA_VAR_DISP7	(T_DATA_VAR_DISP1 + 0x60)
#define T_DATA_VAR_DISP8	(T_DATA_VAR_DISP1 + 0x70)
#define T_DATA_VAR_DISP9	(T_DATA_VAR_DISP1 + 0x80)
#define T_DATA_VAR_DISP10	(T_DATA_VAR_DISP1 + 0x90)
#define T_DATA_VAR_DISP11	(T_DATA_VAR_DISP1 + 0xA0)
#define T_DATA_VAR_DISP12	(T_DATA_VAR_DISP1 + 0xB0)
#define T_DATA_VAR_DISP13	(T_DATA_VAR_DISP1 + 0xC0)
#define T_DATA_VAR_DISP14	(T_DATA_VAR_DISP1 + 0xD0)
#define T_DATA_VAR_DISP15	(T_DATA_VAR_DISP1 + 0xE0)
#define T_DATA_VAR_DISP16	(T_DATA_VAR_DISP1 + 0xF0)
#define T_DATA_VAR_DISP17	(T_DATA_VAR_DISP1 + 0x100)
#define T_DATA_VAR_DISP18	(T_DATA_VAR_DISP1 + 0x110)//0x4720

#define R_DATA_VAR_DISP1	(T_DATA_VAR_DISP18 + 0x10)
#define R_DATA_VAR_DISP2	(R_DATA_VAR_DISP1 + 0x10)
#define R_DATA_VAR_DISP3	(R_DATA_VAR_DISP1 + 0x20)
#define R_DATA_VAR_DISP4	(R_DATA_VAR_DISP1 + 0x30)
#define R_DATA_VAR_DISP5	(R_DATA_VAR_DISP1 + 0x40)
#define R_DATA_VAR_DISP6	(R_DATA_VAR_DISP1 + 0x50)
#define R_DATA_VAR_DISP7	(R_DATA_VAR_DISP1 + 0x60)
#define R_DATA_VAR_DISP8	(R_DATA_VAR_DISP1 + 0x70)
#define R_DATA_VAR_DISP9	(R_DATA_VAR_DISP1 + 0x80)
#define R_DATA_VAR_DISP10	(R_DATA_VAR_DISP1 + 0x90)
#define R_DATA_VAR_DISP11	(R_DATA_VAR_DISP1 + 0xA0)
#define R_DATA_VAR_DISP12	(R_DATA_VAR_DISP1 + 0xB0)
#define R_DATA_VAR_DISP13	(R_DATA_VAR_DISP1 + 0xC0)
#define R_DATA_VAR_DISP14	(R_DATA_VAR_DISP1 + 0xD0)
#define R_DATA_VAR_DISP15	(R_DATA_VAR_DISP1 + 0xE0)
#define R_DATA_VAR_DISP16	(R_DATA_VAR_DISP1 + 0xF0)
#define R_DATA_VAR_DISP17	(R_DATA_VAR_DISP1 + 0x100)
#define R_DATA_VAR_DISP18	(R_DATA_VAR_DISP1 + 0x110)//0x4830

#define CR_DATA_VAR_DISP1	(R_DATA_VAR_DISP18 + 0x10)
#define CR_DATA_VAR_DISP2	(CR_DATA_VAR_DISP1 + 0x10)
#define CR_DATA_VAR_DISP3	(CR_DATA_VAR_DISP1 + 0x20)
#define CR_DATA_VAR_DISP4	(CR_DATA_VAR_DISP1 + 0x30)
#define CR_DATA_VAR_DISP5	(CR_DATA_VAR_DISP1 + 0x40)
#define CR_DATA_VAR_DISP6	(CR_DATA_VAR_DISP1 + 0x50)
#define CR_DATA_VAR_DISP7	(CR_DATA_VAR_DISP1 + 0x60)
#define CR_DATA_VAR_DISP8	(CR_DATA_VAR_DISP1 + 0x70)
#define CR_DATA_VAR_DISP9	(CR_DATA_VAR_DISP1 + 0x80)
#define CR_DATA_VAR_DISP10 	(CR_DATA_VAR_DISP1 + 0x90)
#define CR_DATA_VAR_DISP11 	(CR_DATA_VAR_DISP1 + 0xA0)
#define CR_DATA_VAR_DISP12	  	(CR_DATA_VAR_DISP1 + 0xB0)
#define CR_DATA_VAR_DISP13		(CR_DATA_VAR_DISP1 + 0xC0)
#define CR_DATA_VAR_DISP14		(CR_DATA_VAR_DISP1 + 0xD0)
#define CR_DATA_VAR_DISP15		(CR_DATA_VAR_DISP1 + 0xE0)
#define CR_DATA_VAR_DISP16		(CR_DATA_VAR_DISP1 + 0xF0)
#define CR_DATA_VAR_DISP17		(CR_DATA_VAR_DISP1 + 0x100)
#define CR_DATA_VAR_DISP18		(CR_DATA_VAR_DISP1 + 0x110)//0x4940

#define DEV_T_EVR_DISP 		(CR_DATA_VAR_DISP18+0x10)//0x4980

/*********************系统配置寄存器**************************/
typedef struct
{
	uint16_t LCD_GroupVoltageMax;							/*整组电压上限*/
	uint16_t LCD_GroupVoltageMin;							/*整组电压下限*/
	uint16_t LCD_ChargeCurrentMax;							/*充电电流上限*/
	uint16_t LCD_DischargeCurrentMax;						/*放电电流上限*/
	uint16_t LCD_SingleVoltageMax;							/*单体电压上限*/
	uint16_t LCD_SingelVoltageMin;							/*单体电压下限*/
	uint16_t LCD_SingleCellRMax;							/*单体电池内阻上限*/
	uint16_t LCD_ConnectRMax;								/*连接条内阻上限*/
	int16_t LCD_TempMax;									/*温度上限*/
	int16_t LCD_TempMin;									/*温度下限*/
	uint16_t LCD_AutoMeaRTime;								/*自动测量内阻间隔*/
}LCD_GROUP_CFG;

typedef struct
{
	uint16_t LCD_CellGroupNum;								/*蓄电池组数量*/
	uint16_t LCD_CellNum1;									/*1组蓄电池数量*/
	uint16_t LCD_CellNum2;									/*2组蓄电池数量*/
	LCD_GROUP_CFG LcdGroupCfg[MAX_GROUP_NUM];
	uint16_t LCD_GpsTimeCalEN;								/*使能GPS对时*/
	uint16_t LCD_HostHWVersion;								/*主板硬件版本号*/
	uint16_t LCD_HostSWVersion;								/*主板软件版本号*/
}LCD_SYS_CFG;


/*********************系统时钟寄存器**************************/
typedef struct
{
	uint16_t Year;												/*年*/
	uint16_t Month;												/*月*/
	uint16_t Day;													/*日*/
	uint16_t Hour;												/*时*/
	uint16_t Minute;												/*分*/
	uint16_t Second;												/*秒*/
}LCD_SYS_TIME;


/*********************系统控制寄存器**************************/
typedef struct
{
	uint16_t LCD_StartCellMea;								/*启动电池巡检*/
	uint16_t LCD_StartConnectMea;							/*启动连接条巡检*/
	uint16_t LCD_StartTempMea;								/*启动温度巡检*/
	uint16_t LCD_StartInterMea;								/*启动内阻测量*/
}LCD_SYS_CTRL;

/*********************校准参数寄存器**************************/
typedef struct
{
	fp32 LCD_CellVoltageB;									/*蓄电池电压采集通道B值*/
	fp32 LCD_CellVoltageK;									/*蓄电池电压采集通道K值*/
	fp32 LCD_ConnectVoltageB;								/*连接条电压采集通道B值*/
	fp32 LCD_ConnectVoltageK;								/*连接条电压采集通道K值*/
	fp32 LCD_CurrentB;											/*放电电流电压采集通道B值*/
	fp32 LCD_CurrentK;											/*放电电流电压采集通道K值*/
}LCD_CAL_PARA;


/*********************通讯参数寄存器**************************/
#define LCD_MAX_USART_NUM 5

typedef struct
{
	struct
	{
		uint16_t BaudRate;									/*波特率*/
		uint16_t ComAddr;										/*本端口地址*/
		uint16_t SlaveAddr;									/*从机地址*/
		uint16_t Reserve1;
		uint16_t Reserve2;
		uint16_t Reserve3;	
	}CanPara;
	struct
	{
		uint16_t BaudRate;									/*波特率*/
		uint16_t ComAddr;										/*本端口地址*/
		uint16_t ComProtocal;								/*通讯协议*/
		uint16_t Purpose;										/*用途*/
		uint16_t Reserve1;
		uint16_t Reserve2;
	}UsartPara[LCD_MAX_USART_NUM];							
}LCD_COM_PARA;
typedef	struct
{
	int16_t TotalPageNUM;		//故障数据总页数
	int16_t ErrDatePageNO;		//当前的显示的故障数据页数
}ERR_DATA;


typedef	struct
{
	int16_t TotalPageNUM;			//历史记录总页数
	int16_t HisRecordPageNO;		//当前的显示的历史记录页数
	
}HIS_RCD;


typedef struct
{
	ERR_DATA ErrData;
	HIS_RCD HisRcd;
}ERR_AND_HISTORY;



typedef struct
{
	uint16 HisMeaNOFlag[MAX_GROUP_NUM];			//显示第N次历史测量信息标志
	uint16 MeaYear;								//测试时间-年
	uint16 MeaMon;								//测试时间-月
	uint16 MeaDay;								//测试时间-日
	uint16 MeaHour;								//测试时间-时
	uint16 MeaMin;								//测试时间-分
	uint16 MeaSec;								//测试时间-秒
	uint16 HisInnerRData[MAX_BAT_NUM];	//内阻数据
	uint16 HisCRData[MAX_BAT_NUM];		//连接条数据
	uint16 HisVoltageData[MAX_GROUP_BAT_NUM];	//电压数据
	int16 HisTempData[MAX_GROUP_BAT_NUM];		//温度数据
}HIS_MEA_DATA;

#define NOUSBDEVICE 	0			//无USB设备
#define DEVICEATTACH	1			//有USB设备

typedef enum
{
	NODATAOUTPUT=0,			//USB没有输出数据
	DATAOUTPUTTOUSB=1,			//USB输出数据中
  DATAOUTPUTOVER=2,			//USB输出数据完成
  NODATA=3,					//无测量记录
}DATA_OUTPUT_STAT;

typedef struct
{
	uint8 UsbDeviceFlag;			//有无USB设备标识
	uint8 UsbOutputData;			//有无USB数据输出标识
	uint8 UsbOutReportData;			//导出测试报告
	uint8 OutputDataProgress;		//导出数据进度
	uint8 VOutputFlag;				//电压数据导出标识
	uint8 DiscReportOutput;				//导出放电测试报告
	uint8 DiscDataOutFlag;				//放电实时数据导出
	uint8 BatVCurrentOutput;			//电池电压导出单次
	uint16 OutputTime;				//导出时间间隔，单位为分钟
	uint32 DataOutputTimeFlag;		//上一次数据导出时间
	
}USB_DEVICE;

typedef struct
{
	uint16 SaveAddrOffset;//最近一次存储位置
	uint16 TotalSaveTimes;//总共存储次数
	uint16 DiscSaveAddrOffset;//最近一次存储位置
	uint16 TotalDiscSaveTimes;//放电数据总共存储次数
}BAT_DATA_SAVE;


typedef struct
{
	uint16 RYear;
	uint16 RMon;
	uint16 RDay;
	uint16 RHour;
	uint16 RMin;
	uint16 RSec;
}TEST_TIME;

typedef struct
{
   uint16 MeaYear;								//最近测试时间-年
	uint16 MeaMon;								//最近测试时间-月
	uint16 MeaDay;								//最近测试时间-日
	uint16 MeaHour;								//最近测试时间-时
	uint16 MeaMin;								//最近测试时间-分
	uint16 MeaSec;								//最近测试时间-秒
}INNER_R_TEST_TIME;

typedef enum
{
	TEST_IDLE=0,
	TEST_V_LOW,//电池欠压
	TEST_GROUP_V_OVER,//电池组过压
	TEST_GROUP_V_LOW,//电池组欠压
	TEST_DISCHARGE,//电池放电中
	TEST_BUSY,//测试频繁
	TEST_NO_FUNCTION,//无内阻测量功能
}TEST_CONDITION;


typedef struct
{
   uint8 InnerRTestDevNO;	//当前内阻测试的模块编号
   uint8 InnerRTestFlag[MAX_GROUP_NUM];	//蓄电池组内阻测试标志，2表示测试完成，1表示测试中,0表示测试终止，3表示测试失败，4表示放电板异常，5表示频繁
   uint8 LastTestTimesFlag[MAX_GROUP_NUM];//蓄电池组最近一次内阻的测试存储位置,该变量在每次进入内阻信息界面时显示该次测量信息
   uint8 TotalTestTimes[MAX_GROUP_NUM];//总测量次数
   uint8 RDataReadFlag[MAX_GROUP_NUM][MAX_DEV_NUM];//内阻读写标志
   uint8 CRDataReadFlag[MAX_GROUP_NUM][MAX_DEV_NUM];//连接条读写标志
   uint8 TestConditionFlag[MAX_GROUP_NUM];//内阻测试条件标志
   uint16 TestTimePeriod[MAX_GROUP_NUM];//模块之间的测量时间间隔，单位为秒
   uint16 TestFailFlag[MAX_GROUP_NUM];	//内阻测试失败标识，用来报警提示使用，断电重启后或测试成功后恢复，高四位是组号，再四位是巡检模块编号，后八位是失败原因
	INNER_R_TEST_TIME TestTime[MAX_GROUP_NUM];
}INNER_TEST;

#define DISCHARGE_CURRENT	-1.5
#define FLOAT_CHARGE_CURRENT	1
#define EQ_CHARGE_CURRENT	1


typedef enum
{
	IDLE=0,//开路
	BATFLOATCHARGE,//浮充
	BATEQCHARGE,//均充
	BATDISCHARGE,//放电
}BAT_STAT;

typedef enum
{
	DIS_NORMAL = 0,//放电正常
	GROUPVLOW,//组电压欠压
	BATVLOW,//电池电压欠压
}DIS_STAT_FLAG;

//放电状态
typedef struct
{
	uint8 DisChargeFlag;

}DIS_STAT;

typedef struct
{
	uint16 BatStat[MAX_GROUP_NUM];//均充，浮充，放电
	float GroupV[MAX_GROUP_NUM];//组电压
	float GroupI[MAX_GROUP_NUM];//充放电电流
	uint16 SingleBatMaxV[MAX_GROUP_NUM][5];//单体最大电压
	uint16 SingleBatMinV[MAX_GROUP_NUM][5];//单体最小电压
	uint16 MaxVNO[MAX_GROUP_NUM][5];//最大电压编号
	uint16 MinVNO[MAX_GROUP_NUM][5];//最小电压编号
	uint16 BatMaxR[MAX_GROUP_NUM];//最大内阻
	uint16 BatMinR[MAX_GROUP_NUM];//最小内阻
	uint8 MaxRNO[MAX_GROUP_NUM];//最大内阻编号
	uint8 MinRNO[MAX_GROUP_NUM];//最小内阻编号
	uint16 BatMaxCR[MAX_GROUP_NUM];//最大内阻
	uint16 BatMinCR[MAX_GROUP_NUM];//最小内阻
	uint8 MaxCRNO[MAX_GROUP_NUM];//最大内阻编号
	uint8 MinCRNO[MAX_GROUP_NUM];//最小内阻编号
	float BatMaxTemp[MAX_GROUP_NUM];//单体最大温度
	float BatMinTemp[MAX_GROUP_NUM];//单体最小温度
	uint16 MaxTempNO[MAX_GROUP_NUM];//最大温度编号
	uint16 MinTempNO[MAX_GROUP_NUM];//最小温度编号
	float MaxTempDev[MAX_GROUP_NUM];//最大环境温度
	uint16 MaxTempDevNO[MAX_GROUP_NUM];//最大环境温度编号
	float MinTempDev[MAX_GROUP_NUM];//最小环境温度
	uint16 MinTempDevNO[MAX_GROUP_NUM];//最小环境温度编号
}BAT_SYS_STAT;

typedef struct
{
	uint8 BatEQFlag[MAX_GROUP_NUM];//蓄电池组均衡标志，0表示禁止均衡，1表示启动均衡
}BAT_EQ;//电池均衡结构体

typedef struct
{
	int32_t VoltageAD; //AD值
	int32_t Current1AD;
	int32_t Current2AD;
	int32_t Current3AD;
}CLI_AD;

typedef struct
{
	float VoltageValue; //实际值
	float Current1Value;
	float Current2Value;
	float Current3Value;
}CLI_VALUE;

typedef struct
{
	uint8 ADReadTimes;			// 1表示校准点1的AD值读取，2表示校准点2的AD值读取
	CLI_AD CliAd[2];
	CLI_VALUE CliValue[2];
}GROUP_CLI_INFO;



typedef enum
{
	R_TEST_STOP = 0,//测试终止
	R_TEST_ING = 1,//测试中
	R_TEST_FAIL = 2,//测试失败
	R_TEST_SUCCESS = 3,//测试完成
	DIS_MODULE_ERR = 4,//放电板异常
	R_TEST_BUSY = 5,//测试频繁
	DATA_SAVING = 6,//数据保存中
	DATA_SAVE_SUCCESS = 7,//数据保存成功
	DATA_SAVE_FAIL = 8,//数据保存失败
	R_TEST_PAUSE = 9,//测试暂停中
	R_TEST_FAIL_I_LOW = 0x21,//电流过小
	R_TEST_FAIL_I_OVER = 0x22,//电流过大
	R_TEST_FAIL_STOP = 0x23,//手动停止
	R_TEST_FAIL_V_LOW = 0x24,//组电压过小
	R_TEST_FAIL_V_OVER = 0x25,//组电压过大
	R_TEST_FAIL_BATV_LOW = 0x26,//电池电压过低
	R_TEST_FAIL_BATV_OVER = 0x27,//电池电压过高
	R_TEST_FAIL_TEMP_LOW = 0x28,//温度过低，或温感故障，为0xFAFA
	R_TEST_FAIL_TEMP_OVER = 0x29,//温度过高
}R_TEST_STAT;

//typedef struct
//{
	//float Soc;		//电池剩余容量
	//float Soh;		//健康状态
	//float Sof;		//功能状态
//}BAT_STATE;

#define DIS_MODULE_OFF		0
#define DIS_MODULE_ON_2V	1
#define DIS_MODULE_ON_12V	2

typedef enum
{
	SAVE_IDLE = 0,
	SAVING,
	SAVE_SUCCESS,
	SAVE_FAIL,
}SAVE_STAT;


/*********************屏通信协议功能码**************************/
typedef enum
{
	FRAME_HIGH = 0x5A,
	FRAME_LOW  = 0xA5,
}FRAME;
typedef	enum
{
   CMD_WRITE_DATA_FORCE = 0x12,
   CMD_SWITCH_PIC  = 0x80,
   CMD_READ_PICNUM = 0x81,
   CMD_WRITE_DATA  = 0x82,
   CMD_READ_DATA   = 0x83,
}TOUCH_CMD; 

typedef enum
{
    LCD_RCV_UNDONE = 0,                                 /* 接收未完成 */
    LCD_RCV_FUCTION_ERR = 1,                            /* 接收命令码错误 */
    LCD_RCV_REG_ADD_ERR = 2,                            /* 接收地址错 */
    LCD_RCV_REG_NUM_ERR = 3,                            /* 接收数量错 */	
    LCD_RCV_REG_DATA_ERR = 4,                           /* 接收数据错 */	
    LCD_RCV_REG_DATA_SAVE_ERR = 5, 			/* 铁电存储错 */	
    LCD_RCV_REG_DATA_READ_ERR = 6, 			/* 铁电存储错 */
	
    LCD_RCV_COMM_ADD_ERR = 14,                          /* 接收通讯地址错 */
    LCD_RCV_LEN_ERR = 15,                               /* 接收长度错 */
    LCD_RCV_CRC_ERR = 16,                               /* 接收CRC错 */
    LCD_RCV_OK = 0xff,                                  /* 接收正常 */
	
}LCD_ERROR_CODE;


/********************分机串口处理使用数据结构****************/
#define  LCDCOMM_FAIL_NUM        30                             /*通讯失败最大次数*/
#define  BROADCAST_ADDR          0xFF                           /*广播地址*/
#define  LCD_RCV_QUEUE_SIZE          256


void LcdProcess(uint8 com);
uint8 CalAndSendData(uint8 com,uint8 *txbuf,uint8 txlen);
static uint8 LcdFillTransData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth);
static uint8 LcdFillTransData32(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth);
uint8 SwitchToPicX(uint8 com,uint8 cmd, uint16 pageN);
uint8 PackedAndSendData(uint8 com,uint16 pageno, uint8 cmd);
 uint8 DealScreenData(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum);
 void DealScreenCmd(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum); 
 uint8 SendGroupAndDevNO(uint8 com);
 uint8 LcdCmdCheck(uint16 cmd,uint8 pageno);
uint8 CheckCfgPara(void);
uint8 CountBatRStat(uint8 group);
uint8 CountBatCRStat(uint8 group);
uint8 UsbDeviceFlag(uint8 com);
 
#endif

