#include "i2c.h"


OS_EVENT* 	FPRAM_Lock;	                 		// devece mutex lock

#ifdef  DEF_SIMULATE_I2C   /*ģ��I2C*/

static void Init_IIC(void);

static uint8 PowerOnFirst = FALSE;
// ======================================================================================================
// ����IIC�ӿڳ�ʼ�� 
// ��������
// ���أ���
// ======================================================================================================
void SysI2CInit(void)
{
	if(PowerOnFirst == FALSE)
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(I2C_FRAM_GPIO_CLK, ENABLE);  // IIC Pin ����ʱ�Ӵ�
	
		GPIO_InitStructure.GPIO_Pin =  I2C_FRAM_SCL;// | I2C_FRAM_WP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(I2C_FRAM_GPIO, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin =  I2C_FRAM_SDA; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_IPU;
		GPIO_Init(I2C_FRAM_GPIO, &GPIO_InitStructure);
	
		FPRAM_Lock = OSSemCreate(1);
		Init_IIC();
		PowerOnFirst = TRUE;
	} 
}



/****************************************************************************************
** ��������: void I2CDelay(void)
** ��������: ��ʱ1��ָ������
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC������ʱ
******************************************************************************************/
static void I2CDelay(void)
{  
	uint32 tmp = SYS_CLOCK_HZ/I2C_CLOCK_HZ;
	uint32 i;
	tmp = tmp/2;
	for(i=0;i<tmp;i++)
		;		
}

 /****************************************************************************************
** ��������: void init_eeprom(void)
** ��������: ��ʼ���ⲿģ��IIC���� ��ʼ��EEprom/FRAM  I2C
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static void Init_IIC(void)
{
	GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SCL,Bit_SET);  //SCL = 1;
	I2CDelay();
	GPIO_WriteBit(I2C_FRAM_GPIO,I2C_FRAM_SDA,Bit_SET);  //SDA = 1;
	I2CDelay();
}

static void I2C_SDA_Bit_Set_Iput_Mod()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  I2C_FRAM_SDA; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    
	GPIO_Init(I2C_FRAM_GPIO,&GPIO_InitStructure);  
}

static void I2C_SDA_Bit_Set_Output_Mod()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  I2C_FRAM_SDA; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_Init(I2C_FRAM_GPIO,&GPIO_InitStructure);    	
}

/****************************************************************************************
** ��������: void I2C_Start(void)
** ��������: ����I2C���� EEprom/FRAM  I2C
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static void I2C_Start(void)
{
	I2C_SDA_Bit_Set_Output_Mod();
	I2C_SDA_SET_1	//SDA = 1;
	I2CDelay();
	I2C_SCL_SET_1	//SCL = 1;
	I2CDelay(); 
	I2C_SDA_CLR_0	//SDA = 0; 
	I2CDelay(); 
	I2C_SCL_CLR_0	//SCL = 0; 
	I2CDelay();	
}

/****************************************************************************************
** ��������: void stop(void)
** ��������: ֹͣI2C�Ĳ��� EEprom/FRAM  I2C
** �䡡��: ��
** �䡡��: ��
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static void I2C_Stop(void)
{
	I2C_SDA_Bit_Set_Output_Mod();
	I2C_SDA_CLR_0;	//SDA = 0; 
	I2CDelay(); 
	I2C_SCL_SET_1;	//SCL = 1; 
	I2CDelay(); 
	I2C_SDA_SET_1	//SDA = 1; 
	I2CDelay();
}

/****************************************************************************************
** ��������: unsigned char writebyte(unsigned char j)
** ��������: ��ʱ
** �䡡��: unsigned char j:��д���ֽ�
** �䡡��: FALSE:����  
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static uint8 I2C_Writebyte(uint8 val)
{
	uint16 i;
	I2C_SDA_Bit_Set_Output_Mod();
	for (i=0;i<8;i++)
	{
		//temp = temp<<1; 
		I2C_SCL_CLR_0	//SCL = 0; 
		I2CDelay(); 
		if (val&(1<<(7-i)))
		{
			I2C_SDA_SET_1
		}
		else
		{
			I2C_SDA_CLR_0
		}
		I2CDelay(); 
		I2C_SCL_SET_1	//SCL = 1; 
		I2CDelay();
	}

	I2C_SCL_CLR_0	//SCL = 0; 
	I2CDelay(); 
	I2C_SDA_SET_1	//SDA = 1; 
	I2C_SDA_Bit_Set_Iput_Mod();
	I2CDelay();

	I2C_SCL_SET_1	//SCL = 1; 
	I2CDelay();
//	I2C_SDA_Bit_Set_Iput_Mod();
	i=0;
	while(i<255)
	{
		if(I2C_SDA_READ==0)   //SDA read
		{
			I2C_SCL_CLR_0	//SCL = 0; 
			I2CDelay();
			return TRUE;
		}
		i++;
	}
	I2C_SCL_CLR_0	//SCL=0; 
	I2CDelay();
	return FALSE; //error	
}

/****************************************************************************************
** ��������: unsigned char readbyte(void)
** ��������: ��I2C��һ���ֽ�
** �䡡��: ��
** �䡡��: ����������
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static uint8 I2C_Readbyte(void)
{
	uint8 i,tmp,val=0;
	I2C_SDA_Bit_Set_Output_Mod();
	I2C_SCL_CLR_0	//SCL = 0; 
	I2CDelay(); 
	I2C_SDA_SET_1	//SDA = 1;
	I2C_SDA_Bit_Set_Iput_Mod();

	for(i=0;i<8;i++)
	{
		I2CDelay(); 
		I2C_SCL_SET_1	//SCL = 1; 
		I2CDelay();
		if(I2C_SDA_READ==1) 
			tmp=1;
		else 
			tmp=0;
		val |= (tmp<<(7-i)); 
		I2C_SCL_CLR_0	//SCL=0;
	}
	I2CDelay();
	return(val);
}

/****************************************************************************************
** ��������: unsigned char readbyte(void)
** ��������: ��I2C��һ���ֽ�
** �䡡��: ��
** �䡡��: ����������
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
static uint8 I2C_ReadbyteWithAck(void)
{
	uint8 i,tmp,val=0;
	I2C_SDA_Bit_Set_Output_Mod();
	I2C_SCL_CLR_0	//SCL = 0; 
	I2CDelay(); 
	I2C_SDA_SET_1	//SDA = 1;
	I2C_SDA_Bit_Set_Iput_Mod();

	for(i=0;i<8;i++)
	{
		I2CDelay(); 
		I2C_SCL_SET_1	//SCL = 1; 
		I2CDelay();
		if(I2C_SDA_READ==1) 
			tmp=1;
		else 
			tmp=0;
		val |= (tmp<<(7-i)); 
		I2C_SCL_CLR_0	//SCL=0;
	}
	I2CDelay();
	//rm with ACK start
	I2C_SDA_Bit_Set_Output_Mod();
	I2C_SDA_CLR_0	//SDA = 0;   ACK
	I2CDelay();
	I2C_SCL_SET_1	//SCL = 1;
	I2CDelay(); 
	//end
	return(val);
}

/****************************************************************************************
** ��������: unsigned char read_eeprom(unsigned int address)
** ��������: ��ָ���ĵ�ַ������ I2C����
** �䡡��: unsigned int address:��д���ַ
** �䡡��: ����������
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
//static uint8 Read_EepromByte(uint32 address)
//{
//	uint8 val; 
//	I2C_Start();
//	I2C_Writebyte(0xa0);  //FM24CL16 ֻ��һƬ  ����A2 A1 A0��Ϊ0   ���1λ0��ʾд��1��ʾ��
//	I2C_Writebyte((address>>8)&0xff);
//	I2C_Writebyte(address&0xff);
//	I2C_Start();
//	I2C_Writebyte(0xa1);  
//	val=I2C_Readbyte();
//	I2C_Stop();
//	return(val);	 
//}
static uint8 Read_EepromByte(uint32 address)
{//BIT2��bit3��оƬѡ��bit1��pageѡ��bit0�Ƕ�дѡ��
	uint8 val; 
	uint8 wraddr,rdaddr;
	uint32 Faddr;
	if(address < 65536)
	{
		wraddr = 0xa0;
		rdaddr = 0xa1;
	}
	else if((address > 65535)&&(address < 131072))
	{
		wraddr = 0xa2;
		rdaddr = 0xa3;
	}
	else if((address > 131071)&&(address < 196608))
	{
		wraddr = 0xa4;
		rdaddr = 0xa5;
	}
	else if(address > 196607)
	{
		wraddr = 0xa6;
		rdaddr = 0xa7;
	}
	Faddr = address&0xFFFF;
	
	I2C_Start();
	I2C_Writebyte(wraddr);  //FM24CL16 ֻ��һƬ  ����A2 A1 A0��Ϊ0   ���1λ0��ʾд��1��ʾ��
	I2C_Writebyte((Faddr>>8)&0xff);
	I2C_Writebyte(Faddr&0xff);
	I2C_Start();
	I2C_Writebyte(rdaddr);  
	val=I2C_Readbyte();
	I2C_Stop();
	return(val);	 
}

/****************************************************************************************
** ��������: unsigned char write_page(unsigned int address,unsigned char *buf,unsigned char length)
** ��������: I2C ��ָ���ĵ�ַдһ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: TRUE:��ȷ/FALSE:����
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
//uint8 Write_Page(uint32 address,uint8 *buf,uint16 length)
//{
//	uint16 i;
//	I2C_Start();
//	if(I2C_Writebyte(0xa0)==FALSE) {return FALSE;}  //error
//	if(I2C_Writebyte((address>>8)&0xff)==FALSE) {return FALSE;}
//	if(I2C_Writebyte(address&0xff)==FALSE) {return FALSE;}	
//	
//	for(i=0;i<length;i++)
//	{
//		if(!I2C_Writebyte(*buf))
//		 {
//		 	I2C_Stop();
//			return FALSE;
//		 }
//		buf++; 
//	}
//	I2C_Stop();
//	return TRUE;  
//}

uint8 Write_Page_Cov(uint32 address,uint8 *buf,uint16 length)
{
	uint16 i;
	uint8 wraddr,rdaddr;
	uint32 Faddr;
	if(address < 65536)
	{
		wraddr = 0xa0;
		rdaddr = 0xa1;
	}
	else if((address > 65535)&&(address < 131072))
	{
		wraddr = 0xa2;
		rdaddr = 0xa3;
	}
	else if((address > 131071)&&(address < 196608))
	{
		wraddr = 0xa4;
		rdaddr = 0xa5;
	}
	else if(address > 196607)
	{
		wraddr = 0xa6;
		rdaddr = 0xa7;
	}
	Faddr = address&0xFFFF;
	
	I2C_Start();
	if(I2C_Writebyte(wraddr)==FALSE) {return FALSE;}  //error
	if(I2C_Writebyte((Faddr>>8)&0xff)==FALSE) {return FALSE;}
	if(I2C_Writebyte(Faddr&0xff)==FALSE) {return FALSE;}	
	
	for(i=0;i<length;i++)
	{
		if(!I2C_Writebyte(*buf))
		 {
		 	I2C_Stop();
			return FALSE;
		 }
		buf++; 
	}
	I2C_Stop();
	return TRUE;  
}


uint8 Write_Page(uint32 address,uint8 *buf,uint16 length)
{
	uint16 wrsum;
	if(address > 0x1FFFF)
	{
		Write_Page_Cov(address,buf,length);
	}
	else if((address+length)<=0x1FFFF)
	{
		Write_Page_Cov(address,buf,length);
	}
	else if((address+length)>0x1FFFF)
	{//��ֹ��ַ������������оƬ��
		wrsum = 0x20000 - address;
		Write_Page_Cov(address,buf,wrsum);//д�ڵ�һ��оƬ��
		Write_Page_Cov(0x20000,buf+wrsum,length -wrsum);//д�ڵڶ���оƬ��
	}
	return TRUE;
}

/****************************************************************************************
** ��������: void read_page(unsigned int address,unsigned char *buf,unsigned char length)
** ��������: I2C ��ָ���ĵ�ַ��һ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: ��
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
uint8 Read_Page(uint32 address,uint8 *buf,uint8 length)
{
	uint16 i;

	for(i=0;i<length;i++)
	{
		buf[i]=Read_EepromByte(address+i);
	}
/*	I2C_Start();
	I2C_Writebyte(0xa2);  //FM24C256 A2-A1 = 01  address	R/nW
	I2C_Writebyte(address>>8);
	I2C_Writebyte(address&0xff);
	I2C_Start();
	I2C_Writebyte(0xa3);
	for(i=0;i<length;i++)
	{
		buf[i]=I2C_Readbyte();
	}*/
	return TRUE;
}

