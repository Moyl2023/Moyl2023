/**************************************Copyright (c)**************************************
**                               
**                       ���ڰ���Ѷ�����豸�ɷ����޹�˾              
**                                 
**
**--------------File Info----------------------------------------------------------------
** File Name: appstart.c
** Created By: Abe
** Last modified Date: 2016-3-15  
** Last Version: 1.0
** Descriptions: �������߼��ϵͳ
**               ��������
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


//uint8 UpdateCOM = COM1;		/*����ͨ��ͨ�Ŷ˿ں�*/
//uint8 LCDCOM = COM2;		/*LCDͨ�Ŷ˿ں�*/


JDYTEST JdyTest;


uint8 HostAddr = 1;		//

/* �����ջ���� */
static  OS_STK	AppTaskStartStk[APP_TASK_START_STK_SIZE];
__align(8) static  OS_STK	StackUpdateCom[APP_TASK_UpdateCOM_STK_SIZE];
__align(8) static  OS_STK	StackLCD[APP_TASK_LCD_STK_SIZE];
__align(8) static  OS_STK	StackJdyTest[APP_TASK_JDYTEST_STK_SIZE];

	
/***************************************************************************
**  void RCC_Configuration(void)
**   ����:  ����ʱ��ϵͳ SYSCLK=HCLK=PCLK2=72M	PCLK1=36M 	�Լ�����ʱ��
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void RCC_Configuration(void)
{																
	SystemInit(); 	      				
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);	// ����IO�˿�D��B����������IO��ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 																						// USART2ʱ�Ӵ� 
}



/***************************************************************************
**  void DeviceInit(void)
**   ����: �豸��ʼ��
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void DeviceInit(void)
{
 	InPut_OutPut_Init();
	SysI2CInit();			/*��Ҫ����ʼ��I2C��Ϊ�����ȡ���ò���׼��*/
	sysSerialHwInit();			/*���ڳ�ʼ��*/
	sysRtcHwInit();                 // RTC��ʼ��

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
**   ����: �豸��ʼ��
**   ����:  
**   ���:  
**   ����: �������״̬����ֹ��������
****************************************************************************/
void CheckTaskState(void)
{
	
	#define OVERTIME 1000  //����ʱʱ��
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
**   ����: �ֻ����������ʼ�� 
**   ����:  
**   ���:  
**   ����:  
****************************************************************************/
#define PRINTF 0
#define PRINTF_COM COM3
void InitPrintfUsart(void)
{
	UartHandler[PRINTF_COM] = Uart_Open(PRINTF_COM);      /* ��1#����COM */
	assert_param(UartHandler[PRINTF_COM]); 
}

/***************************************************************************
**  void MainControlFun(void)
**   ����: �豸GIPO����
**   ����:  
**   ���:  
**   ����: 
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
	//LcdComm.BeepFlag = ON;//�������η�����
	//	static uint32 maintaskflag = 0;
/************************** ���ø�GPIO��ʼ״̬ *****************************/
	ContrlGPIOOut(&Speaker,FALSE);//�رշ�����
	while(1)
	{
//		printf("\r\n PRE  7777flag = %d   \r\n",maintaskflag++); 
/*********�źŵƿ��Ƴ���****************************/
		LedCtrl();										
/****************�ɽӵ����***************************/		
		//ErrOutputCheck();
/***************ι��*******************************/
		//FeedDog(WdtTimeFlag);

/***************������*******************************/
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

/*****************����2���ղ������ж�BUG����*****************************/
		
		//printf("\r\n AFTER main \r\n"); 
//		printf("\r\n AFTER 7777  flag = %d   \r\n",maintaskflag++); 

		OSTimeDlyHMSM(0,0,0,5); 
	}
}	
/***************************************************************************
**  void InitUartTask(void)
**   ����: ����1��ʼ��
**   ����:  
**   ���:  
**   ����: 
****************************************************************************/
void InitUartTask(uint8 com)
{
		UartHandler[com] = Uart_Open(com);  /* ����ʾͨѶ���� */
		assert_param(UartHandler[com]);
		UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;
		UartRcvCfg[com].p_tail = UartRcvCfg[com].rcvbuf;
		UartRcvCfg[com].FailCount = 0;               /* ���ڴ��������ʼ�� */
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
// ����   : ����ϵͳ�������� 
// ����   : p_arg   δ��
// ����ֵ : ��
// ====================================================================================================	
static  void  AppTaskStart (void *p_arg)
{
	p_arg = p_arg;	
	
	BSP_Init();                                        /* ��ʼ��ϵͳʱ�� */
	
	ATC_QueueCreat(APP_TASK_START_PRIO);          /* ������Ϣ���� */
	ATC_QueueCreat(APP_TASK_LCD_PRIO);           /* ������Ϣ���� */
	ATC_QueueCreat(APP_TASK_UpdateCOM_PRIO);           /* ������Ϣ���� */

	OS_CPU_SysTickInit();                              /* ����ϵͳ����tick 1ms	*/		
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                      /* Determine CPU capacity */
#endif
    OSTaskCreate(TaskJdyTest,   0, StackJdyTest + (APP_TASK_JDYTEST_STK_SIZE - 1), APP_TASK_JDYTEST_PRIO); 

	OSTaskCreate(TaskUpdateCom,  0, StackUpdateCom + (APP_TASK_UpdateCOM_STK_SIZE - 1), APP_TASK_UpdateCOM_PRIO);

	OSTaskCreate(TaskLCD,   0, StackLCD + (APP_TASK_LCD_STK_SIZE - 1), APP_TASK_LCD_PRIO); 
		
 	MainControlFun();                                  /*���ش���*/
}																 

// =========================================================================================================
// main()
// ����: ������� 
// ��������
// ���أ���
// =========================================================================================================
int  main (void)
{
	CPU_INT08U  err;
	//SCB->VTOR = NVIC_VectTab_FLASH | 0x2000;
	RCC_Configuration();	/* System Clocks Configuration */
	BSP_IntDisAll();        /* Disable all interrupts until we are ready to accept them */
	InPut_OutPut_Init();
	SysI2CInit();			
	InitVar();			/*ȫ�ֱ�����ʼ������ȡ�洢�����������ϵͳ������ͨ�Ų�����*/
	//test2019();
	sysRtcHwInit();                 // RTC��ʼ��
	sysSerialHwInit();			/*���ڳ�ʼ��*/
	OSInit();  
	
	OSTaskCreate(AppTaskStart, (void *)0,	(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], APP_TASK_START_PRIO);	// Create the start task

#if (OS_TASK_NAME_SIZE > 13)
    OSTaskNameSet(APP_TASK_START_PRIO, "Start Task", &err);
#endif

  	OSStart();																				   // Start multitasking (i.e. give control to uC/OS-II)                                                   
}


// ========= ���Ӻ������� ================================================================
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

