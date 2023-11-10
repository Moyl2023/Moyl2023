#ifndef _GPIO_H
#define _GPIO_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"

typedef struct
{
    GPIO_TypeDef    *Port;
    unsigned short   Pin;
}GPIO_Type;

/*光耦IO输入*/
typedef enum 
{
	INPUT0 = 0,
	INPUT1 = 1,
	INPUT2 = 2,
	INPUT3 = 3,
	INPUT4 = 4,
	INPUT5 = 5,
	INPUT6 = 6,
	INPUT7 = 7,
	INPUT8 = 8,
	INPUT9 = 9,
	INPUT10 = 10,
} Input_TypeDef;

#define INPUT_NUM                   11

#define INPUT0_GPIO_PORT          GPIOD		//拨码开关1
#define INPUT0_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT0_GPIO_PIN           GPIO_Pin_2

#define INPUT1_GPIO_PORT          GPIOD		//拨码开关2
#define INPUT1_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT1_GPIO_PIN           GPIO_Pin_1

#define INPUT2_GPIO_PORT          GPIOD		//拨码开关3
#define INPUT2_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT2_GPIO_PIN           GPIO_Pin_0

#define INPUT3_GPIO_PORT          GPIOC		//拨码开关4
#define INPUT3_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define INPUT3_GPIO_PIN           GPIO_Pin_12

#define INPUT4_GPIO_PORT          GPIOD		//输入干节点1
#define INPUT4_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT4_GPIO_PIN           GPIO_Pin_4

#define INPUT5_GPIO_PORT          GPIOD		//输入干节点2
#define INPUT5_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT5_GPIO_PIN           GPIO_Pin_5

#define INPUT6_GPIO_PORT          GPIOD     //输入干节点3
#define INPUT6_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT6_GPIO_PIN           GPIO_Pin_6

#define INPUT7_GPIO_PORT          GPIOD     //输入干节点4
#define INPUT7_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define INPUT7_GPIO_PIN           GPIO_Pin_7

#define INPUT8_GPIO_PORT          GPIOB     //输入干节点5
#define INPUT8_GPIO_CLK           RCC_APB2Periph_GPIOB
#define INPUT8_GPIO_PIN           GPIO_Pin_3

#define INPUT9_GPIO_PORT          GPIOA     //输入干节点4
#define INPUT9_GPIO_CLK           RCC_APB2Periph_GPIOA  
#define INPUT9_GPIO_PIN           GPIO_Pin_11

#define INPUT10_GPIO_PORT          GPIOA     //输入干节点5
#define INPUT10_GPIO_CLK           RCC_APB2Periph_GPIOA
#define INPUT10_GPIO_PIN           GPIO_Pin_12


//------------------------------------------------
//蜂鸣器   speaker
#define SPK_GPIO_PORT      GPIOB
#define SPK_GPIO_CLK       RCC_APB2Periph_GPIOB
#define SPK_GPIO_PIN       GPIO_Pin_9   
static const GPIO_Type Speaker = {SPK_GPIO_PORT,SPK_GPIO_PIN};
//------------------------------------------------
//看门狗 wathdog   
#define WDT_GPIO_PORT      GPIOE    
#define WDT_GPIO_CLK       RCC_APB2Periph_GPIOE  
#define WDT_GPIO_PIN       GPIO_Pin_5   
static const GPIO_Type WDT = {WDT_GPIO_PORT,WDT_GPIO_PIN};
/*------------------------------------------------*/


typedef enum 
{
  LED_OFF = 0,
  LED_ON = 1,
} Led_ONOFF;

//LED 枚举和定义GPIO结构体
typedef enum
{
    LED_RUN  = 0,
}Led_TypeDef;

#define LED_NUM                        1
#define LED1_GPIO_PORT              GPIOA	 											//板上灯
#define LED1_GPIO_CLK               RCC_APB2Periph_GPIOA  
#define LED1_GPIO_PIN               GPIO_Pin_4
  

