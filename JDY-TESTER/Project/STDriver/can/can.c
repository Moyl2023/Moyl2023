
/****************************************Copyright (c)**************************************************
**                               深圳奥特迅电力设备股份有限公司
**                                     	   研发中心
**                                                                 
**
**文件名: CAN.C     
**
**作者:  RM       版本:v1.0        日期: 2010/5/20
**
**描述:    			
**
**		STM32F10XPF CAN 在uCOS-II下的底层驱动功能服务文件
**
**历史记录:        


****************************************************************************************************/
//#include "string.h"
//#include "config.h"


#include "stm32f10x.h"
#include "stm32f10x_can.h"

#include "config.h"

#include "queue.h"
#include "can.h"

/* CAN Filter Master Register bits */
#define FMR_FINIT    ((uint32_t)0x00000001) /* Filter init mode */

/* CAN Receive FIFO  Register bits */
#define RF0R_FULL   ((uint32_t)0x00000008)    /* FIFO 0 full */
#define RF0R_FOVR   ((uint32_t)0x00000010)    /* FIFO 0 overrun */

/*CAN-TSR TME0-2 shift*/
#define TME0_2_SHIFT	26	

/*CAN-TSR TME0-2*/
#define TME0_2   ((uint32_t)0x00000003 << TME0_2_SHIFT)    /* FIFO 0 full */

//CAN defualt parameter
static STM32F10x_SIO_CAN_PARAM CANParameter[] = {
	  {(void *)CAN1, CAN_TYPE_GROUP_RCV/*CAN_TYPE_ALL_RCV*/, 250000},
	  {(void *)CAN2, CAN_TYPE_GROUP_RCV, 250000},
};

/*CAN LED*/
GPIO_TypeDef* CANLED_GPIO_PORT[CANLED_NUM] = {CANLEDRX1_GPIO_PORT, CANLEDTX1_GPIO_PORT, CANLEDRX2_GPIO_PORT,
                                 CANLEDTX2_GPIO_PORT};
const uint16_t CANLED_GPIO_PIN[CANLED_NUM] = {CANLEDRX1_GPIO_PIN, CANLEDTX1_GPIO_PIN, CANLEDRX2_GPIO_PIN,
                                 CANLEDTX2_GPIO_PIN};
const uint32_t CANLED_GPIO_CLK[CANLED_NUM] = {CANLEDRX1_GPIO_CLK, CANLEDTX1_GPIO_CLK, CANLEDRX2_GPIO_CLK,CANLEDTX2_GPIO_CLK
                                 };

#define DEFAULT_FENJI_NO	1   //默认分机号
#define CAN_ADD_START_BIT	11	//根据协议  地址位置为ID[15-8]
#define CAN_ADD_ALL_BIT		(0xff<<CAN_ADD_START_BIT)   //根据协议需要
#define CANLED_ON			Bit_RESET
#define CANLED_OFF		   	Bit_SET

__align(8) static STM32F10x_CAN_CHAN CANChan[CAN_DEVICE_NUMS];
static uint8 PowerOnFirst = FALSE;


RCC_ClocksTypeDef  RCC_Clocks[CAN_DEVICE_NUMS];


static BOOL CANDevDefInit(STM32F10x_CAN_CHAN* pChan);
static VOID CanTxMsgUpConverDown(CanTxMsg* txmsg,CAN_MSG* msg);
static void CANLED_Init(void);

/*************************************************
CANInit - is  starting at first in power on.

writer:rm

description:

RETURN: N
*************************************************/
void CANPowerOnInit()
{
	int8 i;
	if(PowerOnFirst == FALSE)
	{
		for(i=0;i<CAN_DEVICE_NUMS;i++)
		{	
			CANChan[i].deviceOpenFlag = FALSE;	
			CANChan[i].devicType = CANParameter[i].devicType;
			CANChan[i].frameType = CAN_FRAME_EXTERNED;
			CANChan[i].baudRate = CANParameter[i].defBuardRate;
			CANChan[i].base = 	CANParameter[i].base;
			CANChan[i].canLock = OSSemCreate(1);  
			CANChan[i].deviceID = i+1; //open device ID
			CANChan[i].sendTimer = 0;	
			CANChan[i].canSelfJiNo = DEFAULT_FENJI_NO;
			CANChan[i].canID = (DEFAULT_FENJI_NO<<CAN_ADD_START_BIT)|0x4;		   //根据《网络转换器协议V1.0》	 默认地址
			CANChan[i].canMaskId = (0xFF<<CAN_ADD_START_BIT)|0x4;		   //根据《网络转换器协议V1.0》	  ID15-8匹配  扩展帧  数据帧
			CANChan[i].sioMode = 0;
			QueueCreate(CANChan[i].rcvBuf, MAX_CAN_RCV_BUF_LENTH,sizeof(CAN_MSG),0,0);
			QueueCreate(CANChan[i].sendBuf, MAX_CAN_SEND_BUF_LENTH,sizeof(CAN_MSG),0,0);
#if CAN_RCV_SEM_PEND_EN > 0
			CANChan[i].pRcvSem = OSSemCreate(0);					 //recive sem
#endif
			CANChan[i].canLock = OSSemCreate(1);
			CANDevDefInit(&CANChan[i]);
		}
		CANLED_Init();
		PowerOnFirst = TRUE;
	}
}

