/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: MbsSlave.h
** Created By: 
** Last modified Date: 
** Last Version: 1.0
** Descriptions: ��RVMDK 4.72����汾
****************************************************************************************/
#ifndef _MBSSLAVE_H
#define _MBSSLAVE_H

#ifdef  __cplusplus
extern  "C" {
#endif
	
#include "MbsMaster.h"
#include "ModbusDef.h"

/*�������ݶε���ʼ��ַ*/
#define S_BAT_V_DATA_ADDR							0x0000				 /*��ѹ������ʼ��ַ*/
#define S_BAT_R_DATA_ADDR							0x0200				 /*����������ʼ��ַ*/
#define S_CONNECT_R_DATA_ADDR						0x0400				 /*���ӵ���������ʼ��ַ*/
#define S_BAT_TEMP_DATA_ADDR						0x0600				 /*�¶�������ʼ��ַ*/
#define S_BAT_ERR_STAT_ADDR							0x0800				 /*��ع���״̬��ʼ��ַ*/
#define S_MBS_SINGNAL_ADDR							0x0A50				/*MODBUS��̨ң����ʼ��ַ*/
#define S_DEV_STAT_ADDR								0x0B00				 /*ģ������״̬��ʼ��ַ*/
#define S_MEA_DATA_ADDR								0x0C00				 /*����������ʼ��ַ*/
#define S_CAL_PARA_ADDR								0x0C50				/*У׼������ʼ��ַ*/
#define S_SYS_WORK_STAT_ADDR						0x1000				 /*ϵͳ����״̬��ʼ��ַ*/
#define S_ASSIST_STAT_ADDR							0x1010				/*����״̬��ʼ��ַ*/
#define S_SYS_CTRL_ADDR								0x1020				 /*ϵͳ������ʼ��ַ*/
#define S_MSYS_CFG_ADDR								0x1040				 /*ϵͳ������ʼ��ַ*/
#define S_SYS_TIME_ADDR								0x10A0				 /*ϵͳʱ����ʼ��ַ*/
#define S_COM_PARA_ADDR								0x10B0				 /*ͨѶ������ʼ��ַ*/
#define S_GROUP_DATA_ADDR							0x1100				/*�������������ʼ��ַ*/
#define S_GROUP_CAL_ADDR							0x1180				/*����ɼ���У׼����*/
#define S_GROUP_PARA_ADDR							0x1200				/*����ɼ��в�����ַ*/
#define S_GROUP_STAT_ADDR							0x1280				/*����ɼ���״̬��ַ*/
#define S_R_RATIO_ADDR								0x1300				/*����У׼ϵͳ*/
#define S_V_DATA_DEC_ADDR							0x1350				//��ѹ����С����ַ
#define S_TEMP_SET_ADDR								0x1370				//�¶�̽ͷID��Ϣ��ַ
#define S_TEMP_EXID_ADDR							0x13D0				//�¶�̽ͷ�Զ���ID��ַ
#define S_SHIELD_INFO_ADDR							0x1400				//������Ϣ��ʼ��ַ
#define S_HISTORY_RECORD_ADDR						0x4000				 /*��ʷ��¼��ʼ��ַ*/
#define S_USART_TEST_ADDR							0x5000				 /*���ڲ�����ʼ��ַ*/
#define LCD_CMD_ADDR								0x2000				 /*���ڲ�����ʼ��ַ*/


#define S_BAT_V_DATA_NUM							400					/*��ѹ������ʼ��ַ*/
#define S_BAT_R_DATA_NUM							400					/*����������ʼ��ַ*/
#define S_CONNECT_R_DATA_NUM						400					/*���ӵ���������ʼ��ַ*/
#define S_BAT_TEMP_DATA_NUM							400					/*�¶�������ʼ��ַ*/
#define S_BAT_ERR_STAT_NUM							515				 	/*��ع���״̬��ʼ��ַ*/
#define S_MBS_SINGNAL_NUM							4					/*MODBUS��̨ң�żĴ�������*/
#define S_DEV_STAT_NUM								220				 	/*ģ������״̬��ʼ��ַ*/
#define S_MEA_DATA_NUM								3				 		/*����������ʼ��ַ*/
#define S_CAL_PARA_NUM								6 					/*У׼������ʼ��ַ*/
#define S_SYS_WORK_STAT_NUM							2				 		/*ϵͳ����״̬��ʼ��ַ*/
#define S_ASSIST_STAT_NUM							4						/*����״̬��ʼ��ַ*/
#define S_SYS_CTRL_NUM								5				 		/*ϵͳ������ʼ��ַ*/
#define S_MSYS_CFG_NUM								42				 	/*ϵͳ������ʼ��ַ*/
#define S_SYS_TIME_NUM								6				 		/*ϵͳʱ����ʼ��ַ*/
#define S_COM_PARA_NUM								36				 	/*ͨѶ������ʼ��ַ*/
#define S_GROUP_DATA_NUM							16				/*�������������ʼ��ַ*/
#define S_GROUP_CAL_NUM								16				/*����ɼ���У׼����*/
#define S_GROUP_PARA_NUM							16				/*����ɼ��в�����ַ*/
#define S_GROUP_STAT_NUM							16				/*����ɼ���״̬��ַ*/
#define S_RATIO_NUM									4				/*����У׼ϵͳ*/
#define S_V_DATA_DEC_SUM							400				//��ص�ѹ����С����ַ
#define S_TEMP_SET_SUM								76				//�¶�̽ͷID��Ϣ��ַ
#define S_TEMP_EXID_SUM								21				//�¶�̽ͷ�Զ���ID
#define S_SHIELD_INFO_NUM							33				//5�����ε����Ϣ��14��Ѳ��ģ���¶�̽ͷ������Ϣ
#define S_HISTORY_RECORD_NUM						78				 	/*��ʷ��¼��ʼ��ַ*/
#define S_USART_TEST_NUM							9				 		/*���ڲ�����ʼ��ַ*/
#define LCD_CMD_NUM									0x0900					

typedef enum
{
	S_NON_SECT = 0,												/*���������ݶ�*/
	S_BAT_V_DATA_SECT,											/*��ص�ѹ���ݶ�*/
	S_BAT_R_DATA_SECT,											/*�����������*/
	S_CONNECT_R_DATA_SECT,										/*���ӵ������ݶ�*/
	S_BAT_TEMP_DATA_SECT,										/*����¶����ݶ�*/
	S_BAT_ERR_STAT_SECT,										/*��ع���״̬��*/
	S_MBS_SINGNAL_SECT,											/*MODBUSң�����ݶ�*/
	S_DEV_STAT_SECT,											/*ģ������״̬��*/
	S_MEA_DATA_SECT,											/*�������ݶ�*/
	S_CAL_PARA_SECT,											/*У׼������*/
	S_SYS_WORK_STAT_SECT,										/*ϵͳ����״̬��*/
	S_ASSIST_STAT_SECT,											/*����״̬��*/
	S_SYS_CTRL_SECT,											/*ϵͳ���ƶ�*/
	S_MSYS_CFG_SECT,											/*ϵͳ���ö�*/
	S_SYS_TIME_SECT,											/*ϵͳʱ�Ӷ�*/
	S_COM_PARA_SECT,											/*ͨѶ������*/
	S_GROUP_DATA_SECT,											/*�ɼ����������ݶ�*/
	S_GROUP_CAL_SECT,											/*�ɼ���У׼���ݶ�*/
	S_GROUP_PARA_SECT,											/*�ɼ��в������ݶ�*/
	S_GROUP_STAT_SECT,											/*�ɼ���״̬���ݶ�*/
	S_R_RATIO_SECT,												/*����У׼ϵͳ*/
	S_V_DATA_DEC_SECT,											/*��ѹ����С�����ݶ�*/
	S_TEMP_SET_SECT,											/*�¶�̽ͷID���ö�*/
	S_TEMP_EXID_SECT,											//�¶�̽ͷ�Զ���ID���ö�
	S_SHIELD_INFO_SECT,										//������Ϣ���ݶ�
	S_HISTORY_RECORD_SECT,										/*��ʷ��¼��*/
	S_USART_TEST_SECT,											/*���ڲ��Զ�*/
	LCD_CMD_SECT,
}S_SECT_ADDR_DEF;

#define S_MAX_SECT LCD_CMD_SECT
/*********************�������**************************/
typedef struct
{
	uint16_t S_BatV[MAX_DEV_NUM][MAX_BAT_NUM];			/*��ص�ѹ(1mv)*/
	uint16_t S_VDec[MAX_DEV_NUM][MAX_BAT_NUM];			/*��ص�ѹС��(0.1mv)*/
	uint16_t S_InterR[MAX_DEV_NUM][MAX_BAT_NUM];		/*�������(u��)������10��ŷ�����λ��1���洢��λΪ10΢ŷ��δ����10��ŷ�Ĵ洢��λΪ1΢ŷ*/
	uint16_t S_ConnectR[MAX_DEV_NUM][MAX_BAT_NUM];	/*���ӵ���(u��)���洢��ʽ������һ��*/
	float S_Temp[MAX_DEV_NUM][MAX_BAT_NUM];			/*�¶�*/
}S_MOUDLE_MEA_DATA;

/*********************���ع���״̬����**************************/
#define S_MAX_ERR_DATA								500
typedef enum
{
	NORMAL = 0,
	VOLTAGE_OVER,						// 1��ع�ѹ
	VOLTAGE_LOW,						// 2���Ƿѹ
	VOLTAGE_RECOVER,					// 3��ѹ�ָ�
	TEMP_OVER,							// 4�¶ȹ���
	TEMP_LOW,							// 5�¶ȹ���
	TEMP_RECOVER,						// 6�¶Ȼָ�
	INTER_R_OVER,						// 7���賬��
	CR_OVER,							// 8��������ֵ����
	GROUP_V_OVER,						// 9���ѹ����
	GROUP_V_LOW,						// 10���ѹ����
	GROUP_V_RECOVER,					// 11���ѹ�ָ�
	CURRENT_DISCHARGE_OVER,			// 12�ŵ��������
	CURRENT_DISCHARGE_RECOVER,		// 13�ŵ�����ָ�
	CURRENT_CHARGE_OVER,				// 14����������
	CURRENT_CHARGE_RECOVER,			// 15�������ָ�
	ENVIRMENT_TEMP_OVER,				// 16�����¶ȹ���
	ENVIRMENT_TEMP_LOW,				// 17�����¶ȹ���
	ENVIRMENT_TEMP_RECOVER,			// 18�����¶Ȼָ�
	DEV_TEMP_ERR,						// 19�¶ȴ���������
	DEV_TEMP_RECOVER,					// 20�¶ȴ��������ϻָ�
	ENVIR_TEMP_ERR,					// 21�����¸й���
	ENVIR_TEMP_RECOVER,				// 22�����¸й��ϻָ�
	DEV_TEST_FAIL,						//23�������ʧ��	
	DEV_COMM_ERR,						// 24ͨ���쳣
	DEV_COMM_RECOVER,				// 25ͨ���쳣�ָ�	
	FDY_COM_ERR,						// 26�ŵ���ͨ���쳣
	FDY_COM_RECOVER,					// 27�ŵ���ͨ�Żָ�
	GPS_COM_ERR,						// 28GPSͨ���쳣
	GPS_COM_RECOVER,					// 29GPSͨ�Żָ�
	BAT5000_S_COM_ERR,				// 30�ɼ���ͨ���쳣
	BAT5000_S_COM_RECOVER,			// 31�ɼ���ͨ�Żָ�
	BKG_COM_ERR,						// 32Ѳ����������̨ͨ���쳣
	BKG_COM_RECOVER,					// 33Ѳ����������̨ͨ�Żָ�	
	SYS_OPEN_CIRCUIT,					//34��·��·
	SYS_OPEN_CIRCUIT_RECOVER,		// 35��·��·�ָ�
	SYS_POWER_ON,						//36ϵͳ����
	DEV_INNER_R_ERR,					// 37����ģ�����
	DEV_INNER_R_RECOVER,				// 38����ģ����ϻָ�
	DEV_CR_ERR,						// 39������ģ�����
	DEV_CR_RECOVER,					// 40������ģ����ϻָ�
	DEV_EQ_ERR,						// 41����ģ�����
	DEV_EQ_RECOVER,					// 42����ģ����ϻָ�
	START_TEST,							// 43�ֶ��������
	AUTO_TEST,							// 44�Զ��������
	JKQ_TEST,							// 45��̨�������
	DEV_TEST_FAIL1,					//46�������ʧ��ԭ�� :�ŵ����С
	DEV_TEST_FAIL2,					//47�������ʧ��	:�ŵ��������
	DEV_TEST_FAIL3,					//48�������ʧ��ԭ�� :ֹͣ�������
	DEV_TEST_FAIL4,					//49�������ʧ��	:�ŵ������ѹ��С
	DEV_TEST_FAIL5,					//50�������ʧ��ԭ�� :�ŵ������ѹ����
	DEV_TEST_FAIL6,					//51�������ʧ��	:�����ѹ��С
	DEV_TEST_FAIL7,					//52�������ʧ��ԭ�� :�����ѹ����	
	DEV_TEST_FAIL8,					//53�������ʧ��	:�����¶ȹ��ͻ��¶�̽ͷ�쳣
	DEV_TEST_FAIL9,					//54�������ʧ��ԭ�� :�����¶ȹ���	
	DEV_TEST_FAIL10,					//55�������ʧ��ԭ�� :���Գ�ʱ
	BAT_SAMPLE_LINE_DROP,				//56	��ز���������
	GROUP_SAMPLE_LINE_DROP,			//57	��������������
	SAMPLE_LINE_DROP_RECOVER,		//58����������ָ�
	START_BAT_DISCHARGE,				//59 ����������ŵ�
	STOP_BAT_DISCHARGE,				//60 ֹͣ������ŵ�
	GROUP_SAMPLE_LINE_DROP_RECOVER,	//61��������������ָ�
	BAT_SAMPLE_LINE_RESERVE,			//62��ز����߷���
	GROUP_SAMPLE_LINE_RESERVE,		//63���������߷���
	BAT_SAMPLE_LINE_RESERVE_RECOVER,//64��ز����߷��ӻָ�
	GROUP_SAMPLE_LINE_RESERVE_RECOVER,//65���������߷��ӻָ�
	PAUSE_R_TEST,						//66��ͣ�������
	CONTINUE_R_TEST,					//67���������������
	GV_LOW_DISC_STOP,					//68���ѹǷѹֹͣ�ŵ�
	TIME_OVER_DISC_STOP,				//69�ŵ�ʱ�䵽
	DISC_START,						//70�����ŵ�		
	BATV_LOW_DISC_STOP,				//71���ڵ��Ƿѹֹͣ�ŵ�
	STOP_DISC_STOP,					//72�ֶ�ֹͣ�ŵ�
	DISC_PAUSE,						//73��ͣ�ŵ�
	RESTART_DISC,						//74�����ŵ�


	


	DEV_DISCHARGE_VOLTAGE_ERR,			// 23�ŵ������ѹ�쳣
	DEV_DISCHARGE_VOLTAGE_RECOVER,		// 24�ŵ������ѹ�ָ�
	DEV_R_PATROL_ERR,					// 25����Ѳ���쳣
	DEV_R_PATROL_RECOVER,				// 26����Ѳ��ָ�
	DEV_DISCHARGE_DEV_ERR,				// 27�ŵ���쳣
	DEV_DISCHARGE_DEV_RECOVER,			// 28�ŵ���쳣�ָ�
	DEV_DISCHARGE_CURRENT_ERR,			// 29�ŵ�����쳣
	DEV_DISCHARGE_CURRENT_RECOVER,		// 30�ŵ�����쳣�ָ�
	DEV_EQ_OPEN_ERR,					// 31�����쳣(��)
	DEV_EQ_OPEN_RECOVER,				// 32�����쳣(��)�ָ�
	DEV_EQ_CLOSE_ERR,					// 33�����쳣(�ر�)
	DEV_EQ_CLOSE_RECOVER,				// 34�����쳣(�ر�)�ָ�

}ERR_TYPE;
typedef struct
{
		uint8_t S_ErrBatNum;/*�쳣���ر�ż���������*/
		uint8_t S_ErrType;																/*0x01-��ѹ��0x02-Ƿѹ��0x03-��ѹ�ָ���0x04-���£�0x05-���£�0x06-�¶Ȼָ�
																		0x07-������賬�ޣ�0x08-���ӵ��賬��*/
		int16_t S_ErrData;/*�쳣���ع�������*/	
}S_ERR_INFO;

typedef struct
{
	uint16_t S_Quantity;							/*�쳣��������*/
	S_ERR_INFO S_ErrInfo[S_MAX_ERR_DATA];
}S_BAT_ERR_STAT;
/*********************Ѳ��ģ������״̬**************************/
typedef enum
{
	NOERR = 0,
	DEV_TEMP_CONVERT_ERR1,//�¶�ת������
	DEV_TEMP_READ_ERR1,//�¶ȶ�ȡ����
	DEV_DISCHARGE_VOLTAGE_ERR1,//�ŵ������ѹ�쳣
	DEV_R_PATROL_ERR1,//����Ѳ���쳣
	DEV_DISCHARGE_DEV_ERR1,//�ŵ���쳣
	DEV_DISCHARGE_CURRENT_ERR1,//�ŵ��������
	DEV_EQ_OPEN_ERR1,//�����쳣(��)
	DEV_EQ_CLOSE_ERR1,//�����쳣(�ر�)
	SINGLE_V_OVER,//�����ѹ����
	SINGLE_V_LOW,//�����ѹ����
	SINGLE_TEMP_OVER,//�����¶ȹ���
	SINGLE_TEMP_LOW,//�����¶ȹ���
	DISCHARGE_CURRENT_OVER,//�ŵ��������
	COMM_ERR,		//ͨ���쳣
}DEV_ERR;

typedef enum
{
	NO_MEA = 0,
	MEA_OVER = 0x0100,
}MEA_STAT;

typedef struct
{
	int16_t InnerRRatio;//(�Ŵ�1000��)
	int16_t CRRatio;//(�Ŵ�1000��)
	int16_t TempCliPara;//(�Ŵ�ʮ��)
	int16_t VCliPara;//(�Ŵ�ʮ��)
}R_RATIO_INFO;//����У׼ϵ��

typedef struct
{
	float S_Temp;										/*Ѳ��ģ�黷���¶�*/
	uint16_t S_MeaInterRStat;							/*�������״̬*/
	uint16_t S_CellMeaStat;								/*���Ѳ��״̬*/
	uint16_t S_ConnectRMeaStat;							/*������Ѳ��״̬*/
	uint16_t S_TempMeaStat;								/*�¶�Ѳ��״̬*/
	uint16_t S_DevInputStat;							/*Ѳ��ģ������״̬*/
	uint16_t S_DevOutputStat;							/*Ѳ��ģ�����״̬*/
	uint16_t S_DevERR;									/*Ѳ��ģ�����*/
	uint16_t S_DevHWVersion;							/*ģ��Ӳ���汾��*/
	uint16_t S_DevSWVersion;							/*ģ������汾��*/
	uint16_t S_InnerRDev;								/*�ŵ�ģ��ע����Ϣ*/
	uint16_t S_CRDev;									/*������ģ��ע����Ϣ*/
	uint16_t S_EQDev;									/*����ģ��ע����Ϣ*/
	uint16_t S_CalStat;									/*У׼״̬��Ϣ*/
	uint16_t S_Handshake;								/*��ģ��������Ϣ*/	
}S_MOUDLE_STAT;
/*********************����ԭʼ����**************************/
typedef struct
{
	uint16 S_CellVoltageAD;									/*���ص�ѹ�ɼ�ͨ��ADֵ*/
	uint16 S_ConnectVoltageAD;							/*��������ѹ�ɼ�ͨ��ADֵ*/
	uint16 S_CurrentAD;											/*�ŵ�����ɼ�ͨ��ADֵ*/
}S_MEA_DATA;
/*********************У׼����**************************/
typedef struct
{
	int16 S_CellVoltageB;									/*���ص�ѹ�ɼ�ͨ��Bֵ*/
	uint16 S_CellVoltageK;									/*���ص�ѹ�ɼ�ͨ��Kֵ*/
	int16 S_ConnectVoltageB;								/*��������ѹ�ɼ�ͨ��Bֵ*/
	uint16 S_ConnectVoltageK;								/*��������ѹ�ɼ�ͨ��Kֵ*/
	int16 S_CurrentB;											/*�ŵ������ѹ�ɼ�ͨ��Bֵ*/
	uint16 S_CurrentK;											/*�ŵ������ѹ�ɼ�ͨ��Kֵ*/
}S_CAL_PARA;
/*********************�¶�̽ͷID��Ϣ**************************/
typedef struct
{
	uint8 TempType;//0��ʾ����¶�̽ͷ��1��ʾ�����¶�̽ͷ1��2��ʾ�����¶�̽ͷ2��3��ʾ�����¶�̽ͷ3
	uint8 TempNO;
	uint16 WrExid;//д����Զ���ID
	uint16 RdExid;//��ȡ���Զ���ID
	uint8 WrID[8];//д��ĳ���ID
	uint8 RdID[8];//��ȡ�ĳ���ID
}TEMP_SET;//�¶�̽ͷ�����Ϣ����
/*********************���������Ϣ**************************/
typedef struct
{
	uint16 BatVShield[5];
	uint16 TempShield[MAX_DEV_NUM*2];
}SHIELD_INFO;
/*********************ϵͳ����״̬**************************/
typedef struct
{
	uint16 S_PresentMode;										/*��ǰ����ģʽ*/
	uint16 S_ExecutMode;										/*����ִ�е�ģʽ*/
}S_SYS_WORK_STAT;

/*********************����״̬**************************/
typedef struct
{
	uint16 S_OutputStat;									/*IO���״̬*/
	uint16 S_InputStat;										/*IO����״̬*/
	uint16 S_AlarmOutputStat;							/*�������״̬*/
	uint16 S_SysErrStat;									/*ϵͳ����״̬*/
}S_ASSIST_STAT;
/*********************ϵͳ���ƼĴ���**************************/
#define START		0xaa
#define STOP		0x55

typedef struct
{
	uint16 S_StartCellMea;								/*�������Ѳ��*/
	uint16 S_StartConnectMea;							/*����������Ѳ��*/
	uint16 S_StartTempMea;								/*�����¶�Ѳ��*/
	uint16 S_StartInterMea;								/*�����������*/
	uint16 S_StartBatEQ;								/*������ؾ���*/
	uint16 S_DevCliCmd;									/*Ѳ��ģ��У׼����*/
	uint16 S_BatDisCmd;									/*(0x1026)������ŵ��������*/
}S_SYS_CTRL;

/*********************ϵͳ���üĴ���**************************/
typedef struct
{
	uint16 S_CellNum;									/*����������*/
	uint16 S_GroupVoltageMax;							/*�����ѹ����*/
	uint16 S_GroupVoltageMin;							/*�����ѹ����*/
	int16 S_ChargeCurrentMax;							/*����������*/
	int16 S_DischargeCurrentMax;						/*�ŵ��������*/
	uint16 S_SingleVoltageMax;							/*�����ѹ����*/
	uint16 S_SingleVoltageMin;							/*�����ѹ����*/
	uint16 S_SingleCellRMax;							/*��������������*/
	uint16 S_ConnectRMax;								/*��������������*/
	int16 S_TempMax;									/*�¶�����*/
	int16 S_TempMin;									/*�¶�����*/
	uint16 S_AutoMeaRTime;								/*�Զ���������������λΪ"��"*/
	uint16 EqParaCfg;									//����������ã�����ٷֱ�
	uint16	BatVoltageLevel;						//��ص�ѹ�ȼ�
	uint16	InnerRDevStat;							//����Ѳ�칦�����ùر�
	uint16	CRDevStat;								//�������������ùر�
	uint16	EQDevStat;								//���⹦�����ùر�
	uint16  TempDevStat;							//�¶Ȳɼ��������ùر�
}GROUP_CFG;


typedef struct
{
	uint16 S_CellGroupNum;								/*����������*/
	GROUP_CFG GroupCfg[MAX_GROUP_NUM];	
	uint16 CurrentChannel[MAX_GROUP_NUM];
	uint16 S_GpsTimeCalEN;								/*ʹ��GPS��ʱ*/
	uint16 S_HostHWVersion;							/*����Ӳ���汾��*/
	uint16 S_HostSWVersion;							/*��������汾��*/
	
}S_MSYS_CFG;

typedef enum
{
	NO_SENSOR = 0,
	HALL_SENSOR1 = 1,
	HALL_SENSOR2 = 2,
	SHUNT = 3,//������
}I_CHANNEL;

/*********************ϵͳʱ�ӼĴ���**************************/
typedef struct
{
	uint16 Year;												/*��*/
	uint16 Month;												/*��*/
	uint16 Day;													/*��*/
	uint16 Hour;												/*ʱ*/
	uint16 Minute;											/*��*/
	uint16 Second;											/*��*/
}S_SYS_TIME;




/*********************ͨѶ�����Ĵ���**************************/
#define S_MAX_USART_NUM 5

typedef enum
{
	RATE2400 = 0,
	RATE4800,
	RATE9600,
	RATE19200,
	RATE38400,
	RATE115200,
}BAUD_RATE;		//������

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
}COM_PROTOCAL;/*ͨ�Ŷ˿�ͨ��Э��*/

