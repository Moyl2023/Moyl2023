/****************************************Copyright (c)**************************************************
**                               深圳奥特迅电力设备股份有限公司
**                                     			研发中心
**                                                                 
**
**文件名: Config.h     
**
**作者:  RM       版本:v1.0        日期: 2010/4/26
**
**描述:    			
**
**		用户配置文件
**
**历史记录:        


****************************************************************************************************/
#ifndef __CONFIG_H 
#define __CONFIG_H

////////系统配置///////////////

#define MASTER		0		//主终端
#define SLAVE			1		//从终端 
#define ETHNET		0 	//网络
#define	GPRS			1		//gprs 
#define ATC_GPRS	0	//奥特迅GPRS
#define OTHER_GPRS  1	//其他公司GPRS


#define FALSE		0
#define TRUE		1

//#define DEVICE_TYPE		MASTER//SLAVE//		//配置设备类型 默认为主终端
//
//#if	(DEVICE_TYPE == MASTER)
//#define HOST_NET_TYPE	GPRS//ETHNET//		//配置主终端网络类型 默认为网络	
//#endif 
//#if (DEVICE_TYPE == MASTER) && (DEVICE_TYPE == GPRS)
//#define GPRS_TYPE	ATC_GPRS  						//配置GPRS类型
//#endif


///////////////////////////////



typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */

//rm add
typedef void 		(*FUNCPTR) ();	/* ptr to function returning int */
typedef	unsigned char BOOL;
typedef void VOID;
/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II的特殊代码      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE 用户模式,ARM代码                  */
// 
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.只能是0x10,0x30,0x1f,0x3f之一       */
#include "Includes.h"
//#define APP_DEBUG                1        // 应用程序测试
#define APP_DEBUG                0        // 应用程序测试
/********************************/
/*      ARM的特殊代码           */
/*      ARM specital code       */
/********************************/
//这一段无需改动
//This segment should not be modify

#include    "stm32f10x.h"
//#include    "ATCS2410.h"

/********************************/
/*     应用程序配置             */
/*Application Program Configurations*/
/********************************/
//以下根据需要改动
//This segment could be modified as needed.
#include    <stdio.h>
//#include    <ctype.h>	//rm delete 2008/2/26 ,和lwip中有冲定义部分
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>


/********************************/
/*     本例子的配置             */
/*Configuration of the example */
/********************************/
/* System configuration .Fosc、Fcclk、Fcco、Fpclk must be defined */
/* 系统设置, Fosc、Fcclk、Fcco、Fpclk必须定义*/
#define Fosc            12000000                    //Crystal frequence,10MHz~25MHz，should be the same as actual status. 
						    //应当与实际一至晶振频率,10MHz~25MHz，应当与实际一至
#define FCLK           ((Fosc/(5*2))*169)                  //System frequence,208MHZ
						   
#define FHCLK           (FCLK / 2)              //AHB BUS frequence

#define FPCLK           (FHCLK / 2)             //VPB clock frequence.
						    //VPB时钟频率，只能为(FCLK / 4)的1、2、4倍

#define DelayMs(x) OSTimeDly((x+4)/5)		// 延时x毫秒
#define DelaySec(x) OSTimeDly(OS_TICKS_PER_SEC*x) 	// 延时x秒

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