/***********CANLED******************/
void CANLEDIoInit(CANLED_TypeDef inputn)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(CANLED_GPIO_CLK[inputn], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = CANLED_GPIO_PIN[inputn];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(CANLED_GPIO_PORT[inputn], &GPIO_InitStructure);
}


//取反
void CANLED_Toggle(CANLED_TypeDef Led)
{
	CANLED_GPIO_PORT[Led]->ODR ^= CANLED_GPIO_PIN[Led];
}

/****************************************************************************************
** 函数名称: WriteCanLedDataBit(CANLED_TypeDef no,BitAction val)
** 功能描述: 1路输出
** 输　入: no:序号
			val:Bit_RESET = 0,
  				Bit_SET	 = 1
** 输　出: 无
** 全局变量:
** 描述:   写对应输出值
******************************************************************************************/
void WriteCanLedDataBit(CANLED_TypeDef no,BitAction val)
{
	GPIO_WriteBit(CANLED_GPIO_PORT[no],CANLED_GPIO_PIN[no],val);
}

/****************************************************************************************
** 函数名称:  CANLED_IO_Init(void)
** 功能描述:  CANLED初始化
** 输　入: 无
** 输　出: 无
** 全局变量:
** 描述:  
******************************************************************************************/
void CANLED_Init()
{
	uint8 i;
	for(i=0;i<CANLED_NUM;i++)
	{
		CANLEDIoInit((CANLED_TypeDef)i);
		WriteCanLedDataBit((CANLED_TypeDef)i,CANLED_OFF);
	}	
}

/*************************************************
CANBitStructFill - Fill bit time

PARA:
NOTE:
	sample point
	75%      when 波特率 > 800K
	80%      when 波特率 > 500K
	87.5%    when 波特率 <= 500K
	在1个位周期中时间份额的数量必须是8～25之间的
	速率/Kbps 1000 500 250 125 100 50   20    10
	距离/m      40 130 270 530 620 1300 3300 6700
RETURN: TRUE-successful ,or-FALSE
*************************************************/
static BOOL CANBitStructFill(CAN_InitTypeDef* can_initStruct,uint32 baudrate)
{
	BOOL ret = TRUE;
	switch(baudrate)
	{
	case 10000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 200;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
   		break;
	case 50000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 40;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
   		break;
	case 100000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 20;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
   		break;
	case 125000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 15;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
   		break;
	case 250000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;//CAN_BS1_8tq;		//	CAN_BS1_14tq
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;//CAN_BS2_7tq;	    //CAN_BS2_3tq
		can_initStruct->CAN_Prescaler = 8;//9;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
   		break;
	case 500000:
	   	can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 4;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
	//	ret = TRUE;
   		break;
	case 1000000:
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_6tq;
		can_initStruct->CAN_BS2 = CAN_BS2_2tq;
		can_initStruct->CAN_Prescaler = 4;	   //(n)*tpclk
	//	ret = TRUE;
   		break;
	default:	  //default 100K
		can_initStruct->CAN_SJW = CAN_SJW_1tq;
		can_initStruct->CAN_BS1 = CAN_BS1_14tq;
		can_initStruct->CAN_BS2 = CAN_BS2_3tq;
		can_initStruct->CAN_Prescaler = 20;	   //(n)*tpclk  	  //1/tpclk = 36MHZ
		return FALSE;
   }
   return ret;

 /*
   if((baudrate > 1000000) || (CAN_InitStruct == NULL))
   {
		return FALSE;
   }
   if(baudrate > 800000)
   {
		
   }else if(baudrate > 500000)
   {
		CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
		CAN_InitStructure.CAN_BS1 = CAN_BS1_15tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
   }else if(baudrate <= 500000)
   {
		CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
   }
   return ret;
   */
}

