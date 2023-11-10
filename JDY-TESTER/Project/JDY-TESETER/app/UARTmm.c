/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**          Copyright (C), 2000-2014, SHENZHEN AUTO ELECTRIC POWER PLANT Co., Ltd.           
**
**--------------File Info----------------------------------------------------------------
** File Name: 		UART.c
** Created By: 		xu
** Created Date:	2014-01-16
** Last Version:	1.00.00
** Descriptions:	UART驱动层
** 							注意UART5硬件不支持DMA,在电路设计时使用于GPS的B码对时,
**							如果UART5通用串口操作,需要独立的驱动操作
****************************************************************************************/
#include "UART.h"
#include "SetData.h"
#include "SysCtrlData.h"
#include "SubFunction.h"
#include "misc.h"
#include "AnalogData.h"
#include "Gps.h"
#include "AppCAN.h"

/* 定义串口操作变量  */
UART_Type UART_COM[5];

uint16_t RtuCommCnt = 0;
/** 
	* 定义相关的硬件操作
	* @brief 根据控制器MCU的资料,实际电路设计,软件设计思路定义
	*/	
/* 串口基地址 / 发送DMA基地址  / 接收DMA基地址 / */	
static UART_CHAN_Hardware UART_Hardware[] = {
	{USART1_BASE,DMA1_Channel4_BASE,DMA1_Channel5_BASE,USART1_IRQn,0,5},
	{USART2_BASE,DMA1_Channel7_BASE,DMA1_Channel6_BASE,USART2_IRQn,0,6},
	{USART3_BASE,DMA1_Channel2_BASE,DMA1_Channel3_BASE,USART3_IRQn,0,7},
	{UART4_BASE, DMA2_Channel5_BASE,DMA2_Channel3_BASE,UART4_IRQn,0,8},		
	{UART5_BASE, DMA2_Channel5_BASE,DMA2_Channel3_BASE,UART5_IRQn,0,9},
	};


/**
* 有关UART寄存器控制位说明
* USART_SR <Status register>
*	  0   :PE/R					<奇偶错误> 0:NO / 1:
*   1 	:FE/R					<帧错误> 0:NO / 1:
*   2 	:NE/R				  <干扰错误> 0:NO / 1:
*	  3 	:ORE/R				<接收错误> 0:NO / 1:
*	  4	  :IDLE/R				<空闲侦测> 0:NO / 1:EM
*	  5 	:RXNE/RC			<接收寄存器> 0:NO / 1:
* 	6 	:TC/RC	  		<发送完成> 0:NO / 1:
*		7 	:TXE/R		    <发送寄存器空> 0:EM/1:NO
*	  8 	:LBD/RC			  <LIN侦察> 0:NO/1:DETECT
* 	9 	:CTS/RW			  <CTS标志位> 0:NO/1:
*	10~31	:NO
*
* USART_DR <data register>
*	 0~8  :Data
*	 9~31	:NO
*
* USART_BRR <Baud rate register>
*	 0~3  :DIV_Rraction[3:0]    
*	4~15  :DIV_Mantissa[11:0]
*	16~31	:NO
*
* USART_CR1 <control register>
*	 0    :SBK/RW           <发送break>  0:dis / 1:EN
*	 1    :RWU/RW           <接收晚醒>   0:dis / 1:EN
*	 2  	:RE/RW            <接收使能>  0:dis / 1:EN
*  3    :TE/RW            <发送使能>  0:dis / 1:EN
*  4    :IDLEIE/RW        <侦察空闲中断>   0:dis / 1:EN
*  5    :RXNEIE/RW        <接收完成中断>  0:dis / 1:EN
*  6    :TCIE/RW          <发送完成中断>  0:dis / 1:EN
*  7    :TXEIE/RW         <TXE中断>  0:dis / 1:EN
*  8    :PEIE/RW          <PE中断> 0:dis / 1:EN
*  9    :PS/RW            <奇偶校验>  0:EVEN / 1:Odd
*  10   :PCE/RW           <奇偶校验使能> 0:dis / 1:en
*  11   :WAKE/RW          <wakeup> 0:IDLE/1:address mark
*  12   :M/RW             <M字符长度> 0:8 / 1:9
*  13   :UE/RW						<USART EN> 0:dis / 1:EN
* 14~31 :
*
* USART_CR2 <control register 2>
*	0~3   :ADD[3:0]/RW      <地址> 
*	 4    :Res/RW           
*	 5  	:LBDL/RW          <>  
*  6    :LDBIE/RW         <>  
*  7    :Res/RW           
*  8    :LBCL/RW          <最后一位脉冲>  0:dis / 1:EN
*  9    :CPHA/RW          <时钟相位>  0:dis / 1:EN
*  10   :CPOL/RW          <时钟极性>  0:dis / 1:EN
* 12~13 :STOP[1:0]/RW     <停止位> 00:1 / 01:0.5 /10:2 /11:1.5
*  14   :LINEN/RW         <LIN模式>  0:dis / 1:en
* 15~31 :
*
* USART_CR3 <control register 3>
*	 0    :EIE/RW           <错误中断使能> 0:dis / 1:en  
*	 1    :IREN/RW          <IrDA使能> 0:dis / 1:en  
*	 2  	:IRLP/RW          <IrDA低功耗> 0:dis / 1:en   
*  3    :HDSEL/RW         <半双工选择>  0:dis / 1:en  
*  4    :NACK/RW          <SD卡NACK使能> 0:dis / 1:en 
*  5    :SCEN/RW          <SD卡模块>  0:dis / 1:EN
*  6    :DMAR/RW          <DMA接收使能>  0:dis / 1:EN
*  7    :DMAT/RW          <DMA发送使能>  0:dis / 1:EN
*  8    :RTSE/RW          <RTS使能> 0:dis / 1:en
*  9    :CTSE/RW          <CTS使能> 0:dis / 1:en
*  10   :CTSIE/RW         <CTS中断使能>  0:dis / 1:en
* 11~31 :
*
* USART_GTPR <guard time and prescalerregister>
*	 0~7  :PSC[7:0]/RW      <SD传送超时完成时间>  
*	 8~15 :GT[7:0]/RW       <SD/I rDA模式下分频>
* 16~31 :
**/