typedef enum
{
	NOUSE = 0,
	BATGROUP1,		//������1
	BATGROUP2,		//������2
	JKQ,			//�����
	MODBUS,			//mbs��̨�ӻ�
	MODBUSHOST,		//mbs��̨����
	BCODETIME,
	FDY,			//�ŵ���
}COM_PURPOSE;/*ͨ�Ŷ˿���;*/

typedef	struct
{
	uint16 BaudRate;									/*������*/
	uint16 ComAddr;										/*���˿ڵ�ַ*/
	uint16 SlaveAddr;									/*�ӻ���ַ*/
	uint16 Purpose;										/*��;*/
	uint16 Reserve1;
	uint16 Reserve2;	
}CAN_PARA;

typedef	struct
{
	uint16 BaudRate;									/*������*/
	uint16 ComAddr;										/*���˿ڵ�ַ*/
	uint16 ComProtocal;								/*ͨѶЭ��*/
	uint16 Purpose;										/*��;*/
	uint16 Reserve1;
	uint16 Reserve2;
}USART_PART;	

typedef struct
{
	CAN_PARA CanPara;
	USART_PART UsartPara[S_MAX_USART_NUM];
}S_COM_PARA;

/*********************��ʷ��¼**************************/
#define S_MAX_HISTORY_RECORD_NUM   500 //����¼500����ʷ��¼
typedef struct
{
	uint16 HistoryRecordQuantity[MAX_GROUP_NUM];		/*��ʷ��¼���һ���洢λ�ã�0��ʾû����ʷ��¼����0��ʾ�洢λ��*/
	uint8 RecordOverFlag[MAX_GROUP_NUM];				//��ʷ��¼������߼�¼������ʶ��
	uint16 HistoryRecordNum;							/*��ʷ��¼���*/
	uint16 ReadQuantity;								/*��ȡ������*/

	uint16 Year;
	uint16 MonthDay;										/*Month/Day*/
	uint16 HourMinute;									/*Hour/Minute*/
	uint16 Sec;
	uint16 ErrSource;			//������Դ�ǵ��ڵ��ʱ���ñ�����ʾ��ر�ţ������Դ��ģ�������ʱ����4λ��ʾ��ţ���4λ��ʾ�豸���
	uint16 ErrType;											/*��������*/
	int16 ErrData;											/*��������*/
	uint16 ReserveData;									/**/
}S_HISTORY_RECORD;
/*********************���ڲ���**************************/
typedef struct
{
	uint16 TestChannel;										/*����ͨ��*/
	uint16 TestType;											/*��������*/
	uint16 TestDataWordQuantity;					/*���������ֽ���*/
	uint16 RcvBuffer[6];									/*���ջ���*/
}S_COM_TEST;

