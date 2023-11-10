/****************************************Copyright (c)**************************************************
**                               深圳奥特迅电力设备股份有限公司
**                                     	   研发中心
**                                                                 
**
**文件名: UART.C     
**
**作者:  RM       版本:v1.0        日期: 2010/4/26
**
**描述:    			
**
**		stm32f10x UART 在uCOS-II下的底层驱动文件
**
		RS232/485操作模式:option:工作模式操作
		[option.0 - option.1]	00 表示RS232类型
							01 表示RS485类型
							
		[option.2 - option.3]	００　表示５位工作模式
							０１　表示６位工作模式
							１０　表示７位工作模式
							１１　表示８位工作模式
							
		[option.5 ]			0 表示１个停止位
							1 表示２个停止位

		[option.6 - option.8]	000 表示无效验
							001 表示奇效验
							010 表示偶效验
		
	
**历史记录:        
*2008/5/25,添加RS485 的延迟处理。by rm
*2014/6/25,RS485初始化为接收模式。by Abe
****************************************************************************************************/
#include "Uart.h"
#include "stm32f10x_rcc.h"						  
#include "..\queue\queue.h"
#include <includes.h>
#define _IS_USER_FIFO_


uint8 inttimes = 0;


uint8 PowerOnForUart = FALSE;	
        
uint32 UartHandler[] = {0,0,0,0,0};
const uint32  UartBaudIndex[6] = {1200,2400,4800,9600,19200,115200};

USART_InitTypeDef USART_InitStructure;

static STM32F10x_SIO_CHAN_PARAM UARTParameter[] = {	     //注意:参数2-3 等原理图/PCB  再填充 
		{(void *)USART1_BASE, (void *)GPIOA, GPIO_Pin_8, RS485TYPE, 2400,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE},	
		{(void *)USART2_BASE, (void *)GPIOA, GPIO_Pin_1, RS485TYPE, 115200,   USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE},
		{(void *)USART3_BASE, (void *)GPIOE, GPIO_Pin_10, RS485TYPE, 2400, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE},	
		{(void *)UART4_BASE,  (void *)GPIOA, GPIO_Pin_15, RS485TYPE, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE},
		{(void *)UART5_BASE,  (void *)GPIOD, GPIO_Pin_7, RS232TYPE, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, HFC_NONE},
		};

static STM32F10x_UART_CHAN UARTChan[N_UART_CHANNELS];
STM32F10x_UART_CHAN *GetChanHandler(uint8 id)
{
		if(id < 5)
			return  &UARTChan[id] ;
	return NULL;
}
// ***********************************************************************
// 串口设备打开函数 
// 参数:  deviceId	:串口ID   取值范围： 1 ~  N_UART_CHANNELS
// 返回值： FALSE - 打开失败, 其他：串口句柄 
// *************************************************************************
int32 Uart_Open(uint32 deviceId)
{
	uint8 err;

	if(deviceId <= N_UART_CHANNELS)
	{
		OSSemPend(UARTChan[deviceId].uartLock, 0, &err);   // 获取信号量 
			
		if(UARTChan[deviceId].deviceOpenFlag == FALSE)	   // 未打开状态 
		{						
			UARTChan[deviceId].deviceOpenFlag = TRUE;  	   // 标记为已打开    
			OSSemPost(UARTChan[deviceId].uartLock);			
			return (int32)&UARTChan[deviceId];
		}
		OSSemPost(UARTChan[deviceId].uartLock);	           // 释放信号量 		
	}	
	return FALSE;
}
// ***********************************************************************
// 串口波特率设置操作  
// 参数:  pChan - 串口句柄，pbase - 寄存器操作基地址   
// 返回值： FALSE - 打开失败, 其他：串口句柄 
// *************************************************************************

