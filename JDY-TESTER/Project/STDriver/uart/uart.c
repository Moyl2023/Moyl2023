/****************************************Copyright (c)**************************************************
**                               ���ڰ���Ѹ�����豸�ɷ����޹�˾
**                                     	   �з�����
**                                                                 
**
**�ļ���: UART.C     
**
**����:  RM       �汾:v1.0        ����: 2010/4/26
**
**����:    			
**
**		stm32f10x UART ��uCOS-II�µĵײ������ļ�
**
		RS232/485����ģʽ:option:����ģʽ����
		[option.0 - option.1]	00 ��ʾRS232����
							01 ��ʾRS485����
							
		[option.2 - option.3]	��������ʾ��λ����ģʽ
							��������ʾ��λ����ģʽ
							��������ʾ��λ����ģʽ
							��������ʾ��λ����ģʽ
							
		[option.5 ]			0 ��ʾ����ֹͣλ
							1 ��ʾ����ֹͣλ

		[option.6 - option.8]	000 ��ʾ��Ч��
							001 ��ʾ��Ч��
							010 ��ʾżЧ��
		
	
**��ʷ��¼:        
*2008/5/25,���RS485 ���ӳٴ���by rm
*2014/6/25,RS485��ʼ��Ϊ����ģʽ��by Abe
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

static STM32F10x_SIO_CHAN_PARAM UARTParameter[] = {	     //ע��:����2-3 ��ԭ��ͼ/PCB  ����� 
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
// �����豸�򿪺��� 
// ����:  deviceId	:����ID   ȡֵ��Χ�� 1 ~  N_UART_CHANNELS
// ����ֵ�� FALSE - ��ʧ��, ���������ھ�� 
// *************************************************************************
int32 Uart_Open(uint32 deviceId)
{
	uint8 err;

	if(deviceId <= N_UART_CHANNELS)
	{
		OSSemPend(UARTChan[deviceId].uartLock, 0, &err);   // ��ȡ�ź��� 
			
		if(UARTChan[deviceId].deviceOpenFlag == FALSE)	   // δ��״̬ 
		{						
			UARTChan[deviceId].deviceOpenFlag = TRUE;  	   // ���Ϊ�Ѵ�    
			OSSemPost(UARTChan[deviceId].uartLock);			
			return (int32)&UARTChan[deviceId];
		}
		OSSemPost(UARTChan[deviceId].uartLock);	           // �ͷ��ź��� 		
	}	
	return FALSE;
}
// ***********************************************************************
// ���ڲ��������ò���  
// ����:  pChan - ���ھ����pbase - �Ĵ�����������ַ   
// ����ֵ�� FALSE - ��ʧ��, ���������ھ�� 
// *************************************************************************

BOOL Uart_BaudSet(STM32F10x_UART_CHAN* pChan,  USART_TypeDef* pbase)
{
	uint32 apbclock = 0, tmpreg = 0, integerdivider=0, fractionaldivider=0;
	RCC_ClocksTypeDef  RCC_ClocksStatus; 
	assert_param(IS_USART_BAUDRATE(bps));  	                    // �������� 
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
// �������� 
// ���ж� ����ʹ�� 
// pbase -- ���ڼĴ�������ַ 
// *************************************************/
static void  Uart_Start(USART_TypeDef* pbase)
{
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	// ���ͻ����������Ѿ����� 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// ���ջ������յ����� 

	// USARTʹ�� USART_CR1 bit14  		
	USART_Cmd(USART1, ENABLE);

}
// *************************************************
// UartOptionSet -set uart option .byte size,stop bit,parity 
// writer:rm		
// PARA: pChan ���ڻ�����Ϣ���ݽṹ ��	pbase �Ĵ�������ʼ��ַ 
// RETURN: 
// ************************************************
static void UartOptionSet(STM32F10x_UART_CHAN *pChan, USART_TypeDef* pbase)
{
	uint32 tmpreg = pbase->CR1;
		
	if(pChan->WordLength == USART_WordLength_9b)				// byte size
		tmpreg |= (1<<12);                      // һ����ʼλ��9������λ��һ��ֹͣλ 	
	else
		tmpreg &= ~((uint32)(1<<12));				// һ����ʼλ��8������λ��n��ֹͣλ 
	if(pChan->Parity != USART_Parity_No)  // У����Ʊ�ʹ��
	{
		tmpreg |= (1<<10);	
		if(pChan->Parity == USART_Parity_Odd) // ��У��
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
	tmpreg &= ~((uint32)(3<<12));	        // һ��ֹͣλ 
	if(pChan->Stopbits == USART_StopBits_0_5)          // 0.5��ֹͣλ 
		tmpreg |= (1<<12);
	else if(pChan->Stopbits == USART_StopBits_2)      // 2��ֹͣλ 
		tmpreg |= (2<<12);	
	else if(pChan->Stopbits == USART_StopBits_1_5)     // 1.5��ֹͣλ 
		tmpreg |= (3<<12);	
	pbase->CR2 = tmpreg;  	
}
// ***************************************************************************
// UartTypeSet -   �ܽ����ü���ʼ��   485ģʽʱ �շ����ƹܽ����ü����ʼ��		
// PARA:   pChan ���ڻ�����Ϣ���ݽṹ ��pbase �Ĵ�������ʼ��ַ 
// RETURN: �� 
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
		// ��������PA9��PA10����USB(CN2)���ã�������JP42 JP43 ��������ӳ�� 	USB���ܸ��� 
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ��ʹ��USAR1�� IO��ӳ�书��		
		GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
		
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			//GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_SET);  //receive
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);
		}
	 }
	 else if (pbase == (USART_TypeDef*)USART2_BASE)
	 {
		// ������ ��ӳ�� �� PD3-PD7  (USART2_CTS USART2_RTS USART2_TX USART2_RX USART2_CK)
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ʹ��USAR2�� IO��ӳ�书��	 ��Ӱ���������� ֻ��ռ��GPIO 	
		//GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)USART3_BASE)
	 {
		// ����ӳ�䲻���У���Ϊ��ethnet���� 
		// ������ӳ�䲻���У���Ϊӳ��ռ��uart4�ܽ�
		// ȫ����ӳ�� ���ܿ��У� ����ethnetһ��������ӳ�� 	
		// ������ ��ȫ��ӳ�� �� PD8-12  (USART3_TX USART3_RX USART3_CK USART3_CTS USART3_RTS)
		// ���ø�����ӳ��͵���I/O���üĴ���AFIO_MAPR ʹ��USAR3�� IO������ӳ�书��	
//		GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);	 //PB10��PB11û����ӳ��
		//GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE);	 //PB10��PB11û����ӳ��
		
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART4_BASE)
	 {
		// û����ӳ��Ĺ��� PC10-11 (UART4_TX UART4_RX)	
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
							
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)  // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //����4�õ��й��ʹ�ܽſ����෴
		}	 
	 }
	 else if (pbase == (USART_TypeDef*)UART5_BASE)
	 {
		// û����ӳ��Ĺ��� PC12 PD2 (UART5_TX UART5_RX)
		//enble clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
								
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		         // GPIO�ܽ�����	  USART_RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                // GPIO�ܽ�����	  USART_TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		if(pChan->devicType == RS485TYPE)                        // 485��ʽ ��Ҫ����485�շ����ƹܽ� 
		{
			GPIO_InitStructure.GPIO_Pin = pChan->rs485ContrlPin;                // GPIO�ܽ�����	  USART_TXEN
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init((GPIO_TypeDef*)pChan->directionGPIO, &GPIO_InitStructure); 
			GPIO_WriteBit(pChan->directionGPIO,pChan->rs485ContrlPin, Bit_RESET);  //receive
		}	 
	}
	
	if(pChan->devicType == IRDATYPE)
	{	//����ģʽ
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
//ģʽ�ж�
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
	STM32F10x_UART_CHAN* pChan = (STM32F10x_UART_CHAN*)devDesc;	 // ������Ϣ���ݽṹ 
	USART_TypeDef* pbase = (USART_TypeDef*)pChan->base;	         // �Ĵ�������ַ���ݽṹ 

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
			if(Uart_BaudSet(pChan, pbase) == TRUE)     // ���ò����� 
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
	for(ret = 0; ((ret < MAX_UART_BUF_LENTH) && (ret < maxSize)); ret++)	           // ȡ���� 
	{
	   if(QueueRead(buf, pChan->RcvBuf) == QUEUE_OK)           // ��ȡ�ɹ� 
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
		while(QueueWrite((void *)pChan->SendBuf, buf) == QUEUE_OK)    // ������� 
		{
			len --;
			buf++;
			if(len <= 0)
				break;			
		}
		USART_ITConfig(pbase, USART_IT_TXE, ENABLE);	
		USART_ITConfig(pbase, USART_IT_TC, ENABLE);
		if(pChan->devicType == IR38KHZTYPE)
		{	//38K�����ͺ���ģʽ��Ϊ��˫��ģʽ
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
// ����ʱ��ʹ�� 
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
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
					RCC->APB2ENR |= (1<<14);	
				else
					RCC->APB2ENR &= ~((uint32)(1<<14));				
			}
			else	// usart 2-5
			{
				if(cmd != DISABLE)	  // USART1ʱ��ʹ��
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

	UsartClockOperation(pChan, ENABLE);                          // �������� RCCʱ������ 

	USART_InitStructure.USART_BaudRate = pChan->baudRate;	     // ���ڲ������� bps ����λ ֹͣλ У�鷽ʽ ���� 
	USART_InitStructure.USART_WordLength = pChan->WordLength;
	USART_InitStructure.USART_StopBits = pChan->Stopbits;
	USART_InitStructure.USART_Parity = pChan->Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	UartTypeSet(pChan, pbase);                                   // �ܽ�����

	if(pbase == (USART_TypeDef*)USART1_BASE)                     // UART0,(PA10 USART1_RX,PA9 USART1_TX) 
	{		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	 // �������� USART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	     // �жϺ� 37  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	     // �����ȼ� 0 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ���1�ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 						
		
		USART_Init(USART1, &USART_InitStructure);				 // ���ڲ������� 
 		 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				  		
		USART_Cmd(USART1, ENABLE);	                             // USARTʹ�� USART_CR1 bit13
	}
	else if(pbase == (USART_TypeDef*)USART2_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	 // �������� USART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	     // �жϺ� 38  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	     // �����ȼ� 1 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(USART2, &USART_InitStructure);				 // ���ڲ�������
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 		
		USART_Cmd(USART2, ENABLE);		                         // USARTʹ��		
	}
	else if(pbase == (USART_TypeDef*)USART3_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 	 // �������� USART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	     // �жϺ� 39  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		
		USART_Init(USART3, &USART_InitStructure);				 // ���ڲ������� 
		 
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
				
		USART_Cmd(USART3, ENABLE);								 // USARTʹ��
	}	
	else if(pbase == (USART_TypeDef*)UART4_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 	 // �������� UART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	     // �жϺ� 52  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		

		 
		USART_Init(UART4, &USART_InitStructure);				 // ���ڲ�������

		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART4, USART_IT_TC, ENABLE);
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
 		
		USART_Cmd(UART4, ENABLE);						         // USARTʹ��
	}	
	else if(pbase == (USART_TypeDef*)UART5_BASE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 	 // �������� UART����ʱ�� 

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  	     // ���� ����NVIC  Ӧ���ж��븴λ���ƼĴ���   ����Ӧ�����ȼ����з���
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	     // �жϺ� 53  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	     // �����ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // ʹ�ܴ����ж� 
		NVIC_Init(&NVIC_InitStructure);	                         // �����ж����ȼ���ʹ���ж� 		
		 
		USART_Init(UART5, &USART_InitStructure);				 // ���ڲ�������

		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);	         // ������ж�����-���ͻ����������Ѿ�����
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	         // ������ж�����-���ջ������յ����� 
 		
		USART_Cmd(UART5, ENABLE);						         // USARTʹ��
	}
	UartOptionSet(pChan, pbase); //add option	
}