/****************************************************************************************
** ��������: unsigned char Eeprom_Write(unsigned int address,unsigned char *buf,unsigned char length)
** ��������: I2C ��ָ���ĵ�ַдһ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: TRUE:��ȷ/FALSE:����
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
uint8 Eeprom_Write(uint8 *buf, uint32 address,uint16 length)
{
	uint8 err;
	uint8 ret;
	if((address >= 9)&&(address < 15))
	{
		ret = ret;
	}
	OSSemPend(FPRAM_Lock, 0, &err);
	if(Write_Page(address,buf,length) == FALSE)
	{//���д��ʧ�ܣ���д��һ��
		ret = Write_Page(address,buf,length);
	}
	else
	{
		ret = TRUE;
	}
	OSSemPost(FPRAM_Lock);
	return ret;	  
}


/****************************************************************************************
** ��������: unsigned char write_page(unsigned int address,unsigned char *buf,unsigned char length)
** ��������: I2C ��ָ���ĵ�ַдһ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: TRUE:��ȷ/FALSE:����
** ȫ�ֱ���:
** ����: д�ⲿeeprom / FRAM IIC����
******************************************************************************************/
//uint8 Eeprom_Read(uint8 *buf, uint32 address, uint16 length)
//{
//	uint16 i;
//	uint8 err;
//	OSSemPend(FPRAM_Lock, 0, &err);
//	I2C_Start();
//	I2C_Writebyte(0xa0);  //FM24CL16 A2-A0 = 000  address	R/nW
//	I2C_Writebyte((address>>8)&0xff);
//	I2C_Writebyte(address&0xff); 
//	
//	I2C_Start();
//	I2C_Writebyte(0xa1);
//	for(i=0;i<length;i++)
//	{
//		buf[i]=I2C_ReadbyteWithAck();
//	}
//	I2C_Stop();
//	OSSemPost(FPRAM_Lock);	
//	return TRUE;  
//}
uint8 Eeprom_Read_Cov(uint8 *buf, uint32 address, uint16 length)
{
	uint16 i;
	uint8 err;

	uint8 wraddr,rdaddr;
	uint32 Faddr;
	if(address < 65536)
	{
		wraddr = 0xa0;
		rdaddr = 0xa1;
	}
	else if((address > 65535)&&(address < 131072))
	{
		wraddr = 0xa2;
		rdaddr = 0xa3;
	}
	else if((address > 131071)&&(address < 196608))
	{
		wraddr = 0xa4;
		rdaddr = 0xa5;
	}
	else if(address > 196607)
	{
		wraddr = 0xa6;
		rdaddr = 0xa7;
	}
	Faddr = address&0xFFFF;	
	
	OSSemPend(FPRAM_Lock, 0, &err);
	I2C_Start();
	I2C_Writebyte(wraddr);  //FM24CL16 A2-A0 = 000  address	R/nW
	I2C_Writebyte((Faddr>>8)&0xff);
	I2C_Writebyte(Faddr&0xff); 
	
	I2C_Start();
	I2C_Writebyte(rdaddr);
	for(i=0;i<length;i++)
	{
		buf[i]=I2C_ReadbyteWithAck();
	}
	I2C_Stop();
	OSSemPost(FPRAM_Lock);	
	return TRUE;  
}



