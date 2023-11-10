/**************************************Copyright (c)**************************************
** Descriptions:	ADC������
** 			
****************************************************************************************/

#include "stm32f10x.h"
#include "misc.h"
#include "GPIO.h"
#include <includes.h>



/* ADC�������ݿ��Ʊ��� */
ADC_control_data ADC_Data;

uint16 SamppCnt = 0;
uint16 AdVaule[256];

/*********************************************************************
* void ADCDelay(uint16_t temp)
* @brief  :����ADC��ʱ���ú���
* @note   :��ADCʱ�����ڵ�1MHz
* @param  :uint16_t temp:��������
* @retval :NO
*********************************************************************/
void ADCDelay(uint16_t temp)
{
	uint32 i;
	uint32 tmp = 72;  /* 1MHz */
	for(i=0;i<tmp*temp;i++) ;	
}

/*********************************************************************
* void initADCTimer(void)
* @brief  :��ʼ������ADC��ʱ�Ķ�ʱ�� TIM4
* @note   : ���ڴ���ADCת��
*			36MHzʱ��Դ ��ƵΪ2MHz   625����һ��
* @param  :NO
* @retval :N
*********************************************************************/
void initADCTimer(void)
{
	/* TIMx control register 1  TIMx_CR1 */
	/* [15~10] [9~8:CKD[1:0]] */ 
	/* [7:ARPE] [6~5:CMS[1:0]] [4:DIR] [3:OPM] [2:URS] [1:UDIS] [0:CEN] */	
	TIM4->CR1 = 0;
	
	/* TIMx control register 2   TIMx_CR2 */
	/* [15~8] / [7:TI1S] [6~4:MMS] [3:CCDS] [2~0] */		
	TIM4->CR2 = 0x20;
	
	/* TIMx DMA/Interrupt enable register  TIMx_DIER */
	/* [15] [14:TDE] [13] [12:CC4DE] [11:CC3DE] [10:CC2DE] [9:CC1DE] [8:UDE] */ 
	/* [7] [6:TIE] [5] [4:CC4IE] [3:CC3IE] [2:CC2IE] [1:CC1IE] [0:UIE] */		
	TIM4->DIER = 0x00;
	
	/* TIMx prescaler register  TIMx_PSC */
	/* [15~0:PSC] 0~65535 */ 	
	TIM4->PSC = 17;	
	
	/* TIMx auto-reload register  TIMx_ARR */
	/* [15~0:PSC] 0~65535 0x287F*/ 	
	TIM4->ARR = 624;	
}

/* У׼AD */
void VerfiyAdVaule(ADC_TypeDef *pAd,uint8_t AdNo)
{
	uint16 i=0;
	uint32 temp = 0;
	
	/* �� ADC ��Դ */
// 	if( ADC1_ADON )		/* ADC1 */
// 		ADC1_ADON = 0;
// 	if( ADC2_ADON )		/* ADC2 */
// 		ADC2_ADON = 0;
	pAd->CR1 = 0;
	pAd->CR2 = 0;
	/* ��ʱ����Դ�ȶ� */
	ADCDelay(100);
	/* �� ADC1 ��Դ */
	pAd->CR2 |= 1;//ADC1_ADON = 1;
	/* ���״̬��־λ */
	pAd->SR = 0;
	ADCDelay(100);
	/* ADC1 У׼��� */
	if(pAd == ADC1)
	{
		ADC1_RSTCAL = 1;
		while( ADC1_RSTCAL );		/* ��λУ׼�Ĵ�ֵ */
		ADC1_CAL = 1;
		while( ADC1_CAL );  		/* �ȴ�У׼��� */
	}
	else
	{
		ADC2_RSTCAL = 1;
		while( ADC2_RSTCAL );		/* ��λУ׼�Ĵ�ֵ */
		ADC2_CAL = 1;
		while( ADC2_CAL );  		/* �ȴ�У׼��� */
	}
//	pAd->CR1 |= 1<<5;
	/* ADC_CR1 <ADC control register 1> */
	/*[31~24] / [23:AWDEN] [22:JAWDEN] [21~20] [19~17:DUALMOD] */
	/*[15~13:DISCNUM] [12:JDISCEN] [11:DISCEN] [10:JAUTO] [9:AWDSGL] [8:SCAN] */
	/*[7:JEOC IE] [6:AWDIE] [5:EOCIE] [4~0:AWDCH] */		
	pAd->SQR1 = 0<<20;
	pAd->SQR3 |= 5;

	/* SMP ����ʱ�� */
	pAd->SMPR1 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)
								|(SMP28_5<<12)|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21));
	pAd->SMPR2 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)|(SMP28_5<<12)
								|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21)|(SMP28_5<<24)|(SMP28_5<<27));
	
	
	pAd->CR2 |= (7<<17)|(1<<20);
	
	pAd->CR2 |= 1|(1<<22);
	
	SamppCnt = 0;
	while(SamppCnt<256)
	{
		if(pAd->SR & 0x02)
		{
			AdVaule[SamppCnt++] = pAd->DR;
			pAd->CR2 |= (1<<22) | 1;
		}
	}
	/* ������Ʈֵ */
	for(i = 0; i < 256; i++) temp += AdVaule[i];
	ADC_Data.AdZeroOffset[AdNo] = 2048 - temp/256;	
}

