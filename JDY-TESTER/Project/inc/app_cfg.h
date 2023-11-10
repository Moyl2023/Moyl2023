/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210c Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  uC_PROBE_OS_PLUGIN              DEF_DISABLED            /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  uC_PROBE_COM_MODULE             DEF_DISABLED
#define  uC_LCD_MODULE                   DEF_DISABLED

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define APP_TASK_START_PRIO              10			//��ʼ�������ȼ�
#define APP_TASK_JDYTEST_PRIO        	 11			//LCD��ʾ�������ȼ�
#define APP_TASK_LCD_PRIO        	     12			//LCD��ʾ�������ȼ�
#define APP_TASK_UpdateCOM_PRIO          13

//#define APP_TASK_POWER24V_PRIO	         20 
#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define APP_TASK_START_STK_SIZE         64
#define APP_TASK_UpdateCOM_STK_SIZE 	512
#define APP_TASK_LCD_STK_SIZE		    512
#define APP_TASK_JDYTEST_STK_SIZE       512

#define  OS_TASK_CANTEST_EN	 1
/*
*********************************************************************************************************
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                         1                /* Task will be created for uC/Probe OS Plug-In             */
#define  OS_PROBE_TMR_32_BITS                  0                /* uC/Probe OS Plug-In timer is a 16-bit timer              */
#define  OS_PROBE_TIMER_SEL                    2
#define  OS_PROBE_HOOKS_EN                     1

/*
*********************************************************************************************************
*                                          uC/LCD CONFIGURATION
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  DISP_BUS_WIDTH                        4                /* LCD controller is accessed with a 4 bit bus              */
#define  LIB_STR_CFG_FP_EN                     DEF_DISABLED



#endif
