#ifndef __LCDCOMM_H
#define __LCDCOMM_H
#include "config.h"

/////////////ͼƬ����ַ����/////////////////
typedef enum
{
   PICTURE0 = 0,  // ��������
   PICTURE1,
   PICTURE2,


}PICTURE;


typedef enum
{
	FIRST_BUTTON = 0,

	FINAL_BUTTON,
}LCD_BUTTON;

/////////////�������׵�ַ/////////////////
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


#define LCD_R_PATROL_ADDR				0x1023		//����Ѳ��
#define LCD_EQ_PATROL_ADDR			0x1024		//��ؾ���
#define LCD_PARA1_SET_ADDR				0x104E		//����ǰϵͳ���õ���ʼ��ַ
#define LCD_PARA2_SET_ADDR				0x1060		//����ǰϵͳ���õ���ʼ��ַ
#define LCD_PARA_SET_NUM				5

#define LCD_DEV_ERR_ADDR				LCD_DEV_STAT_ADDR+7
#define LCD_CAN_PARA_ADDR				LCD_COM_PARA_ADDR
#define LCD_COM1_PARA_ADDR			LCD_CAN_PARA_ADDR+6
#define LCD_COM2_PARA_ADDR			LCD_CAN_PARA_ADDR+12
#define LCD_COM3_PARA_ADDR			LCD_CAN_PARA_ADDR+18
#define LCD_COM4_PARA_ADDR			LCD_CAN_PARA_ADDR+24


#define BAT_GROUP_NO_ADDR					0x201C		//��ǰ��ʾ�ĵ������
#define DEV_NO_ADDR						0x201D		//��ǰҳ������ʾ��ģ����


#define ERR_TOTAL_NUM_ADDR				0x0800		//����������������LCD����ʾ��ַ
#define ERR_PAGE_NUM_ADDR					0x0801		//�������ݵ�ǰҳ��LCD����ʾ��ַ
#define ERR_INFO_NUM1_ADDR				0x0810		//�������ݵ�ǰҳ��LCD����ʾ��ַ

#define SYS_TIME_DISPLAY_ADDR				0x2500		//ϵͳʱ����LCD����ʾ�ĵ�ַ


#define HISTORY_TOTAL_NUM_ADDR			0x4000		//��ʷ��¼��������ʾ��ַ
#define HISTORY_PAGE_NUM_ADDR				0x4001		//��ʷ��¼��ǰ��ʾ��ҳ����ַ
/////////////�Ĵ�������/////////////////


#define LCD_COM_PARA_NUM					0x04	//ÿ��ͨ��ͨ���ļĴ�������


/////////////��������ַ����/////////////////
#define	FIRST_BUTTON						0x2000			//LCD�ϵİ�����һ������ʼ��ַ��
#define LCD_BUTTON_MAX_ADDR				0x2900			//LCD�ϵİ�������ַ���������οռ�
#define BUTTON_RETU_VALUE					0x0200			//��������ֵ



/*************************************************************/
/***********����һЩ�Ĵ���λ�ã�����ϵͳ���ý�����չ***********/
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
}DATA_VAR_DISP;//���ݱ�����ʾ

typedef struct
{
	uint16 VarAddr;
	uint16 Range;
	uint16 BorderColour;//�߿�ɫ
	uint16 ForeColour;//ǰ��ɫ
}PROGRESS_BAR_DISP;//���ݱ�����ʾ

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

/*********************ϵͳ���üĴ���**************************/
typedef struct
{
	uint16_t LCD_GroupVoltageMax;							/*�����ѹ����*/
	uint16_t LCD_GroupVoltageMin;							/*�����ѹ����*/
	uint16_t LCD_ChargeCurrentMax;							/*����������*/
	uint16_t LCD_DischargeCurrentMax;						/*�ŵ��������*/
	uint16_t LCD_SingleVoltageMax;							/*�����ѹ����*/
	uint16_t LCD_SingelVoltageMin;							/*�����ѹ����*/
	uint16_t LCD_SingleCellRMax;							/*��������������*/
	uint16_t LCD_ConnectRMax;								/*��������������*/
	int16_t LCD_TempMax;									/*�¶�����*/
	int16_t LCD_TempMin;									/*�¶�����*/
	uint16_t LCD_AutoMeaRTime;								/*�Զ�����������*/
}LCD_GROUP_CFG;

typedef struct
{
	uint16_t LCD_CellGroupNum;								/*����������*/
	uint16_t LCD_CellNum1;									/*1����������*/
	uint16_t LCD_CellNum2;									/*2����������*/
	LCD_GROUP_CFG LcdGroupCfg[MAX_GROUP_NUM];
	uint16_t LCD_GpsTimeCalEN;								/*ʹ��GPS��ʱ*/
	uint16_t LCD_HostHWVersion;								/*����Ӳ���汾��*/
	uint16_t LCD_HostSWVersion;								/*��������汾��*/
}LCD_SYS_CFG;