typedef enum
{
	VOVER = 0,	//�����ѹ
	VLOW = 1,		//����Ƿѹ
	GROUPVOVERSIG = 2,		//���ѹ��ѹ
	GROUPVLOWSIG = 3,			//���ѹǷѹ
	TOVER = 4,			//�¶ȹ���
	TLOW = 5,			//�¶ȹ���
	INNERROVER = 6,		//�������
	CROVER = 7,			//��������ֵ����
	IOVER = 8,			//����������
	DISIOVER = 9,		//�ŵ��������
}BAT_SIGNAL;/*ͨ�Ŷ˿���;*/

typedef enum
{
	S_COMM_ERR = 0,	//�����ѹ
	SMM1_COMM_ERR = 1,//ģ��1ͨ�Ź���
	SMM2_COMM_ERR = 2,//ģ��2ͨ�Ź���
	SMM3_COMM_ERR = 3,//ģ��3ͨ�Ź���
	SMM4_COMM_ERR = 4,//ģ��4ͨ�Ź���
	SMM5_COMM_ERR = 5,//ģ��5ͨ�Ź���
	SMM6_COMM_ERR = 6,//ģ��6ͨ�Ź���
	SMM7_COMM_ERR = 7,//ģ��7ͨ�Ź���
	SMM8_COMM_ERR = 8,//ģ��8ͨ�Ź���
	SMM9_COMM_ERR = 9,//ģ��9ͨ�Ź���
	SMM10_COMM_ERR = 10,//ģ��10ͨ�Ź���
	SMM11_COMM_ERR = 11,//ģ��11ͨ�Ź���
	SMM12_COMM_ERR = 12,//ģ��12ͨ�Ź���
	SMM13_COMM_ERR = 13,//ģ��13ͨ�Ź���
	SMM14_COMM_ERR = 14,//ģ��14ͨ�Ź���
	GROUP_SMP_LINE_ERR = 15,//��ص�ѹ�������쳣
	BAT_SMP_LINE_ERR = 16,//��ص�ѹ�������쳣
	BAT_DEVT_ERR = 17,//����¶�̽ͷ�쳣
	TEMP_DEVT_ERR = 18,//�����¶�̽ͷ�쳣
}DEV_SIGNAL;/*ͨ�Ŷ˿���;*/

