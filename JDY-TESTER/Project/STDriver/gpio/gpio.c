#include "include.h"
#include "gpio.h"
#include "stm32f10x_tim.h"

GPIO_TypeDef* LED_GPIO_PORT[LED_NUM] = {LED1_GPIO_PORT};
const uint16_t LED_GPIO_PIN[LED_NUM] = {LED1_GPIO_PIN};
const uint32_t LED_GPIO_CLK[LED_NUM] = {LED1_GPIO_CLK};

/*����������*/
GPIO_TypeDef* INPUT_GPIO_PORT[INPUT_NUM] = {INPUT0_GPIO_PORT, INPUT1_GPIO_PORT, INPUT2_GPIO_PORT,INPUT3_GPIO_PORT, INPUT4_GPIO_PORT, INPUT5_GPIO_PORT,
                                            INPUT6_GPIO_PORT, INPUT7_GPIO_PORT, INPUT8_GPIO_PORT,INPUT9_GPIO_PORT, INPUT10_GPIO_PORT};
const uint16_t INPUT_GPIO_PIN[INPUT_NUM] = {INPUT0_GPIO_PIN, INPUT1_GPIO_PIN, INPUT2_GPIO_PIN,INPUT3_GPIO_PIN, INPUT4_GPIO_PIN, INPUT5_GPIO_PIN,
                                            INPUT6_GPIO_PIN, INPUT7_GPIO_PIN, INPUT8_GPIO_PIN,INPUT9_GPIO_PIN,INPUT10_GPIO_PIN};
const uint32_t INPUT_GPIO_CLK[INPUT_NUM] = {INPUT0_GPIO_CLK, INPUT1_GPIO_CLK, INPUT2_GPIO_CLK,INPUT3_GPIO_CLK, INPUT4_GPIO_CLK, INPUT5_GPIO_CLK,
                                            INPUT6_GPIO_CLK, INPUT7_GPIO_CLK, INPUT8_GPIO_CLK,INPUT9_GPIO_CLK, INPUT10_GPIO_CLK};


/*�̵������������*/
GPIO_TypeDef* JOUTPUT_GPIO_PORT[JOUTPUT_NUM] = {JOUTPUT0_GPIO_PORT, JOUTPUT1_GPIO_PORT,JOUTPUT2_GPIO_PORT, JOUTPUT3_GPIO_PORT,JOUTPUT4_GPIO_PORT, JOUTPUT5_GPIO_PORT,
                                                JOUTPUT6_GPIO_PORT, JOUTPUT7_GPIO_PORT,JOUTPUT8_GPIO_PORT, JOUTPUT9_GPIO_PORT,JOUTPUT10_GPIO_PORT, JOUTPUT11_GPIO_PORT,
                                                JOUTPUT12_GPIO_PORT, JOUTPUT13_GPIO_PORT,JOUTPUT14_GPIO_PORT, JOUTPUT15_GPIO_PORT,JOUTPUT16_GPIO_PORT, JOUTPUT17_GPIO_PORT,
                                                JOUTPUT18_GPIO_PORT, JOUTPUT19_GPIO_PORT,JOUTPUT20_GPIO_PORT};
const uint16_t JOUTPUT_GPIO_PIN[JOUTPUT_NUM] = {JOUTPUT0_GPIO_PIN, JOUTPUT1_GPIO_PIN,JOUTPUT2_GPIO_PIN, JOUTPUT3_GPIO_PIN,JOUTPUT4_GPIO_PIN, JOUTPUT5_GPIO_PIN,
                                                JOUTPUT6_GPIO_PIN, JOUTPUT7_GPIO_PIN,JOUTPUT8_GPIO_PIN, JOUTPUT9_GPIO_PIN,JOUTPUT10_GPIO_PIN, JOUTPUT11_GPIO_PIN,
                                                JOUTPUT12_GPIO_PIN, JOUTPUT13_GPIO_PIN,JOUTPUT14_GPIO_PIN, JOUTPUT15_GPIO_PIN,JOUTPUT16_GPIO_PIN, JOUTPUT17_GPIO_PIN,
                                                JOUTPUT18_GPIO_PIN, JOUTPUT19_GPIO_PIN,JOUTPUT20_GPIO_PIN};
const uint32_t JOUTPUT_GPIO_CLK[JOUTPUT_NUM] = {JOUTPUT0_GPIO_CLK, JOUTPUT1_GPIO_CLK,JOUTPUT2_GPIO_CLK, JOUTPUT3_GPIO_CLK,JOUTPUT4_GPIO_CLK, JOUTPUT5_GPIO_CLK,
                                                JOUTPUT6_GPIO_CLK, JOUTPUT7_GPIO_CLK,JOUTPUT8_GPIO_CLK, JOUTPUT9_GPIO_CLK,JOUTPUT10_GPIO_CLK, JOUTPUT11_GPIO_CLK,
                                                JOUTPUT12_GPIO_CLK, JOUTPUT13_GPIO_CLK,JOUTPUT14_GPIO_CLK, JOUTPUT15_GPIO_CLK,JOUTPUT16_GPIO_CLK, JOUTPUT17_GPIO_CLK,
                                                JOUTPUT18_GPIO_CLK, JOUTPUT19_GPIO_CLK,JOUTPUT20_GPIO_CLK};

