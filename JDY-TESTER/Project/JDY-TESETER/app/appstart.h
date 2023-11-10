/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
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
*       ����汾˵��                                                *
* ��ʽ�����汾����汾����ż����β,δ�����Ի��ȶ��汾��������β   *

1.0.5---����������������͸�ģ�飬�Խ���¶�̽ͷû���������,�޸ı����ָ�������
1.0.6---�����ָ������죬����ȼ�Ĭ������Ϊ15%	//20180427
1.0.7---�������Ե�MODBUS��̨ͨ�Ź��ܣ���δ���ⷢ��,�������ʲôׯһ�ݣ�����ģ������ʱ�����ù��ܣ�û�в�������ʱ��������״̬
1.0.8---�����豸���ϸɽӵ�������ܣ���ع�Ƿѹ���������䳣�������ͨ�Ź��ϳ������������mbs��̨ͨ�Ź���
1.1.0---������ϿҪ���޸İ汾
1.1.1---������Ͽ���󣬵ڶ����޸�,20190224
1.1.2---���ѹǷѹ���������죬�����������������¶�̽ͷ,ʹ�ô���ת����������������
1.1.3---����������3000Bͨ�Ź���
1.1.4---������Զ������������⣬����������ʱ������
*********************************************************************/
#define HW_MAIN_VERSION_NO		1	//Ӳ�����汾��
#define HW_SEC_VERSION_NO		2	//Ӳ���ΰ汾��
#define HW_REVISION_NO			0	//Ӳ���޶���
#define SW_MAIN_VERSION_NO		1	//������汾��
#define SW_SEC_VERSION_NO		1	//����ΰ汾��
#define SW_REVISION_NO			4	//����޶���//�����¶�̽ͷID���ù���
#define HARDWARE_VERSION     (HW_MAIN_VERSION_NO<<13)|(HW_SEC_VERSION_NO<<8)|HW_REVISION_NO             /* Ӳ���汾�� */
#define SOFTWARE_VERSION     (SW_MAIN_VERSION_NO<<13)|(SW_SEC_VERSION_NO<<8)|SW_REVISION_NO          /* ����汾�� */
/********************************************************************/

/********************************************************************
*                    restore license                                 *
********************************************************************/

//#define _RESTOREPARA__			//�ָ����ò���
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
#define CURRENTCHANNEL1				0//Ĭ����û�е�����������
#define CURRENTCHANNEL2				0//Ĭ����û�е�����������

#define AUTO_TEST_TIME				9	//�Զ������������ʱ��ѡ������ŵ�

#define BAT_V_SHIELD_SUM			5	//���ε������

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
	BOOL LED_POWER;				//��Դ��
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