/*************************************************
CANFilterStructFill - Fill filter struct

PARA:
NOTE:
RETURN: TRUE-successful ,or-FALSE
*************************************************/
BOOL CANFilterStructFill(STM32F10x_CAN_CHAN* pChan,uint32 type,uint32 canid,uint32 maskid,uint8 fiterno)
{
	BOOL ret = FALSE;
	if(type == CAN_TYPE_ALL_RCV)
	{	// 全接收
		pChan->CAN_FilterInitStructure.CAN_FilterNumber=fiterno;
		pChan->CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
		pChan->CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
		pChan->CAN_FilterInitStructure.CAN_FilterIdHigh = (canid>>16) & 0xffff;;
		pChan->CAN_FilterInitStructure.CAN_FilterIdLow = canid & 0xffff;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
		pChan->CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
//		pChan->devicType = type;
		ret = TRUE;
	}else if (type == CAN_TYPE_GROUP_RCV)
	{// 组接收
		pChan->CAN_FilterInitStructure.CAN_FilterNumber=fiterno;
		pChan->CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
		pChan->CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
		pChan->CAN_FilterInitStructure.CAN_FilterIdHigh = (canid>>16) & 0xffff;;
		pChan->CAN_FilterInitStructure.CAN_FilterIdLow = canid & 0xffff;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (maskid>>16) & 0xffff;;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdLow = maskid & 0xffff;
		pChan->CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	//	pChan->devicType = type;
		ret = TRUE;
	}
	else if (type == CAN_TPYE_FILTE)
	{
		pChan->CAN_FilterInitStructure.CAN_FilterNumber=fiterno;
		pChan->CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
		pChan->CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
		pChan->CAN_FilterInitStructure.CAN_FilterIdHigh = (canid>>16) & 0xffff;;
		pChan->CAN_FilterInitStructure.CAN_FilterIdLow = canid & 0xffff;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (maskid>>16) & 0xffff;;
		pChan->CAN_FilterInitStructure.CAN_FilterMaskIdLow = maskid & 0xffff;
		pChan->CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	//	pChan->devicType = type;
		ret = TRUE;
	}
	if(ret == TRUE)
	{
		pChan->devicType = type;
		pChan->canID = 	canid;
		pChan->canMaskId = 	maskid;
	}
	return ret;
}

/*************************************************
CANFilterStructInitFill - Fill filter struct

PARA:
NOTE:
RETURN: TRUE-successful ,or-FALSE
*************************************************/
BOOL CANFilterStructInitFill(STM32F10x_CAN_CHAN* pChan,CAN_FilterInitTypeDef* can_filterInitStruct,uint8 fiterno)
{
	if(pChan->devicType == CAN_TYPE_ALL_RCV)
	{	// 全接收
		can_filterInitStruct->CAN_FilterNumber=fiterno;
		can_filterInitStruct->CAN_FilterMode=CAN_FilterMode_IdMask;
		can_filterInitStruct->CAN_FilterScale=CAN_FilterScale_32bit;
		can_filterInitStruct->CAN_FilterIdHigh=0x0000;
		can_filterInitStruct->CAN_FilterIdLow=0x0000;
		can_filterInitStruct->CAN_FilterMaskIdHigh=0x0000;
		can_filterInitStruct->CAN_FilterMaskIdLow=0x0000;
		can_filterInitStruct->CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
		can_filterInitStruct->CAN_FilterActivation=ENABLE;
	}else if (pChan->devicType == CAN_TYPE_GROUP_RCV)
	{
		// 组接收
		can_filterInitStruct->CAN_FilterNumber=fiterno;
		can_filterInitStruct->CAN_FilterMode=CAN_FilterMode_IdMask;
		can_filterInitStruct->CAN_FilterScale=CAN_FilterScale_32bit;
		can_filterInitStruct->CAN_FilterIdHigh=(pChan->canID>>16) & 0xffff;  //根据《网络转换器协议V1.0》协议待定
		can_filterInitStruct->CAN_FilterIdLow=(pChan->canID) & 0xffff;	//根据协议
		can_filterInitStruct->CAN_FilterMaskIdHigh=(pChan->canMaskId>>16) & 0xffff;	//根据协议 	
		can_filterInitStruct->CAN_FilterMaskIdLow=(pChan->canMaskId) & 0xffff;	//ID15-ID8地址必须匹配，扩展帧，数据帧
		can_filterInitStruct->CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
		can_filterInitStruct->CAN_FilterActivation=ENABLE;
	}
	else
	{
		can_filterInitStruct->CAN_FilterNumber=fiterno;
		can_filterInitStruct->CAN_FilterMode=CAN_FilterMode_IdList;
		can_filterInitStruct->CAN_FilterScale=CAN_FilterScale_32bit;
		can_filterInitStruct->CAN_FilterIdHigh=(pChan->canID>>16) & 0xffff;
		can_filterInitStruct->CAN_FilterIdLow=(pChan->canID) & 0xffff;
		can_filterInitStruct->CAN_FilterMaskIdHigh=(pChan->canID>>16) & 0xffff;
		can_filterInitStruct->CAN_FilterMaskIdLow=(pChan->canID) & 0xffff;
		can_filterInitStruct->CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
		can_filterInitStruct->CAN_FilterActivation=ENABLE;
	}
	return TRUE;
}