/*********************ϵͳʱ�ӼĴ���**************************/
typedef struct
{
	uint16_t Year;												/*��*/
	uint16_t Month;												/*��*/
	uint16_t Day;													/*��*/
	uint16_t Hour;												/*ʱ*/
	uint16_t Minute;												/*��*/
	uint16_t Second;												/*��*/
}LCD_SYS_TIME;


/*********************ϵͳ���ƼĴ���**************************/
typedef struct
{
	uint16_t LCD_StartCellMea;								/*�������Ѳ��*/
	uint16_t LCD_StartConnectMea;							/*����������Ѳ��*/
	uint16_t LCD_StartTempMea;								/*�����¶�Ѳ��*/
	uint16_t LCD_StartInterMea;								/*�����������*/
}LCD_SYS_CTRL;

/*********************У׼�����Ĵ���**************************/
typedef struct
{
	fp32 LCD_CellVoltageB;									/*���ص�ѹ�ɼ�ͨ��Bֵ*/
	fp32 LCD_CellVoltageK;									/*���ص�ѹ�ɼ�ͨ��Kֵ*/
	fp32 LCD_ConnectVoltageB;								/*��������ѹ�ɼ�ͨ��Bֵ*/
	fp32 LCD_ConnectVoltageK;								/*��������ѹ�ɼ�ͨ��Kֵ*/
	fp32 LCD_CurrentB;											/*�ŵ������ѹ�ɼ�ͨ��Bֵ*/
	fp32 LCD_CurrentK;											/*�ŵ������ѹ�ɼ�ͨ��Kֵ*/
}LCD_CAL_PARA;


/*********************ͨѶ�����Ĵ���**************************/
#define LCD_MAX_USART_NUM 5

typedef struct
{
	struct
	{
		uint16_t BaudRate;									/*������*/
		uint16_t ComAddr;										/*���˿ڵ�ַ*/
		uint16_t SlaveAddr;									/*�ӻ���ַ*/
		uint16_t Reserve1;
		uint16_t Reserve2;
		uint16_t Reserve3;	
	}CanPara;
	struct
	{
		uint16_t BaudRate;									/*������*/
		uint16_t ComAddr;										/*���˿ڵ�ַ*/
		uint16_t ComProtocal;								/*ͨѶЭ��*/
		uint16_t Purpose;										/*��;*/
		uint16_t Reserve1;
		uint16_t Reserve2;
	}UsartPara[LCD_MAX_USART_NUM];							
}LCD_COM_PARA;
typedef	struct
{
	int16_t TotalPageNUM;		//����������ҳ��
	int16_t ErrDatePageNO;		//��ǰ����ʾ�Ĺ�������ҳ��
}ERR_DATA;


typedef	struct
{
	int16_t TotalPageNUM;			//��ʷ��¼��ҳ��
	int16_t HisRecordPageNO;		//��ǰ����ʾ����ʷ��¼ҳ��
	
}HIS_RCD;


typedef struct
{
	ERR_DATA ErrData;
	HIS_RCD HisRcd;
}ERR_AND_HISTORY;



typedef struct
{
	uint16 HisMeaNOFlag[MAX_GROUP_NUM];			//��ʾ��N����ʷ������Ϣ��־
	uint16 MeaYear;								//����ʱ��-��
	uint16 MeaMon;								//����ʱ��-��
	uint16 MeaDay;								//����ʱ��-��
	uint16 MeaHour;								//����ʱ��-ʱ
	uint16 MeaMin;								//����ʱ��-��
	uint16 MeaSec;								//����ʱ��-��
	uint16 HisInnerRData[MAX_BAT_NUM];	//��������
	uint16 HisCRData[MAX_BAT_NUM];		//����������
	uint16 HisVoltageData[MAX_GROUP_BAT_NUM];	//��ѹ����
	int16 HisTempData[MAX_GROUP_BAT_NUM];		//�¶�����
}HIS_MEA_DATA;

#define NOUSBDEVICE 	0			//��USB�豸
#define DEVICEATTACH	1			//��USB�豸

typedef enum
{
	NODATAOUTPUT=0,			//USBû���������
	DATAOUTPUTTOUSB=1,			//USB���������
  DATAOUTPUTOVER=2,			//USB����������
  NODATA=3,					//�޲�����¼
}DATA_OUTPUT_STAT;