/*********************************************************************
* void initADC(void)
* @brief  :��ʼ��ADC 
* @note   :injected ���� ��λ�������У׼
*						У׼Ҫע��ȴ�ʱ������ο�Դ�ȶ�
*						���ܴ���ԴADC1/2������ѡ����ͬ�ĵ� ͬ����ʱ,����Դ����һ��
* @param  :NO 
* @retval :NO
*********************************************************************/
void initADC(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* Configure the GPIO pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
//	VerfiyAdVaule(ADC1,0);
//	VerfiyAdVaule(ADC2,1);
	/* �� ADC ��Դ */
	if( ADC1_ADON )		/* ADC1 */
		ADC1_ADON = 0;
	if( ADC2_ADON )		/* ADC2 */
		ADC2_ADON = 0;
	ADC1->CR2 = 0;
	
//	ADC2->CR2 = 0;

	/* ��ʱ����Դ�ȶ� */
	ADCDelay(100);
	
	/* �� ADC1 ��Դ */
	ADC1_ADON = 1;
	/* ���״̬��־λ */
	ADC1->SR = 0;		
	
	ADCDelay(100);
	/* ADC1 У׼��� */
	ADC1_RSTCAL = 1;
	while( ADC1_RSTCAL );		/* ��λУ׼�Ĵ�ֵ */
	ADC1_CAL = 1;
	while( ADC1_CAL );  		/* �ȴ�У׼��� */
	
	/* ADC_CR1 <ADC control register 1> */
	/*[31~24] / [23:AWDEN] [22:JAWDEN] [21~20] [19~17:DUALMOD] */
	/*[15~13:DISCNUM] [12:JDISCEN] [11:DISCEN] [10:JAUTO] [9:AWDSGL] [8:SCAN] */
	/*[7:JEOC IE] [6:AWDIE] [5:EOCIE] [4~0:AWDCH] */	
	ADC1->CR1 = (1<<7)|(2<<13)|(5<<16)|(1<<12);

	/* SMP ����ʱ�� */
	ADC1->SMPR1 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)
								|(SMP28_5<<12)|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21));
	ADC1->SMPR2 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)|(SMP28_5<<12)
								|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21)|(SMP28_5<<24)|(SMP28_5<<27));

	/* ADC_CR2 <ADC control register 2> */
	/*[31~24] / [23:TSVREFE] [22:SWSTART] [21:JSWSTART] [20:EXTTRIG] [19~17:EXTSEL] [16] */
	/*[15:SEXTTIRIG][14~12:JEXTSEL] [11:ALIGN] [10~9] [8:DMA] */
	/*[7~4] [3:RSTCAL] [2:CAL] [1:CONT][0:ADON] */	
	ADC1->CR2 = (1<<0)|(5<<12)|(1<<15);		/* ADC1/2 TIM4 TRGO */
	
	
	
/*********************��ʹ��ADC2,��ʱ������ADC2****************************/
//	/* �� ADC2 ��Դ */
//	ADC2_ADON = 1;
//	/* ���״̬��־λ */
//	ADC2->SR = 0;