/***************************************************************************
**  void ComRate(void)
**   ����: ���ڲ�����ת��
**   ����:  
**   ���:  
**   ����: 
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
			UARTChan[i].uartLock = OSSemCreate(1);	                     // �����ź���  ��ʼ��Ϊ�Ѵ� 
			UARTChan[i].base = UARTParameter[i].baseAddr;                // �Ĵ�������ַ 
			UARTChan[i].directionGPIO =  UARTParameter[i].directionGPIO; // �������GPIO 
			UARTChan[i].rs485ContrlPin = UARTParameter[i].rs485ContrlPin;// ������ƽ���� 
			UARTChan[i].deviceID = i+1;                                  // ���� ID
			UARTChan[i].deviceOpenFlag = FALSE;	                         // �򿪱�־��ʼ��Ϊ δ�� 
			UARTChan[i].devicType = UARTParameter[i].devicType;	         // �豸���� 0 rs232 1 rs485

			UARTChan[i].baudRate = UARTParameter[i].defBuardRate;	     // ������ 
			UARTChan[i].WordLength = UARTParameter[i].defWordLength;	 // Ĭ������λ 5-8 
			UARTChan[i].Stopbits = UARTParameter[i].defStopbits;	     // Ĭ��ֹͣλ 1-2  
			UARTChan[i].Parity = UARTParameter[i].defParity;	         // Ĭ��У��λ
			QueueCreate(UARTChan[i].RcvBuf, MAX_UART_BUF_LENTH,1,0,0);     // �������ݶ��� 
			QueueCreate(UARTChan[i].SendBuf, MAX_UART_BUF_LENTH,1,0,0);    // �������ݶ��� 			
			UartDevInit(&UARTChan[i]);
			
		}
	}
}
/*************************************************************************************************
**  void ComInit(void)
**   ����: ͨ�Ų�������
**   ����:  
**   ���:  
**   ����: ������2400---0;4800---1;9600---2;19200---3;38400---4;115200---5
			ͨ��Э��BAT5000-MA --- 1��BAT5000-MBS--- 2;BATM30B---3;BAT5000-MM --- 4��BAT5000-JKQ --- 5��BAT5000-FDY --- 6��
			ͨ�Ŷ˿���;:������1---1;������2---2;�����---3;MODBUS��̨���---4;�ŵ���---5
***************************************************************************************************/
void ComInit(uint8 com)
{
	UARTChan[com].uartLock = OSSemCreate(1);	                     // �����ź���  ��ʼ��Ϊ�Ѵ� 
	UARTChan[com].base = UARTParameter[com].baseAddr;                // �Ĵ�������ַ 
	UARTChan[com].directionGPIO =  UARTParameter[com].directionGPIO; // �������GPIO 
	UARTChan[com].rs485ContrlPin = UARTParameter[com].rs485ContrlPin;// ������ƽ���� 
	UARTChan[com].deviceID = com+1;                                  // ���� ID
	UARTChan[com].deviceOpenFlag = FALSE;	                         // �򿪱�־��ʼ��Ϊ δ�� 
	UARTChan[com].devicType = UARTParameter[com].devicType;	         // �豸���� 0 rs232 1 rs485

	UARTChan[com].baudRate = UARTParameter[com].defBuardRate;	     // ������ 
	UARTChan[com].WordLength = UARTParameter[com].defWordLength;	 // Ĭ������λ 5-8 
	UARTChan[com].Stopbits = UARTParameter[com].defStopbits;	     // Ĭ��ֹͣλ 1-2  
	UARTChan[com].Parity = UARTParameter[com].defParity;	         // Ĭ��У��λ
	
	QueueCreate(UARTChan[com].RcvBuf, MAX_UART_BUF_LENTH,1,0,0);     // �������ݶ��� 
	QueueCreate(UARTChan[com].SendBuf, MAX_UART_BUF_LENTH,1,0,0);    // �������ݶ��� 			
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
	if(USART_GetITStatus(pChan->base, USART_IT_RXNE) != RESET)  // ���ջ������ǿ� 
	{	
		inttimes++;
		rx_data = (uint8)USART_ReceiveData(pChan->base);
		QueueWriteInInt(pChan->RcvBuf, &rx_data); 
		// дһ���ֽڵ�����  �˴��ݲ�������� ������ݲ��ٽ���  		
	}
	if(USART_GetITStatus(pChan->base, USART_IT_TXE) != RESET)  // ���ͻ������Ѿ����� ����дһ���ֽڳ�ȥ 
	{   
		if(QueueReadInInt(&tmpdata, pChan->SendBuf) == QUEUE_OK) // ������Ҫ���� 
		{
			if((pChan->deviceID == 2)||(pChan->deviceID == 4))
			{//����4���������ڲ�ͬ��ʹ�ܿ����߼��෴
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
			{	//38K�����ͺ���ģʽ��Ϊ��˫��ģʽ
				USART_ITConfig(pChan->base, USART_IT_RXNE, ENABLE); //rm add 20110402
			}
		}		
	}
//#if 0
	if(USART_GetITStatus(pChan->base, USART_IT_TC) != RESET)
	{
		if((pChan->deviceID == 2)||(pChan->deviceID == 4))
		{//����4���������ڲ�ͬ��ʹ�ܿ����߼��෴
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
	if(USART_GetITStatus(pChan->base, USART_IT_TXE) != RESET)  // ���ͻ������Ѿ����� ����дһ���ֽڳ�ȥ 
	{   
		if(QueueReadInInt(&tmpdata, pChan->SendBuf) == QUEUE_OK) // ������Ҫ���� 
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

