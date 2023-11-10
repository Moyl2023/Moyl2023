
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <stdint.h>

#include    "ucos_ii.h"

#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>

#include    <stm32f10x_conf.h>
#include    <stm32f10x.h>

#include    <app_cfg.h>
#include    <bsp.h>

#include "mbsslave.h"
#include "uart.h"
#include "gpio.h"
#include "rtc.h"
#include "Init.h"
#include "MbsMaster.h"
#include "Uart.h"
#include "Message.h"
#include "appstart.h"
#include "appLCD.h"
#include "AppJdyTest.h"
#include "AppUpdate.h"
#include "i2c.h"
#include "crc.h"
#include "LcdComm.h"
#include "ADC.h"
#include "SysCtrl.h"


extern	u8 err;


#if (uC_LCD_MODULE > 0)
//#include    <lcd.h>
#endif

#if (uC_PROBE_OS_PLUGIN > 0)
#include    <os_probe.h>
#endif

#if (uC_PROBE_COM_MODULE > 0)
#include    <probe_com.h>

#if (PROBE_COM_METHOD_RS232 > 0)
#include    <probe_rs232.h>
#endif
#endif




#endif
