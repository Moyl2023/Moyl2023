/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsMaster.h
** Created By: 
** Last modified Date:  
** Last Version: 1.0
** Descriptions:    Modbus��̨ͨѶ���ݽṹ
****************************************************************************************/
#ifndef _MBSMASTER_H
#define _MBSMASTER_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "ModbusDef.h"

/*****************************BAT5K***********************************/
#define BAT_V_DATA_ADDR							0x0000				 	/*��ѹ������ʼ��ַ*/
#define BAT_R_DATA_ADDR							0x0100				 	/*����������ʼ��ַ*/
#define CONNECT_R_DATA_ADDR						0x0200				/*���ӵ���������ʼ��ַ*/
#define BAT_TEMP_DATA_ADDR						0x0300				/*�¶�������ʼ��ַ*/
#define BAT_ERR_STAT_ADDR						0x0400				/*��ع���״̬��ʼ��ַ*/
#define DEV_STAT_ADDR							0x0700				 	/*ģ������״̬��ʼ��ַ*/
#define MEA_DATA_ADDR							0x0800				 	/*����������ʼ��ַ*/
#define CAL_PARA_ADDR							0x0850					/*У׼������ʼ��ַ*/
#define SYS_WORK_STAT_ADDR						0x1000				/*ϵͳ����״̬��ʼ��ַ*/
#define ASSIST_STAT_ADDR						0x1010				/*����״̬��ʼ��ַ*/
#define SYS_CTRL_ADDR							0x1020				 	/*ϵͳ������ʼ��ַ*/
#define BAT_V_TEST_ADDR							0x1020
#define BAT_CR_TEST_ADDR						0x1021
#define BAT_T_TEST_ADDR							0x1022
#define BAT_R_TEST_ADDR							0x1023
#define BAT_EQ_ADDR								0x1024
#define MSYS_CFG_ADDR							0x1040				 	/*ϵͳ������ʼ��ַ*/
#define SYS_TIME_ADDR							0x1060				 	/*ϵͳʱ����ʼ��ַ*/
#define COM_PARA_ADDR							0x1070				 	/*ͨѶ������ʼ��ַ*/
#define GROUP_DATA_ADDR							0x1100				/*�������������ʼ��ַ*/
#define GROUP_CAL_ADDR							0x1180				/*����ɼ���У׼����*/
#define GROUP_PARA_ADDR							0x1200				/*����ɼ��в�����ַ*/
#define GROUP_STAT_ADDR							0x1280				/*����ɼ���״̬��ַ*/
#define RATIO_ADDR								0x1300				/*����У׼ϵ��*/
#define V_DATA_DEC_ADDR							0x1350				/*��ѹС��*/
#define TEMP_SET_ADDR							0x1370				//�¶�̽ͷID��Ϣ��ַ
#define TEMP_EXID_ADDR							0x13D0				//�¶�̽ͷ�Զ���ID��ַ
#define M_SHIELD_INFO_ADDR						0x1400				//������Ϣ��ַ
#define HISTORY_RECORD_ADDR						0x4000				/*��ʷ��¼��ʼ��ַ*/
#define USART_TEST_ADDR							0x5000				 	/*���ڲ�����ʼ��ַ*/