BOOL Uart_BaudSet(STM32F10x_UART_CHAN* pChan,  USART_TypeDef* pbase)
{
	uint32 apbclock = 0, tmpreg = 0, integerdivider=0, fractionaldivider=0;
	RCC_ClocksTypeDef  RCC_ClocksStatus; 
	assert_param(IS_USART_BAUDRATE(bps));  	                    // 参数检验 
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	if (pbase == (USART_TypeDef*)USART1_BASE)
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	}
	integerdivider = ((0x19 * apbclock) / (0x04 * (pChan->baudRate)));		// Determine the integer part
	tmpreg = (integerdivider / 0x64) << 0x04;	
	fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));		    // Determine the fractional part 
	tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F);	
	pbase->BRR = (uint16_t)tmpreg;		                                    // Write to USART BRR
	return TRUE;
}	
// *************************************************
// 开启串口 
// 开中断 串口使能 
// pbase -- 串口寄存器基地址 
// *************************************************/
static void  Uart_Start(USART_TypeDef* pbase)
{
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	// 发送缓冲区数据已经移走 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// 接收缓冲区收到数据 

	// USART使能 USART_CR1 bit14  		
	USART_Cmd(USART1, ENABLE);

}
// *************************************************
// UartOptionSet -set uart option .byte size,stop bit,parity 
// writer:rm		
// PARA: pChan 串口基本信息数据结构 ，	pbase 寄存器组起始地址 
// RETURN: 
// ************************************************
static void UartOptionSet(STM32F10x_UART_CHAN *pChan, USART_TypeDef* pbase)
{
	uint32 tmpreg = pbase->CR1;
		
	if(pChan->WordLength == USART_WordLength_9b)				// byte size
		tmpreg |= (1<<12);                      // 一个起始位，9个数据位，一个停止位 	
	else
		tmpreg &= ~((uint32)(1<<12));				// 一个起始位，8个数据位，n个停止位 
	if(pChan->Parity != USART_Parity_No)  // 校验控制被使能
	{
		tmpreg |= (1<<10);	
		if(pChan->Parity == USART_Parity_Odd) // 奇校验
		{
		 	tmpreg |= (1<<9);	
		}
		else
			tmpreg &= ~((uint32)(1<<9));		
	}
	else
		tmpreg &= ~((uint32)(1<<10));	
	pbase->CR1 = tmpreg;   

	tmpreg = pbase->CR2;
	tmpreg &= ~((uint32)(3<<12));	        // 一个停止位 
	if(pChan->Stopbits == USART_StopBits_0_5)          // 0.5个停止位 
		tmpreg |= (1<<12);
	else if(pChan->Stopbits == USART_StopBits_2)      // 2个停止位 
		tmpreg |= (2<<12);	
	else if(pChan->Stopbits == USART_StopBits_1_5)     // 1.5个停止位 
		tmpreg |= (3<<12);	
	pbase->CR2 = tmpreg;  	
}
// ***************************************************************************
// UartTypeSet -   管脚配置及初始化   485模式时 收发控制管脚配置及其初始化		
// PARA:   pChan 串口基本信息数据结构 ，pbase 寄存器组起始地址 
// RETURN: 无 
/*
USART1 - TX:PA9 RX:PA10 TEN:PB7
USART2 - TX:PA2 RX:PA3 TEN:PB9
USART3 - TX:PB10 RX:PB11 TEN:PB14
UART4  - TX:PC10 RX:PC11 TEN:PB15
UART5  - TX:PC12 RX:PD2 TEN:PD7
*/
// ****************************************************************************
static void UartTypeSet(STM32F10x_UART_CHAN *pChan, USART_TypeDef* pbase)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(pbase == (USART_TypeDef*)USART1_BASE) // UART0,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		// 板子上用PA9和PA10，与USB(CN2)复用，接跳线JP42 JP43 不进行重映射 	USB不能复用 
		// 配置复用重映射和调试I/O配置寄存器AFIO_MAPR 不使用USAR1的 IO重映射功能		
		GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
		
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		         // GPIO管脚配置	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                // GPIO管脚配置	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485方式 需要配置485收发控制管脚 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO管脚配置	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			//GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_SET);  //receive
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);
		}
	 }
	 else if (pbase == (USART_TypeDef*)USART2_BASE)
	 {
		// 板子上 重映射 到 PD3-PD7  (USART2_CTS USART2_RTS USART2_TX USART2_RX USART2_CK)
		// 配置复用重映射和调试I/O配置寄存器AFIO_MAPR 使用USAR2的 IO重映射功能	 不影响其他功能 只是占用GPIO 	
		//GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		         // GPIO管脚配置	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                // GPIO管脚配置	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485方式 需要配置485收发控制管脚 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO管脚配置	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)USART3_BASE)
	 {
		// 不重映射不可行，因为与ethnet复用 
		// 部分重映射不可行，因为映射占用uart4管脚
		// 全部重映射 可能可行， 这样ethnet一定不能重映射 	
		// 板子上 完全重映像 到 PD8-12  (USART3_TX USART3_RX USART3_CK USART3_CTS USART3_RTS)
		// 配置复用重映射和调试I/O配置寄存器AFIO_MAPR 使用USAR3的 IO部分重映射功能	
//		GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);	 //PB10和PB11没有重映射
		//GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE);	 //PB10和PB11没有重映射
		
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO管脚配置	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                // GPIO管脚配置	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485方式 需要配置485收发控制管脚 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO管脚配置	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART4_BASE)
	 {
		// 没有重映射的功能 PC10-11 (UART4_TX UART4_RX)	
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
							
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO管脚配置	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                // GPIO管脚配置	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485方式 需要配置485收发控制管脚 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO管脚配置	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //串口4用的有光耦，使能脚控制相反
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART5_BASE)
	 {
		// 没有重映射的功能 PC12 PD2 (UART5_TX UART5_RX)
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
								
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		         // GPIO管脚配置	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                // GPIO管脚配置	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)                        // 485方式 需要配置485收发控制管脚 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO管脚配置	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	}
	
	if(pChan->devicType == IRDATYPE)
	{	//红外模式
		/* Configure the USARTy IrDA mode */
		USART_IrDAConfig(pbase,USART_IrDAMode_Normal);
		/* Enable the USARTy IrDA mode */
		USART_IrDACmd(pbase, ENABLE);  		
	}
	else
	{
		/* disable the USARTy IrDA mode */
		USART_IrDACmd(pbase, DISABLE);
	}    

}
//模式判断
BOOL IsUartMode(uint8 mode)
{
	if((mode == RS232TYPE)||(mode == RS485TYPE)||(mode == IRDATYPE)||(mode == IR38KHZTYPE))
		return TRUE;

	return FALSE;
}

// **************************************************************************
// Uart_IOCtrl -opreat  the UART 
// PARA: devDesc--device description, cmd--request cmd, arg--some argument.
// RETURN: FALSE - error,or TRUE - successful
//****************************************************************************
int Uart_IOCtrl(int32 devDesc, int32 cmd, void* arg)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
	int ret = FALSE;	
	uint8 err;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;	 // 串口信息数据结构 
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;	         // 寄存器基地址数据结构 

	if((devDesc == 0) || (pbase == 0))
	{
		return FALSE;
	}
	OSSemPend(pChan->uartLock, 0, &err);
	switch(cmd)
	{
		case SIO_BAUD_SET:	                           // Set the baud rate 
		{
			OS_ENTER_CRITICAL();
			pChan->baudRate = *(int32*)arg;				// add by wxh 2010-11-20
			if(Uart_BaudSet(pChan, pbase) == TRUE)     // 设置波特率 
			{
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();			
		}
		break;
		case SIO_START:	                               // UART Start work
			OS_ENTER_CRITICAL();
			Uart_Start(pbase);
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_HW_OPTS_SET:	                       // Set wordbits
			OS_ENTER_CRITICAL();
			UartOptionSet(pChan, pbase);	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_PARITY_BIT_SET:	                       // Parity bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_PARITY(*(int32*)arg))
			{
				pChan->Parity = *(int32*)arg;
				UartOptionSet(pChan, pbase);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_STOP_BIT_SET:	                       // STOP bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_STOPBITS(*(int32*)arg))
			{
				pChan->Stopbits = *(int32*)arg;		 //modify by wxh 2010-11-20	   pChan->Parity = *(int32*)arg;
				UartOptionSet(pChan, pbase);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_DATA_BIT_SET:	                       // DATA bit set
			OS_ENTER_CRITICAL();
			if(IS_USART_WORD_LENGTH(*(int32*)arg))
			{
				pChan->WordLength = *(int32*)arg;	//modify by wxh 2010-11-20		pChan->Parity = *(int32*)arg;
				UartOptionSet(pChan, pbase);
			}	
			OS_EXIT_CRITICAL();
			ret = TRUE;
			break;
		case SIO_WORK_TYPE_SET:	                      // type set(rs232 or rs485)
			OS_ENTER_CRITICAL();
			if(IsUartMode(*(int*)arg))
			{
				pChan->devicType = *(int*)arg;
				UartTypeSet(pChan, pbase);
				ret = TRUE;
			}
			OS_EXIT_CRITICAL();				
			break;
		default:
			break;
	}
	OSSemPost(pChan->uartLock);
	return ret;
}

// *************************************************
// Uart_EnIsr - desable a ISR .	
// RETURN: N
// *************************************************
static void Uart_DisableIsr(STM32F10x_UART_CHAN* pChan)
{	
	USART_Cmd((USART_TypeDef*)pChan->base, DISABLE);	
}

// *************************************************
// Uart_Close
// RETURN: FALSE - error,or TRUE - successful
// *************************************************
int32 Uart_Close(uint32 devDesc)		//device description
{
	int8 i;
	uint8 err;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(devDesc == (uint32)&UARTChan[i])
		{
			OSSemPend(UARTChan[i].uartLock, 0, &err); 		
			UARTChan[i].deviceOpenFlag = FALSE;
			Uart_DisableIsr(&UARTChan[i]);
			OSSemPost(UARTChan[i].uartLock);			
			return TRUE;
		}
	}
	return FALSE;
}
// ********************************************************************************
// Uart_Read -read data from the UART .
// note:
//	the function will be pend when no data 
// PARA: devDesc - device description struct ,	buf	- save to buffer
// RETURN: byte numbers readed
// *********************************************************************************
int32 Uart_Read(int32 devDesc,uint8 *buf, uint16 maxSize)
{
	uint16 ret = 0;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	if(devDesc == 0)
	{
		return 0;
	}
	for(ret = 0; ((ret < MAX_UART_BUF_LENTH) && (ret < maxSize)); ret++)	           // 取数据 
	{
	   if(QueueRead(buf, pChan->RcvBuf) == QUEUE_OK)           // 读取成功 
			buf++;	   		
	   else
		break;
	}
	return ret;
}

// ***************************************************************************
// Uart_Write -write data to the UART .
// PARA: devDesc -- device description, buf size--buffer and  data size writed
// RETURN: numbers writed 
// ****************************************************************************
int32 Uart_Write(int32 devDesc,	uint8 *buf,	int32 size)
{
	uint8 delayMax = 200, err;
	uint16 len = size;
	int32 temp;
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;
	if(devDesc == 0)
	{		
		return 0;
	}
	OSSemPend(pChan->uartLock, 0, &err);
	if (len > 0)
	{		
		while(QueueWrite((void *)pChan->SendBuf, buf) == QUEUE_OK)    // 数据入队 
		{
			len --;
			buf++;
			if(len <= 0)
				break;			
		}
		USART_ITConfig(pbase, USART_IT_TXE, ENABLE);	
		USART_ITConfig(pbase, USART_IT_TC, ENABLE);
		if(pChan->devicType == IR38KHZTYPE)
		{	//38K调制型红外模式，为半双工模式
			USART_ITConfig(pbase, USART_IT_RXNE, DISABLE); //rm add 20110402
		}		
	}
	OSSemPost(pChan->uartLock);
	while(--delayMax)
	{
		OSTimeDlyHMSM(0,0,0,5);
		if((len = QueueNData(pChan->SendBuf)) == 0)
			break;
	}
	temp = (int32)(size - len);
	return temp;
}
//===================================================================
// 串口时钟使能 
//
//===================================================================
void UsartClockOperation(STM32F10x_UART_CHAN *pChan, uint8 cmd)
{
	uint8 i;
	for(i = 0;i < N_UART_CHANNELS; i++)
	{
		if(pChan == &UARTChan[i])  
		{
			if(i == 0)
			{
				if(cmd != DISABLE)	  // USART1时钟使能
					RCC->APB2ENR |= (1<<14);	
				else
					RCC->APB2ENR &= ~((uint32)(1<<14));				
			}
			else	// usart 2-5
			{
				if(cmd != DISABLE)	  // USART1时钟使能
					RCC->APB1ENR |= (1<<(16+i));	
				else
					RCC->APB1ENR &= ~((uint32)(1<<(16+i)));				
					
			}
		} 	
	}
}

/*************************************************
LPCUartDevInit - initialize  a  UART .

writer:rm

RETURN: N
*************************************************/
static void UartDevInit(STM32F10x_UART_CHAN* pChan)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;

	UsartClockOperation(pChan, ENABLE);                          // 串口外设 RCC时钟配置 

	USART_InitStructure.USART_BaudRate = pChan->baudRate;	     // 串口参数配置 bps 数据位 停止位 校验方式 流控 
	USART_InitStructure.USART_WordLength = pChan->WordLength;
	USART_InitStructure.USART_StopBits = pChan->Stopbits;
	USART_InitStructure.USART_Parity = pChan->Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	UartTypeSet(pChan, pbase);                                   // 管脚配置

	if(pbase == (USART_TypeDef*)USART1_BASE)                     // UART0,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	 // 配置启用 USART外设时钟 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	     // 中断号 37  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // 子优先级 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能串口1中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断 						
		
		USART_Init(USART1, &USART_InitStructure);				 // 串口参数配置 
 		 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	         // 允许的中断类型-发送缓冲区数据已经移走
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	         // 允许的中断类型-接收缓冲区收到数据 
				  		
		USART_Cmd(USART1, ENABLE);	                             // USART使能 USART_CR1 bit13
	}
	else if(pbase == (USART_TypeDef*)USART2_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	 // 配置启用 USART外设时钟 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	     // 中断号 38  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	     // 子优先级 1 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能串口中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断 		
		 
		USART_Init(USART2, &USART_InitStructure);				 // 串口参数配置
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 	         // 允许的中断类型-发送缓冲区数据已经移走
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	         // 允许的中断类型-接收缓冲区收到数据 		
		USART_Cmd(USART2, ENABLE);		                         // USART使能		
	}
	else if(pbase == (USART_TypeDef*)USART3_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 	 // 配置启用 USART外设时钟 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	     // 中断号 39  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	     // 子优先级 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能串口中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断 		
		
		USART_Init(USART3, &USART_InitStructure);				 // 串口参数配置 
		 
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	         // 允许的中断类型-发送缓冲区数据已经移走
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	         // 允许的中断类型-接收缓冲区收到数据 
				
		USART_Cmd(USART3, ENABLE);								 // USART使能
	}	
	else if(pbase == (USART_TypeDef*)UART4_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 	 // 配置启用 UART外设时钟 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	     // 中断号 52  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	     // 子优先级 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能串口中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断 		

		 
		USART_Init(UART4, &USART_InitStructure);				 // 串口参数配置

		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);	         // 允许的中断类型-发送缓冲区数据已经移走
		USART_ITConfig(UART4, USART_IT_TC, ENABLE);
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	         // 允许的中断类型-接收缓冲区收到数据 
 		
		USART_Cmd(UART4, ENABLE);						         // USART使能
	}	
	else if(pbase == (USART_TypeDef*)UART5_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 	 // 配置启用 UART外设时钟 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // 配置 串口NVIC  应用中断与复位控制寄存器   不对应用优先级进行分组
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	     // 中断号 53  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	     // 子优先级 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // 使能串口中断 
		NVIC_Init(&NVIC_InitStructure);	                         // 配置中断优先级并使能中断 		
		 
		USART_Init(UART5, &USART_InitStructure);				 // 串口参数配置

		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);	         // 允许的中断类型-发送缓冲区数据已经移走
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	         // 允许的中断类型-接收缓冲区收到数据 
 		
		USART_Cmd(UART5, ENABLE);						         // USART使能
	}
	UartOptionSet(pChan, pbase); //add option	
}

