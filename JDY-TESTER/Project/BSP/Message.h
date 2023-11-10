#ifndef _MESSAGE_H
#define _MESSAGE_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"



//typedef enum
//{
//	MSG_NONE = 0,         	/*�գ�������Ϊ��Ϣ*/
//	MSG_SAVE_PARA,          /*������� */ 
//	MSG_SAVE_PARA_ERR,      /*�������ʧ��*/
//	MSG_PARAREAD_REG,       /*�������Ĵ���,����ָ��ҳ��*/
//	MSG_PARAREAD_OK,        /*���Ĵ����ɹ�*/
//	MSG_BUSREAD_REG,        /*��ȡ�Ĵ�����д�����Ĵ���*/
//	MSG_INITREAD_REG,       /*��ȡ��ʼ������*/
//	MSG_INITREAD_OK,        /*��ȡ��ʼ�����óɹ�*/
//	MSG_ADJREAD_REG,		/*У׼��ȡ����*/
//	MSG_STATUS_READ,        /*ĸ�����ݼ�״̬����ȡ*/
//	MSG_TIME_CALIBRA,       /*ʱ��У׼*/
	
//} MSG_TYPE;

typedef enum
{
	MSG_NONE = 0,         	
	MSG_RD_REG, 
	MSG_WR_REG,
	MSG_PARAREAD_REG,
} MMSG_TYPE;


typedef struct 
{
	uint8	Valid; /*0��Ч 1��Ч*/
	uint8    MsgType;		/*��ʾ�����,�����ͨ���б�ʾ��Ϣ��*/
	uint8    MsgLenth;		/*��ʾ�м�������,*/
	uint8    MsgCOM;		/*�豸ͨ�Ŷ˿ں�*/
	uint8    MsgDevAddr;	/*�豸��ַ,�����ͨ���б�ʾ��ر��*/
	uint8    MsgData[100];/*Data[0]:������Data[1](��ַ��8λ) Data[2](��ַ��8λ) :��д���ݵ���ʼ��ַ
						//Data[3]:����ָ������,д����ʱ��ʾҪд�ļĴ������� Data[4](��Ϊ��ȡ����ʱΪҪ��ȡ�ļĴ�������) .....:���� */
}MSG_STRUCT;


extern BOOL  ATC_QueueCreat(uint8 TaskPrio);
extern BOOL RcvMsgFromQueue(MSG_STRUCT* outmsg);
extern BOOL  SendMsg2Queue(MSG_STRUCT *msg, uint8 DestPrio);
extern void ClearMsgQueue(uint8 TaskPrio);
void DelQueue(uint8 com);

#ifdef  __cplusplus
}
#endif
#endif