typedef struct
{
	uint8 UsbDeviceFlag;			//����USB�豸��ʶ
	uint8 UsbOutputData;			//����USB���������ʶ
	uint8 UsbOutReportData;			//�������Ա���
	uint8 OutputDataProgress;		//�������ݽ���
	uint8 VOutputFlag;				//��ѹ���ݵ�����ʶ
	uint8 DiscReportOutput;				//�����ŵ���Ա���
	uint8 DiscDataOutFlag;				//�ŵ�ʵʱ���ݵ���
	uint8 BatVCurrentOutput;			//��ص�ѹ��������
	uint16 OutputTime;				//����ʱ��������λΪ����
	uint32 DataOutputTimeFlag;		//��һ�����ݵ���ʱ��
	
}USB_DEVICE;

typedef struct
{
	uint16 SaveAddrOffset;//���һ�δ洢λ��
	uint16 TotalSaveTimes;//�ܹ��洢����
	uint16 DiscSaveAddrOffset;//���һ�δ洢λ��
	uint16 TotalDiscSaveTimes;//�ŵ������ܹ��洢����
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
   uint16 MeaYear;								//�������ʱ��-��
	uint16 MeaMon;								//�������ʱ��-��
	uint16 MeaDay;								//�������ʱ��-��
	uint16 MeaHour;								//�������ʱ��-ʱ
	uint16 MeaMin;								//�������ʱ��-��
	uint16 MeaSec;								//�������ʱ��-��
}INNER_R_TEST_TIME;

typedef enum
{
	TEST_IDLE=0,
	TEST_V_LOW,//���Ƿѹ
	TEST_GROUP_V_OVER,//������ѹ
	TEST_GROUP_V_LOW,//�����Ƿѹ
	TEST_DISCHARGE,//��طŵ���
	TEST_BUSY,//����Ƶ��
	TEST_NO_FUNCTION,//�������������
}TEST_CONDITION;


typedef struct
{
   uint8 InnerRTestDevNO;	//��ǰ������Ե�ģ����
   uint8 InnerRTestFlag[MAX_GROUP_NUM];	//������������Ա�־��2��ʾ������ɣ�1��ʾ������,0��ʾ������ֹ��3��ʾ����ʧ�ܣ�4��ʾ�ŵ���쳣��5��ʾƵ��
   uint8 LastTestTimesFlag[MAX_GROUP_NUM];//���������һ������Ĳ��Դ洢λ��,�ñ�����ÿ�ν���������Ϣ����ʱ��ʾ�ôβ�����Ϣ
   uint8 TotalTestTimes[MAX_GROUP_NUM];//�ܲ�������
   uint8 RDataReadFlag[MAX_GROUP_NUM][MAX_DEV_NUM];//�����д��־
   uint8 CRDataReadFlag[MAX_GROUP_NUM][MAX_DEV_NUM];//��������д��־
   uint8 TestConditionFlag[MAX_GROUP_NUM];//�������������־
   uint16 TestTimePeriod[MAX_GROUP_NUM];//ģ��֮��Ĳ���ʱ��������λΪ��
   uint16 TestFailFlag[MAX_GROUP_NUM];	//�������ʧ�ܱ�ʶ������������ʾʹ�ã��ϵ����������Գɹ���ָ�������λ����ţ�����λ��Ѳ��ģ���ţ����λ��ʧ��ԭ��
	INNER_R_TEST_TIME TestTime[MAX_GROUP_NUM];
}INNER_TEST;

#define DISCHARGE_CURRENT	-1.5
#define FLOAT_CHARGE_CURRENT	1
#define EQ_CHARGE_CURRENT	1


typedef enum
{
	IDLE=0,//��·
	BATFLOATCHARGE,//����
	BATEQCHARGE,//����
	BATDISCHARGE,//�ŵ�
}BAT_STAT;

typedef enum
{
	DIS_NORMAL = 0,//�ŵ�����
	GROUPVLOW,//���ѹǷѹ
	BATVLOW,//��ص�ѹǷѹ
}DIS_STAT_FLAG;

//�ŵ�״̬
typedef struct
{
	uint8 DisChargeFlag;

}DIS_STAT;

typedef struct
{
	uint16 BatStat[MAX_GROUP_NUM];//���䣬���䣬�ŵ�
	float GroupV[MAX_GROUP_NUM];//���ѹ
	float GroupI[MAX_GROUP_NUM];//��ŵ����
	uint16 SingleBatMaxV[MAX_GROUP_NUM][5];//��������ѹ
	uint16 SingleBatMinV[MAX_GROUP_NUM][5];//������С��ѹ
	uint16 MaxVNO[MAX_GROUP_NUM][5];//����ѹ���
	uint16 MinVNO[MAX_GROUP_NUM][5];//��С��ѹ���
	uint16 BatMaxR[MAX_GROUP_NUM];//�������
	uint16 BatMinR[MAX_GROUP_NUM];//��С����
	uint8 MaxRNO[MAX_GROUP_NUM];//���������
	uint8 MinRNO[MAX_GROUP_NUM];//��С������
	uint16 BatMaxCR[MAX_GROUP_NUM];//�������
	uint16 BatMinCR[MAX_GROUP_NUM];//��С����
	uint8 MaxCRNO[MAX_GROUP_NUM];//���������
	uint8 MinCRNO[MAX_GROUP_NUM];//��С������
	float BatMaxTemp[MAX_GROUP_NUM];//��������¶�
	float BatMinTemp[MAX_GROUP_NUM];//������С�¶�
	uint16 MaxTempNO[MAX_GROUP_NUM];//����¶ȱ��
	uint16 MinTempNO[MAX_GROUP_NUM];//��С�¶ȱ��
	float MaxTempDev[MAX_GROUP_NUM];//��󻷾��¶�
	uint16 MaxTempDevNO[MAX_GROUP_NUM];//��󻷾��¶ȱ��
	float MinTempDev[MAX_GROUP_NUM];//��С�����¶�
	uint16 MinTempDevNO[MAX_GROUP_NUM];//��С�����¶ȱ��
}BAT_SYS_STAT;