/***************************************************************************
**  void ComRate(void)
**   功能: 串口波特率转换
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void ComRate(uint8 com)
{
	switch(MbsSlave.SysPara.ComPara.UsartPara[com].BaudRate)
	{
		case RATE2400:
			UARTParameter[com].defBuardRate = 2400;
			break;
		case RATE4800:
			UARTParameter[com].defBuardRate = 4800;
			break;
		case RATE9600:
			UARTParameter[com].defBuardRate = 9600;
			break;
		case RATE19200:
			UARTParameter[com].defBuardRate = 19200;
			break;
		case RATE38400:
			UARTParameter[com].defBuardRate = 38400;
			break;
		case RATE115200:
			UARTParameter[com].defBuardRate = 115200;
			break;			
		default:
			break;
	}
	if(MbsSlave.SysPara.ComPara.UsartPara[com].Purpose == BCODETIME)
	{
		UARTParameter[com].defBuardRate = 1000;
	}
}

// ******************************************************************************
// sysSerialHwInit - initialize  Serial Devece struct and Hardware at first.
// RETURN: NONE
// ******************************************************************************
void sysSerialHwInit(void)
{
	uint8 i;
	if(PowerOnForUart == FALSE)
	{
		PowerOnForUart = TRUE;
		for(i = 0;i < N_UART_CHANNELS; i++)		 
		{
			DeInitUsart(i);
			UARTChan[i].uartLock = OSSemCreate(1);	                     // 创建信号锁  初始化为已打开 
			UARTChan[i].base = UARTParameter[i].baseAddr;                // 寄存器基地址 
			UARTChan[i].directionGPIO =  UARTParameter[i].directionGPIO; // 方向控制GPIO 
			UARTChan[i].rs485ContrlPin = UARTParameter[i].rs485ContrlPin;// 方向控制脚序号 
			UARTChan[i].deviceID = i+1;                                  // 串口 ID
			UARTChan[i].deviceOpenFlag = FALSE;	                         // 打开标志初始化为 未打开 
			UARTChan[i].devicType = UARTParameter[i].devicType;	         // 设备类型 0 rs232 1 rs485

			UARTChan[i].baudRate = UARTParameter[i].defBuardRate;	     // 波特率 
			UARTChan[i].WordLength = UARTParameter[i].defWordLength;	 // 默认数据位 5-8 
			UARTChan[i].Stopbits = UARTParameter[i].defStopbits;	     // 默认停止位 1-2  
			UARTChan[i].Parity = UARTParameter[i].defParity;	         // 默认校验位
			QueueCreate(UARTChan[i].RcvBuf, MAX_UART_BUF_LENTH,1,0,0);     // 接收数据队列 
			QueueCreate(UARTChan[i].SendBuf, MAX_UART_BUF_LENTH,1,0,0);    // 发送数据队列 			
			UartDevInit(&UARTChan[i]);
			
		}
	}
}
/*************************************************************************************************
**  void ComInit(void)
**   功能: 通信参数配置
**   参数:  
**   输出:  
**   描述: 波特率2400---0;4800---1;9600---2;19200---3;38400---4;115200---5
			通信协议BAT5000-MA --- 1；BAT5000-MBS--- 2;BATM30B---3;BAT5000-MM --- 4；BAT5000-JKQ --- 5；BAT5000-FDY --- 6；
			通信端口用途:蓄电池组1---1;蓄电池组2---2;监控器---3;MODBUS后台软件---4;放电仪---5
***************************************************************************************************/
void ComInit(uint8 com)
{
	UARTChan[com].uartLock = OSSemCreate(1);	                     // 创建信号锁  初始化为已打开 
	UARTChan[com].base = UARTParameter[com].baseAddr;                // 寄存器基地址 
	UARTChan[com].directionGPIO =  UARTParameter[com].directionGPIO; // 方向控制GPIO 
	UARTChan[com].rs485ContrlPin = UARTParameter[com].rs485ContrlPin;// 方向控制脚序号 
	UARTChan[com].deviceID = com+1;                                  // 串口 ID
	UARTChan[com].deviceOpenFlag = FALSE;	                         // 打开标志初始化为 未打开 
	UARTChan[com].devicType = UARTParameter[com].devicType;	         // 设备类型 0 rs232 1 rs485

	UARTChan[com].baudRate = UARTParameter[com].defBuardRate;	     // 波特率 
	UARTChan[com].WordLength = UARTParameter[com].defWordLength;	 // 默认数据位 5-8 
	UARTChan[com].Stopbits = UARTParameter[com].defStopbits;	     // 默认停止位 1-2  
	UARTChan[com].Parity = UARTParameter[com].defParity;	         // 默认校验位
	
	QueueCreate(UARTChan[com].RcvBuf, MAX_UART_BUF_LENTH,1,0,0);     // 接收数据队列 
	QueueCreate(UARTChan[com].SendBuf, MAX_UART_BUF_LENTH,1,0,0);    // 发送数据队列 			
	UartDevInit(&UARTChan[com]);
	
	//InitUartTask(com);	
}
void DeInitUsart(uint8 com)
{
	switch(com)
	{
		case COM1:
			USART_DeInit(USART1);
			break;
		case COM2:
			USART_DeInit(USART2);
			break;
		case COM3:
			USART_DeInit(USART3);
			break;
		case COM4:
			USART_DeInit(UART4);
			break;
		default:
			break;
			
	}

}



