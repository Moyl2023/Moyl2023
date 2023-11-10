/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**          Copyright (C), 2000-2014, SHENZHEN AUTO ELECTRIC POWER PLANT Co., Ltd.           
**
**--------------File Info----------------------------------------------------------------
** File Name: 		UART.c
** Created By: 		xu
** Created Date:	2014-01-16
** Last Version:	1.00.00
** Descriptions:	UART������
** 							ע��UART5Ӳ����֧��DMA,�ڵ�·���ʱʹ����GPS��B���ʱ,
**							���UART5ͨ�ô��ڲ���,��Ҫ��������������
****************************************************************************************/
#include "UART.h"
#include "SetData.h"
#include "SysCtrlData.h"
#include "SubFunction.h"
#include "misc.h"
#include "AnalogData.h"
#include "Gps.h"
#include "AppCAN.h"

/* ���崮�ڲ�������  */
UART_Type UART_COM[5];

uint16_t RtuCommCnt = 0;
/** 
	* ������ص�Ӳ������
	* @brief ���ݿ�����MCU������,ʵ�ʵ�·���,������˼·����
	*/	
/* ���ڻ���ַ / ����DMA����ַ  / ����DMA����ַ / */	
static UART_CHAN_Hardware UART_Hardware[] = {
	{USART1_BASE,DMA1_Channel4_BASE,DMA1_Channel5_BASE,USART1_IRQn,0,5},
	{USART2_BASE,DMA1_Channel7_BASE,DMA1_Channel6_BASE,USART2_IRQn,0,6},
	{USART3_BASE,DMA1_Channel2_BASE,DMA1_Channel3_BASE,USART3_IRQn,0,7},
	{UART4_BASE, DMA2_Channel5_BASE,DMA2_Channel3_BASE,UART4_IRQn,0,8},		
	{UART5_BASE, DMA2_Channel5_BASE,DMA2_Channel3_BASE,UART5_IRQn,0,9},
	};


/**
* �й�UART�Ĵ�������λ˵��
* USART_SR <Status register>
*	  0   :PE/R					<��ż����> 0:NO / 1:
*   1 	:FE/R					<֡����> 0:NO / 1:
*   2 	:NE/R				  <���Ŵ���> 0:NO / 1:
*	  3 	:ORE/R				<���մ���> 0:NO / 1:
*	  4	  :IDLE/R				<�������> 0:NO / 1:EM
*	  5 	:RXNE/RC			<���ռĴ���> 0:NO / 1:
* 	6 	:TC/RC	  		<�������> 0:NO / 1:
*		7 	:TXE/R		    <���ͼĴ�����> 0:EM/1:NO
*	  8 	:LBD/RC			  <LIN���> 0:NO/1:DETECT
* 	9 	:CTS/RW			  <CTS��־λ> 0:NO/1:
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
*	 0    :SBK/RW           <����break>  0:dis / 1:EN
*	 1    :RWU/RW           <��������>   0:dis / 1:EN
*	 2  	:RE/RW            <����ʹ��>  0:dis / 1:EN
*  3    :TE/RW            <����ʹ��>  0:dis / 1:EN
*  4    :IDLEIE/RW        <�������ж�>   0:dis / 1:EN
*  5    :RXNEIE/RW        <��������ж�>  0:dis / 1:EN
*  6    :TCIE/RW          <��������ж�>  0:dis / 1:EN
*  7    :TXEIE/RW         <TXE�ж�>  0:dis / 1:EN
*  8    :PEIE/RW          <PE�ж�> 0:dis / 1:EN
*  9    :PS/RW            <��żУ��>  0:EVEN / 1:Odd
*  10   :PCE/RW           <��żУ��ʹ��> 0:dis / 1:en
*  11   :WAKE/RW          <wakeup> 0:IDLE/1:address mark
*  12   :M/RW             <M�ַ�����> 0:8 / 1:9
*  13   :UE/RW						<USART EN> 0:dis / 1:EN
* 14~31 :
*
* USART_CR2 <control register 2>
*	0~3   :ADD[3:0]/RW      <��ַ> 
*	 4    :Res/RW           
*	 5  	:LBDL/RW          <>  
*  6    :LDBIE/RW         <>  
*  7    :Res/RW           
*  8    :LBCL/RW          <���һλ����>  0:dis / 1:EN
*  9    :CPHA/RW          <ʱ����λ>  0:dis / 1:EN
*  10   :CPOL/RW          <ʱ�Ӽ���>  0:dis / 1:EN
* 12~13 :STOP[1:0]/RW     <ֹͣλ> 00:1 / 01:0.5 /10:2 /11:1.5
*  14   :LINEN/RW         <LINģʽ>  0:dis / 1:en
* 15~31 :
*
* USART_CR3 <control register 3>
*	 0    :EIE/RW           <�����ж�ʹ��> 0:dis / 1:en  
*	 1    :IREN/RW          <IrDAʹ��> 0:dis / 1:en  
*	 2  	:IRLP/RW          <IrDA�͹���> 0:dis / 1:en   
*  3    :HDSEL/RW         <��˫��ѡ��>  0:dis / 1:en  
*  4    :NACK/RW          <SD��NACKʹ��> 0:dis / 1:en 
*  5    :SCEN/RW          <SD��ģ��>  0:dis / 1:EN
*  6    :DMAR/RW          <DMA����ʹ��>  0:dis / 1:EN
*  7    :DMAT/RW          <DMA����ʹ��>  0:dis / 1:EN
*  8    :RTSE/RW          <RTSʹ��> 0:dis / 1:en
*  9    :CTSE/RW          <CTSʹ��> 0:dis / 1:en
*  10   :CTSIE/RW         <CTS�ж�ʹ��>  0:dis / 1:en
* 11~31 :
*
* USART_GTPR <guard time and prescalerregister>
*	 0~7  :PSC[7:0]/RW      <SD���ͳ�ʱ���ʱ��>  
*	 8~15 :GT[7:0]/RW       <SD/I rDAģʽ�·�Ƶ>
* 16~31 :
**/