typedef struct
{
	uint8 BatEQFlag[MAX_GROUP_NUM];//����������־��0��ʾ��ֹ���⣬1��ʾ��������
}BAT_EQ;//��ؾ���ṹ��

typedef struct
{
	int32_t VoltageAD; //ADֵ
	int32_t Current1AD;
	int32_t Current2AD;
	int32_t Current3AD;
}CLI_AD;

typedef struct
{
	float VoltageValue; //ʵ��ֵ
	float Current1Value;
	float Current2Value;
	float Current3Value;
}CLI_VALUE;

typedef struct
{
	uint8 ADReadTimes;			// 1��ʾУ׼��1��ADֵ��ȡ��2��ʾУ׼��2��ADֵ��ȡ
	CLI_AD CliAd[2];
	CLI_VALUE CliValue[2];
}GROUP_CLI_INFO;



typedef enum
{
	R_TEST_STOP = 0,//������ֹ
	R_TEST_ING = 1,//������
	R_TEST_FAIL = 2,//����ʧ��
	R_TEST_SUCCESS = 3,//�������
	DIS_MODULE_ERR = 4,//�ŵ���쳣
	R_TEST_BUSY = 5,//����Ƶ��
	DATA_SAVING = 6,//���ݱ�����
	DATA_SAVE_SUCCESS = 7,//���ݱ���ɹ�
	DATA_SAVE_FAIL = 8,//���ݱ���ʧ��
	R_TEST_PAUSE = 9,//������ͣ��
	R_TEST_FAIL_I_LOW = 0x21,//������С
	R_TEST_FAIL_I_OVER = 0x22,//��������
	R_TEST_FAIL_STOP = 0x23,//�ֶ�ֹͣ
	R_TEST_FAIL_V_LOW = 0x24,//���ѹ��С
	R_TEST_FAIL_V_OVER = 0x25,//���ѹ����
	R_TEST_FAIL_BATV_LOW = 0x26,//��ص�ѹ����
	R_TEST_FAIL_BATV_OVER = 0x27,//��ص�ѹ����
	R_TEST_FAIL_TEMP_LOW = 0x28,//�¶ȹ��ͣ����¸й��ϣ�Ϊ0xFAFA
	R_TEST_FAIL_TEMP_OVER = 0x29,//�¶ȹ���
}R_TEST_STAT;

//typedef struct
//{
	//float Soc;		//���ʣ������
	//float Soh;		//����״̬
	//float Sof;		//����״̬
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


/*********************��ͨ��Э�鹦����**************************/
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
    LCD_RCV_UNDONE = 0,                                 /* ����δ��� */
    LCD_RCV_FUCTION_ERR = 1,                            /* ������������� */
    LCD_RCV_REG_ADD_ERR = 2,                            /* ���յ�ַ�� */
    LCD_RCV_REG_NUM_ERR = 3,                            /* ���������� */	
    LCD_RCV_REG_DATA_ERR = 4,                           /* �������ݴ� */	
    LCD_RCV_REG_DATA_SAVE_ERR = 5, 			/* ����洢�� */	
    LCD_RCV_REG_DATA_READ_ERR = 6, 			/* ����洢�� */
	
    LCD_RCV_COMM_ADD_ERR = 14,                          /* ����ͨѶ��ַ�� */
    LCD_RCV_LEN_ERR = 15,                               /* ���ճ��ȴ� */
    LCD_RCV_CRC_ERR = 16,                               /* ����CRC�� */
    LCD_RCV_OK = 0xff,                                  /* �������� */
	
}LCD_ERROR_CODE;


/********************�ֻ����ڴ���ʹ�����ݽṹ****************/
#define  LCDCOMM_FAIL_NUM        30                             /*ͨѶʧ��������*/
#define  BROADCAST_ADDR          0xFF                           /*�㲥��ַ*/
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