/******************************************************/
/*继电器输出*/
typedef enum 
{
	JOUTPUT0 = 0,		
	JOUTPUT1 = 1,
	JOUTPUT2 = 2,
	JOUTPUT3 = 3,
	JOUTPUT4 = 4,
	JOUTPUT5 = 5,
	JOUTPUT6 = 6,
	JOUTPUT7 = 7,
	JOUTPUT8 = 8,
	JOUTPUT9 = 9,
	JOUTPUT10 = 10,
	JOUTPUT11 = 11,
	JOUTPUT12 = 12,
	JOUTPUT13 = 13,
	JOUTPUT14 = 14,
	JOUTPUT15 = 15,
	JOUTPUT16 = 16,
	JOUTPUT17 = 17,
	JOUTPUT18 = 18,
	JOUTPUT19 = 19,
	JOUTPUT20 = 20,
} Joutput_TypeDef;

#define JOUTPUT_NUM					21

#define JOUTPUT0_GPIO_PORT			GPIOC
#define JOUTPUT0_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define JOUTPUT0_GPIO_PIN           GPIO_Pin_2

#define JOUTPUT1_GPIO_PORT			GPIOC
#define JOUTPUT1_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define JOUTPUT1_GPIO_PIN           GPIO_Pin_3

#define JOUTPUT2_GPIO_PORT			GPIOA
#define JOUTPUT2_GPIO_CLK           RCC_APB2Periph_GPIOA  
#define JOUTPUT2_GPIO_PIN           GPIO_Pin_5

#define JOUTPUT3_GPIO_PORT			GPIOA
#define JOUTPUT3_GPIO_CLK           RCC_APB2Periph_GPIOA  
#define JOUTPUT3_GPIO_PIN           GPIO_Pin_6

#define JOUTPUT4_GPIO_PORT			GPIOA
#define JOUTPUT4_GPIO_CLK           RCC_APB2Periph_GPIOA  
#define JOUTPUT4_GPIO_PIN           GPIO_Pin_7

#define JOUTPUT5_GPIO_PORT			GPIOC
#define JOUTPUT5_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define JOUTPUT5_GPIO_PIN           GPIO_Pin_4

#define JOUTPUT6_GPIO_PORT			GPIOC
#define JOUTPUT6_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define JOUTPUT6_GPIO_PIN           GPIO_Pin_5

#define JOUTPUT7_GPIO_PORT			GPIOB
#define JOUTPUT7_GPIO_CLK           RCC_APB2Periph_GPIOB  
#define JOUTPUT7_GPIO_PIN           GPIO_Pin_0

#define JOUTPUT8_GPIO_PORT			GPIOB
#define JOUTPUT8_GPIO_CLK           RCC_APB2Periph_GPIOB  
#define JOUTPUT8_GPIO_PIN           GPIO_Pin_1

#define JOUTPUT9_GPIO_PORT			GPIOE
#define JOUTPUT9_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT9_GPIO_PIN           GPIO_Pin_7

#define JOUTPUT10_GPIO_PORT			GPIOE
#define JOUTPUT10_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT10_GPIO_PIN           GPIO_Pin_8

#define JOUTPUT11_GPIO_PORT			GPIOE
#define JOUTPUT11_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT11_GPIO_PIN           GPIO_Pin_9

#define JOUTPUT12_GPIO_PORT			GPIOE
#define JOUTPUT12_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT12_GPIO_PIN           GPIO_Pin_10

#define JOUTPUT13_GPIO_PORT			GPIOE
#define JOUTPUT13_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT13_GPIO_PIN           GPIO_Pin_11

#define JOUTPUT14_GPIO_PORT			GPIOE
#define JOUTPUT14_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT14_GPIO_PIN           GPIO_Pin_12

#define JOUTPUT15_GPIO_PORT			GPIOE
#define JOUTPUT15_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT15_GPIO_PIN           GPIO_Pin_13

#define JOUTPUT16_GPIO_PORT			GPIOE
#define JOUTPUT16_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT16_GPIO_PIN           GPIO_Pin_14

#define JOUTPUT17_GPIO_PORT			GPIOE
#define JOUTPUT17_GPIO_CLK           RCC_APB2Periph_GPIOE  
#define JOUTPUT17_GPIO_PIN           GPIO_Pin_15