/********************************************************************************
// Function Name  : GetDevChan()
// Description    : This function get device object.
// Input          : dev address
// Output         : None
// Return         : device struct
*********************************************************************************/
STM32F10x_UART_CHAN *GetDevChan(USART_TypeDef* uart)
{
	uint8 i;
	for(i = 0;i < N_UART_CHANNELS; i++)	
	{
		if(uart == UARTChan[i].base)
			return &UARTChan[i];
	}	
	return NULL;
}
// *******************************************************************************
// Function Name  : USART1_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
// *******************************************************************************
//extern SERIALPORT serial;
void  UartDealInIRQ(STM32F10x_UART_CHAN* pChan)
{
	uint8 tmpdata,rx_data;
	if(USART_GetITStatus(pChan->base, USART_IT_RXNE) != RESET)  // 接收缓冲区非空 
	{	
		inttimes++;
		rx_data = (uint8)USART_ReceiveData(pChan->base);
		QueueWriteInInt(pChan->RcvBuf, &rx_data); 
		// 写一个字节到队列  此处暂不考虑溢出 溢出数据不再接收  		
	}
	if(USART_GetITStatus(pChan->base, USART_IT_TXE) != RESET)  // 发送缓冲区已经空闲 可以写一个字节出去 
	{   
		if(QueueReadInInt(&tmpdata, pChan->SendBuf) == QUEUE_OK) // 有数据要发送 
		{
			if((pChan->deviceID == 2)||(pChan->deviceID == 4))
			{//串口4与其它串口不同，使能控制逻辑相反
				GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //set ctr pin
			}
			else
			{
				GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //set ctr pin
			}
			USART_SendData(pChan->base, tmpdata);		
		}
		else
		{
			USART_ITConfig(pChan->base, USART_IT_TXE, DISABLE);
			if(pChan->devicType == IR38KHZTYPE)
			{	//38K调制型红外模式，为半双工模式
				USART_ITConfig(pChan->base, USART_IT_RXNE, ENABLE); //rm add 20110402
			}
		}		
	}
//#if 0
	if(USART_GetITStatus(pChan->base, USART_IT_TC) != RESET)
	{
		if((pChan->deviceID == 2)||(pChan->deviceID == 4))
		{//串口4与其它串口不同，使能控制逻辑相反
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //clr ctr pin
		}
		else
		{
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //clr ctr pin
		}
	  	USART_ITConfig(pChan->base, USART_IT_TC, DISABLE);
		USART_ITConfig(pChan->base, USART_IT_RXNE, ENABLE);
	}	
//#endif
/*		
	if(USART_GetITStatus(pChan->base, USART_IT_TXE) != RESET)  // 发送缓冲区已经空闲 可以写一个字节出去 
	{   
		if(QueueReadInInt(&tmpdata, pChan->SendBuf) == QUEUE_OK) // 有数据要发送 
		{
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_SET);  //set ctr pin
			USART_SendData(pChan->base, tmpdata);		
		}
		else
		{
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin,Bit_RESET);  //clr ctr pin
			USART_ITConfig(pChan->base, USART_IT_TXE, DISABLE);
		}		
	}
	*/
		
}

