/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: appstart.h
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions: 
****************************************************************************************/
#ifndef _Appstart_H
#define _Appstart_H

#ifdef  __cplusplus
extern  "C" {																				    
#endif

#include "config.h"

/********************************************************************
*        SWITCH                                                     *
*********************************************************************/


/********************************************************************
*       程序版本说明                                                *
* 正式发布版本软件版本号以偶数结尾,未经测试或不稳定版本以奇数结尾   *

1.0.5---将电池组电池数量发送给模块，以解决温度探头没接满的情况,修改报警恢复后不再响
1.0.6---报警恢复后不再响，均衡等级默认设置为15%	//20180427
1.0.7---增加南自的MODBUS后台通信功能，暂未对外发布,发给天津什么庄一份，增加模块间测量时间设置功能，没有测量电流时屏蔽蓄电池状态
1.0.8---增加设备故障干接点输出功能，电池过欠压、反接脱落常开输出，通信故障常闭输出，完善mbs后台通信功能
1.1.0---根据三峡要修修改版本
1.1.1---根据三峡需求，第二次修改,20190224
1.1.2---组电压欠压蜂鸣器不响，可以配置三个环境温度探头,使用串口转蓝牙进行升级程序
1.1.3---增加与监控器3000B通信功能
1.1.4---解决不自动测量内阻问题，内阻测量间隔时间增大
*********************************************************************/
#define HW_MAIN_VERSION_NO		1	//硬件主版本号
#define HW_SEC_VERSION_NO		2	//硬件次版本号
#define HW_REVISION_NO			0	//硬件修订号
#define SW_MAIN_VERSION_NO		1	//软件主版本号
#define SW_SEC_VERSION_NO		1	//软件次版本号
#define SW_REVISION_NO			4	//软件修订号//增加温度探头ID设置功能
#define HARDWARE_VERSION     (HW_MAIN_VERSION_NO<<13)|(HW_SEC_VERSION_NO<<8)|HW_REVISION_NO             /* 硬件版本号 */
#define SOFTWARE_VERSION     (SW_MAIN_VERSION_NO<<13)|(SW_SEC_VERSION_NO<<8)|SW_REVISION_NO          /* 软件版本号 */
/********************************************************************/

/********************************************************************
*                    restore license                                 *
********************************************************************/

//#define _RESTOREPARA__			//恢复配置参数
/********************************************************************
*                    CONST DEF                                      *
********************************************************************/
#define SYS_LICENSE			"123456"
#define FACTORY_LICENSE		"654321"
#define BAT5000_S_ADDR		0xFB
/********************************************************************
*                    DEFAULT PARA                                      *
********************************************************************/
#define CELLGROUPNUMVALUE			1
#define CELLNUMVAULE1				104
#define GROUPVMAXVALUE1				260
#define	GROUPVMINVALUE1				180	
#define	CHARGECURRENTMAXVALUE1		50
#define DISCHARGECURRENTMAXVALUE1	40
#define SINGEVMAXVALUE1				2600
#define SINGLEVMINVALUE1			1500
#define SINGLECELLRMAXVALUE1		3000
#define CONNECTRMAXVALUE1			1000
#define TEMPMAXVALUE1				85 
#define TEMPMINVALUE1				-20
#define AUTOMEARTIMEVALUE1			30
#define EQPARACFG1					20
#define BATVLEVEL1					0
#define INNERRDEVSTAT1				OFF
#define CRDEVSTAT1					OFF
#define EQDEVSTAT1					OFF
#define TEMPDEVSTAT1				OFF
#define CELLNUMVAULE2				0
#define GROUPVMAXVALUE2				260
#define	GROUPVMINVALUE2				180
#define	CHARGECURRENTMAXVALUE2		50
#define DISCHARGECURRENTMAXVALUE2	40
#define SINGEVMAXVALUE2				2600
#define SINGLEVMINVALUE2			1500
#define SINGLECELLRMAXVALUE2		3000
#define CONNECTRMAXVALUE2			1000
#define TEMPMAXVALUE2				85
#define TEMPMINVALUE2				-20
#define AUTOMEARTIMEVALUE2			30
#define EQPARACFG2					20
#define BATVLEVEL2					0
#define INNERRDEVSTAT2				OFF
#define CRDEVSTAT2					OFF
#define EQDEVSTAT2					OFF
#define TEMPDEVSTAT2				OFF
#define CURRENTCHANNEL1				0//默认是没有电流测量功能
#define CURRENTCHANNEL2				0//默认是没有电流测量功能

#define AUTO_TEST_TIME				9	//自动进行内阻测量时，选择上午九点

#define BAT_V_SHIELD_SUM			5	//屏蔽电池数量

/********************************************************************
*                    COMM DEF                                       *
********************************************************************/

/********************************************************************
*                   FUNCTION DEF                                    *
********************************************************************/
#define  RSTWDT()    ToggleGPIORev(&WDT);         
/********************************************************************
*                   DATA STRUCT DEF                                 *
********************************************************************/

#define OFF 0
#define ON 	1

typedef struct
{
	BOOL LED_POWER;				//电源灯
	uint32 LedPowerTimeFlag;
}LED_FLAG;


typedef struct 
{
    float KmVolt;
    float TestR;
	



	
}DATA_YC;

typedef struct 
{
    uint8 WorkStat;
	uint8 OldWorkStat;

	
}DATA_YX;

typedef struct 
{
	uint32 PictureNO;
	uint32 OldPictureNO;



	uint8 SystemReset;

}DATA_YK;

typedef struct 
{
    uint8 para;
}DATA_DZ;

typedef struct 
{
		uint32 JdyTestTimeFlag;
	uint32 UpdateTimeFlag;
	uint32 LcdTimeFlag;
	uint32 RstTimeFlag;
    uint8 assdata;
}DATA_ASS;

typedef struct
{
    uint32 PageOperateTimeFlag;
}LCD_DATA;

typedef struct 
{
	DATA_YC DataYc;
	DATA_YX DataYx;
	DATA_YK DataYk;
	DATA_DZ DataDz;
    DATA_ASS DataAss;
	LCD_DATA Lcd; 
		uint32 g_TimerMs;
	uint32 g_TimerS;
	
	
	uint16 HWVersion;
	uint16 SWVersion;
}JDYTEST;




/********************************************************************
*                  EXTERN  DEF                                      *
********************************************************************/

extern OS_STK	StackUpdateCom[APP_TASK_UpdateCOM_STK_SIZE];

extern OS_STK	StackLCD[APP_TASK_LCD_STK_SIZE];

extern OS_STK	StackJdyTest[APP_TASK_JDYTEST_STK_SIZE];


extern JDYTEST JdyTest;
//extern uint8 LCDCOM;

void ReCreate(uint8 prio);
void InitUartTask(uint8 com);
void CheckTaskState(void);

#ifdef  __cplusplus
}
#endif
#endif

