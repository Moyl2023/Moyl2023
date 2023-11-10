#ifndef _MESSAGE_H
#define _MESSAGE_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"



//typedef enum
//{
//	MSG_NONE = 0,         	/*空，不能作为消息*/
//	MSG_SAVE_PARA,          /*保存参数 */ 
//	MSG_SAVE_PARA_ERR,      /*保存参数失败*/
//	MSG_PARAREAD_REG,       /*读参数寄存器,返回指定页面*/
//	MSG_PARAREAD_OK,        /*读寄存器成功*/
//	MSG_BUSREAD_REG,        /*读取寄存器或写单个寄存器*/
//	MSG_INITREAD_REG,       /*读取初始化配置*/
//	MSG_INITREAD_OK,        /*读取初始化配置成功*/
//	MSG_ADJREAD_REG,		/*校准读取请求*/
//	MSG_STATUS_READ,        /*母线数据及状态量读取*/
//	MSG_TIME_CALIBRA,       /*时钟校准*/
	
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
	uint8	Valid; /*0无效 1有效*/
	uint8    MsgType;		/*表示命令功能,信瑞达通信中表示消息码*/
	uint8    MsgLenth;		/*表示有几条命令,*/
	uint8    MsgCOM;		/*设备通信端口号*/
	uint8    MsgDevAddr;	/*设备地址,信瑞达通信中表示电池编号*/
	uint8    MsgData[100];/*Data[0]:功能码Data[1](地址高8位) Data[2](地址低8位) :读写数据的起始地址
						//Data[3]:读的指令条数,写命令时表示要写的寄存器数量 Data[4](作为读取命令时为要读取的寄存器数量) .....:数据 */
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