// *******************************************************************************
// Function Name  : USART1_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
// *******************************************************************************
void USART1_IRQHandler(void)
{    
//	uint8 tmpdata,rx_data;
	uint32 regsr, regcr1;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART1);	
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	regsr = USART1->SR;
	regcr1 = USART1->CR1;
	if(regsr == regcr1)
	{
	   regsr = 0;
	}
	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
//*******************************************************************************
// Function Name  : USART2_IRQHandler
// Description    : This function handles USART2 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART2_IRQHandler(void)
{    
//	uint8 tmpdata,rx_data;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART2);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
// *******************************************************************************
// Function Name  : USART3_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void USART3_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(USART3);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

// *******************************************************************************
// Function Name  : USART4_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART4_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(UART4);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}

// *******************************************************************************
// Function Name  : USART5_IRQHandler
// Description    : This function handles USART1 global interrupt request.
// Input          : None
// Output         : None
// Return         : None
//*******************************************************************************
void UART5_IRQHandler(void)
{    
//	uint8 tmpdata, rx_data;
	OS_CPU_SR  cpu_sr;
	STM32F10x_UART_CHAN* pChan = GetDevChan(UART5);
		
	OS_ENTER_CRITICAL();                                       // Tell uC/OS-II that we are starting an ISR
	OSIntNesting++;
	OS_EXIT_CRITICAL();

	if(pChan != NULL)
	{
		UartDealInIRQ(pChan);
	} 	
	OSIntExit();                                                // Tell uC/OS-II that we are leaving the ISR 
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