//	ADCDelay(100);
//	/* ADC2 У׼��� */
//	ADC2_RSTCAL = 1;
//	while( ADC2_RSTCAL );		/* ��λУ׼�Ĵ�ֵ */
//	ADC2_CAL = 1;
//	while( ADC2_CAL );  		/* �ȴ�У׼��� */
//	
//	/* ADC_CR1 <ADC control register 1> */
//	/*[31~24] / [23:AWDEN] [22:JAWDEN] [21~20] [19~16:DUALMOD] */
//	/*[15~13:DISCNUM] [12:JDISCEN] [11:DISCEN] [10:JAUTO] [9:AWDSGL] [8:SCAN] */
//	/*[7:JEOC IE] [6:AWDIE] [5:EOCIE] [4~0:AWDCH] */	
//	ADC2->CR1 = (1<<7)|(3<<13)|(5<<16)|(1<<12);

//	/* SMP ����ʱ�� */
//	ADC2->SMPR1 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)
//								|(SMP28_5<<12)|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21));
//	ADC2->SMPR2 = (SMP28_5 | (SMP28_5<<3)|(SMP28_5<<6)|(SMP28_5<<9)|(SMP28_5<<12)
//								|(SMP28_5<<15)|(SMP28_5<<18)|(SMP28_5<<21)|(SMP28_5<<24)|(SMP28_5<<27));

//	/* ADC_CR2 <ADC control register 2> */
//	/*[31~24] / [23:TSVREFE] [22:SWSTART] [21:JSWSTART] [20:EXTTRIG] [19~17:EXTSEL] [16] */
//	/*[15:SEXTTIRIG][14~12:JEXTSEL] [11:ALIGN] [10~9] [8:DMA] */
//	/*[7~4] [3:RSTCAL] [2:CAL] [1:CONT][0:ADON] */	
//	ADC2->CR2 = (1<<0)|(5<<12)|(1<<15);		/* ADC1/2 TIM4 TRGO */

	/* ��ʼ�ⲿ����Դ */
	initADCTimer();

	/* ����ͨ���� JSQ���� 4ͨ�� */
	ADC1->JSQR = AD1_CH;
	
	
//	ADC2->JSQR = AD2_CH;

	/* �������ݱ�����ʼ�� */
	ADC_Data.AdHead = 0;
	ADC_Data.AdEnd = 0; 
	ADC_Data.AdOneEnd = 0;
	
	/* ʹ��NVIC�������� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);
	
	StartADC();	
}

/*********************************************************************
* void StartADC( void )
* @brief  :����TIM4����ADC��������
* @note   :ע�����ʱ��ʹ���ADC����ʱ����
* @param  :
* @retval :NO
*********************************************************************/
void StartADC( void )
{
	ADC_Data.AdOneEnd = 0;
	ADC_Data.AdHead = 0;
	ADC_Data.AdEnd = 0;
	
	TIM2->CR1 = 1;
}
/*********************************************************************
* void restartADC(void)
* @brief  :�������¼���
* @note   : �������ȫ�ֱ���
* @param  :NO 
* @retval :None
*********************************************************************/
void restartADC(void)
{
	ADC_Data.AdOneEnd = 0;
	ADC_Data.AdHead = 0;
	ADC_Data.AdEnd = 0;	
}
/*********************************************************************
* void ADC1_2_IRQHandler( void )
* @brief  :ADC1/2�жϷ������
* @note   :��ʼADC������TIM4����
* @param  :
* @retval :NO
*********************************************************************/
void ADC1_2_IRQHandler( void )
{
	ADC_Data.ADCBuf[0][ADC_Data.AdHead] =ADC1->JDR1;//+ADC_Data.AdZeroOffset[0];
	ADC_Data.ADCBuf[1][ADC_Data.AdHead] =ADC1->JDR2;//+ADC_Data.AdZeroOffset[0];
	ADC_Data.ADCBuf[2][ADC_Data.AdHead] =ADC1->JDR3;//+ADC_Data.AdZeroOffset[0];
	
	
//	ADC_Data.ADCBuf[0][3][ADC_Data.AdHead] =ADC1->JDR4;//+ADC_Data.AdZeroOffset[0];

//	ADC_Data.ADCBuf[1][0][ADC_Data.AdHead] =ADC2->JDR1;//+ADC_Data.AdZeroOffset[1];
//	ADC_Data.ADCBuf[1][1][ADC_Data.AdHead] =ADC2->JDR2;//+ADC_Data.AdZeroOffset[1];
//	ADC_Data.ADCBuf[1][2][ADC_Data.AdHead] =ADC2->JDR3;//+ADC_Data.AdZeroOffset[1];
//	ADC_Data.ADCBuf[1][3][ADC_Data.AdHead] =ADC2->JDR4;//+ADC_Data.AdZeroOffset[1];

	ADC1_JEOC = 0;
	

	ADC_Data.AdOneEnd = 1;
	if(ADC_Data.AdHead == 255)
	{
		ADC_Data.AdHead = 0;
		ADC_Data.AdEnd = 1;
		TIM2->CR1 = 0;//�ɼ���256������֮�󣬹رն�ʱ�������ٽ������ݲɼ�
	}
	else
		ADC_Data.AdHead++;	
}