uint8 Eeprom_Read(uint8 *buf,uint32 address,uint16 length)
{
	uint16 rdsum;
	if(address > 0x1FFFF)
	{
		Eeprom_Read_Cov(buf,address,length);
	}
	else if((address+length)<=0x1FFFF)
	{
		Eeprom_Read_Cov(buf,address,length);
	}
	else if((address+length)>0x1FFFF)
	{//��ֹ��ַ������������оƬ��
		rdsum = 0x20000 - address;
		Eeprom_Read_Cov(buf,address,rdsum);//д�ڵ�һ��оƬ��
		Eeprom_Read_Cov(buf+rdsum,0x20000,length -rdsum);//д�ڵڶ���оƬ��
	}
	return TRUE;
}



/****************************************************************************************
** ��������: Eeprom_Write_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length)
** ��������: I2C ��ָ���ĵ�ַдһ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: TRUE:��ȷ/FALSE:����
** ȫ�ֱ���:
** ����: ͬʱд������ȥ�ͱ���������
******************************************************************************************/
uint8 Eeprom_Write_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length)
{
	uint8 buff[30];
	if(Eeprom_Write(buf,address,length)==TRUE)
	{
		if(Eeprom_Write(buf,bkpaddress,length)==TRUE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/****************************************************************************************
** ��������: uint8 Eeprom_Read_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length)
** ��������: I2C ��ָ���ĵ�ַ��ȡһ��ָ�����ȵ�����
** �䡡��: unsigned int address:ָ����ַ
			uint32 bkpaddress:�������ݵ�ַ
**			unsigned char *buf:д�����ݵ�����ָ��
**			unsigned char length:д�����ݵĳ���
** �䡡��: TRUE:��ȷ/FALSE:����
** ȫ�ֱ���:
** ����: ͬʱ��ȡ�������ͱ�����������������������������ͬ���ȡ�ɹ�������ʧ��
******************************************************************************************/
uint8 Eeprom_Read_I2c(uint8 *buf,uint32 address,uint32 bkpaddress,uint16 length)
{
	uint8 rdbuff[100];
	uint8 buff[100];
	uint8 i;
	if(Eeprom_Read( buf, address,length)==FALSE)
	{
		return FALSE;
	}
	if(Eeprom_Read(rdbuff, bkpaddress,length)==FALSE)
	{
		return FALSE;
	}
	for(i=0;i<length;i++)
	{
		if(buf[i] != rdbuff[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}


#else 	//   /*��ģ��I2C*/

// ======================================================================================================
// ����IIC�ӿڳ�ʼ�� 
// ��������
// ���أ���
// ======================================================================================================
void sysI2CInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  I2C_InitTypeDef  I2C_InitStructure; 

  RCC_APB1PeriphClockCmd(I2C_FRAM_CLK, ENABLE);   	  // IIC����ʱ�Ӵ� 
  RCC_APB2PeriphClockCmd(I2C_FRAM_GPIO_CLK, ENABLE);  // IIC Pin ����ʱ�Ӵ�
	
  GPIO_InitStructure.GPIO_Pin =  I2C_FRAM_SCL | I2C_FRAM_SDA; // PB6-IIC_SCL  PB7-IIC_SDA
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(I2C_FRAM_GPIO, &GPIO_InitStructure);

  
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                   	             // IICģʽ ���� SMBusģʽ
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                         // ����ģʽʱ��ռ�ձ� Tlow/Thigh = 2 
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;										 // �����ַ 0x50
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				 	                       // Ӧ��ʹ�� 
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	 // Ѱַģʽ 7λ�ӵ�ַ ����10λ�ӵ�ַ
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;	                             // IIC �ٶ� 200K 
  
  I2C_Cmd(I2C_FRAM, ENABLE);	                                               // IICʹ��               
  I2C_Init(I2C_FRAM, &I2C_InitStructure);							                       // ���õ��Ĵ��� 

	FPRAM_Lock = OSSemCreate(1);	   
}
// ==========================================================================================================
// uint16 I2C_FRAM_BufferWrite(uint8* buf, uint16 ext_addr, size)
// ��IIC FRAMд��һ������
// ������buf ��д�����ݴ���ʼ��ַ	��ext_addr ��д��FRAM��ַ��Χ0-0x7ff ��size ��д�����ݳ��� 
// ���أ�д�����ݳ��� 
// =========================================================================================================
uint16 I2C_FRAM_BufferWrite(uint8* buf, uint16 ext_addr, uint16 size)
{
	uint8 err;
	uint16 ret = 0;
	if((ext_addr + size) > FRAM_DATA_SIZE)
	{
		return ret;
	}
	OSSemPend(FPRAM_Lock, 0, &err);
	while(I2C_GetFlagStatus(I2C_FRAM, I2C_FLAG_BUSY));	                      // �ȴ����� -- stop״̬
  
  I2C_GenerateSTART(I2C_FRAM, ENABLE);	                                    // ����IIC��ʼ���� 
  
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_MODE_SELECT));           // ��ģʽ+����ͨѶ+��ʼ�����Ѿ�����  -- start ״̬ 
  
  I2C_Send7bitAddress(I2C_FRAM, FRAM_DEV_ADDR, I2C_Direction_Transmitter);  // ���� д������Ŀ��IIC������ַ

  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // ��ģʽ+��ַ�����Ѿ�����+���ݼĴ���Ϊ�� -- �豸��ַ�ѷ���״̬


	I2C_SendData(I2C_FRAM, (uint8_t)((ext_addr & 0xFF00) >> 8));              // ����д�������ݵ�ַ���ֽ� 
	while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	    // �ȴ����ݷ������ 
	I2C_SendData(I2C_FRAM, (uint8_t)(ext_addr & 0x00FF));                     // ����д�������ݵ�ַ���ֽ� 

  while(! I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	    // �ȴ�д���ݵ�ַ���  -- ���ݵ�ַ�ѷ���״̬

	while(size--)  	                                              					  // �������ݳ�ȥ
  {
    I2C_SendData(I2C_FRAM, *buf);                                           // �����ֽڳ�ȥ 
    buf++; 	
		ret++;				                                                          // ˢ��ָ�� ָ����һ�ֽ�
    while (!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	  // �ȴ����ݷ������
  }
  I2C_GenerateSTOP(I2C_FRAM, ENABLE);	                                      // ����ֹͣ״̬
	OSSemPost(FPRAM_Lock);
	return ret;
}

// ==========================================================================================================
// uint16 I2C_FRAM_BufferRead(uint8* buf, uint16 ext_addr, size)
// ��IIC FRAM�����ȡһ������
// ������buf ����ȡ���ݴ���ʼ��ַ	��ext_addr ����ȡFRAM��ַ��Χ0-0x7ff ��size ����ȡ���ݳ��� 
// ���أ���ȡ���ݳ��� 
// =========================================================================================================
uint16 I2C_FRAM_BufferRead(uint8* buf, uint16 ext_addr, uint16 size)
{
  uint8 err;
	uint16 ret = 0;  
 	if((ext_addr + size) > FRAM_DATA_SIZE)
	{
		return ret;
	}
	
	OSSemPend(FPRAM_Lock, 0, &err);
  while(I2C_GetFlagStatus(I2C_FRAM, I2C_FLAG_BUSY));	                          // bus free OK 
  
  I2C_GenerateSTART(I2C_FRAM, ENABLE);	               
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_MODE_SELECT));	              // start  OK 
   
  I2C_Send7bitAddress(I2C_FRAM, FRAM_DEV_ADDR, I2C_Direction_Transmitter);	    // ����д����������ַ
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	// devAddr OK
  I2C_Cmd(I2C_FRAM, ENABLE);

  I2C_SendData(I2C_FRAM, (uint8_t)((ext_addr & 0xFF00) >> 8));    	            // д���ȡ���ݵ�ַ
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C_FRAM, (uint8_t)(ext_addr & 0x00FF));    
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTART(I2C_FRAM, ENABLE);		                                      // ����������start״̬��ͬʱ�˳�д����ģʽ
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress(I2C_FRAM, FRAM_DEV_ADDR, I2C_Direction_Receiver);		      // ������ַ���λ��1 ��Ϊ������
  while(!I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  while(size)  
  {
    if(size == 1)				                                                       // ���һ�ֽ� 
    {
      I2C_AcknowledgeConfig(I2C_FRAM, DISABLE);		                             // ��ֹӦ��
      I2C_GenerateSTOP(I2C_FRAM, ENABLE);		                                   // stop״̬���� 
    }
    if(I2C_CheckEvent(I2C_FRAM, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      *buf = I2C_ReceiveData(I2C_FRAM);	                                       // ��ȡһ�ֽ�
      buf++; 		                                                               // ˢ��ָ��
      size--;  
			ret++;      
    }   
  }
  I2C_AcknowledgeConfig(I2C_FRAM, ENABLE);	                                   // ȷ�Ͽ��Խ�����һ֡����  
	OSSemPost(FPRAM_Lock);
	return ret; 
}

 #endif  //DEF_SIMULATE_I2C  
 