/*************************************************
CanPortInit - initialize  a  CAN port and clock.

writer:rm

RETURN: N
*************************************************/
void CanPortAndClockInit(STM32F10x_CAN_CHAN* pChan)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;
	if(CAN1 == pbase)
	{
		// 配置复用重映射和调试I/O配置寄存器AFIO_MAPR  IO重映射功能	
		//PD0 PD1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN1_RX;		         // GPIO管脚配置	  CAN1_RX  = PD0
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIO_CAN1, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN1_TX;                // GPIO管脚配置	  CAN1_TX  = PD1
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIO_CAN1, &GPIO_InitStructure);
		
		//remap	
		GPIO_PinRemapConfig(GPIO_Remap_CAN_1, ENABLE);

		/* GPIO clock enable */
  		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIO_CAN1, ENABLE);

		/* CAN1 Periph clock enable */
  		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	}
	else if(CAN2 == pbase)
	{
		 // 配置复用重映射和调试I/O配置寄存器AFIO_MAPR  IO重映射功能	
		//PB5 PB6
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN2_RX;		         // GPIO管脚配置	  CAN1_RX  = PB5
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIO_CAN2, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN2_TX;                // GPIO管脚配置	  CAN1_TX  = PB6
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIO_CAN2, &GPIO_InitStructure);
		
		//remap	
		GPIO_PinRemapConfig(GPIO_Remap_CAN_2, ENABLE);

		/* GPIO clock enable */
  		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIO_CAN2, ENABLE);
		/* CAN2 Periph clock enable */
  		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	}
}

/*************************************************
CanIntInit - initialize  a  CAN INT.

writer:rm

RETURN: N
*************************************************/
void CanIntInit(STM32F10x_CAN_CHAN* pChan)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;
	if(CAN1 == pbase)
	{
	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

	//	CAN_ITConfig(pChan->base, CAN_IT_BOF|CAN_IT_EPV, ENABLE);	         // 

	}
	else if(CAN2 == pbase)
	{
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

		NVIC_InitStructure.NVIC_IRQChannel = CAN2_SCE_IRQn;	     // 中断号   
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断

	//	CAN_ITConfig(pChan->base, CAN_IT_BOF|CAN_IT_EPV, ENABLE);	         //
	}
}

/*************************************************
CANBroadcastAddrSet() - initialize  CAN broadcast .

writer:

RETURN: 
*************************************************/
void CANBroadcastAddrSet(STM32F10x_CAN_CHAN* pChan)
{
/*根据协议《网络转换器协议V1.0》需要通过广播地址0XFF*/
		CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;
		int32 tmpid,tmptype;
		uint8 fitercnt = 1; //根据协议，本滤波单元只用到2路滤波器，所以本部分都使用随后的滤波器，如果要到2组以上，则需要重新改写该句算法
		tmpid = pChan->canID;
		pChan->canID = 	pChan->canID | (0xff<<CAN_ADD_START_BIT);
		tmptype = pChan->devicType; 
		pChan->devicType = CAN_TYPE_GROUP_RCV;
		if(CAN2 == pbase)
		{
			CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,pChan->CAN_FilterInitStructure.CAN_FilterNumber+fitercnt);
		}
		else
		{
			CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,pChan->CAN_FilterInitStructure.CAN_FilterNumber+fitercnt);
		}
		CAN_FilterInit(&pChan->CAN_FilterInitStructure);
		pChan->canID = tmpid;
		pChan->devicType = tmptype;

}