#define BAT_V_DATA_NUM							18						/*��ѹ������ʼ��ַ*/
#define BAT_R_DATA_NUM							18						/*����������ʼ��ַ*/
#define CONNECT_R_DATA_NUM						18					/*���ӵ���������ʼ��ַ*/
#define BAT_TEMP_DATA_NUM						18					/*�¶�������ʼ��ַ*/
#define BAT_ERR_STAT_NUM						500				 	/*��ع���״̬��ʼ��ַ*/
#define DEV_STAT_NUM							15				 		/*ģ������״̬��ʼ��ַ*/
#define MEA_DATA_NUM							3				 			/*����������ʼ��ַ*/
#define CAL_PARA_NUM							179 						/*У׼������ʼ��ַ,������ԭָ��08B2*/
#define SYS_WORK_STAT_NUM						2				 		/*ϵͳ����״̬��ʼ��ַ*/
#define ASSIST_STAT_NUM							4							/*����״̬��ʼ��ַ*/
#define SYS_CTRL_NUM							5				 			/*ϵͳ������ʼ��ַ*/
#define MSYS_CFG_NUM							28				 		/*ϵͳ������ʼ��ַ*/
#define SYS_TIME_NUM							6				 			/*ϵͳʱ����ʼ��ַ*/
#define COM_PARA_NUM							36				 		/*ͨѶ������ʼ��ַ*/
#define GROUP_DATA_NUM							16				/*�������������ʼ��ַ*/
#define GROUP_CAL_NUM							16				/*����ɼ���У׼����*/
#define GROUP_PARA_NUM							16				/*����ɼ��в�����ַ*/
#define GROUP_STAT_NUM							16				/*����ɼ���״̬��ַ*/
#define RATIO_NUM								4				/*����У׼ϵ��*/
#define V_DATA_DEC_SUM							18				/*��ѹ����С����ʼ��ַ*/
#define TEMP_SET_SUM							76				/*�¶�̽ͷID������ʼ��ַ*/
#define TEMP_EXID_SUM							21				//�¶�̽ͷ�Զ���ID������ʼ��ַ
#define SHIELD_INFO_NUM							33				//5�����ε����Ϣ��14��Ѳ��ģ���¶�̽ͷ������Ϣ
#define HISTORY_RECORD_NUM						78				 	/*��ʷ��¼��ʼ��ַ*/
#define USART_TEST_NUM							9				 		/*���ڲ�����ʼ��ַ*/



typedef enum
{
	NON_SECT = 0,											/*���������ݶ�*/
	BAT_V_DATA_SECT,									/*��ص�ѹ���ݶ�*/
	BAT_R_DATA_SECT,									/*�����������*/
	CONNECT_R_DATA_SECT,							/*���ӵ������ݶ�*/
	BAT_TEMP_DATA_SECT,								/*����¶����ݶ�*/
	BAT_ERR_STAT_SECT,								/*��ع���״̬��*/
	DEV_STAT_SECT,										/*ģ������״̬��*/
	MEA_DATA_SECT,										/*�������ݶ�*/
	CAL_PARA_SECT,										/*У׼������*/
	SYS_WORK_STAT_SECT,								/*ϵͳ����״̬��*/
	ASSIST_STAT_SECT,									/*����״̬��*/
	SYS_CTRL_SECT,										/*ϵͳ���ƶ�*/
	MSYS_CFG_SECT,										/*ϵͳ���ö�*/
	SYS_TIME_SECT,										/*ϵͳʱ�Ӷ�*/
	COM_PARA_SECT,										/*ͨѶ������*/
	GROUP_DATA_SECT,									/*�ɼ����������ݶ�*/
	GROUP_CAL_SECT,									/*�ɼ���У׼���ݶ�*/
	GROUP_PARA_SECT,									/*�ɼ��в������ݶ�*/
	GROUP_STAT_SECT,									/*�ɼ���״̬���ݶ�*/
	R_RATIO_SECT,									/*�ɼ���״̬���ݶ�*/
	V_DATA_DEC_SECT,								/*��ѹ����С�����ݶ�*/
	TEMP_SET_SECT,									//�¶�̽ͷ�������ݶ�
	TEMP_EXID_SECT,									//�¶�̽ͷ�Զ���ID���ݶ�
	SHIELD_INFO_SECT,								//������Ϣ���ݶ�
	HISTORY_RECORD_SECT,							/*��ʷ��¼��*/
	USART_TEST_SECT,									/*���ڲ��Զ�*/
}SECT_ADDR_DEF;

#define MAX_SECT USART_TEST_SECT
/*********************�������**************************/
#define MAX_BAT_NUM								18//ÿ��ģ�����18�ڵ��
#define MAX_DEV_NUM								14//���֧��240�ڵ�أ��߸�ģ��
#define MAX_GROUP_NUM							2//�ܵ������
#define MAX_GROUP_BAT_NUM						240//һ�����е�������������ʽʵ��Ҫ��

//typedef struct
//{
//	uint16_t BatV[MAX_BAT_NUM];			/*��ص�ѹ(uv)*/
//	uint16_t InterR[MAX_BAT_NUM];		/*�������(u��)*/
//	uint16_t ConnectR[MAX_BAT_NUM];	/*���ӵ���(u��)*/
//	int16_t Temp[MAX_BAT_NUM];			/*�¶�*/
//}MOUDLE_MEA_DATA;

