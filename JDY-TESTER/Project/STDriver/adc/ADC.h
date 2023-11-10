/**************************************Copyright (c)**************************************
**                               
**                       深圳奥特讯电力设备股份有限公司              
**          Copyright (C), 2000-2014, SHENZHEN AUTO ELECTRIC POWER PLANT Co., Ltd.           
**
**--------------File Info----------------------------------------------------------------
** File Name: 		BSP.h
** Created By: 		xu
** Created Date:	2014-01-16
** Last Version:	1.00.00
** Descriptions:	底层初始化操作
** 
****************************************************************************************/

/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSCLK_H
#define __SYSCLK_H   



#ifdef __cplusplus
 extern "C" {
#endif 
	 
	 
#include <include.h>

/**
* 有关ADC寄存器控制位说明
* ADC_SR <ADC status register>
* 	0		:AWD/RC_W0 			<模拟看门狗> 0:NO/1:YES
*   1		:EOC/RC_W0			<转换结束> 0:NO/1:YES
*	  2	  :JEOC/RC_W0			<ADC 多通道切换转换完成> 0:NO/1:YES
*   3 	:JSTRT/RC_W0		<ADC 多通道切换转换> 0:OFF/1:ON
*   4 	:STRT/RC_W0			<ADC转换启动标志位> 0:OFF/1:ON
*	 5~31	:
*
* ADC_CR1 <ADC control register 1>
*	 0~4	:AWDCH/RW					<看门狗选通道> 00000~10001 CH0~CH17
*   5 	:EOCIE/RW					<中断使能> EOC转换完成 0:dis/1:en 
*   6 	:AWDIE/RW					<中断使能> 看门狗 0:dis/1:en
*	  7 	:JEOCIE/RW				<中断使能> 交错模式 0:dis/1:en
*	  8 	:SCAN/RW					<扫描模式> 0:dis/1:en
*	  9 	:AWD SGL/RW				<看门狗> 单通道扫描模式 0:dis/1:en
* 	10	:JAUTO/RW					<自动模式>on injected group 0:dis/1:en
*		11	:DISC EN/RW				<不连续模式> on regular 0:dis/1:en
*	  12	:JDISCEN/RW				<不连续模式控制> on injected ch 0:dis/1:en
* 13~15	:DISCNUM[2:0]/RW	<不连续模式> 0~7
*	16~19	:DUALMOD[3:0]/RW	<双模式选择> 
*														0000:indep/0001:RS+JS/0010:RS+AT/0011:JS+快交错/0100:JS+慢交错
*														0101:inject连续/0110:规律连续/0111:快交错/1000:慢交错/1001:交替触发
*	20~21	:
*	  22	:JAWDEN/RW				<模拟看门狗使能> 切换通道 0:dis/1:en
*	  23	:AWDEN/RW					<模拟看门狗使能> 规律性 0:dis/1:en
*
* ADC_CR2 <ADC control register 2>
*		0		:ADON/RW				<A/D转换> 0: NO /1:EN
*   1		:CONT/RW 				<连续转换> 0: NO /1:EN
*		2		:CAL/RW					<A/D校准> 0: NO /1:EN
*		3 	:RST CAL/RW			<复位校准> 0: NO /1:EN
*	 4~7	:
*		8		:DMA/RW					<DMA模式> 0: NO /1:EN
*	 9~10	:
*		11	:DMA/RW					<数据对齐> 0: R/1: L
* 12~14	:JEXTSEL/RW			<交错外部触发源>  000: ADC1-2 TIM1TRGO/ADC3 TIM1TRGO
*																					001: ADC1-2 TIM1CC4/ADC3 TIM1CC4
*																					010: ADC1-2 TIM2TRGO/ADC3 TIM4CC3
*																					011: ADC1-2 TIM2CC1/ADC3 TIM8CC2
*																					100: ADC1-2 TIM3CC4/ADC3 TIM8CC4
*																					101: ADC1-2 TIM4TRGO/ADC3 TIM5TRGO
*																					110: ADC1-2 TIM8CC4/ADC3 TIM5CC4
*																					111: ADC1-2 JSWSTART/ADC3 JSWSTART
*		15	:JEXTTRIG/RW		<交错外部触发>  0: NO /1:EN
*		16 	:
*	17~19	:EXTSEL/RW			<规律外部触发源>  000: ADC1-2 TIM1CC1/ADC3 TIM3CC1
*																					001: ADC1-2 TIM1CC2/ADC3 TIM2CC3
*																					010: ADC1-2 TIM1CC3/ADC3 TIM1CC3
*																					011: ADC1-2 TIM2CC2/ADC3 TIM8CC1
*																					100: ADC1-2 TIM3TRGO/ADC3 TIM8TRGO
*																					101: ADC1-2 TIM4CC4/ADC3 TIM5CC1
*																					110: ADC1-2 TIM8TRGO/ADC3 TIM5CC3
*																					111: ADC1-2 SWSTART/ADC3 SWSTART
*		20	:EXITTRIG/RW		<外部触发规律转换通道> 0: dis /1:en
*   21	:JSWSTART/RW		<启动交错转换通道> 0: dis /1:en
*		22	:SWSTART/RW			<启动规律转换通道> 0: dis /1:en
*		23 	:TSVREFE/RW			<温度传感器和Vref> 0: dis /1:en
*	24~31	:
*
* ADC_SMPR1/2 <ADC sample time register 1/2>
* ADC_SMPR1 [31~24] [23~21:SMP17] [20~18:SMP16] [17~15:SMP15] 
*  					[14~12:SMP14] [11~9:SMP13] [8~6:SMP12] [5~3:SMP11] [2~0:SMP10] 
* ADC_SMPR2 [31~30] [29~27:SMP9] [26~24:SMP8] [23~21:SMP7] [20~18:SMP6] [17~15:SMP5] 
*  					[14~12:SMP4] [11~9:SMP3] [8~6:SMP2] [5~3:SMP1] [2~0:SMP0] 
*	 		SMP4[2:0]:通道采样时间选择
*					000:1.5cycles				100:41.5cycles
*					001:7.5cycles				101:55.5cycles
*					010:13.5cycles			110:71.5cycles
*					011:28.5cycles			111:239.5cycles
*					
* ADC_JOFRx <ADC injected channel data offset register x>					
*			[31~12] [11~0:JOFFSETx]		
*					
* ADC_HTR <ADC warchdog high threshold register>	
* ADC_LTR <ADC warchdog low threshold register>	
*			[31~12] [11~0:HT/LT]
*
*	ADC_SQR1/2/3 <ADC regular sequence register 1/2/3>
* ADC_SQR1 [31~24] [23~20:L] [19~15:SQ16] [14~10:SQ15] [9~5:SQ14]  [4~0:SQ13]
* ADC_SQR2 [31~30] [29~25:SQ12] [24~20:SQ11] [19~15:SQ10] [14~10:SQ9] [9~5:SQ8] [4~0:SQ7] 
* ADC_SQR3 [31~30] [29~25:SQ6] [24~20:SQ5] [19~15:SQ4] [14~10:SQ3] [9~5:SQ2] [4~0:SQ1]
*				L:转换数量 0~17
*			 SQ:规律转换的通道数 0~17CH
*
* ADC_JSQR <ADC injected regular sequence register >
* ADC_JSQR [31~22] [21~20:JL] [19~15:JSQ4] [14~10:JSQ3] [9~5:JSQ2]  [4~0:JSQ1]
*			 JL:转换数量 0~3  从高算起,顺序从底开始
*			JSQ:规律转换的通道数 0~17CH
*
* ADC_JDRx <ADC injected data registerx [1:4] >
*  [31~16] [15~0:JDATA]
*
* ADC_DR <ADC regular data register>
*  [31~16:ADC2DATA] [15~0:DATA]
**/	 

/** 
	* 定义位操作寄存器 重映射位寄存器
	* @brief 据据需要定义位定义
	*/	
/* ADC 2:JEOC */	
#define		ADC1_JEOC				*((vu32*)( PERIPH_BB_BASE+(ADC1_BASE-PERIPH_BASE+0)*32+(2*4) ) )

#define		ADC2_JEOC				*((vu32*)( PERIPH_BB_BASE+(ADC2_BASE-PERIPH_BASE+0)*32+(2*4) ) )

/* ADC 0:ADON 2:CAL / 3:RSTCAL / 21:JSWSTART */
#define		ADC1_JSWSTART		*((vu32*)( PERIPH_BB_BASE+(ADC1_BASE-PERIPH_BASE+8)*32+(21*4) ) )
#define		ADC1_RSTCAL			*((vu32*)( PERIPH_BB_BASE+(ADC1_BASE-PERIPH_BASE+8)*32+(3*4) ) )
#define		ADC1_CAL				*((vu32*)( PERIPH_BB_BASE+(ADC1_BASE-PERIPH_BASE+8)*32+(2*4) ) )
#define		ADC1_ADON				*((vu32*)( PERIPH_BB_BASE+(ADC1_BASE-PERIPH_BASE+8)*32+(0*4) ) )

#define		ADC2_JSWSTART		*((vu32*)( PERIPH_BB_BASE+(ADC2_BASE-PERIPH_BASE+8)*32+(21*4) ) )
#define		ADC2_RSTCAL			*((vu32*)( PERIPH_BB_BASE+(ADC2_BASE-PERIPH_BASE+8)*32+(3*4) ) )
#define		ADC2_CAL				*((vu32*)( PERIPH_BB_BASE+(ADC2_BASE-PERIPH_BASE+8)*32+(2*4) ) )
#define		ADC2_ADON				*((vu32*)( PERIPH_BB_BASE+(ADC2_BASE-PERIPH_BASE+8)*32+(0*4) ) )


/* 采样数据对齐  */
#define ADC_DataRA                        ((uint32)0)		/* 右对齐 */
#define ADC_DataLA                        ((uint32)1)   /* 左对齐 */

/* 采样时间宏定义 SMP  */
#define SMP1_5			(uint32)0							/* 1.5 cycles */
#define SMP7_5			(uint32)1							/* 7.5 cycles */
#define SMP13_5			(uint32)2							/* 13.5 cycles */	
#define SMP28_5			(uint32)3							/* 28.5 cycles */
#define SMP41_5			(uint32)4							/* 41.5 cycles */
#define SMP55_5			(uint32)5							/* 55.5 cycles */
#define SMP71_5			(uint32)6							/* 71.5 cycles */
#define SMP239_5		(uint32)7							/* 239.5 cycles */



/* ADC 采样通道 */
#define sampnum				(uint32)2			/* sampnum+1 */
/* JSQ通道顺序 HM+	KM+ KM- I */
#define ADC1_JSQ1CH		(uint32)11
#define ADC1_JSQ2CH		(uint32)12
#define ADC1_JSQ3CH		(uint32)13


//#define ADC1_JSQ4CH		(uint32)11

//#define ADC2_JSQ1CH		(uint32)13
//#define ADC2_JSQ2CH		(uint32)3
//#define ADC2_JSQ3CH		(uint32)2
//#define ADC2_JSQ4CH		(uint32)10


/* ADC_JSQR <ADC injected sequence register> */
/*[31~22] / [21~20:JL] [19~15:JSQ4] [24~10:JSQ3] [9~5:JSQ2] [4~0:JSQ1] */
#define AD1_CH ( (sampnum<<20)|(ADC1_JSQ3CH<<10)|(ADC1_JSQ2CH<<5)|ADC1_JSQ1CH )
//#define AD2_CH ( (sampnum<<20)|(ADC2_JSQ4CH<<15)|(ADC2_JSQ3CH<<10)|(ADC2_JSQ2CH<<5)|ADC2_JSQ1CH )


/** 
	* 采样数据控制
	* @brief 控制ADC采亲数据
	*/	
typedef struct
{
	uint8			AdHead;		      	  /* 采样存储位置*/
	uint8			AdEnd;							/* 采样满一周期 */
	uint8			AdOneEnd;						/* 采样一次完成 */
	int8			AdZeroOffset[2];		 /* ad零飘值 */
	
							/* 母线编号/采样通道编号/采样数据 */
							/* 母线编号 0:一段母线数据 /1:二段母线 */
							/* 采样通道:JSQ通道顺序 电压，霍尔传感器，分流器*/
	uint16 		ADCBuf[3][256];		/* 采样数据存储缓冲区 */
}ADC_control_data; 																			
																				
/* ADC 采样数据控制变量 */
extern ADC_control_data ADC_Data;																				
																				
/* 外部使用函数 */
void initADC(void);
void StartADC( void );
void restartADC(void);



void ADC_TIM3_Configuration(void);
void ADC_Configuration(void);
void ADC1_2_IRQHandler( void );
void GetValue(void);


#ifdef __cplusplus
}
#endif	 
	 
#endif /*__SYSCLK_H */


/*************************************************************************************
**   BSP.h end file
**************************************************************************************/
