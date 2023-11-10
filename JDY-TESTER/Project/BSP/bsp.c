/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
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
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS
#include "includes.h"
#include "stm3210c_eval_lcd.h"
#include "gpio.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
/* ---------------------- GPIOA Pins ---------------------- */
#define  GPIOA_PB2        DEF_BIT_00
#define  GPIOA_PB3        DEF_BIT_02
#define  GPIOA_LED3       DEF_BIT_03
/* ---------------------- GPIOB Pins ---------------------- */
#define  GPIOB_LED1       DEF_BIT_09
#define  GPIOB_PB1        DEF_BIT_08
/* ---------------------- GPIOC Pins ---------------------- */


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  Tmr_TickInit  (void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         Global Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    Tmr_TickInit();                                             /* Initialize the uC/OS-II tick interrupt                   */
}


/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
*
* Arguments   : None.
*
* Returns     : None.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                      GET THE CPU CLOCK FREQUENCY
*
* Description: This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
*
* Argument(s): None.
*
* Returns    : The CPU clock frequency, in Hz.
*********************************************************************************************************
*/

//CPU_INT32U  BSP_CPU_ClkFreq (void)		 //mask by ljw 131217
//{
//    static  RCC_ClocksTypeDef  rcc_clocks;
//
//
//    RCC_GetClocksFreq(&rcc_clocks);
//
//    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
//}
//
//INT32U  OS_CPU_SysTickClkFreq (void)
//{
//    INT32U  freq;
//
//
//    freq = BSP_CPU_ClkFreq();
//    return (freq);
//}

/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                   PB, LED, LCD Backlight, and ADC Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         PB INITIALIZATION
*
* Description : This function initializes the board's PB
*
* Arguments   : none
*
* Returns     ; none
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                         GET 'PUSH BUTTON' STATUS
*
* Description : This function is used to get the status of any push button on the board.
*
* Arguments   : push_button    is the number of the push button to probe
*                              1    probe the PB1    push button
*                              2    probe the PB2    push button
*                              3    probe the PB3    push button
*                              4    probe the wakeup push button
*
* Returns     : DEF_TRUE  if the push button is pressed
*               DEF_FALSE if the push button is not pressed
*********************************************************************************************************
*/

CPU_BOOLEAN  PB_GetStatus (CPU_INT08U pb)
{
    CPU_BOOLEAN  status;
    CPU_INT32U   pin;


    status = DEF_FALSE;

    switch (pb) {
        case 1:
             pin = GPIO_ReadInputDataBit(GPIOB, GPIOB_PB1);
             if (pin > 0) {
                 status = DEF_TRUE;
             }
             break;

        case 2:
             pin = GPIO_ReadInputDataBit(GPIOA, GPIOA_PB2);
             if (pin > 0) {
                 status = DEF_TRUE;
             }
             break;

        case 3:
             pin = GPIO_ReadInputDataBit(GPIOA, GPIOA_PB3);
             if (pin > 0) {
                 status = DEF_TRUE;
             }
             break;

        default:
             break;
    }

    return (status);
}


/*
*********************************************************************************************************
*                                         LED INITIALIZATION
*
* Description : This function initializes the board's LEDs
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

//static void LCD(void){
//
// /* Initialize the LCD */
//  STM3210C_LCD_Init();
//  /* Clear the LCD */ 
//  LCD_Clear(White);
//  /* Set the LCD Text Color */
//  LCD_SetTextColor(Black);
//   LCD_DisplayStringLine(Line0,"     Mailbox  test");
//}

/*
*********************************************************************************************************
*                                             LED ON
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON user LED1  on the board
*                      2    turns ON user LED2  on the board
*                            .
*                            .
*                            .
*                      16   turns ON user LED16 on the board
*
* Returns     : none
*********************************************************************************************************
*/

void  LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0:
             GPIO_ResetBits(GPIOB, GPIOB_LED1);
             GPIO_ResetBits(GPIOA, GPIOA_LED3);
             break;

        case 1:
             GPIO_ResetBits(GPIOB, GPIOB_LED1);
             break;

        case 3:
             GPIO_ResetBits(GPIOA, GPIOA_LED3);;
             break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             LED OFF
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF user LED1  on the board
*                      2    turns OFF user LED2  on the board
*                            .
*                            .
*                            .
*                      16   turns OFF user LED16 on the board
*
* Returns     ; none
*********************************************************************************************************
*/

void  LED_Off (CPU_INT08U led)
{
     switch (led) {
        case 0:
             GPIO_SetBits(GPIOB, GPIOB_LED1);
             GPIO_SetBits(GPIOA, GPIOA_LED3);
             break;

        case 1:
             GPIO_SetBits(GPIOB, GPIOB_LED1);
             break;

        case 3:
             GPIO_SetBits(GPIOA, GPIOA_LED3);;
             break;

        default:
             break;
    }
    
}


/*
*********************************************************************************************************
*                                             LED TOGGLE
*
* Description : This function is used to toggle any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want to toggle ALL the LEDs
*                      1    toggles user LED on the board
*
* Returns     ; none
*********************************************************************************************************
*/

void  LED_Toggle (CPU_INT08U led)
{
    CPU_INT32U  pins;


    switch (led) {
        case 0:
             pins =  GPIO_ReadOutputData(GPIOB);
             pins ^= GPIOB_LED1;
             GPIO_SetBits(  GPIOB,   pins  & GPIOB_LED1);
             GPIO_ResetBits(GPIOB, (~pins) & GPIOB_LED1);

             pins =  GPIO_ReadOutputData(GPIOA);
             pins ^= GPIOA_LED3;
             GPIO_SetBits(  GPIOA,   pins  & GPIOA_LED3);
             GPIO_ResetBits(GPIOA, (~pins) & GPIOA_LED3);
             break;

        case 1:
		     pins =  GPIO_ReadOutputData(GPIOB);
             pins ^= GPIOB_LED1;
             GPIO_SetBits(  GPIOB,   pins  & GPIOB_LED1);
             GPIO_ResetBits(GPIOB, (~pins) & GPIOB_LED1);
			 break;
        case 3:
		     pins =  GPIO_ReadOutputData(GPIOA);
             pins ^= GPIOA_LED3;
             GPIO_SetBits(  GPIOA,   pins  & GPIOA_LED3);
             GPIO_ResetBits(GPIOA, (~pins) & GPIOA_LED3);
             break;

        default:
             break;
    }
}





/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         uC/OS-II Timer Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*
* Note(s)     : 1) The timer is setup for output compare mode BUT 'MUST' also 'freerun' so that the timer
*                  count goes from 0x00000000 to 0xFFFFFFFF to ALSO be able to read the free running count.
*                  The reason this is needed is because we use the free-running count in uC/OS-View.
*********************************************************************************************************
*/

static  void  Tmr_TickInit (void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    CPU_INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (CPU_INT32U)rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC;
			 
	SysTick_Config(cnts);
}


/*
*********************************************************************************************************
*                                         TIMER IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*
* Note(s)     : 1) The timer is 'reloaded' with the count at compare + the time for the next interrupt.
*                  Since we are using 'unsigned' integer math, overflows are irrelevant.
*********************************************************************************************************
*/