typedef struct
{
	uint16 MbsSignal1;//���ң��
	uint16 MbsSignal2;//���ң��
	uint16 MbsSignal3;//�豸ң��
	uint16 MbsSignal4;//�豸ң��

}MBS_SIGNAL;

typedef struct
{
	S_MOUDLE_MEA_DATA 	DevMeaData;//��ص�ѹ���¶ȣ����裬��������ֵ
	S_MEA_DATA 			MeaData[MAX_DEV_NUM];//У׼ADֵ
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

	S_BAT_DATA BatData[MAX_GROUP_NUM];         /*����������*/
	S_STATUS_DATA StatusData;	        /*������״̬����*/
	S_SYSTEM_PARA SysPara;	        /*ϵͳ����*/
	S_HISTORY_RECORD HistoryRecord;   /*��ʷ��¼*/
	S_GROUP	Group[MAX_GROUP_NUM];
	S_COM_TEST 		ComTest;
}MBS_SLAVE;

/*��ȡ�����ݶε�ַ���� */
typedef enum
{
	DEV_NON_SECT = 0,                                /*��Ч���ݶ�*/
	
}DEV_SECT_ADDR_DEF;

extern MBS_SLAVE MbsSlave;

/********************�ֻ����ڴ���ʹ�����ݽṹ****************/
#define  DEVCOMM_FAIL_NUM        251                             /*ͨѶʧ��������*/
#define  BROADCAST_ADDR          0xFF                           /*�㲥��ַ*/
#define  RCV_QUEUE_SIZE          256
typedef struct
{
	uint8 	*p_head;                                            /* �������ݽ��յ�ַ */
	uint8 	*p_tail;                                            /* ���������Ѵ����ַ */	
	uint8   FailCount;                                          /* ͨѶʧ�ܼ��� */
	uint8 	rcvbuf[RCV_QUEUE_SIZE];                             /* �������ݻ���,���δ洢 */
}UART_RCV_CFG;

extern UART_RCV_CFG     UartRcvCfg[5];                          /*���ڴ����ѽ��մ�������*/

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