///*********************Ѳ��ģ������״̬**************************/
//typedef struct
//{
//	int16_t Temp;													/*Ѳ��ģ�黷���¶�*/
//	uint16_t MeaInterRStat;									/*�������״̬*/
//	uint16_t CellMeaStat;										/*���Ѳ��״̬*/
//	uint16_t ConnectRMeaStat;								/*������Ѳ��״̬*/
//	uint16_t TempMeaStat;										/*�¶�Ѳ��״̬*/
//	uint16_t DevInputStat;								/*Ѳ��ģ������״̬*/
//	uint16_t DevOutputStat;							/*Ѳ��ģ�����״̬*/
//	uint16_t DevERR;											/*Ѳ��ģ�����*/
//	uint16_t DevHWVersion;								/*ģ��Ӳ���汾��*/
//	uint16_t DevSWVersion;								/*ģ������汾��*/
//	uint16_t DischargeMoudle;							/*�ŵ�ģ��״̬*/
//	uint16_t ConnectMoudle;								/*������ģ��״̬*/
//	uint16_t EQMoudle;									/*����ģ��״̬*/
//}MOUDLE_STAT;
///*********************����ԭʼ����**************************/
//typedef struct
//{
//	uint16_t CellVoltageAD;									/*���ص�ѹ�ɼ�ͨ��ADֵ*/
//	uint16_t ConnectVoltageAD;							/*��������ѹ�ɼ�ͨ��ADֵ*/
//	uint16_t CurrentAD;											/*�ŵ�����ɼ�ͨ��ADֵ*/
//}MEA_DATA;
///*********************У׼����**************************/
//typedef struct
//{
//	fp32 CellVoltageB;									/*���ص�ѹ�ɼ�ͨ��Bֵ*/
//	fp32 CellVoltageK;									/*���ص�ѹ�ɼ�ͨ��Kֵ*/
//	fp32 ConnectVoltageB;								/*��������ѹ�ɼ�ͨ��Bֵ*/
//	fp32 ConnectVoltageK;								/*��������ѹ�ɼ�ͨ��Kֵ*/
//	fp32 CurrentB;											/*�ŵ������ѹ�ɼ�ͨ��Bֵ*/
//	fp32 CurrentK;											/*�ŵ������ѹ�ɼ�ͨ��Kֵ*/
//}CAL_PARA;

///*********************ϵͳ����״̬**************************/
//typedef struct
//{
//	uint16_t PresentMode;										/*��ǰ����ģʽ*/
//	uint16_t ExecutMode;										/*����ִ�е�ģʽ*/
//}SYS_WORK_STAT;

///*********************����״̬**************************/
//typedef struct
//{
//	uint16_t OutputStat;									/*IO���״̬*/
//	uint16_t InputStat;										/*IO����״̬*/
//	uint16_t AlarmOutputStat;							/*�������״̬*/
//	uint16_t SysErrStat;									/*ϵͳ����״̬*/
//}ASSIST_STAT;
///*********************ϵͳ���ƼĴ���**************************/

//#define START 0xaa
//#define STOP   0x55

//typedef struct
//{
//	uint16_t StartCellMea;								/*�������Ѳ��*/
//	uint16_t StartConnectMea;							/*����������Ѳ��*/
//	uint16_t StartTempMea;								/*�����¶�Ѳ��*/
//	uint16_t StartInterMea;								/*�����������*/
//}SYS_CTRL;

///*********************���ڲ���**************************/
//typedef struct
//{
//	uint16_t TestChannel;										/*����ͨ��*/
//	uint16_t TestType;											/*��������*/
//	uint16_t TestDataWordQuantity;					/*���������ֽ���*/
//	uint16_t RcvBuffer[6];									/*���ջ���*/
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
	uint8  Bat5kCommErrStatus[MAX_GROUP_NUM]; //�ɼ��� 
	uint8  Bat5kCommErrCount[MAX_GROUP_NUM];
	uint32 Bat5kCommErrStartTime[MAX_GROUP_NUM];
	uint8  BkgCommErrStatus; //��̨����
	uint8  BkgCommErrCount;
	uint32 BkgCommErrStartTime;
	//BAT_DATA BatData;         /*����*/
	//STATUS_DATA StatusData;	        /*״̬����*/
	//SYSTEM_PARA SysPara;	        /*ϵͳ����*/
	//HISTORY_RECORD HistoryRecord;   /*��ʷ��¼*/
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

