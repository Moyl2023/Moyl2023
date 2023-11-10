/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: appstart.c
** Created By: Abe
** Last modified Date: 2016-3-15  
** Last Version: 1.0
** Descriptions: 蓄电池在线监测系统
**               启动任务
******************************************************************************************/
#include <includes.h>
#include "uart.h"
#include "gpio.h"
#include "rtc.h"
#include "Init.h"
#include "MbsMaster.h"
#include "Uart.h"
#include "Message.h"
#include "appstart.h"
#include "appLCD.h"
#include "math.h"
#include "stm32f10x.h"


//uint8 UpdateCOM = COM1;		/*升级通信通信端口号*/
//uint8 LCDCOM = COM2;		/*LCD通信端口号*/


JDYTEST JdyTest;


uint8 HostAddr = 1;		//

/* 任务堆栈设置 */
static  OS_STK	AppTaskStartStk[APP_TASK_START_STK_SIZE];
__align(8) static  OS_STK	StackUpdateCom[APP_TASK_UpdateCOM_STK_SIZE];
__align(8) static  OS_STK	StackLCD[APP_TASK_LCD_STK_SIZE];
__align(8) static  OS_STK	StackJdyTest[APP_TASK_JDYTEST_STK_SIZE];

	
/***************************************************************************
**  void RCC_Configuration(void)
**   功能:  配置时钟系统 SYSCLK=HCLK=PCLK2=72M	PCLK1=36M 	以及外设时钟
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void RCC_Configuration(void)
{																
	SystemInit(); 	      				
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);	// 开启IO端口D、B、辅助功能IO的时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 																						// USART2时钟打开 
}



/***************************************************************************
**  void DeviceInit(void)
**   功能: 设备初始化
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void DeviceInit(void)
{
 	InPut_OutPut_Init();
	SysI2CInit();			/*需要早点初始化I2C，为下面读取配置参数准备*/
	sysSerialHwInit();			/*串口初始化*/
	sysRtcHwInit();                 // RTC初始化

	//Init();//add by ljw		
//#if (CFG_IWDG_EN)
//	sysIWdgInit();
//#endif
//#if (CFG_WWDG_EN)
//	sysWWdgInit();
//#endif 
}


void LedCtrl(void)
{
	static uint32_t TimeFlag = 0;
	static uint8_t LedFlag = 0;
	if((JdyTest.g_TimerMs - TimeFlag)>500)
	{
		TimeFlag = JdyTest.g_TimerMs;
		LedFlag++;
		if(LedFlag % 2 == 0)
		{
			STM_EVAL_LED_ON(LED_RUN);
		}
		else
		{
			STM_EVAL_LED_OFF(LED_RUN);
		}
	}
}

/***************************************************************************
**  void DeviceInit(void)
**   功能: 设备初始化
**   参数:  
**   输出:  
**   描述: 检查任务状态，防止任务死机
****************************************************************************/
void CheckTaskState(void)
{
	
	#define OVERTIME 1000  //任务超时时间
	if((JdyTest.g_TimerMs - JdyTest.DataAss.JdyTestTimeFlag) > OVERTIME)
	{
//		OSTaskDel(APP_TASK_COM1_PRIO);
//		ReCreate(APP_TASK_COM1_PRIO);
	}
	if((JdyTest.g_TimerMs - JdyTest.DataAss.UpdateTimeFlag) > OVERTIME)
	{
//		OSTaskDel(APP_TASK_COM2_PRIO);
//		ReCreate(APP_TASK_COM2_PRIO);
	}
	if((JdyTest.g_TimerMs - JdyTest.DataAss.LcdTimeFlag) > OVERTIME)
	{
//		OSTaskDel(APP_TASK_COM3_PRIO);
//		ReCreate(APP_TASK_COM3_PRIO);
	}
}
/***************************************************************************
**  void InitDevCommTask(void)
**   功能: 分机串口任务初始化 
**   参数:  
**   输出:  
**   描述:  
****************************************************************************/
#define PRINTF 0
#define PRINTF_COM COM3
void InitPrintfUsart(void)
{
	UartHandler[PRINTF_COM] = Uart_Open(PRINTF_COM);      /* 打开1#主机COM */
	assert_param(UartHandler[PRINTF_COM]); 
}