/*************************************************
CANDevDefInit - initialize  a  CAN .

writer:rm

RETURN: N
*************************************************/
static BOOL CANDevDefInit(STM32F10x_CAN_CHAN* pChan)
{
	CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;
	uint8 fitercnt = 0;
	RCC_ClocksTypeDef rcc_Clocks; 

	/*get the frequencies of different on chip clocks*/
	RCC_GetClocksFreq(&rcc_Clocks);

	/* CAN register init */
	CAN_DeInit(pbase);

	/*port and clock init*/
	CanPortAndClockInit(pChan);

	/*INT set */
	CanIntInit(pChan);


	/* CAN  init */
	CAN_StructInit(&pChan->CAN_InitStructure);

	/* CAN cell init */
	pChan->CAN_InitStructure.CAN_TTCM = DISABLE;	//禁止时间触发
	pChan->CAN_InitStructure.CAN_ABOM = ENABLE;  //自动退出离线状态
	pChan->CAN_InitStructure.CAN_AWUM = ENABLE;	 //自动唤醒模式
	pChan->CAN_InitStructure.CAN_NART = DISABLE; //自动重传
	pChan->CAN_InitStructure.CAN_RFLM = DISABLE; //接收FIFO不锁定
	pChan->CAN_InitStructure.CAN_TXFP = DISABLE; //标识符优先级
	pChan->CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	if(CANBitStructFill(&pChan->CAN_InitStructure,pChan->baudRate)==FALSE)
	{
		return FALSE;
	}
	CAN_Init(pbase, &pChan->CAN_InitStructure);

	/* CAN filter init */
	if(CAN2 == pbase)
	{	//slave can2
		CAN_SlaveStartBank(CAN_FITER_NO);
		CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,CAN_FITER_NO+fitercnt);
	}else
	{
		CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,fitercnt);
	}
	CAN_FilterInit(&pChan->CAN_FilterInitStructure); 
	fitercnt ++;
	/*根据协议《网络转换器协议V1.0》需要通过广播地址0XFF*/
	{
		int32 tmpid,tmptype;
		tmpid = pChan->canID;
		pChan->canID = 	pChan->canID | (0xff<<CAN_ADD_START_BIT);
		tmptype = pChan->devicType; 
		pChan->devicType = CAN_TYPE_GROUP_RCV;
		if(CAN2 == pbase)
		{
			CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,CAN_FITER_NO+fitercnt);
		}
		else
		{
			CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,fitercnt);
		}
		CAN_FilterInit(&pChan->CAN_FilterInitStructure);
		pChan->canID = tmpid;
		pChan->devicType = tmptype;
	}

	return TRUE;
}

/*************************************************
CANDevSet - CANDevSet  a  CAN .

writer:rm
	根据设置重新设置CAN
RETURN: N
*************************************************/
static BOOL CANDevSet(STM32F10x_CAN_CHAN* pChan)
{
	CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;

	if (pbase==NULL)
	{
		return FALSE;
	}

	/* CAN  init */
	CAN_Init(pbase, &pChan->CAN_InitStructure);

	/* CAN filter set*/
	CANFilterStructInitFill(pChan,&pChan->CAN_FilterInitStructure,pChan->CAN_FilterInitStructure.CAN_FilterNumber);
 
	CAN_FilterInit(&pChan->CAN_FilterInitStructure);

	/*broadcast set*/
	/*根据协议《网络转换器协议V1.0》需要通过广播地址0XFF*/
	CANBroadcastAddrSet(pChan);

	return TRUE;
}


/*************************************************
CAN_Open - open can device.

writer:rm

description:

RETURN: device dirscription if successful,or 0
*************************************************/
int32 Can_Open(uint32 deviceId)
{
	int32 i;
	if((deviceId > 0) && (deviceId <= CAN_DEVICE_NUMS))
	{
		for(i=0;i<CAN_DEVICE_NUMS;i++)
		{		
			if((CANChan[i].deviceOpenFlag != TRUE) && (CANChan[i].deviceID == deviceId))
			{		
				CANChan[i].deviceOpenFlag = TRUE;
                CAN_ITConfig(CANChan[i].base, CAN_IT_FMP0 | CAN_IT_FMP1, ENABLE);
				CAN_ITConfig(CANChan[i].base, CAN_IT_BOF|CAN_IT_EPV, ENABLE);
				return (int32)&CANChan[i];
			}
		}
	}	
	return 0;
}

/*************************************************
CAN_Close - close can device.

writer:rm

description:

RETURN: TURE if successful,or FALSE
*************************************************/
int32 Can_Close(int32 devDesc)
{
	uint8 ret = FALSE;
	STM32F10x_CAN_CHAN *pChan = (STM32F10x_CAN_CHAN*)devDesc;
	if((pChan != NULL) && (pChan->deviceOpenFlag == TRUE))
	{
		pChan->deviceOpenFlag = FALSE;
		CAN_ITConfig(pChan->base, CAN_IT_FMP0 | CAN_IT_FMP1, DISABLE);
		CAN_ITConfig(pChan->base, CAN_IT_BOF|CAN_IT_EPV, DISABLE);
		ret = TRUE;
	}
	return ret;	
}

 #if 0
