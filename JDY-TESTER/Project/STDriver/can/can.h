/****************************************Copyright (c)**************************************************
**                               深圳奥特迅电力设备股份有限公司
**                                     	   研发中心
**                                                                 
**
**文件名: AtcCanServer.h     
**
**作者:  RM       版本:v1.0        日期: 2007/9/20
**
**描述:    			
**
**		LPC2292 CAN 在uCOS-II下的底层驱动功能服务头文件
**
**历史记录:        


****************************************************************************************************/

#ifndef _CAN_H
#define _CAN_H

#include "stm32f10x_can.h"
#include "config.h"
#include "queue.h"

#ifdef  __cplusplus
extern  "C" {
#endif

#define ERROR   -1

#define CAN_1	1	//can 1
#define CAN_2	2 	//can 2

#define CAN_DEVICE_NUMS 2	/*numbers of can*/

#define RCC_APB2Periph_GPIO_CAN1    RCC_APB2Periph_GPIOD
#define GPIO_Remap_CAN_1            GPIO_Remap2_CAN1
#define GPIO_CAN1                   GPIOD  
#define GPIO_Pin_CAN1_RX            GPIO_Pin_0
#define GPIO_Pin_CAN1_TX            GPIO_Pin_1

//RM ADD 
#define RCC_APB2Periph_GPIO_CAN2    RCC_APB2Periph_GPIOB
#define GPIO_Remap_CAN_2             GPIO_Remap_CAN2
#define GPIO_CAN2                   GPIOB  
#define GPIO_Pin_CAN2_RX            GPIO_Pin_5
#define GPIO_Pin_CAN2_TX            GPIO_Pin_6

/*CAN LED*/
typedef enum 
{
  CANRX1 = 0,
  CANTX1 = 1,
  CANRX2 = 2,
  CANTX2 = 3,
} CANLED_TypeDef;
#define CANLED_NUM					4
#define CANLEDRX1_GPIO_PORT			GPIOA
#define CANLEDRX1_GPIO_CLK          RCC_APB2Periph_GPIOA  
#define CANLEDRX1_GPIO_PIN          GPIO_Pin_11

#define CANLEDTX1_GPIO_PORT			GPIOA
#define CANLEDTX1_GPIO_CLK          RCC_APB2Periph_GPIOA  
#define CANLEDTX1_GPIO_PIN          GPIO_Pin_8

#define CANLEDRX2_GPIO_PORT			GPIOC
#define CANLEDRX2_GPIO_CLK          RCC_APB2Periph_GPIOC  
#define CANLEDRX2_GPIO_PIN          GPIO_Pin_9

#define CANLEDTX2_GPIO_PORT			GPIOA
#define CANLEDTX2_GPIO_CLK          RCC_APB2Periph_GPIOA  
#define CANLEDTX2_GPIO_PIN          GPIO_Pin_12

#define CAN_TYPE_ALL_RCV	1 /*CAN 全接收*/
#define CAN_TYPE_GROUP_RCV	2 /*CAN 组接收*/
#define CAN_TPYE_FILTE	3 /*CAN 滤波*/

#define	CAN_FRAME_STANDARD	0 /*FF:standard frame type*/
#define	CAN_FRAME_EXTERNED	1  /*FF:externed frame type*/
#define CAN_DATA_TYPE_FRAME  0 /*RTR:data frame*/ 
#define CAN_REMOTE_TYPE_FRAME  1 /*RTR:REMOTE frame*/

/* Can device  controls */
#define CAN_BAUD_SET		0x1003		//baudrate set
#define CAN_BAUD_GET		0x1004		//baudrate get
#define CAN_PROTOCOL_SET	0x1005
#define CAN_FRAME_SET	0x1007
#define CAN_RCV_TYPE_SET	0x1009		//receive fitler type set
#define CAN_ADD_SET			0x1010	    //address set
#define CAN_RCV_PEND_SET			0x1011	    //receive pend type set
#define CAN_RCV_PEND_GET			0x1012	    //receive pend type get

#define CAN_RCV_SEM_PEND_EN		1	// 1-使能接收阻塞，0-禁止接收阻塞模式

/**   CAN sioMODE 
sioMode[0]: 1 - 接收阻塞模式，0-接收非阻塞模式  
***/
#define  CAN_SIOMODE_RCV_PEND_EN	1	//接收阻塞模式


#define CAN_FITER_NO   14  //CAN fiter no


/** 
* @brief  CAN message structure definition  
*/
typedef struct
{
	union {
		uint32 StdId;  /*!< Specifies the standard identifier.
					   This parameter can be a value between 0 to 0x7FF. */

		uint32 ExtId;  /*!< Specifies the extended identifier.
					   This parameter can be a value between 0 to 0x1FFFFFFF. */
	}Id;
	struct{
		uint8 FF:1;     /*!< Specifies the type of identifier for the message that will be transmitted.
						This parameter can be a value of @ref CAN_identifier_type */

		uint8 RTR:1;     /*!< Specifies the type of frame for the message that will be transmitted.
										 This parameter can be a value of @ref CAN_remote_transmission_request */
	}FrmDef;		  /*frame define*/

	uint8 DLC;     /*!< Specifies the length of the frame that will be transmitted.
				   This parameter can be a value between 0 to 8 */

	uint8 Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 to 0xFF. */
}CAN_MSG,*P_CAN_MSG;

#define MAX_CAN_RCV_PK_BUF_NUM	20	/*can rcv package numbers in buffer*/
#define MAX_CAN_SEND_PK_BUF_NUM	20	/*can send package numbers in buffer*/
#define MAX_CAN_SEND_BUF_LENTH   (sizeof(DataQueue)+MAX_CAN_SEND_PK_BUF_NUM*sizeof(CAN_MSG))   /*can send buf*/
#define MAX_CAN_RCV_BUF_LENTH    (sizeof(DataQueue)+MAX_CAN_RCV_PK_BUF_NUM*sizeof(CAN_MSG))   /*can rcv buf*/


/* Device Initialization Structure */
typedef struct {
	void*	base;	          // register base address
	uint8	devicType;		      // CAN_TYPE_ALL_RCV master ,=CAN_TPYE_FILTE slave
	uint32	defBuardRate;         // 默认波特率 
} STM32F10x_SIO_CAN_PARAM;

typedef struct {
	uint8 	sendBuf[MAX_CAN_SEND_BUF_LENTH]; // send    buf
	uint8	rcvBuf[MAX_CAN_RCV_BUF_LENTH];  // receive buf
	uint8	deviceOpenFlag;	             // device flag opened
	uint8	devicType;		             /*CAN_TYPE_ALL_RCV master ,=CAN_TPYE_FILTE slave*/
 	uint8   frameType;		             /*CAN_FRAME_STANDARD= standard identifiy , CAN_FRAME_EXTERNED= extended identifiy*/
	OS_EVENT *canLock;	                 // devece mutex lock
	VOID*	base;	                     // register base address

#if CAN_RCV_SEM_PEND_EN > 0
	VOID*	pRcvSem;					 //recive sem
#endif

	uint32	deviceID;                    // device ID 
	uint32   baudRate;                    // baud rate 

	int32	sioMode;                     // SIO mode
	int32   options;                     // SIO options
	
	uint32  sendTimer;                   // CAN发送超时计数器
	uint32  canSelfJiNo;				 //can device address no
	uint32  canID;						//can ID:   EID[28-0]  IDE   RTR  保留
										//			D[31-3]	  D[2]	D[1] D[0]
	uint32	canMaskId;					//can mask ID: EID[28-0]  IDE   RTR  保留
										// 			   D[31-3]	  D[2]	D[1] D[0]
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	//can filter init structure
	CAN_InitTypeDef        CAN_InitStructure;	//can init structure
}STM32F10x_CAN_CHAN,*STM32F10x_CAN_PCHAN;

/*
FF-帧格式 1=EFF(扩展帧) ;
RTR远程帧 0=数据帧
*/


#define  CAN_FRAME_TYPE_REMOTE_FRAME	1    //远程帧
#define  CAN_FRAME_TYPE_DATD_FRAME	    0    //数据帧
#define  CAN_FRAME_FORMAT_STD		    0	 //标准帧
#define  CAN_FRAME_FORMAT_EFF		    1	 //扩展帧




 #if 1
extern void CANPowerOnInit(void); /*is  starting at first in power on*/

extern int32 Can_Open(uint32 canId); /*open the CAN,id is canId*/

extern int32 Can_Close(int32 devDesc); /*close can device. the devDesc is value of CAN_Open(uint32 canId) return */

#if 0
extern int32 CAN_Init(int devDesc,	/*device discription*/
			uint32 fenJiNo,	/*fen ji No*/
			uint32 baudRate,	/*baud rate*/
			uint8 devType,	/*fen ji type*/
			uint8 frameType, 	/*frame type*/
			uint8 protocolType	/*PROTOCOL*/
			);
#endif

extern int32 Can_Write(int32 devDesc,	/*device discription*/
				CAN_MSG* msg	/*can msg*/
				);

extern int32 Can_Read(int32 devDesc,	/*device discription*/
				CAN_MSG* buf,	/*data buf*/
				uint32 timeOut /*time out*/
				);

extern int Can_Ctrl(int32 devDesc,	/*device description*/
			int32 cmd,	/*request cmd*/
			void* arg,	/*some argument1*/
			void* arg2,	/*some argument2*/
			void* arg3	/*some argument3*/
			);

#endif

#ifdef  __cplusplus
}
#endif


#endif /*_CAN_H*/

