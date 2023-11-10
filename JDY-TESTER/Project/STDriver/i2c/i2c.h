#ifndef _IIC_H
#define _IIC_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"
#include "stm32f10x_i2c.h"

#define  I2C_DEBUG

#define  DEF_SIMULATE_I2C


#ifdef  DEF_SIMULATE_I2C   /*ģ��I2C*/

#define SYS_CLOCK_HZ	72000000
#define I2C_CLOCK_HZ	  400000

#define I2C_FRAM_GPIO        	GPIOB
#define I2C_FRAM_GPIO_CLK    	RCC_APB2Periph_GPIOB
#define I2C_FRAM_SCL         	GPIO_Pin_6
#define I2C_FRAM_SDA         	GPIO_Pin_7
#define I2C_FRAM_WP         	GPIO_Pin_8	  //û��ʹ��

#define I2C_SCL_SET_1		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SCL,Bit_SET);  //SCL = 1;
#define I2C_SCL_CLR_0		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SCL,Bit_RESET);  //SCL = 0;
#define I2C_SDA_SET_1		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SDA,Bit_SET);  //SDA = 1;
#define I2C_SDA_CLR_0		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SDA,Bit_RESET);  //SDA = 0;
#define I2C_WP_SET_1		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_WP,Bit_SET);  //WP = 1;
#define I2C_WP_CLR_0		GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_WP,Bit_RESET);  //WP = 0;
#define I2C_WR_EN			I2C_WP_CLR_0		//дʹ��
#define I2C_WR_DIS			I2C_WP_SET_1		//д��ֹ
#define I2C_SDA_READ		GPIO_ReadInputDataBit(I2C_FRAM_GPIO,I2C_FRAM_SDA)


typedef struct {
//	void*		baseAddr;	          	// register base address
	void*		GPIOBase;	      	// direction contrl pin's GPIO
	uint32		I2C_SCL_Pin;       // scl contrl pin
	uint32		I2C_SDA_Pin;       // SDA contrl pin
	uint32		I2C_WP_Pin;       // WP contrl pin
} STM32F10x_I2C_CHAN_PRAM;

typedef struct {
	void*		GPIOBase;	      	// direction contrl pin's GPIO
	OS_EVENT* 	uartLock;	       // devece mutex lock
	uint16		I2C_SCL_Pin;       // scl contrl pin
	uint16		I2C_SDA_Pin;       // SDA contrl pin
	uint16		I2C_WP_Pin;       // WP contrl pin
} STM32F10x_I2C_CHAN;

extern void SysI2CInit(void);
extern uint8 Eeprom_Read(uint8 *buf,  uint32 address, uint16 length);
extern uint8 Eeprom_Write(uint8 *buf, uint32 address, uint16 length);
uint8 Eeprom_Write_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length);
uint8 Eeprom_Read_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length);

#else	/*�ڲ�I2C*/ //no def DEF_SIMULATE_I2C

#define FRAM_DEV_ADDR			      0xA0       // IIC�豸��ַ  bit7-4 1010B, bit3-1 A2-0,bit0 /WP 
#define FRAM_DATA_SIZE          0x8000 		 // FRAM �洢����32K�ֽ�  
#define I2C_FLASH_PAGESIZE      16				 // �ֽ�/ҳ

#define I2C_Speed              200000
#define I2C_SLAVE_ADDRESS7     0xA0

#define I2C_FRAM           		I2C1
#define I2C_FRAM_CLK       		RCC_APB1Periph_I2C1
#define I2C_FRAM_GPIO        	GPIOB
#define I2C_FRAM_GPIO_CLK    	RCC_APB2Periph_GPIOB
#define I2C_FRAM_SCL         	GPIO_Pin_6
#define I2C_FRAM_SDA         	GPIO_Pin_7


extern   void sysI2CInit(void);
extern uint16 I2C_FRAM_BufferWrite(uint8* buf, uint16 ext_addr, uint16 size);
extern uint16 I2C_FRAM_BufferRead(uint8* buf, uint16 ext_addr, uint16 size);

#endif	//DEF_SIMULATE_I2C


#ifdef  __cplusplus
}
#endif

#endif