#define JOUTPUT18_GPIO_PORT			GPIOD
#define JOUTPUT18_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define JOUTPUT18_GPIO_PIN           GPIO_Pin_8

#define JOUTPUT19_GPIO_PORT			GPIOD
#define JOUTPUT19_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define JOUTPUT19_GPIO_PIN           GPIO_Pin_9

#define JOUTPUT20_GPIO_PORT			GPIOD
#define JOUTPUT20_GPIO_CLK           RCC_APB2Periph_GPIOD  
#define JOUTPUT20_GPIO_PIN           GPIO_Pin_10






void LED_IO_Init(void);

extern void STM_EVAL_LEDInit(Led_TypeDef Led);
extern void STM_EVAL_LEDToggle(Led_TypeDef Led);
//关灯
extern void STM_EVAL_LED_OFF(Led_TypeDef Led);
//开灯
extern void STM_EVAL_LED_ON(Led_TypeDef Led);
extern void STM_EVAL_LED_CTRL(Led_TypeDef Led,BitAction val);

//全部开关量初始化
extern void InPut_OutPut_Init(void);

/*开关量输入*/
//初始化
extern void Input_IO_Init(void);
//读取8路 Input IO
//extern uint8 ReadInputVal(void);
//根据序号读取对应开关量状态
extern uint8 ReadInputDataBit(Input_TypeDef inputn);

/*光耦输出*/
//初始化
extern void Loutput_IO_Init(void);

/*继电器输出*/
//初始化
extern void Joutput_IO_Init(void);
//读取1路 继电器输出量
extern uint8 ReadJoutputDataBit(Joutput_TypeDef no);
//输出1路 继电器动作
extern void WriteJoutputDataBit(Joutput_TypeDef no,BitAction val);


void LedCtrl(void);
void BeepON(void);
void BeepOFF(void);

void OutPutON(Joutput_TypeDef outputno);
void OutPutOFF(Joutput_TypeDef outputno);


////////////////////////////////////////////////////////////////////////////




/***************************************************************************
* void ToggleGPIORev(const GPIO_Type *p)
* 功能描述:对输出GPIO取反控制操作
* 输　入: const GPIO_Type *p:GPIO引脚参数
* 输　出: 无
* 全局变量: 
* 描  述:只对输出操作有效
****************************************************************************/
extern void ToggleGPIORev(const GPIO_Type *p);
/***************************************************************************
* void ContrlGPIOOut(const GPIO_Type *p,unsigned char ctrl)
* 功能描述:对输出GPIO控制输出操作
* 输　入: const GPIO_Type *p:GPIO引脚参数
*         unsigned char ctrl:控制输出高还是底 1:高
* 输　出: 无
* 全局变量: 
* 描  述:只对输出操作有效
****************************************************************************/
extern void ContrlGPIOOut(const GPIO_Type *p,unsigned char ctrl);
/***************************************************************************
* void SetGPIO(const GPIO_Type *p)
* 功能描述:对输出GPIO控制置位输出操作
* 输　入: const GPIO_Type *p:GPIO引脚参数
* 输　出: 无
* 全局变量: 
* 描  述:只对输出操作有效   输出高电压
****************************************************************************/
extern void SetGPIO(const GPIO_Type *p);
/***************************************************************************
* void ResetGPIO(const GPIO_Type *p)
* 功能描述:对输出GPIO控制复位输出操作
* 输　入: const GPIO_Type *p:GPIO引脚参数
* 输　出: 无
* 全局变量: 
* 描  述:只对输出操作有效   输出底电压
****************************************************************************/
extern void ResetGPIO(const GPIO_Type *p);
/***************************************************************************
* unint8 ReadInput(const GPIO_Type *p)
* 功能描述:读取输入GPIO的状态
* 输　入: const GPIO_Type *p:GPIO引脚参数
* 输　出: 无
* 全局变量: 
* 描  述:只对输入操作有效   
****************************************************************************/
extern unsigned char ReadInput(const GPIO_Type *p);

#ifdef  __cplusplus
}
#endif

#endif