/***************************************************************************
**  void MainControlFun(void)
**   功能: 设备GIPO操作
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void MainControlFun(void)
{
	uint32 BeepTimeTemp;
	uint32 WdtTimeFlag = JdyTest.g_TimerMs;
	uint8 flag = 0;
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  

#ifdef PRINTF
	InitPrintfUsart();
#endif
	//uint8 tempflag = OFF;
	//LcdComm.BeepFlag = ON;//测试屏蔽蜂鸣器
	//	static uint32 maintaskflag = 0;
/************************** 设置各GPIO初始状态 *****************************/
	ContrlGPIOOut(&Speaker,FALSE);//关闭蜂鸣器
	while(1)
	{
//		printf("\r\n PRE  7777flag = %d   \r\n",maintaskflag++); 
/*********信号灯控制程序****************************/
		LedCtrl();										
/****************干接点控制***************************/		
		//ErrOutputCheck();
/***************喂狗*******************************/
		//FeedDog(WdtTimeFlag);

/***************任务检查*******************************/
		CheckTaskState();

		if(JdyTest.DataYk.SystemReset == ON)
		{
				if((JdyTest.g_TimerMs - JdyTest.DataAss.RstTimeFlag)>1000)
				{
						NVIC_SystemReset();
				}
		}
		else
		{
				JdyTest.DataAss.RstTimeFlag = JdyTest.g_TimerMs;
		}

/*****************串口2接收不进入中断BUG处理*****************************/
		
		//printf("\r\n AFTER main \r\n"); 
//		printf("\r\n AFTER 7777  flag = %d   \r\n",maintaskflag++); 

		OSTimeDlyHMSM(0,0,0,5); 
	}
}	
/***************************************************************************
**  void InitUartTask(void)
**   功能: 串口1初始化
**   参数:  
**   输出:  
**   描述: 
****************************************************************************/
void InitUartTask(uint8 com)
{
		UartHandler[com] = Uart_Open(com);  /* 打开显示通讯串口 */
		assert_param(UartHandler[com]);
		UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;
		UartRcvCfg[com].p_tail = UartRcvCfg[com].rcvbuf;
		UartRcvCfg[com].FailCount = 0;               /* 串口处理变量初始化 */
}


void ReCreate(uint8 prio)
{
	switch(prio)
	{
		case APP_TASK_JDYTEST_PRIO:
			OSTaskCreate(TaskJdyTest,  0, StackJdyTest + (APP_TASK_JDYTEST_STK_SIZE - 1), APP_TASK_JDYTEST_PRIO);
		break;
		case APP_TASK_LCD_PRIO :
			OSTaskCreate(TaskLCD,  0, StackLCD + (APP_TASK_LCD_STK_SIZE - 1), APP_TASK_LCD_PRIO);
		break;
		case APP_TASK_UpdateCOM_PRIO:
			OSTaskCreate(TaskUpdateCom,  0, StackUpdateCom + (APP_TASK_UpdateCOM_STK_SIZE - 1), APP_TASK_UpdateCOM_PRIO);
		break;

		default:
			break;
	}
}

// ====================================================================================================
// static  void  AppTaskStart (void *p_arg)
// 描述   : 操作系统启动任务 
// 参数   : p_arg   未用
// 返回值 : 无
// ====================================================================================================	
static  void  AppTaskStart (void *p_arg)
{
	p_arg = p_arg;	
	
	BSP_Init();                                        /* 初始化系统时钟 */
	
	ATC_QueueCreat(APP_TASK_START_PRIO);          /* 创建消息队列 */
	ATC_QueueCreat(APP_TASK_LCD_PRIO);           /* 创建消息队列 */
	ATC_QueueCreat(APP_TASK_UpdateCOM_PRIO);           /* 创建消息队列 */

	OS_CPU_SysTickInit();                              /* 操作系统调度tick 1ms	*/		
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                      /* Determine CPU capacity */
#endif
    OSTaskCreate(TaskJdyTest,   0, StackJdyTest + (APP_TASK_JDYTEST_STK_SIZE - 1), APP_TASK_JDYTEST_PRIO); 

	OSTaskCreate(TaskUpdateCom,  0, StackUpdateCom + (APP_TASK_UpdateCOM_STK_SIZE - 1), APP_TASK_UpdateCOM_PRIO);

	OSTaskCreate(TaskLCD,   0, StackLCD + (APP_TASK_LCD_STK_SIZE - 1), APP_TASK_LCD_PRIO); 
		
 	MainControlFun();                                  /*主控处理*/
}																 

// =========================================================================================================
// main()
// 描述: 程序入口 
// 参数：无
// 返回：无
// =========================================================================================================
int  main (void)
{
	CPU_INT08U  err;
	//SCB->VTOR = NVIC_VectTab_FLASH | 0x2000;
	RCC_Configuration();	/* System Clocks Configuration */
	BSP_IntDisAll();        /* Disable all interrupts until we are ready to accept them */
	InPut_OutPut_Init();
	SysI2CInit();			
	InitVar();			/*全局变量初始化，读取存储在铁电里面的系统参数，通信参数等*/
	//test2019();
	sysRtcHwInit();                 // RTC初始化
	sysSerialHwInit();			/*串口初始化*/
	OSInit();  
	
	OSTaskCreate(AppTaskStart, (void *)0,	(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], APP_TASK_START_PRIO);	// Create the start task

#if (OS_TASK_NAME_SIZE > 13)
    OSTaskNameSet(APP_TASK_START_PRIO, "Start Task", &err);
#endif

  	OSStart();																				   // Start multitasking (i.e. give control to uC/OS-II)                                                   
}


// ========= 钩子函数若干 ================================================================
#if (OS_APP_HOOKS_EN > 0)
void  App_TaskCreateHook (OS_TCB *ptcb)
{
	
}


void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}


#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


void  App_TaskStatHook (void)
{
}


#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
	
}
#endif


#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
	
}
#endif
#endif