/*************************************************
CAN_Init - initilize can device.

writer:rm

description:

RETURN: TRUE if successful,or FALSE
*************************************************/
int32 Can_Init(int32 devDesc,	/*device discription*/
			uint32 fenJiNo,	/*fen ji No*/
			uint32 baudRate,	/*baud rate*/
			uint8 devType,	/*fen ji type*/
			uint8 frameType 	/*frame type*/
			)
{	
	CAN_FUC_DESC *pCanDesc = (CAN_FUC_DESC*)devDesc;
	if(devDesc == 0)
	{
		return FALSE;
	}
	
	pCanDesc->DevicType = devType;
	pCanDesc->FrameType = frameType;
	
	if(CanInit(pCanDesc->devPointerDesc,fenJiNo,baudRate,devType, frameType) == FALSE)
	{	
		return FALSE;
	}
	return TRUE;
}

#endif

/*
判断发送缓存有空闲否
返回：空闲缓存数目
*/
uint8 CanFreeTxBufNum(CAN_TypeDef* CANx)
{ 
	uint8 num = 0;
	 assert_param(IS_CAN_ALL_PERIPH(CANx));
	 num = (uint8_t)((CANx->TSR & TME0_2) >> TME0_2_SHIFT);	 //check datasheet
	 return num;
}

/*************************************************
Can_Write - write data to Can

writer:rm
	
RETURN: data number writed,-1 error
*************************************************/
int32 Can_Write(int32 devDesc,	/*device discription*/
				CAN_MSG* msg	/*can msg*/
				)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
	int32 ret = 0;
	CanTxMsg txmessage;
//	uint8 err; 
	STM32F10x_CAN_CHAN *pChan = (STM32F10x_CAN_CHAN*)devDesc;
	//CanTxMsg TxMessage;
	if((pChan == NULL) || (pChan->deviceOpenFlag == FALSE))
	{
		return -1;
	}

	//OSSemPend(pChan->canLock,0,&err);

	if(QueueNData(pChan->sendBuf)>0)
//	if((QueueNData(pChan->sendBuf)>0) || (CanFreeTxBufNum(pChan->base) == 0))
	{
		ret = QueueWrite(pChan->sendBuf,(uint8*)msg);
	}
	else
	{
		OS_ENTER_CRITICAL();
		if(CanFreeTxBufNum(pChan->base) == 0)
		{
			ret = QueueWrite(pChan->sendBuf,(uint8*)msg);
		}
		else
		{
			CanTxMsgUpConverDown(&txmessage,msg);
			if(CAN_Transmit(pChan->base, &txmessage) == CAN_NO_MB)
			{
				ret = QueueWrite(pChan->sendBuf,(uint8*)msg);
			}
			ret = QUEUE_OK;
		}
		OS_EXIT_CRITICAL();
	} 

	if (ret == QUEUE_OK)
	{
		if(CAN1 == pChan->base)
		{
			WriteCanLedDataBit(CANTX1,CANLED_OFF);
		}
		else
		{
			WriteCanLedDataBit(CANTX2,CANLED_OFF);
		}
		CAN_ITConfig(pChan->base, CAN_IT_TME, ENABLE);
		ret =  sizeof(CAN_MSG);
	}
//	else
//	{
//		 ret = 0;
//	}
	
	//OSSemPost(pChan->canLock);
					
	return ret;
}


/*************************************************
Can_Read - read data from Can

writer:rm

note:
	the function will be pend when no data 
	
RETURN: data byte numbers readed
*************************************************/
int32 Can_Read(int devDesc,	/*device discription*/
				CAN_MSG* buf,	/*data buf*/
				uint32 timeOut /*time out*/
				)
{
	int32 ret = 0;
//	uint8 err;
	STM32F10x_CAN_CHAN *pChan = (STM32F10x_CAN_CHAN*)devDesc;
	if((pChan == NULL) || (pChan->deviceOpenFlag == FALSE))
	{
		return ERROR;
	}

	//OSSemPend(pChan->canLock,0,&err);

	if (pChan->sioMode & CAN_SIOMODE_RCV_PEND_EN)
	{
#if CAN_RCV_SEM_PEND_EN > 0
		uint8 err; 
		OSSemPend(pChan->pRcvSem,timeOut,&err);
		if (err == OS_ERR_NONE)
		{
			if (QueueRead((uint8*)buf,(VOID*)pChan->rcvBuf)==QUEUE_OK)
			{
				ret = sizeof(CAN_MSG);
			}
		}
#endif
	}else{
		if (QueueRead((uint8*)buf,(VOID*)pChan->rcvBuf)==QUEUE_OK)
		{
			ret = sizeof(CAN_MSG);
		}
	}

	if(CAN1 == pChan->base)
	{
		WriteCanLedDataBit(CANRX1,CANLED_OFF);
	}
	else
	{
		WriteCanLedDataBit(CANRX2,CANLED_OFF);
	}

	//OSSemPost(pChan->canLock);

	return ret;		
}



