/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**          Copyright (C), 2000-2014, SHENZHEN AUTO ELECTRIC POWER PLANT Co., Ltd.           
**
**--------------File Info----------------------------------------------------------------
** File Name: 		UART.h
** Created By: 		xu
** Created Date:	2014-01-16
** Last Version:	1.00.00
** Descriptions:	UARTs �� USARTs������
** 
****************************************************************************************/
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __UART_H
#define __UART_H   


#ifdef __cplusplus
 extern "C" {
#endif 

/* ����ͷ�ļ� */
#include "GPIO.h"	
#include "config.h"	 

/** 
	* ����λ�����Ĵ��� ��ӳ��λ�Ĵ���
	* @brief �ݾ���Ҫ����λ����
	*/		 
// #define		UART1_TCIE			*((vu32*)( PERIPH_BB_BASE+(USART1_BASE-PERIPH_BASE+0XC)*32+(6*4) ) )
// #define		UART2_TCIE			*((vu32*)( PERIPH_BB_BASE+(USART2_BASE-PERIPH_BASE+0XC)*32+(6*4) ) )
// #define		UART3_TCIE			*((vu32*)( PERIPH_BB_BASE+(USART3_BASE-PERIPH_BASE+0XC)*32+(6*4) ) )
// #define		UART4_TCIE			*((vu32*)( PERIPH_BB_BASE+(UART4_BASE-PERIPH_BASE+0XC )*32+(6*4) ) )

// #define		UART1_RE				*((vu32*)( PERIPH_BB_BASE+(USART1_BASE-PERIPH_BASE+0XC)*32+(2*4) ) )
// #define		UART2_RE				*((vu32*)( PERIPH_BB_BASE+(USART2_BASE-PERIPH_BASE+0XC)*32+(2*4) ) )
// #define		UART3_RE				*((vu32*)( PERIPH_BB_BASE+(USART3_BASE-PERIPH_BASE+0XC)*32+(2*4) ) )
// #define		UART4_RE				*((vu32*)( PERIPH_BB_BASE+(UART4_BASE-PERIPH_BASE+0XC )*32+(2*4) ) )

// #define		UART1_DMAT			*((vu32*)( PERIPH_BB_BASE+(USART1_BASE-PERIPH_BASE+0X14)*32+(7*4) ) )
// #define		UART2_DMAT			*((vu32*)( PERIPH_BB_BASE+(USART2_BASE-PERIPH_BASE+0X14)*32+(7*4) ) )
// #define		UART3_DMAT			*((vu32*)( PERIPH_BB_BASE+(USART3_BASE-PERIPH_BASE+0X14)*32+(7*4) ) )
// #define		UART4_DMAT			*((vu32*)( PERIPH_BB_BASE+(UART4_BASE-PERIPH_BASE+0X14 )*32+(7*4) ) )

// #define		UART1_DMA_TXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel4_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART2_DMA_TXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel7_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART3_DMA_TXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel2_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART4_DMA_TXD		*((vu32*)( PERIPH_BB_BASE+(DMA2_Channel5_BASE-PERIPH_BASE+0)*32+(0*4) ) )

// #define		UART1_DMA_RXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel5_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART2_DMA_RXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel6_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART3_DMA_RXD		*((vu32*)( PERIPH_BB_BASE+(DMA1_Channel3_BASE-PERIPH_BASE+0)*32+(0*4) ) )
// #define		UART4_DMA_RXD		*((vu32*)( PERIPH_BB_BASE+(DMA2_Channel3_BASE-PERIPH_BASE+0)*32+(0*4) ) )

// #define		UART1_RTS_SET		*((vu32*)( PERIPH_BB_BASE+(GPIOA_BASE-PERIPH_BASE+0X10)*32+(8*4) ) )
// #define		UART1_RTS_CLR		*((vu32*)( PERIPH_BB_BASE+(GPIOA_BASE-PERIPH_BASE+0X14)*32+(8*4) ) )
// #define		UART2_RTS_SET		*((vu32*)( PERIPH_BB_BASE+(GPIOD_BASE-PERIPH_BASE+0X10)*32+(7*4) ) )
// #define		UART2_RTS_CLR		*((vu32*)( PERIPH_BB_BASE+(GPIOD_BASE-PERIPH_BASE+0X14)*32+(7*4) ) )
// #define		UART3_RTS_SET		*((vu32*)( PERIPH_BB_BASE+(GPIOD_BASE-PERIPH_BASE+0X10)*32+(10*4) ) )
// #define		UART3_RTS_CLR		*((vu32*)( PERIPH_BB_BASE+(GPIOD_BASE-PERIPH_BASE+0X14)*32+(10*4) ) )
// #define		UART4_RTS_SET		*((vu32*)( PERIPH_BB_BASE+(GPIOA_BASE-PERIPH_BASE+0X10)*32+(12*4) ) )
// #define		UART4_RTS_CLR		*((vu32*)( PERIPH_BB_BASE+(GPIOA_BASE-PERIPH_BASE+0X14)*32+(12*4) ) )	 
// #define		UART1_RTS_SET		Periph_BB(GPIOA_BASE+0x10,8)
// #define		UART1_RTS_CLR		Periph_BB(GPIOA_BASE+0x14,8)
// #define		UART2_RTS_SET		Periph_BB(GPIOD_BASE+0x10,7)
// #define		UART2_RTS_CLR		Periph_BB(GPIOD_BASE+0x14,7)
// #define		UART3_RTS_SET		Periph_BB(GPIOD_BASE+0x10,10)
// #define		UART3_RTS_CLR		Periph_BB(GPIOD_BASE+0x14,10)
// #define		UART4_RTS_SET		Periph_BB(GPIOA_BASE+0x10,12)
// #define		UART4_RTS_CLR		Periph_BB(GPIOA_BASE+0x14,12)	

	 
/** 
	* ����UART Ӳ��������ص���Դ
	* @brief ֻ��Ե׳�ʼ������Ӳ��
	*/		 
typedef struct 
{
	uint32_t	UARTAddr;	          				    /* UART  PORT */
	uint32_t	DMATxdAddr;								/* ����DMA  */
	uint32_t	DMARxdAddr;								/* ����DMA  */	
	uint8_t 	NVIC_IRQChannel;						/* �жϺ�  */
	uint8_t 	NVIC_IRQIP1;							/* �����ȼ�  */
	uint8_t 	NVIC_IRQIP2;							/* �����ȼ�  */
} UART_CHAN_Hardware;	 

/** 
	* ����UART ��ʼ�������ṹ��
	* @brief ֻ��Ե׳�ʼ������Ӳ��
	*/		 
typedef struct 
{	
	uint8_t		devicType;		      	/* UARTtype 0= rs232 / 1= rs485*/
	uint32_t	defBuardRate;         	/* Ĭ�ϲ����� */
	uint8_t		defWordLength;        	/* Ĭ������λ 5-8 */
	uint16_t	defStopbits;          	/* Ĭ��ֹͣλ 1-2 <0:1λ/1:��λ/2:2λ/3:1λ��>  */
	uint16_t	defParity;            	/* Ĭ��У��λ */
	uint16_t	HardwareFlowControl;  	/* Ӳ������ */
} UART_Control_PARAM;		 

#define COM1    	0
#define COM2    	1
#define COM3    	2
#define COM4    	3
#define COM5    	4

#define COM_DISP	COM3
#define COM_CT      COM1
#define COM_RTU2	COM2
#define COM_RTU1    COM5
#define COM_GPS		COM4
#define UART_COMM_MaxfailNum	5		/* ���ڽ��մ������� */	 

#define RS232		0
#define RS485 		1

#define USART_StopBits_1                     0
#define USART_StopBits_0_5                   1
#define USART_StopBits_2                     2
#define USART_StopBits_1_5                   3


/** 
	* ����UART ���Ʊ����ṹ��
	* @brief �йز��������б���
	*/	 
#define UART_TXD_BUF_SIZE     256
#define UART_RXD_BUF_SIZE     256

#define Idle			0
#define Ing				1
#define End				2
#define Data			2

typedef struct
{
	USART_TypeDef* pUart;					        /* UART port */
	uint8_t	ComNum;		      	  		            /* UART port number*/
	        
	uint8_t	devicType;		      	                /* UARTtype 0= rs232 / 1= rs485*/
	uint8_t	RxdStatus;						        /* ���ջ�����״̬ 0:Ϊ��  1:���ڽ��� 2:������� */
	uint8_t	TxdStatus;						        /* ���ͻ�����״̬ 0:Ϊ��  1:���ڷ��� 2:������� */
	
	uint16_t iTxdNum;						        /* ���ͳ��� */
	uint16_t iTxdCount;						        /* ���ͼ��� */
	uint32_t iTxdTimeout;					        /* ������ɳ�ʱ */
	uint16_t iRxdNum;						        /* ���ճ��� */
	uint16_t iRxdCount;						        /* ���ռ��� */
	uint32_t iRxdTimeout;					        /* ������ɳ�ʱ */	
	uint8_t cTxdBuf[UART_TXD_BUF_SIZE];		        /* ���ͻ����� */
	uint8_t cRxdBuf[UART_RXD_BUF_SIZE];		        /* ���ջ����� */
	
	uint16_t  UartRcvTimeShoeld;			        /* ���ڽ��ճ�ʱ��ֵ */
	uint16_t  RcvFinshDelayShoeld;			        /* ���ճɹ�����ʱ��ֵ */

	void (*SetUartBaund)(USART_TypeDef*,uint32_t);	/* ���ò����ʺ��� */
	void (*StartUartSend)(uint8_t,uint16_t);		/* ��ʼ���� */
}UART_Type; 

typedef struct
{	
	uint8_t 	COMType;							/* �˿����� 0:GPScode /1:���豸״̬/2:���豸״̬ */
	uint8_t 	COMstatus;							/* COMʹ��״̬0:����  /1:�ȴ�������/2:�ȴ�����/3:��ʱ */
	uint8_t		CMDreturn;							/* 0:�㲥����Ҫ����/1:��Ҫ���� */
	uint8_t		failConut;							/* ͨ��ʧ�ܴ��� */
	uint16_t	TimerCount;							/* ʱ������� */
	uint8_t		Rxdfailflag;						/* ���մ���λ/���ճ�ʱ */
	uint8_t 	DelayEndflag;						/* ��ʱ�ȴ�������־ */
	                                                
	uint8_t		TxdNextCMDflag;						/* ���ڴ�������ط� */
	uint8_t		TxdCMD;								/* ��ǰ���͵İ����� */
	uint16_t	TxdAddr;							/* ��ǰ���͵İ���ַ */
}UART_COMM_Control; 


extern UART_Type UART_COM[5];

/* �ⲿʹ�ú��� */
void initUARTTimer(void);
UART_Type* initUART(uint8_t com_num,UART_Control_PARAM UartCtlParam );
void SetUartBaund( USART_TypeDef* pUart, uint32_t baund );

#ifdef __cplusplus
}
#endif	 
	 
#endif /*__UART_H */
/*************************************************************************************
**   UART.h end file
**************************************************************************************/