void ADC_Configuration(void) 
{ 
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStruct;
	/* Enable the GPIO Clock */

	ADC_TIM3_Configuration();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1,&ADC_InitStructure);

	ADC_InjectedSequencerLengthConfig(ADC1,3);
	ADC_InjectedChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_28Cycles5);
	ADC_InjectedChannelConfig(ADC1,ADC_Channel_12,2,ADC_SampleTime_28Cycles5);
	ADC_InjectedChannelConfig(ADC1,ADC_Channel_13,3,ADC_SampleTime_28Cycles5);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)); 

	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_T3_CC4);

	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

	ADC_Data.AdHead = 0;
	ADC_Data.AdEnd = 0; 
	ADC_Data.AdOneEnd = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	   
	NVIC_Init(&NVIC_InitStruct);

	ADC_ITConfig(ADC1,ADC_IT_JEOC,ENABLE);
}

void ADC_TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 1000;//û1000����������һ���������൱��1ms����һ�δ���
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//��Ƶ��1MHZ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);

	TIM_Cmd(TIM3,ENABLE);

	TIM_InternalClockConfig(TIM3);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_UpdateDisableConfig(TIM3,DISABLE);
}

//void ADC_TIM2_Configuration(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	
//	TIM_TimeBaseStructure.TIM_Period = 1000;//û1000����������һ���������൱��1ms����һ�δ���
//	TIM_TimeBaseStructure.TIM_Prescaler = 71;//��Ƶ��1MHZ
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = 500;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//	TIM_OC1Init(TIM2,&TIM_OCInitStructure);




//	TIM_Cmd(TIM2,ENABLE);

//	TIM_InternalClockConfig(TIM2);
//	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
//	TIM_UpdateDisableConfig(TIM2,DISABLE);
//	
//}

/*********************************************************************
* void GetValue(void)
* @brief  :����ADת��������ȡ�����ѹ����������������ADֵ��������
* @note   :
* @param  :
* @retval :NO
*********************************************************************/
void GetValue(void)
{
	uint32 overtimeflag;
	uint32 VoltageAdValue,Current1AdValue,Current2AdValue;
	uint16 i;
	overtimeflag = JdyTest.g_TimerMs;
	VoltageAdValue = 0;
	Current1AdValue = 0;
	Current2AdValue = 0;
	StartADC();
	while(ADC_Data.AdEnd == 0)
	{
		if((JdyTest.g_TimerMs - overtimeflag)>3000)
		{
			break;
		}
	}
	for(i = 0;i<256;i++)
	{
		VoltageAdValue = ADC_Data.ADCBuf[0][i];
		Current1AdValue = ADC_Data.ADCBuf[1][i];
		Current2AdValue = ADC_Data.ADCBuf[2][i];
	}
	VoltageAdValue = (VoltageAdValue>>8) & 0xFF;
	Current1AdValue = (Current1AdValue>>8) & 0xFF;
	Current2AdValue = (Current2AdValue>>8) & 0xFF;
}


/*************************************************************************************
**   ADC.c end file
**************************************************************************************/
