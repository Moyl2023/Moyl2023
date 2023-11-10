/****************************************Copyright (c)**************************************************
**                               深圳奥特迅电力设备股份有限公司
**                                     	   研发中心
**                                                                 
**
**文件名: UART.h     
**
****************************************************************************************************/
#ifndef _UART_H
#define _UART_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"
#include "../queue/queue.h"

extern uint8 inttimes;

typedef enum
{
	COM1 = 0,
	COM2,
	COM3,
	COM4,
	COM5,
	CAN,
}USART_COM;

//#define PRINTF_COM 		COM4

enum HFControl							 
{
	HFC_NONE,
	HFC_CTRL,
};

#define N_UART_CHANNELS	5 	        // rs232/485 个数 

#define	RS485TYPE	0
#define	RS232TYPE	1
#define	IRDATYPE	2			   //红外模式
#define	IR38KHZTYPE	3			   //38K调制型红外模式


#define SIO_BAUD_SET		0x1003	  	/* serial device I/O controls */
#define SIO_BAUD_GET		0x1004

#define SIO_HW_OPTS_SET		0x1005
#define SIO_HW_OPTS_GET		0x1006

#define SIO_MODE_SET		0x1007
#define SIO_MODE_GET		0x1008
#define SIO_AVAIL_MODES_GET	0x1009
#define SIO_START	0x2008 /*channel start*/
#define SIO_WORK_TYPE_SET	0x2009 /*type set(rs232 or rs485)*/
#define SIO_WORK_TYPE_GET	0x200A /*type Get(rs232 or rs485)*/

#define SIO_PARITY_BIT_SET	0x3001/*Parity set*/

#define SIO_STOP_BIT_SET	0x3003/*STOP BIT set*/

#define SIO_DATA_BIT_SET	0x3005/*DATA set*/


/* options to SIO_HW_OPTS_SET (ala POSIX), bitwise or'ed together */
#define CSIZE		0xc	/* bits 3 and 4 encode the character size */
#define CS5		0x0	/* 5 bits */
#define CS6		0x4	/* 6 bits */
#define CS7		0x8	/* 7 bits */
#define CS8		0xc	/* 8 bits */

#define HUPCL		0x10	/* hang up on last close */
#define STOPB		0x20	/* send two stop bits (else one) */

#define PARENB		0x1c0	/* parity detection setect */
#define PARODD		0x40	/* odd parity*/
#define PAREVEN		0x80	/* even parity  */
#define PARMASK		0xc0	/*Forced “1” stick parity*/
#define PARSPACE	0x100	/* Forced “0” stick parity  */



#define 	MAX_UART_BUF_LENTH    512    //   串口收发缓冲区大小 


typedef struct {
	void*		baseAddr;	          	// register base address
	void*		directionGPIO;	      // direction contrl pin's GPIO
	uint32	rs485ContrlPin;       // rs485  direction contrl pin
	uint8		devicType;		      	// 0= rs232 , 1= rs485*/
	uint32	defBuardRate;         // 默认波特率 
	uint16		defWordLength;        // 默认数据位 5-8 
	uint16		defStopbits;          // 默认停止位 1-2 
	uint16		defParity;            // 默认校验位
	uint16		HardwareFlowControl;  // 硬件流控 
} STM32F10x_SIO_CHAN_PARAM;	
//extern STM32F10x_SIO_CHAN_PARAM UARTParameter[];


/* Device and Channel Structures */
typedef struct {
	OS_EVENT* 	uartLock;	                 		// devece mutex lock	
	void*				base;	                     		// register base address
	void*				directionGPIO;	              // direction contrl pin's GPIO
	uint32			rs485ContrlPin;               // rs485  direction contrl pin
	uint32			deviceID;                     // uart ID 
	uint8				deviceOpenFlag;	              // device flag opened
	uint8				devicType;		             		// 0= rs232 , 1= rs485
	int32   		baudRate;                     // baud rate 
	uint16				WordLength;        			 			// 默认数据位 5-8 
	uint16				Stopbits;          			 			// 默认停止位 1-2 
	uint16				Parity;            			 			// 默认校验位
	uint16				HardwareFlowControl;  		 		// 硬件流控 

	int32				sioMode;                     	// SIO mode
	int32   		options;                     	// SIO options
//	uint32 			read_status_mask;
	uint8				SendBuf[MAX_UART_BUF_LENTH]; // send    buf
	uint8				RcvBuf[MAX_UART_BUF_LENTH];  // receive buf
	uint32  		SendTimer;                  	 // 串口发送超时计数器
} STM32F10x_UART_CHAN;

typedef union SERIALPORT_STR
{
	unsigned char byteData[30];
	struct 
	{
			
		unsigned char serialport_status;			

		unsigned short rev_byte_timeout;			
			
		unsigned short rev_data_count;			
//		unsigned char rev_buf[200];			

				
		unsigned short send_data_count;			
//		unsigned char send_buf[200];		
		unsigned short sending_protect_timeout;
		
//		unsigned int lv1_cmd_index[16];		
		unsigned char lv1_cmd_number;	
		unsigned char CmdType;	

//		unsigned int cur_cmd;		//串口当前发送的命令，用于主机发送命令后处理接收数据 touch_com,ats_com,ujkq_com,jkq3k_com,njkq_com,
	
	}struData;
}SERIALPORT;





extern uint32 UartHandler[];
extern const uint32  UartBaudIndex[];


// 硬件初始化
extern void sysSerialHwInit(void);

void DeInitUsart(uint8 com);
//设备打开
extern int32 Uart_Open(uint32 deviceId);/*device ID*/ 
extern STM32F10x_UART_CHAN *GetChanHandler(uint8 id);
//设备关闭
extern int32 Uart_Close(uint32 devDesc);/*device description*/	

//从设备读数据，返回读出的数据长度
extern int32 Uart_Read(int32 devDesc,	/*device description*/
				uint8 *buf,	            /*save to buffer*/
				uint16 maxSize          // 最大读取数量 
				);
//写入数据,返回成功写入长度
extern int32 Uart_Write(int32 devDesc,	/*device description*/
				uint8 *buf,	/* buffer*/
				int32 size	/*data size writed*/
				);

//串口操作功能
extern int Uart_IOCtrl(int32 devDesc,	/*device description*/
			int32 cmd,	/*request cmd*/
			void* arg	/*some argument*/
			);

void ComRate(uint8 com);
void ComInit(uint8 com);


#ifdef  __cplusplus
}
#endif

#endif  /* _UART_H */


/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/



