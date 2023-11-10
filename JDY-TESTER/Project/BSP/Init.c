/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: init.c
** Created By: xu
** Last modified Date:  2012-05-24
** Last Version: 1.0
** Descriptions: 基IAR ARM 6.30软件版本 初始化程序
****************************************************************************************/
#include "def.h"
//#include "stm32f10x_map.h"
#include <includes.h>
#include "stm32f10x_rcc.h"
#include "cpu.h"
#include "Init.h"
#include "uart.h"
#include "gpio.h"
#include "MbsSlave.h"
#include "MbsMaster.h"
#include "appstart.h"

//-------------------------------------------------
//文件内部调用函数
void  BSP_IntInit (void);
void IntVectSet (CPU_DATA int_id, CPU_FNCT_VOID  isr);
static  void  IntHandlerDummy (void);
void FLASH_SetLatency(u32 FLASH_Latency);
void FLASH_PrefetchBufferCmd(u32 FLASH_PrefetchBuffer);

void InitPort( void );
void InitFSMC( void );
//void InitDAC( void );
void InitRemap( void );

#define BSP_INT_SRC_NBR 60
 CPU_FNCT_VOID  IntVectTbl[BSP_INT_SRC_NBR];



#define ACR_LATENCY_Mask                ((u32)0x00000038)
#define ACR_HLFCYA_Mask                 ((u32)0xFFFFFFF7)
#define ACR_PRFTBE_Mask                 ((u32)0xFFFFFFEF)
#if 0
#define FLASH_Latency_0                 ((u32)0x00000000)  /* FLASH Zero Latency cycle */
#define FLASH_Latency_1                 ((u32)0x00000001)  /* FLASH One Latency cycle */
#define FLASH_Latency_2                 ((u32)0x00000002)  /* FLASH Two Latency cycles */
#define FLASH_PrefetchBuffer_Enable     ((u32)0x00000010)  /* FLASH Prefetch Buffer Enable */
#define FLASH_PrefetchBuffer_Disable    ((u32)0x00000000)  /* FLASH Prefetch Buffer Disable */
#endif
/***************************************************************************
**  void Init( void )
**   功能: 初始化各外设和时钟
**   参数:  
**   输出:  
**   描述: 初始化系统时钟/各种外设
****************************************************************************/
//
void Init( void )
{
    //复位初始化时间控制
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    //sysclk = RCC_PLLMul_x*8;
    //RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) 
    {
        ;
    }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while (RCC_GetSYSCLKSource() != 0x08) 
    {
        ;
    }

    InitFSMC( );
    //InitDac( );
    InitRemap( );
   
}
/***************************************************************************
**  void InitDAC( void )
**   功能: 初始化DAC
**   参数:  
**   输出:  
**   描述: 直接操作寄存器 2个DAC
****************************************************************************/
/*void InitDAC( void )
{
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC, ENABLE );
    DAC->CR = 1;
    DAC->SWTRIGR = 0;
    DAC->DHR12R1 = 0xA30;
}*/
/***************************************************************************
**  void InitPort( void )
**   功能: 初始化端口
**   参数:  
**   输出:  
**   描述: 直接操作寄存器
**        CRH/L   CNF[1:0] MODE[1:0]组成4位  <复位值:0x4444 4444>  RW
**        CNF I->00:analog input mode               O ->00:general purpose output push-pull
**             ->01:floating input (reset state)      ->01:general purpose output open-drain 
**             ->10:input with pull-up/pull-down      ->10:alternate function output push-pull
**             ->11:reserved                          ->11:alternate function output open-drain
**          MODE->00:input mode(reset state)
**             ->01:output mode,max speed 10MHz
**             ->10:output mode,max speed 20MHZ
**             ->11:output mode,max speed 50MHZ
**          IDR<32bit> port input data register <复位值:0x0000 xxxx> R
**          ODR<32bit> port outpur data register <复位值:0x0000 0000> RW
**          BSRR<32bit> port bit set/reset register<复位值:0x0000 0000> W  BR15~BR0(32:16)/BS15~BS0(15:0)
**                      BR->port rest bit 0:no action/1:reset
**                      BS->port set bit  0:no action/1:set
**          BRR<32bit> port bit reset register <复位值:0x0000 00000> W
**          LCKR<32bit> port configuration register<复位值:0x0000 00000>RW
**                      LCKK[16]/LCK   1:lock
**          ----------------------------------------------------------------
**          AFIO_EVCR<32bit> event control register <复位值:0x0000 0000>   EVOE7 PORT6~4[2:0] PIN3~0:
**              EVOE:event output enable
**              PORT[2:0]:port selection PA~PE
**              PIN[3:0]:pin selection pin0~15
**          AFIO_MAPR<32bit> AF remap and debug I/O configuration register <复位值:0x0000 0000> RW
****************************************************************************/
void InitPort( void )
{
    //---------------------------------------------------
    //打开各端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    //---------------------------------------------------
	
//PORTA
    //PA15	PA14	PA13	PA12	PA11	PA10	PA9	    PA8
    //OUT	SWCLK	SWDIO	CANTx	CANRx	OUT	    OUT	    OUT
    //0001	1000	1111	1010	1000	0001	0001	0001
    //														0001
	//0/RTS1 0	    x	    0	    0	    0/BL	1/LED	1/LED
    GPIOA->CRH = 0x18FA8111;
    //PA7	PA6	    PA5	    PA4	    PA3	    PA2	    PA1	    PA0
    //IN	OUT     DAC	    DAC	    OUT	    IN	    IN	    IN
    //1000	0101	0000	0000	0001	1000	1000	1000
	//		1000(key)								1000	1000(key)
    //1/I1	1/RTS	0	    0	    0/RELAY	1/key	1/key	1/key
    GPIOA->CRL = 0x88001888;//0x85001888;
    GPIOA->ODR = 0x03C7;		
    //---------------------------------------------------
//PORTB
    //PB15	PB14	PB13	PB12	PB11	PB10	PB9	    PB8
    //OUT	OUT	    OUT	     OUT	RXD3	TXD3	SDA	    SCL
    //0001	0001	0001	0001	1000	1101	1110	1110
    //1/DRST 0/NC	0/NC	1/spk	1      	1	    1	     1
    GPIOB->CRH &= 0x0000ff00;  //RX和TX在前面已经初始化  ljw 在后面会将这个函数整理一下
	GPIOB->CRH |= 0x111100EE;
    //PB7	PB6	    PB5	    PB4	    PB3	    PB2	    PB1	    PB0
    //IN	IN	    IN	    IN	    SWO	    BOOT1	OUT	    OUT
    //1000	1000	1000	1000	1000	1000	0001	0001
    //1000	1000	1000	1000	1000
	//1/key	1/key	1/key	1/key	1	     1  	1/NC	1/NC
    GPIOB->CRL = 0x88888811;
    GPIOB->ODR = 0x9FFF;
    //---------------------------------------------------
//PORTC
    //PC15	PC14	PC13	PC12	PC11	PC10	PC9	    PC8
    //RTCX2	RTCX1	IN	    OUT	    RXD	    TXD	    OUT	    OUT
    //0001	0100	1000	1000	1000	1101	0001	0001
    //				1000	 						0001	0001
	//x		x	    1/key	KEY		1	    1	    1/LED	1/LED
    GPIOC->CRH = 0x14888D11;//0x14888D11;
    //PC7	PC6	    PC5	    PC4	    PC3	    PC2	    PC1	    PC0	
    //OUT	OUT	    OUT	    IN	    IN	    IN	    IN	    IN
    //0001	0001	0001	1000	1000	1000	1000	1000 //1.0硬件
	//0001							1000	1000	1000	1000 //2.0硬件
    //1/LED	LED  	1/NC	1/I2	1/key	1/key	1/key	1/key
    GPIOC->CRL = 0x11188888;//0x11188888;
    GPIOC->ODR = 0x3FFF;
    //---------------------------------------------------
//PORTD
    //PD15	PD14	PD13	PD12	PD11	PD10	PD9	PD8
    //D1	D0	NC	NC	A16	NC	NC	NC
    //1001	1001	0001	0001	1001	0001	0001	0001
    //x		x	0	0	x	0	0	0
    GPIOD->CRH = 0x99119111;
    //PD7	PD6	    PD5	    PD4	    PD3	    PD2	    PD1	    PD0	
    //CS	IN	    NWE	    NOE	    IN	    IN	    D3	    D2
    //1011	1000	1001	1001	1000	1000	1001	1001
	//										1000
    //x		1/Dwait	 x	    x	    1/key	1/key	x	x
    GPIOD->CRL = 0xb8998899;
    GPIOD->ODR = 0X004C;
    //---------------------------------------------------
//PORTE
    //PE15	PE14	PE13	PE12	PE11	PE10	PE9	    PE8
    //NC	NC	    NC	    NC	    NC	    D7	    D6	    D5
    //0001	0001	0001	0001	0001	1001	1001	1001
    //0		0	    0	    0	    0	    x	    x	    x
    GPIOE->CRH &= 0xf0000000;//0x11111999;	  //PE15作为UART3的方向控制脚，在前面已经初始化
	GPIOE->CRH |= 0x01111999;
    //PE7	PE6	    PE5	    PE4	    PE3	   PE2	   PE1	   PE0
    //D4	IN	    IN	    IN	    IN     IN	   OUT	   IN
    //1001	1000	1000	1000	1000   1000	   0001	   1000
	//		1000	1000	1000	1000   1000			   1000
    //x		1/key	1/key	1/key	1/key	1/key	1/dog	1/key
    GPIOE->CRL = 0x98888818;
    GPIOE->ODR = 0X007F;	  
}
/***************************************************************************
**  void InitFSMC( void )
**   功能: 初始化FSMC
**   参数:  
**   输出:  
**   描述: 
**  Bank1 0x6000 0000~0x6fff ffff<nor/psram>
** HADDR[27:26] 0~3 bank
****************************************************************************/
void InitFSMC( void )
{
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    //0xa0000000 <复位值0x000030DX>
    //BTCR <BCR/BTR>
    //BTR <32bit>
    FSMC_Bank1->BTCR[0] = (U32)(1|(1<<12)|(1<<13));
    //BCR <32bit> ACCMOD[2]  DATLAT[4]  CLKDV[4]  BUSTURN[4]  DATAST[8]  ADDHLD[4]  ADDSET[4]  RW
    FSMC_Bank1->BTCR[1] = (U32)0x00003F0A;
    //0xa0000104 <复位值0x0fffffff>
    FSMC_Bank1E->BWTR[0] = (U32)0x0fffffff;
}