//LED��ʼ��
void STM_EVAL_LEDInit(Led_TypeDef Led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK[Led], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN[Led];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO_Speed_50MHz;

	GPIO_Init(LED_GPIO_PORT[Led], &GPIO_InitStructure);
}


void STM_EVAL_LEDToggle(Led_TypeDef Led)
{
	LED_GPIO_PORT[Led]->ODR ^= LED_GPIO_PIN[Led];
}
/***********���������������******************/
void BeepInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LCD_BACK Clock */
	RCC_APB2PeriphClockCmd(SPK_GPIO_CLK, ENABLE);

	/* Configure the GPIO_LCD_BACK pin */
	GPIO_InitStructure.GPIO_Pin = SPK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO_Speed_50MHz;

	GPIO_Init(SPK_GPIO_PORT, &GPIO_InitStructure);
	ContrlGPIOOut(&Speaker,0);
}
/***********WDT���������******************/
void WDTInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LCD_BACK Clock */
	RCC_APB2PeriphClockCmd(WDT_GPIO_CLK, ENABLE);

	/* Configure the GPIO_LCD_BACK pin */
	GPIO_InitStructure.GPIO_Pin = WDT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO_Speed_50MHz;

	GPIO_Init(WDT_GPIO_PORT, &GPIO_InitStructure);	
}
//�ص� ����
/*
** �䡡��: no:���������
			val:Bit_RESET = 0,
  				Bit_SET	 = 1
*/
void STM_EVAL_LED_CTRL(Led_TypeDef Led,BitAction val)
{
	GPIO_WriteBit(LED_GPIO_PORT[Led],LED_GPIO_PIN[Led],val);
}

//�ص�
void STM_EVAL_LED_OFF(Led_TypeDef Led)
{
	STM_EVAL_LED_CTRL(Led,Bit_RESET);	
}

//����
void STM_EVAL_LED_ON(Led_TypeDef Led)
{
	STM_EVAL_LED_CTRL(Led,Bit_SET);	
}




//��������
void BeepON(void)
{
	ContrlGPIOOut(&Speaker,TRUE);
}


//����������
void BeepOFF(void)
{
	ContrlGPIOOut(&Speaker,FALSE);
}
//����̵�����
void OutPutON(Joutput_TypeDef outputno)
{
	WriteJoutputDataBit(outputno,SET);
}
//����̵����ر�
void OutPutOFF(Joutput_TypeDef outputno)
{
	WriteJoutputDataBit(outputno,RESET);
}


//ȫ����������ʼ��
void InPut_OutPut_Init()
{	
	WDTInit();
	SetGPIO(&WDT);
	ResetGPIO(&WDT);
	
	Joutput_IO_Init();
	Input_IO_Init();
	LED_IO_Init();

}

/****************************************************************************************
** ��������:  LED_IO_Init(void)
** ��������: LED�ƿ�������ʼ��
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����:  ��ʼ������
******************************************************************************************/
void LED_IO_Init(void)
{
	uint8 i;
	for(i=0;i<LED_NUM;i++)
	{
		STM_EVAL_LEDInit((Led_TypeDef)i);
		STM_EVAL_LED_ON((Led_TypeDef)i);
	}	
}

void InputIoInit(Input_TypeDef inputn)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(INPUT_GPIO_CLK[inputn], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = INPUT_GPIO_PIN[inputn];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(INPUT_GPIO_PORT[inputn], &GPIO_InitStructure);
}
void Input_IO_Init(void)
{
	uint8 i;
	for(i=0;i<INPUT_NUM;i++)
	{
		InputIoInit((Input_TypeDef)i);
	}	
}
/***********�̵������*****************/
//ע�⣺PE12:PE13 =  S:RESET  [1:0] ��� 0
//						      [0:1] ��� 1
void JoutputIoInit(Joutput_TypeDef outputn)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(JOUTPUT_GPIO_CLK[outputn], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = JOUTPUT_GPIO_PIN[outputn];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(JOUTPUT_GPIO_PORT[outputn], &GPIO_InitStructure);
}

