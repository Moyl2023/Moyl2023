/****************************************Copyright (c)**************************************************
**                               ���ڰ���Ѹ�����豸�ɷ����޹�˾
**                                     			�з�����
**                                                                 
**
**�ļ���: Config.h     
**
**����:  RM       �汾:v1.0        ����: 2010/4/26
**
**����:    			
**
**		�û������ļ�
**
**��ʷ��¼:        


****************************************************************************************************/
#ifndef __CONFIG_H 
#define __CONFIG_H

////////ϵͳ����///////////////

#define MASTER		0		//���ն�
#define SLAVE			1		//���ն� 
#define ETHNET		0 	//����
#define	GPRS			1		//gprs 
#define ATC_GPRS	0	//����ѸGPRS
#define OTHER_GPRS  1	//������˾GPRS


#define FALSE		0
#define TRUE		1

//#define DEVICE_TYPE		MASTER//SLAVE//		//�����豸���� Ĭ��Ϊ���ն�
//
//#if	(DEVICE_TYPE == MASTER)
//#define HOST_NET_TYPE	GPRS//ETHNET//		//�������ն��������� Ĭ��Ϊ����	
//#endif 
//#if (DEVICE_TYPE == MASTER) && (DEVICE_TYPE == GPRS)
//#define GPRS_TYPE	ATC_GPRS  						//����GPRS����
//#endif


///////////////////////////////



typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */

//rm add
typedef void 		(*FUNCPTR) ();	/* ptr to function returning int */
typedef	unsigned char BOOL;
typedef void VOID;
/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II���������      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE �û�ģʽ,ARM����                  */
// 
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.ֻ����0x10,0x30,0x1f,0x3f֮һ       */
#include "Includes.h"
//#define APP_DEBUG                1        // Ӧ�ó������
#define APP_DEBUG                0        // Ӧ�ó������
/********************************/
/*      ARM���������           */
/*      ARM specital code       */
/********************************/
//��һ������Ķ�
//This segment should not be modify

#include    "stm32f10x.h"
//#include    "ATCS2410.h"

/********************************/
/*     Ӧ�ó�������             */
/*Application Program Configurations*/
/********************************/
//���¸�����Ҫ�Ķ�
//This segment could be modified as needed.
#include    <stdio.h>
//#include    <ctype.h>	//rm delete 2008/2/26 ,��lwip���г嶨�岿��
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>


/********************************/
/*     �����ӵ�����             */
/*Configuration of the example */
/********************************/
/* System configuration .Fosc��Fcclk��Fcco��Fpclk must be defined */
/* ϵͳ����, Fosc��Fcclk��Fcco��Fpclk���붨��*/
#define Fosc            12000000                    //Crystal frequence,10MHz~25MHz��should be the same as actual status. 
						    //Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
#define FCLK           ((Fosc/(5*2))*169)                  //System frequence,208MHZ
						   
#define FHCLK           (FCLK / 2)              //AHB BUS frequence

#define FPCLK           (FHCLK / 2)             //VPB clock frequence.
						    //VPBʱ��Ƶ�ʣ�ֻ��Ϊ(FCLK / 4)��1��2��4��

#define DelayMs(x) OSTimeDly((x+4)/5)		// ��ʱx����
#define DelaySec(x) OSTimeDly(OS_TICKS_PER_SEC*x) 	// ��ʱx��

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