//void FLASH_SetLatency(u32 FLASH_Latency)
//{
//  /* Check the parameters */
//  assert_param(IS_FLASH_LATENCY(FLASH_Latency));
//  
//  /* Sets the Latency value */
//  FLASH->ACR &= ACR_LATENCY_Mask;
//  FLASH->ACR |= FLASH_Latency;
//}

//void FLASH_PrefetchBufferCmd(u32 FLASH_PrefetchBuffer)
//{
//  /* Check the parameters */
//  assert_param(IS_FLASH_PREFETCHBUFFER_STATE(FLASH_PrefetchBuffer));
//  
//  /* Enable or disable the Prefetch Buffer */
//  FLASH->ACR &= ACR_PRFTBE_Mask;
//  FLASH->ACR |= FLASH_PrefetchBuffer;
//}

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        IntVectSet(int_id, IntHandlerDummy);
    }
}

void IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif


    if( int_id < BSP_INT_SRC_NBR )
	{
        CPU_CRITICAL_ENTER();
        IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}

static  void  IntHandlerDummy (void)
{

}
/***************************************************************************
**  CPU_INT32U  BSP_CPU_ClkFreq (void)
**   功能: 获取系统其本时间分频
**   参数:  
**   输出:  
**   描述: RCC_clock<sysclk/Hclk/Pclk1/Pclk2/ADCclk>
****************************************************************************/
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}
/***************************************************************************
**  U32  OS_CPU_SysTickClkFreq (void)
**   功能: 系统处理片时间分频
**   参数:  
**   输出:  
**   描述:
****************************************************************************/
U32  OS_CPU_SysTickClkFreq (void)
{
    U32  freq;
    freq = BSP_CPU_ClkFreq();
    return (freq);
}

/***************************************************************************
**  void InitRemap( void )
**   功能: 重映射GPIO
**   参数:  
**   输出:  
**   描述:
****************************************************************************/
void InitRemap( void )
{    
    //AFIO->MAPR = (2<<24)|(1<<3)|(3<<4);
  AFIO->MAPR = (2<<24);
}

/***************************************************************************
**  void InitVar( void )
**   功能: 变量初始化
**   参数:  
**   输出:  
**   描述:
****************************************************************************/
void InitVar( void )
{    

	memset(&JdyTest.g_TimerMs,0,sizeof(JdyTest));
/**********************软硬件版本号初始化**********************************/	
	JdyTest.HWVersion = HARDWARE_VERSION;//硬件版本:主版本号，次版本号，修订号
	JdyTest.SWVersion = SOFTWARE_VERSION;//软件版本:主版本号，次版本号，修订号

/**********************对MODBUS通信协议里面的变量进行初始化**********************************/


/**********************LCD相关变量初始化**********************************/	

/**********************从铁电里面读取相关参数**********************************/	

	

}
/**************************************************************************************************
**                            End Of File
****************************************************************************************************/