/*********************************************************************
* void SetUartBaund( USART_TypeDef* pUart, uint16_t baund )
* @brief  :设置串口波特率
* @note   : USART1 APB1  / USART2~UART5 APB2
* @param  :NO 
* @retval :NO
*********************************************************************/
void SetUartBaund( USART_TypeDef* pUart, uint32_t baund )
{
	uint32_t tempCR1,tempREG,DIV_RR,DIV_MA;

	tempCR1 = pUart->CR1;
	/* 关闭串口 */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
	pUart->CR1 = 0x00000000;	
	
	/* 设置波特率 */
	DIV_MA = ((0x19 * 36000000) / (0x04 * baund));
	tempREG = (DIV_MA / 0x64) << 0x04;	
	DIV_RR = DIV_MA - (0x64 * (tempREG >> 0x04));
	tempREG |= (((((DIV_RR * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F));	
	pUart->BRR = (uint16_t)tempREG;
	
	/* 清除状态寄存器 */
	/* [31~24] / [23~16] */
	/* [15~10] [9:CTS/RC] [8:LBD/RC] */ 
	/* [7:TXE] [6:TC] [5:RXNE] [4:IDLE] [3:ORE] [2:NE] [1:FE] [0:PE] */
	tempREG = pUart->SR;

	/* 清除数据寄存器 */
	tempREG = pUart->DR;

	/* 启动UART 使能UART位 接收位 发送位 */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
	pUart->CR1 |= (tempCR1|(1<<13)|(0<<3)|(1<<2));
}

/*********************************************************************
* void StartUartSend(uint8_t com_num,uint16_t count)
* @brief  :启动发送外理
* @note   : 此函数只用于DMA发送 UART5没有DMA支持不能使用此函数
* @param  : uint8_t com_num:第几个UART 只为UART1~UART4 
* @retval :NO
*********************************************************************/
void StartUartSend(uint8_t com_num,uint16_t count)
{
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;
	
	/* RS485 RTS*/
	if(UART_COM[com_num].devicType == 1)
	{
		/* 使能RS485发送 */
		ResetGPIO(&RS485RTS[com_num]);
		/* 清除接收状态位 */
		UART_COM[com_num].RxdStatus = 0;
		UART_COM[com_num].iRxdCount = 0;
		UART_COM[com_num].iRxdTimeout = 0;		
	}
	UART_COM[com_num].iTxdCount = 0;
	UART_COM[com_num].iTxdTimeout = 0;
	UART_COM[com_num].iTxdNum = count;
	UART_COM[com_num].TxdStatus = 1;
	pUART->DR = UART_COM[com_num].cTxdBuf[0];
	/* 位置发送寄存器非空中断 */
	pUART->CR1 |= ((uint16_t)1<<7 |(1<<3));
}

/*********************************************************************
* UART_Type* initUART(uint8_t com_num,UART_Control_PARAM UartCtlParam )
* @brief  :初始始化串口
* @note   :此函数初始化的串口端口没有使用DMA
* @param  :COMM_Type* pCOM:调用操作和控制变量
*          uint8_t com_num:初始串口端口
*					 UART_Control_PARAM UartCtlParam:传送控制	
* @retval :UART_Type* :指示结构体
*********************************************************************/
UART_Type* initUART(uint8_t com_num,UART_Control_PARAM UartCtlParam )
{
	uint8_t baudIndex;
    NVIC_InitTypeDef NVIC_InitStruct;
	uint32_t tempREG,DIV_RR,DIV_MA;
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;	
	
	/* COM 全局变量 */
	UART_COM[com_num].pUart = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;
	UART_COM[com_num].ComNum = com_num;
	UART_COM[com_num].devicType = UartCtlParam.devicType;
	
	/* 关闭串口 */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
 	pUART->CR1 = 0x00000000;	

 	baudIndex = AnalogData.KeyIo.bit.KeySW2 & 0x07;     //add by lgh 2015-07-30 主控显示串口波特率可修改，SW2前3位决定，0为最大：115200
    if (baudIndex > 5)
        baudIndex = 0;
    UartCtlParam.defBuardRate = UartBaudGroup[5-baudIndex];  //5为最小：1200

    /* 设置波特率 */
 	DIV_MA = ((0x19 * 36000000) / (0x04 * UartCtlParam.defBuardRate));
	tempREG = (DIV_MA / 0x64) << 0x04;	
	DIV_RR = DIV_MA - (0x64 * (tempREG >> 0x04));
	tempREG |= (((((DIV_RR * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F));	
	pUART->BRR = (uint16_t)tempREG;
	
	/* 设置停止位 */
	/*[31~24] / [23~16] */ 
	/*[15][14:LINEN][13~12:STOP] [11:CLKEN][10:CPOL][9:CPHA][8:LBCL] */
	/*[7] [6:LBDIE] [5:LBDL] [4] [3~0:ADD] */
	tempREG = 0x00000000;	        /* 一个停止位 */
	/* tempREG |= ((UartCtlParam.defStopbits & 0x03)<<12); */
	pUART->CR2 = tempREG;
	
	/* 设置工作模式 */	
	/*[31~24] / [23~16] */
  /*[15~11][10:CTSIE][9:CTSE] [8:RTSE] */
	/*[7:DMAT] [6:DMAR] [5:SCEN] [4:NACK][3:HDSEL][2:IRLP][1:IREN][0:EIE] */
	pUART->CR3 = (0<<7)|(0<<6);
	
	/* 清除状态寄存器 */
	/* [31~24] / [23~16] */
	/* [15~10] [9:CTS/RC] [8:LBD/RC] */ 
	/* [7:TXE] [6:TC] [5:RXNE] [4:IDLE] [3:ORE] [2:NE] [1:FE] [0:PE] */
	tempREG = pUART->SR;
	pUART->SR = 0;

	/* 清除数据寄存器 */
	tempREG = pUART->DR;

	/* 启动UART 使能端口 使能接收 使能发送*/
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
	pUART->CR1 = ((1<<13)|(1<<6)|(1<<5)|(0<<3)|(1<<2));
	
	/* RS485 RTS*/
	if(UART_COM[com_num].devicType == 1)
	{
		SetGPIO(&RS485RTS[com_num]);
	}

	/* 使能NVIC控制向量 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = UART_Hardware[com_num].NVIC_IRQChannel;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UART_Hardware[com_num].NVIC_IRQIP1;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UART_Hardware[com_num].NVIC_IRQIP2;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);		
	
	/* 初始化控制变量*/
	UART_COM[com_num].iTxdNum = 0;
	UART_COM[com_num].iTxdCount = 0;
	UART_COM[com_num].iTxdTimeout = 0;
	UART_COM[com_num].iRxdNum = 0;
	UART_COM[com_num].iRxdCount = 0;
	UART_COM[com_num].iRxdTimeout = 0;
	UART_COM[com_num].TxdStatus = 0;
	UART_COM[com_num].RxdStatus = 0;
	
	UART_COM[com_num].SetUartBaund = &SetUartBaund;
	UART_COM[com_num].StartUartSend = &StartUartSend;
	
	return &UART_COM[com_num];
}

/*********************************************************************
* void WriteRxdBuf(uint8_t com_num,uint8_t rxddata)
* @brief  :写缓冲区队列
* @note   :
* @param  :uint8_t com_num:串口端口
*					 uint8_t rxddata:待写放数据
* @retval :NO
*********************************************************************/
void WriteRxdBuf(uint8_t com_num,uint8_t rxddata)
{
	/* xu20140911 数据处理过程中不接收数据 */
	if(UART_COM[com_num].RxdStatus == Data)
		return;
	
	/* 开始第一次接收 */
	if(UART_COM[com_num].RxdStatus != Ing)
	{
		UART_COM[com_num].RxdStatus = Ing;
		UART_COM[com_num].iRxdCount = 0;
	}
	/* 复位定时计数器 */
	UART_COM[com_num].iRxdTimeout = 0;
	UART_COM[com_num].cRxdBuf[UART_COM[com_num].iRxdCount] = rxddata;
	/* 存储指针递增 */
	UART_COM[com_num].iRxdCount++;
	/* 判断指针 */
	if(UART_COM[com_num].iRxdCount >= UART_RXD_BUF_SIZE)
		UART_COM[com_num].iRxdCount = 0;
}
/*********************************************************************
* void GpsUartDealIRQ(uint16_t rxData)
* @brief  :GPS串口中断处理函数
* @note   :GPS串口中断调用
* @param  :uint16_t rxData：串口接收到的数据
* @retval :NO
*********************************************************************/
void GpsUartDealIRQ(uint16_t rxData)
{
    uint8_t tempData;

    tempData = rxData & 0x00FF;
    if (GpsUartTimeStr.startFlag == FALSE)
    {
        if (tempData == 0x80 && GpsUartTimeStr.head1 == 0x80)
        {
            GpsTimeSetMsValue(MS_OFFSET);
            GpsUartTimeStr.startFlag = TRUE;
            GpsUartTimeStr.count = 1;
        }
        else
        {
            GpsUartTimeStr.head1 = tempData;
        }    
    }
    else
    {
        if (GpsUartTimeStr.count < 46)
        {
            GpsUartTimeStr.gpsBuf[GpsUartTimeStr.count++] = tempData;
        }
        else
        {
            GpsTimeCtrl.gpsTimeFlashFlag = TRUE;
            GpsUartTimeStr.rcvGpsSynTime = GpsTimeCtrl.gpsTimerMs;
            GpsUartTimeStr.count = 0;
            GpsUartTimeStr.startFlag = FALSE;
        }
    }
}
/*********************************************************************
* void dealUartIRQ(uint8_t com_num,uint32_t REG)
* @brief  :串口中断处理函数
* @note   :中断调用
* @param  :uint8_t com_num:串口端口
* @retval :NO
*********************************************************************/
/* USART status register  USART_SR */
/* [31~24] / [23~16] */
/* [15~10] [9:CTS/RC] [8:LBD/RC] */ 
/* [7:TXE] [6:TC] [5:RXNE] [4:IDLE] [3:ORE] [2:NE] [1:FE] [0:PE] */
void dealUartIRQ(uint8_t com_num)
{
	uint16_t tempDate;
	uint32_t tempREG;
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;		
	
	/* 接收寄存器非空 */
	tempREG = pUART->SR;
	if( tempREG & (1<<5) )
	{
		tempDate = (uint16_t)(pUART->DR & (uint16_t)0x01FF);
        if (com_num != 3 || SysPara.SysCfg.EnableGps == 0)
        {
            WriteRxdBuf(com_num,(uint8_t)tempDate);
    		UART_COM[com_num].iRxdTimeout = 0;
    		if(com_num == 2)  /* 接收显示接收 */
    			SetGPIO(COMLED);
        }
		else                   /* GPS B码对时口*/
        {
            GpsUartDealIRQ(tempDate);
        }
	}	 	
	
	/* 发送寄存器空 */
	tempREG = pUART->SR;
	if( tempREG & (1<<7) )
	{
		UART_COM[com_num].iTxdTimeout = 0;
		UART_COM[com_num].iTxdCount ++;
		if(UART_COM[com_num].iTxdCount < UART_COM[com_num].iTxdNum)
		{
			pUART->DR = UART_COM[com_num].cTxdBuf[UART_COM[com_num].iTxdCount];
		}
		else
		{
			/* 禁止发空寄存器空中断 */
			pUART->CR1 &= ~((uint16_t)1<<7);
		}			
	}		
	
	/* 发送完成 */
	/* TCflag read SR and write DR  or write it 0*/
	tempREG = pUART->SR;
	if( tempREG & (1<<6) )
	{		 	
		/* RS485 RTS control */
		if(UART_COM[com_num].devicType == 1)
		{
			SetGPIO(&RS485RTS[com_num]);
		}		
		pUART->SR = ~((uint16_t)1<<6);
		UART_COM[com_num].TxdStatus = 2;
		UART_COM[com_num].iTxdTimeout = 0;
	}		
}

/*********************************************************************
* UART IRQ中断函数集中数服务函数
* @brief  :有关UART中断函数
* @note   :注意关总中断,中断中调用相同函数
* @param  :
* @retval :NO
*********************************************************************/
/* USART1 ISR */
void USART1_IRQHandler( void )
{
	/* 禁止全局中断 */
	DIS_irq;
	/* 调用处理函数 */
	dealUartIRQ(0);
	/* 使能全局中断 */
	EN_irq;
}
/* USART2 ISR */
void USART2_IRQHandler( void )
{		
	/* 禁止全局中断 */
	DIS_irq;
	/* 调用处理函数 */
	dealUartIRQ(1);
	/* 使能全局中断 */
	EN_irq;
}
/* USART3 ISR */
void USART3_IRQHandler( void )
{
	/* 禁止全局中断 */
	DIS_irq;
	/* 调用处理函数 */
	dealUartIRQ(2);
	/* 使能全局中断 */
	EN_irq;	
}
/* UART4 ISR */
void UART4_IRQHandler( void )
{
	/* 禁止全局中断 */
	DIS_irq;
	/* 调用处理函数 */
	dealUartIRQ(3);
	
	/* 使能全局中断 */
	EN_irq;	
}
/* UART5 ISR */
void UART5_IRQHandler( void )
{
	/* 禁止全局中断 */
	DIS_irq;
	/* 调用处理函数 */
	dealUartIRQ(4);
	/* 使能全局中断 */
	EN_irq;	
}

/*********************************************************************
* void initUARTTimer(void)
* @brief  :初始化用于UASRT定时的定时器 TIM6
* @note   : 主要用于接收和发送时间定时 定时发送超时和接收完成
*					 72MHz时钟源 分频为1MHz 定时1KHz 
*						APB1/APB2 clock 分频72MHz为2 ->TIM clk (72/2)*2=72MHz TIM时钟自倍频
* @param  :NO
* @retval :N
*********************************************************************/
void initUARTTimer(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* TIMx control register 1  TIMx_CR1 */
	/* [15~10] [9~8:CKD[1:0]] */ 
	/* [7:ARPE] [6~5:CMS[1:0]] [4:DIR] [3:OPM] [2:URS] [1:UDIS] [0:CEN] */	
	TIM6->CR1 = 0;
	
	/* TIMx DMA/Interrupt enable register  TIMx_DIER */
	/* [15] [14:TDE] [13] [12:CC4DE] [11:CC3DE] [10:CC2DE] [9:CC1DE] [8:UDE] */ 
	/* [7] [6:TIE] [5] [4:CC4IE] [3:CC3IE] [2:CC2IE] [1:CC1IE] [0:UIE] */		
	TIM6->DIER = 0X01;
	
	/* TIMx prescaler register  TIMx_PSC */
	/* [15~0:PSC] 0~65535 */ 
	TIM6->PSC = 71;
	
	/* TIMx auto-reload register  TIMx_ARR */
	/* [15~0:PSC] 0~65535 0x287F*/ 	
	TIM6->ARR = 0x3e7;
	
	/* TIMx control register 1  TIMx_CR1 */
	/* [15~10] [9~8:CKD[1:0]] */ 
	/* [7:ARPE] [6~5:CMS[1:0]] [4:DIR] [3:OPM] [2:URS] [1:UDIS] [0:CEN] */		
	TIM6->CR1 = 0X01;	
	
	/* 初始化定时器6中断 使能NVIC控制向量 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 15;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);	
}

/*********************************************************************
* void dealUARTtimer(uint8_t com_num)
* @brief  :处理UART超时
* @note   :中断调用函数
* @param  :
* @retval :NO
*********************************************************************/
void dealUARTtimer(uint8_t com_num)
{
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;		
	/* 接收超时完成 */
	if(UART_COM[com_num].RxdStatus == Ing)
	{
		UART_COM[com_num].iRxdTimeout ++;
		if(UART_COM[com_num].iRxdTimeout >= 50)
		{
			if(com_num == 4)
			{
				uint16_t sum;
				
				sum = sum;
			}
			UART_COM[com_num].iRxdTimeout = 0;
			UART_COM[com_num].iRxdNum = UART_COM[com_num].iRxdCount;
			UART_COM[com_num].RxdStatus = Data;
		}
	}
	else
		UART_COM[com_num].iRxdTimeout = 0;
	
	/* 发送错误超时 */
	if(UART_COM[com_num].TxdStatus == Ing)
	{
		UART_COM[com_num].iTxdTimeout ++;
		if(UART_COM[com_num].iTxdTimeout >= 50)
		{
			UART_COM[com_num].iTxdTimeout = 0;
			UART_COM[com_num].TxdStatus = 0;
			
			/* RS485 RTS control */
			if(UART_COM[com_num].devicType == 1)
			{
				SetGPIO(&RS485RTS[com_num]);
			}		
			/* 禁止发空寄存器空中断 */
			pUART->CR1 &= ~((uint16_t)1<<7);			
		}
	}
	else
		UART_COM[com_num].iTxdTimeout = 0;
}


/*********************************************************************
* void TIM6_IRQHandler( void )
* @brief  :TIM6中断服务程序
* @note   :UART使用时必须使用
* @param  :
* @retval :NO
*********************************************************************/
/* TIMx status register  TIMx_SR */
/* [15] [14] [13] [12:CC4OF] [11:CC3OF] [10:CC2OF] [9:CC1OF] [8] */ 
/* [7] [6:TIF] [5] [4:CC4IF] [3:CC3IF] [2:CC2IF] [1:CC1IF] [0:UIF] */
void TIM6_IRQHandler( void )
{
	/* 清除标志位 */
	TIM6->SR = 0;
	
	/* 禁止全局中断 */
	DIS_irq;
	
	/* 调用处理函数 */
	dealUARTtimer(0);
	dealUARTtimer(1);
	dealUARTtimer(2);
	dealUARTtimer(3);
	dealUARTtimer(4);
	
	SubData.SndBroadTimers++;
    GpsTimeCtrl.gpsTimerMs++;
    GpsTimeCtrl.synTimeTickFlag = TRUE;
    if (SysCtrlData.Assist.bit.GpsDiffSynFlag)
    {
        if (++GpsTimeCtrl.gpsDiffSynDelayCnt >= 600000)   /* 10min延时*/
        {
            GpsTimeCtrl.gpsDiffSynDelayCnt = 0;
            SysCtrlData.Assist.bit.GpsDiffSynFlag = 0;
        }
    }
    if (GpsTimeCtrl.gpsTimerMs%3600000 == 0)        /* 每小时复位一次CAN口 */
    {
        CAN_Ctrl.LMCANCtrl.ResetFlag = TRUE;
    }
    
    if (SysCtrlData.Work.ReSNBTestTimeFlag == TRUE) /* 重新进行单次不平衡桥检测定时计数 */
        SysCtrlData.Work.ReSNBTestTimeCnt++;
        
	/* 后台通信处理 */
	if(RtuCommCnt++ > 5000)
	{
		SysCtrlData.Assist.bit.RtuCommStatus = 0;
	}
	
	/* 使能全局中断 */
	EN_irq;		
}


/*************************************************************************************
**   UART.c end file
**************************************************************************************/