/****************************************************************************************
** ��������:  Joutput_IO_Init(void)
** ��������: �̵������������ʼ��
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����:  ��ʼ���̵����������
******************************************************************************************/
void Joutput_IO_Init(void)
{
	uint8 i;
	for(i=0;i<JOUTPUT_NUM;i++)
	{
		JoutputIoInit((Joutput_TypeDef)i);
		WriteJoutputDataBit((Joutput_TypeDef)i,Bit_RESET);
	}	
}

/****************************************************************************************
** ��������: uint8 ReadJoutputDataBit(void)
** ��������: 
** �䡡��: ���������
** �䡡��: ��Ӧͨ��IOֵ
** ȫ�ֱ���:
** ����:   ������Ŷ�ȡ��Ӧ������״̬
******************************************************************************************/
uint8 ReadInputDataBit(Input_TypeDef no)
{
	return GPIO_ReadInputDataBit(INPUT_GPIO_PORT[no],INPUT_GPIO_PIN[no]);
}

/****************************************************************************************
** ��������: WriteJoutputDataBit(Loutput_TypeDef no,BitAction val)
** ��������: ���1· �̵�������
** �䡡��: no:���������
			val:Bit_RESET = 0,
  				Bit_SET	 = 1
** �䡡��: ��
** ȫ�ֱ���:
** ����:   д��Ӧ���ֵ
******************************************************************************************/
void WriteJoutputDataBit(Joutput_TypeDef no,BitAction val)
{
	if(GPIO_ReadOutputDataBit(JOUTPUT_GPIO_PORT[no],JOUTPUT_GPIO_PIN[no]) != val)
	{
		GPIO_WriteBit(JOUTPUT_GPIO_PORT[no],JOUTPUT_GPIO_PIN[no],val);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
* void ToggleGPIORev(const GPIO_Type *p)
* ��������:�����GPIOȡ�����Ʋ���
* �䡡��: const GPIO_Type *p:GPIO���Ų���
* �䡡��: ��
* ȫ�ֱ���: 
* ��  ��:ֻ�����������Ч
****************************************************************************/
void ToggleGPIORev(const GPIO_Type *p)
{
    assert_param(IS_GPIO_ALL_PERIPH( p->Port));
    assert_param(IS_GPIO_PIN(p->Pin));
    p->Port->ODR ^= p->Pin;
}
/***************************************************************************
* void ContrlGPIOOut(const GPIO_Type *p,unsigned char ctrl)
* ��������:�����GPIO�����������
* �䡡��: const GPIO_Type *p:GPIO���Ų���
*         unsigned char ctrl:��������߻��ǵ� 1:��
* �䡡��: ��
* ȫ�ֱ���: 
* ��  ��:ֻ�����������Ч
****************************************************************************/
void ContrlGPIOOut(const GPIO_Type *p,unsigned char ctrl)
{
    assert_param(IS_GPIO_ALL_PERIPH( p->Port));
    assert_param(IS_GPIO_PIN(p->Pin));
    if(ctrl >= 1)
    {
        //GPIO_SetBits(p->Port,p->Pin);
        p->Port->BSRR = p->Pin;
    }
    else
    {
        //GPIO_ResetBits(p->Port,p->Pin);
        p->Port->BRR = p->Pin;
    }
}
/***************************************************************************
* void SetGPIO(const GPIO_Type *p)
* ��������:�����GPIO������λ�������
* �䡡��: const GPIO_Type *p:GPIO���Ų���
* �䡡��: ��
* ȫ�ֱ���: 
* ��  ��:ֻ�����������Ч   ����ߵ�ѹ
****************************************************************************/
void SetGPIO(const GPIO_Type *p)
{
    assert_param(IS_GPIO_ALL_PERIPH( p->Port));
    assert_param(IS_GPIO_PIN(p->Pin));  
    p->Port->BSRR = p->Pin;
}
/***************************************************************************
* void ResetGPIO(const GPIO_Type *p)
* ��������:�����GPIO���Ƹ�λ�������
* �䡡��: const GPIO_Type *p:GPIO���Ų���
* �䡡��: ��
* ȫ�ֱ���: 
* ��  ��:ֻ�����������Ч   ����׵�ѹ
****************************************************************************/
void ResetGPIO(const GPIO_Type *p)
{
    assert_param(IS_GPIO_ALL_PERIPH( p->Port));
    assert_param(IS_GPIO_PIN(p->Pin));  
    p->Port->BRR = p->Pin;
}
/***************************************************************************
* unint8 ReadInput(const GPIO_Type *p)
* ��������:��ȡ����GPIO��״̬
* �䡡��: const GPIO_Type *p:GPIO���Ų���
* �䡡��: ��
* ȫ�ֱ���: 
* ��  ��:ֻ�����������Ч   
****************************************************************************/
unsigned char ReadInput(const GPIO_Type *p)
{
    return GPIO_ReadInputDataBit(p->Port,p->Pin);
}

