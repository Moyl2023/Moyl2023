/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**          Copyright (C), 2000-2014, SHENZHEN AUTO ELECTRIC POWER PLANT Co., Ltd.           
**
**--------------File Info----------------------------------------------------------------
** File Name: 		UART.h
** Created By: 		xu
** Created Date:	2014-01-16
** Last Version:	1.00.00
** Descriptions:	UARTs 和 USARTs驱动层
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

/* 包含头文件 */
#include "GPIO.h"	
#include "config.h"	 

/** 
	* 定义位操作寄存器 重映射位寄存器
	* @brief 据据需要定义位定义
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
	* 定义UART 硬件外设相关的资源
	* @brief 只针对底初始化操作硬件
	*/		 
typedef struct 
{
	uint32_t	UARTAddr;	          				    /* UART  PORT */
	uint32_t	DMATxdAddr;								/* 发送DMA  */
	uint32_t	DMARxdAddr;								/* 接收DMA  */	
	uint8_t 	NVIC_IRQChannel;						/* 中断号  */
	uint8_t 	NVIC_IRQIP1;							/* 主优先级  */
	uint8_t 	NVIC_IRQIP2;							/* 次优先级  */
} UART_CHAN_Hardware;	 

/** 
	* 定义UART 初始化变量结构体
	* @brief 只针对底初始化操作硬件
	*/		 
typedef struct 
{	
	uint8_t		devicType;		      	/* UARTtype 0= rs232 / 1= rs485*/
	uint32_t	defBuardRate;         	/* 默认波特率 */
	uint8_t		defWordLength;        	/* 默认数据位 5-8 */
	uint16_t	defStopbits;          	/* 默认停止位 1-2 <0:1位/1:半位/2:2位/3:1位半>  */
	uint16_t	defParity;            	/* 默认校验位 */
	uint16_t	HardwareFlowControl;  	/* 硬件流控 */
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
#define UART_COMM_MaxfailNum	5		/* 串口接收错误数量 */	 

#define RS232		0
#define RS485 		1

#define USART_StopBits_1                     0
#define USART_StopBits_0_5                   1
#define USART_StopBits_2                     2
#define USART_StopBits_1_5                   3


/** 
	* 定义UART 控制变量结构体
	* @brief 有关操作的所有变量
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
	uint8_t	RxdStatus;						        /* 接收缓冲区状态 0:为空  1:正在接收 2:接收完成 */
	uint8_t	TxdStatus;						        /* 发送缓冲区状态 0:为空  1:正在发送 2:发送完成 */
	
	uint16_t iTxdNum;						        /* 发送长度 */
	uint16_t iTxdCount;						        /* 发送计数 */
	uint32_t iTxdTimeout;					        /* 发送完成超时 */
	uint16_t iRxdNum;						        /* 接收长度 */
	uint16_t iRxdCount;						        /* 接收计数 */
	uint32_t iRxdTimeout;					        /* 接收完成超时 */	
	uint8_t cTxdBuf[UART_TXD_BUF_SIZE];		        /* 发送缓冲区 */
	uint8_t cRxdBuf[UART_RXD_BUF_SIZE];		        /* 接收缓冲区 */
	
	uint16_t  UartRcvTimeShoeld;			        /* 串口接收超时阈值 */
	uint16_t  RcvFinshDelayShoeld;			        /* 接收成功，延时阈值 */

	void (*SetUartBaund)(USART_TypeDef*,uint32_t);	/* 设置波特率函数 */
	void (*StartUartSend)(uint8_t,uint16_t);		/* 开始发送 */
}UART_Type; 

typedef struct
{	
	uint8_t 	COMType;							/* 端口类型 0:GPScode /1:从设备状态/2:主设备状态 */
	uint8_t 	COMstatus;							/* COM使用状态0:空闲  /1:等待发送完/2:等待接收/3:延时 */
	uint8_t		CMDreturn;							/* 0:广播不需要返回/1:需要返回 */
	uint8_t		failConut;							/* 通信失败次数 */
	uint16_t	TimerCount;							/* 时间计数器 */
	uint8_t		Rxdfailflag;						/* 接收错误位/接收超时 */
	uint8_t 	DelayEndflag;						/* 延时等待结束标志 */
	                                                
	uint8_t		TxdNextCMDflag;						/* 用于处理错误重发 */
	uint8_t		TxdCMD;								/* 当前发送的包命令 */
	uint16_t	TxdAddr;							/* 当前发送的包地址 */
}UART_COMM_Control; 


extern UART_Type UART_COM[5];

/* 外部使用函数 */
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