/*********************************************************************
* void SetUartBaund( USART_TypeDef* pUart, uint16_t baund )
* @brief  :���ô��ڲ�����
* @note   : USART1 APB1  / USART2~UART5 APB2
* @param  :NO 
* @retval :NO
*********************************************************************/
void SetUartBaund( USART_TypeDef* pUart, uint32_t baund )
{
	uint32_t tempCR1,tempREG,DIV_RR,DIV_MA;

	tempCR1 = pUart->CR1;
	/* �رմ��� */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
	pUart->CR1 = 0x00000000;	
	
	/* ���ò����� */
	DIV_MA = ((0x19 * 36000000) / (0x04 * baund));
	tempREG = (DIV_MA / 0x64) << 0x04;	
	DIV_RR = DIV_MA - (0x64 * (tempREG >> 0x04));
	tempREG |= (((((DIV_RR * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F));	
	pUart->BRR = (uint16_t)tempREG;
	
	/* ���״̬�Ĵ��� */
	/* [31~24] / [23~16] */
	/* [15~10] [9:CTS/RC] [8:LBD/RC] */ 
	/* [7:TXE] [6:TC] [5:RXNE] [4:IDLE] [3:ORE] [2:NE] [1:FE] [0:PE] */
	tempREG = pUart->SR;

	/* ������ݼĴ��� */
	tempREG = pUart->DR;

	/* ����UART ʹ��UARTλ ����λ ����λ */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
	pUart->CR1 |= (tempCR1|(1<<13)|(0<<3)|(1<<2));
}

/*********************************************************************
* void StartUartSend(uint8_t com_num,uint16_t count)
* @brief  :������������
* @note   : �˺���ֻ����DMA���� UART5û��DMA֧�ֲ���ʹ�ô˺���
* @param  : uint8_t com_num:�ڼ���UART ֻΪUART1~UART4 
* @retval :NO
*********************************************************************/
void StartUartSend(uint8_t com_num,uint16_t count)
{
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;
	
	/* RS485 RTS*/
	if(UART_COM[com_num].devicType == 1)
	{
		/* ʹ��RS485���� */
		ResetGPIO(&RS485RTS[com_num]);
		/* �������״̬λ */
		UART_COM[com_num].RxdStatus = 0;
		UART_COM[com_num].iRxdCount = 0;
		UART_COM[com_num].iRxdTimeout = 0;		
	}
	UART_COM[com_num].iTxdCount = 0;
	UART_COM[com_num].iTxdTimeout = 0;
	UART_COM[com_num].iTxdNum = count;
	UART_COM[com_num].TxdStatus = 1;
	pUART->DR = UART_COM[com_num].cTxdBuf[0];
	/* λ�÷��ͼĴ����ǿ��ж� */
	pUART->CR1 |= ((uint16_t)1<<7 |(1<<3));
}

/*********************************************************************
* UART_Type* initUART(uint8_t com_num,UART_Control_PARAM UartCtlParam )
* @brief  :��ʼʼ������
* @note   :�˺�����ʼ���Ĵ��ڶ˿�û��ʹ��DMA
* @param  :COMM_Type* pCOM:���ò����Ϳ��Ʊ���
*          uint8_t com_num:��ʼ���ڶ˿�
*					 UART_Control_PARAM UartCtlParam:���Ϳ���	
* @retval :UART_Type* :ָʾ�ṹ��
*********************************************************************/
UART_Type* initUART(uint8_t com_num,UART_Control_PARAM UartCtlParam )
{
	uint8_t baudIndex;
    NVIC_InitTypeDef NVIC_InitStruct;
	uint32_t tempREG,DIV_RR,DIV_MA;
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;	
	
	/* COM ȫ�ֱ��� */
	UART_COM[com_num].pUart = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;
	UART_COM[com_num].ComNum = com_num;
	UART_COM[com_num].devicType = UartCtlParam.devicType;
	
	/* �رմ��� */
	/* USART control register 1  USART_CR1*/
	/*[31~24] / [23~16] */
	/*[15~14] [13:UE] [12:M] [11:WAKE] [10:PCE] [9:PS] [8:PEIE]*/
	/*[7:TXEIE] [6:TCIE] [5:RXNEIE] [4:IDLEIE] [3:TE] [2:RE] [1:RWU] [0:SBK] */
 	pUART->CR1 = 0x00000000;	

 	baudIndex = AnalogData.KeyIo.bit.KeySW2 & 0x07;     //add by lgh 2015-07-30 ������ʾ���ڲ����ʿ��޸ģ�SW2ǰ3λ������0Ϊ���115200
    if (baudIndex > 5)
        baudIndex = 0;
    UartCtlParam.defBuardRate = UartBaudGroup[5-baudIndex];  //5Ϊ��С��1200

    /* ���ò����� */
 	DIV_MA = ((0x19 * 36000000) / (0x04 * UartCtlParam.defBuardRate));
	tempREG = (DIV_MA / 0x64) << 0x04;	
	DIV_RR = DIV_MA - (0x64 * (tempREG >> 0x04));
	tempREG |= (((((DIV_RR * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F));	
	pUART->BRR = (uint16_t)tempREG;
	
	/* ����ֹͣλ */
	/*[31~24] / [23~16] */ 
	/*[15][14:LINEN][13~12:STOP] [11:CLKEN][10:CPOL][9:CPHA][8:LBCL] */
	/*[7] [6:LBDIE] [5:LBDL] [4] [3~0:ADD] */
	tempREG = 0x00000000;	        /* һ��ֹͣλ */
	/* tempREG |= ((UartCtlParam.defStopbits & 0x03)<<12); */
	pUART->CR2 = tempREG;
	
	/* ���ù���ģʽ */	
	/*[31~24] / [23~16] */
  /*[15~11][10:CTSIE][9:CTSE] [8:RTSE] */
	/*[7:DMAT] [6:DMAR] [5:SCEN] [4:NACK][3:HDSEL][2:IRLP][1:IREN][0:EIE] */
	pUART->CR3 = (0<<7)|(0<<6);
	
	/* ���״̬�Ĵ��� */
	/* [31~24] / [23~16] */
	/* [15~10] [9:CTS/RC] [8:LBD/RC] */ 
	/* [7:TXE] [6:TC] [5:RXNE] [4:IDLE] [3:ORE] [2:NE] [1:FE] [0:PE] */
	tempREG = pUART->SR;
	pUART->SR = 0;

	/* ������ݼĴ��� */
	tempREG = pUART->DR;

	/* ����UART ʹ�ܶ˿� ʹ�ܽ��� ʹ�ܷ���*/
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

	/* ʹ��NVIC�������� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = UART_Hardware[com_num].NVIC_IRQChannel;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UART_Hardware[com_num].NVIC_IRQIP1;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UART_Hardware[com_num].NVIC_IRQIP2;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);		
	
	/* ��ʼ�����Ʊ���*/
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
* @brief  :д����������
* @note   :
* @param  :uint8_t com_num:���ڶ˿�
*					 uint8_t rxddata:��д������
* @retval :NO
*********************************************************************/
void WriteRxdBuf(uint8_t com_num,uint8_t rxddata)
{
	/* xu20140911 ���ݴ�������в��������� */
	if(UART_COM[com_num].RxdStatus == Data)
		return;
	
	/* ��ʼ��һ�ν��� */
	if(UART_COM[com_num].RxdStatus != Ing)
	{
		UART_COM[com_num].RxdStatus = Ing;
		UART_COM[com_num].iRxdCount = 0;
	}
	/* ��λ��ʱ������ */
	UART_COM[com_num].iRxdTimeout = 0;
	UART_COM[com_num].cRxdBuf[UART_COM[com_num].iRxdCount] = rxddata;
	/* �洢ָ����� */
	UART_COM[com_num].iRxdCount++;
	/* �ж�ָ�� */
	if(UART_COM[com_num].iRxdCount >= UART_RXD_BUF_SIZE)
		UART_COM[com_num].iRxdCount = 0;
}
/*********************************************************************
* void GpsUartDealIRQ(uint16_t rxData)
* @brief  :GPS�����жϴ�����
* @note   :GPS�����жϵ���
* @param  :uint16_t rxData�����ڽ��յ�������
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
* @brief  :�����жϴ�����
* @note   :�жϵ���
* @param  :uint8_t com_num:���ڶ˿�
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
	
	/* ���ռĴ����ǿ� */
	tempREG = pUART->SR;
	if( tempREG & (1<<5) )
	{
		tempDate = (uint16_t)(pUART->DR & (uint16_t)0x01FF);
        if (com_num != 3 || SysPara.SysCfg.EnableGps == 0)
        {
            WriteRxdBuf(com_num,(uint8_t)tempDate);
    		UART_COM[com_num].iRxdTimeout = 0;
    		if(com_num == 2)  /* ������ʾ���� */
    			SetGPIO(COMLED);
        }
		else                   /* GPS B���ʱ��*/
        {
            GpsUartDealIRQ(tempDate);
        }
	}	 	
	
	/* ���ͼĴ����� */
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
			/* ��ֹ���ռĴ������ж� */
			pUART->CR1 &= ~((uint16_t)1<<7);
		}			
	}		
	
	/* ������� */
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
* UART IRQ�жϺ���������������
* @brief  :�й�UART�жϺ���
* @note   :ע������ж�,�ж��е�����ͬ����
* @param  :
* @retval :NO
*********************************************************************/
/* USART1 ISR */
void USART1_IRQHandler( void )
{
	/* ��ֹȫ���ж� */
	DIS_irq;
	/* ���ô����� */
	dealUartIRQ(0);
	/* ʹ��ȫ���ж� */
	EN_irq;
}
/* USART2 ISR */
void USART2_IRQHandler( void )
{		
	/* ��ֹȫ���ж� */
	DIS_irq;
	/* ���ô����� */
	dealUartIRQ(1);
	/* ʹ��ȫ���ж� */
	EN_irq;
}
/* USART3 ISR */
void USART3_IRQHandler( void )
{
	/* ��ֹȫ���ж� */
	DIS_irq;
	/* ���ô����� */
	dealUartIRQ(2);
	/* ʹ��ȫ���ж� */
	EN_irq;	
}
/* UART4 ISR */
void UART4_IRQHandler( void )
{
	/* ��ֹȫ���ж� */
	DIS_irq;
	/* ���ô����� */
	dealUartIRQ(3);
	
	/* ʹ��ȫ���ж� */
	EN_irq;	
}
/* UART5 ISR */
void UART5_IRQHandler( void )
{
	/* ��ֹȫ���ж� */
	DIS_irq;
	/* ���ô����� */
	dealUartIRQ(4);
	/* ʹ��ȫ���ж� */
	EN_irq;	
}

/*********************************************************************
* void initUARTTimer(void)
* @brief  :��ʼ������UASRT��ʱ�Ķ�ʱ�� TIM6
* @note   : ��Ҫ���ڽ��պͷ���ʱ�䶨ʱ ��ʱ���ͳ�ʱ�ͽ������
*					 72MHzʱ��Դ ��ƵΪ1MHz ��ʱ1KHz 
*						APB1/APB2 clock ��Ƶ72MHzΪ2 ->TIM clk (72/2)*2=72MHz TIMʱ���Ա�Ƶ
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
	
	/* ��ʼ����ʱ��6�ж� ʹ��NVIC�������� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 15;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);	
}

/*********************************************************************
* void dealUARTtimer(uint8_t com_num)
* @brief  :����UART��ʱ
* @note   :�жϵ��ú���
* @param  :
* @retval :NO
*********************************************************************/
void dealUARTtimer(uint8_t com_num)
{
	USART_TypeDef* pUART; 
	pUART = (USART_TypeDef *)UART_Hardware[com_num].UARTAddr;		
	/* ���ճ�ʱ��� */
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
	
	/* ���ʹ���ʱ */
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
			/* ��ֹ���ռĴ������ж� */
			pUART->CR1 &= ~((uint16_t)1<<7);			
		}
	}
	else
		UART_COM[com_num].iTxdTimeout = 0;
}


/*********************************************************************
* void TIM6_IRQHandler( void )
* @brief  :TIM6�жϷ������
* @note   :UARTʹ��ʱ����ʹ��
* @param  :
* @retval :NO
*********************************************************************/
/* TIMx status register  TIMx_SR */
/* [15] [14] [13] [12:CC4OF] [11:CC3OF] [10:CC2OF] [9:CC1OF] [8] */ 
/* [7] [6:TIF] [5] [4:CC4IF] [3:CC3IF] [2:CC2IF] [1:CC1IF] [0:UIF] */
void TIM6_IRQHandler( void )
{
	/* �����־λ */
	TIM6->SR = 0;
	
	/* ��ֹȫ���ж� */
	DIS_irq;
	
	/* ���ô����� */
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
        if (++GpsTimeCtrl.gpsDiffSynDelayCnt >= 600000)   /* 10min��ʱ*/
        {
            GpsTimeCtrl.gpsDiffSynDelayCnt = 0;
            SysCtrlData.Assist.bit.GpsDiffSynFlag = 0;
        }
    }
    if (GpsTimeCtrl.gpsTimerMs%3600000 == 0)        /* ÿСʱ��λһ��CAN�� */
    {
        CAN_Ctrl.LMCANCtrl.ResetFlag = TRUE;
    }
    
    if (SysCtrlData.Work.ReSNBTestTimeFlag == TRUE) /* ���½��е��β�ƽ���ż�ⶨʱ���� */
        SysCtrlData.Work.ReSNBTestTimeCnt++;
        
	/* ��̨ͨ�Ŵ��� */
	if(RtuCommCnt++ > 5000)
	{
		SysCtrlData.Assist.bit.RtuCommStatus = 0;
	}
	
	/* ʹ��ȫ���ж� */
	EN_irq;		
}


/*************************************************************************************
**   UART.c end file
**************************************************************************************/