/*************************************************
Can_Ctrl -opreat  the can .

writer:rm

RETURN: FALSE - error,or TRUE - successful
*************************************************/
int32 Can_Ctrl(int32 devDesc,	/*device description*/
			int32 cmd,	/*request cmd*/
			void* arg,	/*some argument1*/
			void* arg2,	/*some argument2*/
			void* arg3	/*some argument3*/
			)
{
	int ret = FALSE;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif 
	STM32F10x_CAN_CHAN *pChan = (STM32F10x_CAN_CHAN*)devDesc;
//	CAN_TypeDef* pbase = (CAN_TypeDef *)pChan->base;
	if((pChan == NULL) || (pChan->deviceOpenFlag == FALSE))
	{
		return ERROR;
	}
	
	switch(cmd)
	{
		case CAN_BAUD_SET:	/* Set the baud rate. */
			OS_ENTER_CRITICAL();
			if (CANBitStructFill(&pChan->CAN_InitStructure,*(uint32*)arg) == TRUE)
			{
				CANDevSet(pChan);
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();
			break;
		case CAN_BAUD_GET: /*Get baud rate*/
			*(uint32*)arg = pChan->baudRate;
			ret = TRUE;
			break;	
		case CAN_ADD_SET:
			pChan->canID &= ~CAN_ADD_ALL_BIT;
			pChan->canID |= ((*(uint32*)arg & 0xFF) << CAN_ADD_START_BIT);
			CANDevSet(pChan);
			//CANDevDefInit(pChan);
			//CAN_ITConfig(pChan->base, CAN_IT_FMP0 | CAN_IT_FMP1, ENABLE);
			ret = TRUE;
			break;
		case CAN_RCV_TYPE_SET:	/*set receive fiter:arg: type*/
			OS_ENTER_CRITICAL();
			ret = CANFilterStructFill(pChan,*(uint32*)arg,pChan->canID,pChan->canMaskId,pChan->CAN_FilterInitStructure.CAN_FilterNumber);
			if (ret == TRUE)
			{
				CANDevSet(pChan);
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();
			break;
		case CAN_RCV_PEND_SET: /*set receive pend type*/
			OS_ENTER_CRITICAL();
#if CAN_RCV_SEM_PEND_EN > 0			
			if(*(uint32*)arg & CAN_SIOMODE_RCV_PEND_EN)
			{
				pChan->sioMode |= CAN_SIOMODE_RCV_PEND_EN;	
			}
			else
			{
				pChan->sioMode &= ~CAN_SIOMODE_RCV_PEND_EN;	
			}
			ret = TRUE;
#endif
			OS_EXIT_CRITICAL();
			break;	
		 case CAN_RCV_PEND_GET: /*GET receive pend type*/
			OS_ENTER_CRITICAL();
#if CAN_RCV_SEM_PEND_EN > 0			
			if(pChan->sioMode & CAN_SIOMODE_RCV_PEND_EN)
			{
				*(uint32*)arg = CAN_SIOMODE_RCV_PEND_EN;	
			}
			else
			{
				*(uint32*)arg = ~CAN_SIOMODE_RCV_PEND_EN & CAN_SIOMODE_RCV_PEND_EN;	
			}
			ret = TRUE;
#endif
			OS_EXIT_CRITICAL();
			break;	

		default:
			break;
	}
	return ret;
}

/*************************************************
CanRxMsgDownConverUp - .

*************************************************/
VOID CanRxMsgDownConverUp(CAN_MSG* msg,CanRxMsg* rxmsg)
{
	uint32 i;
	if (rxmsg->IDE == CAN_ID_STD)
	{
		msg->Id.StdId = rxmsg->StdId;
		msg->FrmDef.FF = CAN_FRAME_STANDARD;
	}else{
		msg->Id.ExtId = rxmsg->ExtId;
		msg->FrmDef.FF = CAN_FRAME_EXTERNED;
	}
	
	if (rxmsg->RTR==CAN_RTR_DATA)
	{
		msg->FrmDef.RTR = CAN_DATA_TYPE_FRAME;
	}else{
		msg->FrmDef.RTR = CAN_REMOTE_TYPE_FRAME;
	}
	msg->DLC = rxmsg->DLC;
	for (i=0;i<8;i++)
	{
		msg->Data[i] = rxmsg->Data[i];
	}
}

/*************************************************
Can_Ctrl -opreat  the can .

*************************************************/
VOID CanTxMsgUpConverDown(CanTxMsg* txmsg,CAN_MSG* msg)
{
	uint32 i;
	if (msg->FrmDef.FF == CAN_FRAME_STANDARD)
	{
		txmsg->StdId = msg->Id.StdId;
		txmsg->IDE = CAN_ID_STD;
	}else{
		txmsg->ExtId = msg->Id.ExtId;
		txmsg->IDE = CAN_ID_EXT;
	}

	if (msg->FrmDef.RTR==CAN_DATA_TYPE_FRAME)
	{
		txmsg->RTR = CAN_RTR_DATA;
	}else{
		txmsg->RTR = CAN_RTR_REMOTE;
	}
	txmsg->DLC = msg->DLC;
	for (i=0;i<8;i++)
	{
		txmsg->Data[i] = msg->Data[i];
	}
}

void CAN_FIFOStatusClear(CAN_TypeDef* CANx, uint8_t FIFONumber)
{
  /* Check the parameters */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_FIFO(FIFONumber));
  
  if (FIFONumber == CAN_FIFO0)
  {
  	if(CANx->RF0R & RF0R_FULL)
    	CANx->RF0R = RF0R_FULL;
	if(CANx->RF0R & RF0R_FOVR)
    	CANx->RF0R = RF0R_FOVR;
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
  	if(CANx->RF1R & RF0R_FULL)
    	CANx->RF1R = RF0R_FULL;
	if(CANx->RF1R & RF0R_FOVR)
    	CANx->RF1R = RF0R_FOVR;
  }
}


void Can_Rx_Deal(CAN_TypeDef* CANx,uint8_t FIFONumber)
{
	CanRxMsg rxmessage;
	CAN_MSG msg;
	int32 i;
	CAN_FIFOStatusClear(CANx,FIFONumber);   //接收FIFO状态清楚
	CAN_Receive(CANx, FIFONumber, &rxmessage);
	for(i=0;i<CAN_DEVICE_NUMS;i++)
	{	
		if(((CAN_TypeDef *)CANChan[i].base == CANx) && (CANChan[i].deviceOpenFlag == TRUE))
		{
			CanRxMsgDownConverUp(&msg,&rxmessage);
			if(QueueWriteInInt(CANChan[i].rcvBuf,(uint8*)&msg) == QUEUE_OK)
			{
#if CAN_RCV_SEM_PEND_EN > 0
				if (CANChan[i].sioMode & CAN_SIOMODE_RCV_PEND_EN)
					OSSemPost(CANChan[i].pRcvSem);
#endif
			}
			break;
		}
	}
}

void Can_Tx_Deal(CAN_TypeDef* CANx)
{
	CanTxMsg txmessage;
	CAN_MSG msg;
	int32 i;
	for(i=0;i<CAN_DEVICE_NUMS;i++)
	{	
		if(((CAN_TypeDef *)CANChan[i].base == CANx) && (CANChan[i].deviceOpenFlag == TRUE))
		{
			if(CanFreeTxBufNum(CANx) == 0)
				break;	//no free buf	
			if(QueueReadInInt((uint8*)&msg,(VOID*)CANChan[i].sendBuf) == QUEUE_OK)
			{
				CanTxMsgUpConverDown(&txmessage,&msg);
				//CAN_Transmit(CANx, &txmessage);
				if(CAN_Transmit(CANx, &txmessage) == CAN_NO_MB)
				{//no free buf
				//	break;
				}
			}else{
				CAN_ITConfig(CANChan[i].base, CAN_IT_TME, DISABLE);
			}
			break;
		}
	}
}

//
void CAN1_RX0_IRQHandler(void)
{
	CANLED_Toggle(CANRX1);
	Can_Rx_Deal(CAN1,CAN_FIFO0);
}

void CAN1_RX1_IRQHandler(void)
{
	CANLED_Toggle(CANRX1);
	Can_Rx_Deal(CAN1,CAN_FIFO1);
}

void CAN1_TX_IRQHandler(void)
{
	//led option
	static int CanLedCount = 0;
	if(CanLedCount++ >= 5)
	{
		CANLED_Toggle(CANTX1);//LED
		CanLedCount = 0;
	}
	
	Can_Tx_Deal(CAN1);
}

//
void CAN2_RX0_IRQHandler(void)
{
	CANLED_Toggle(CANRX2);
	Can_Rx_Deal(CAN2,CAN_FIFO0);
}

void CAN2_RX1_IRQHandler(void)
{
	CANLED_Toggle(CANRX2);
	Can_Rx_Deal(CAN2,CAN_FIFO1);
}

void CAN2_TX_IRQHandler(void)
{
	//led option
	static int CanLedCount = 0;
	if(CanLedCount++ >= 5)
	{
		CANLED_Toggle(CANTX2);//LED
		CanLedCount = 0;
	}
	Can_Tx_Deal(CAN2);
}

void CAN1_SCE_IRQHandler(void)
{

}

void CAN2_SCE_IRQHandler(void)
{

}

