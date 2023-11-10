#include <includes.h>




const uint8 NoDev[]={"û��ģ��!"};
const uint8 StartRMea[]={"      ��������Ѳ��!      "};
const uint8 StartRMea1[]={"�Ǹ���״̬,��������Ѳ��! "};
const uint8 StopRMea[]={"      ֹͣ����Ѳ��!       "};
const uint8 SaveSuccess[]={"����ɹ�!"};
const uint8 SaveFail[]={"����ʧ��!"};
const uint8 SetSuccess[]={"�趨�ɹ�!"};
const uint8 SetFail[]={"�趨ʧ��!"};
const uint8 NoTestRecord[]={"û�в�����¼!"};
const uint8 SaveParaRecord[]={"         ��������!         "};


#define MAIN_PICTURE_CHANGE_TIME		2000000

uint16 ErrBatNOAddr[5] = {0x0810,0x0840,0x0870,0x08A0,0x08D0};//�������ݵ�ر����LCD�еĵ�ַ
uint16 ErrTypeAddr[5] = {0x0820,0x0850,0x0880,0x08B0,0x08E0};//�������ݹ���������LCD�еĵ�ַ
uint16 ErrDataAddr[5] = {0x0830,0x0860,0x0890,0x08C0,0x08F0};//�������ݹ���������LCD�еĵ�ַ



/*************************************************************************************
**  uint16 Big2Samll(uint16 temp) 
**   ����: ��С��ģʽ�任
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint16 Big2Samll(uint16 temp) 
{ 
   uint16 tempdata = 0; 
   tempdata = (temp >> 8) & 0x00ff; 
   tempdata = (((temp & 0x00ff) << 8) + tempdata); 
   return tempdata;
}

/***********************************************************/

/*************************************************************************************
**  uint8 SwitchToPicX(uint8 *Txbuf, uint8 cmd, uint16 pageN) 
**   ����: �л�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
/***********************************************************/
uint8 SwitchToPicX(uint8 com,uint8 cmd, uint16 pageN) // �л���ĳ����
{ 
   uint8 length = 0; 
   uint8 Txbuf[20];
   
   Txbuf[length++] = FRAME_HIGH; 
   Txbuf[length++] = FRAME_LOW; 
   Txbuf[length++] = 0;      // length
   Txbuf[length++] = cmd;    // cmd
   Txbuf[length++] = 0x03;   // register add
   
   pageN = Big2Samll(pageN); 
   memcpy(&Txbuf[length], &pageN, sizeof(pageN));    //ͼƬ���
   length += sizeof(pageN); 

	 CalAndSendData(com,Txbuf,length);
   return length;
}


/*************************************************************************************
**  uint8 ReadPicID(uint8 com)
**   ����: �л�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
/***********************************************************/
uint8 ReadPicID(uint8 com) // �л���ĳ����
{ 
   uint8 length = 0; 
   uint8 Txbuf[20];
   
   Txbuf[length++] = FRAME_HIGH; 
   Txbuf[length++] = FRAME_LOW; 
   Txbuf[length++] = 0;      // length
   Txbuf[length++] = CMD_READ_PICNUM;    // cmd
   Txbuf[length++] = 0x03;   // register add
   Txbuf[length++] = 0x02; 

   CalAndSendData(com,Txbuf,length);
   return length;
}

/*************************************************************************************
**  uint8 SendInnerRCtrlValueToLCD(uint8 com)
**   ����: ����ʱ�䵽LCD
**   ����:   ͨ�Ŷ˿ں�
**   ���:  
**   ����: 
**************************************************************************************/
uint8 SendInnerRCtrlValueToLCD(uint8 com)
{
	uint8 length = 0;
	uint8 Txbuf[30];

	Txbuf[length++] = FRAME_HIGH; 
	Txbuf[length++] = FRAME_LOW; 
	Txbuf[length++] = 0;      // length
	Txbuf[length++] = CMD_WRITE_DATA;    // cmd
	//Txbuf[length++] = (R_PATROL_BUTTON >>8) & 0xFF;   // register add
	//Txbuf[length++] = R_PATROL_BUTTON & 0xFF;   // register add	
	Txbuf[length++] = 0;
	Txbuf[length++] = 0;
	
	CalAndSendData(com,Txbuf,length);	
	return length;
}


/*************************************************************************************
**  uint8 CloseLcdLed(uint8 com)
**   ����: �رձ����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 CloseLcdLed(uint8 com)
{
	uint8 length = 0;
	uint8 Txbuf[15];	



	memset(Txbuf,0,length);
	length = 0;
 
	Txbuf[length++] = FRAME_HIGH; 
	Txbuf[length++] = FRAME_LOW;
	Txbuf[length++] = 0;      // length
	Txbuf[length++] = CMD_SWITCH_PIC;
	Txbuf[length++] = 0x01;
	Txbuf[length++] = 0x01;
	CalAndSendData(com,Txbuf,length);
	return length;
}



/*************************************************************************************
**  uint8 OpenLcdLed(uint8 com)
**   ����: �򿪱����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 OpenLcdLed(uint8 com)
{
	uint8 length = 0;
	uint8 Txbuf[15];	

	memset(Txbuf,0,length);
	length = 0;
	Txbuf[length++] = FRAME_HIGH; 
	Txbuf[length++] = FRAME_LOW;
	Txbuf[length++] = 0;      // length
	Txbuf[length++] = CMD_SWITCH_PIC;
	Txbuf[length++] = 0x01;
	Txbuf[length++] = 0x3F;
	CalAndSendData(com,Txbuf,length);
	return length;
}
/*************************************************************************************
**  uint8 WriteDataToPicture0(uint8 com)
**   ����: �������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 WriteDataToPicture0(uint8 com)
{

	return 1;
}
/*************************************************************************************
**  uint8 WriteDataToPicture1(uint8 com)
**   ����: �������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 WriteDataToPicture1(uint8 com)
{
		uint8 length = 0;

	return length;
}

/*************************************************************************************
**  uint8 WriteDataToPicture2(uint8 com)
**   ����: �������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 WriteDataToPicture2(uint8 com)
{
	uint8 length = 0;

	
	return length;
}
/*************************************************************************************
**  uint8 WriteDataToPicture3(uint8 com)
**   ����: �������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 WriteDataToPicture3(uint8 com)
{
	uint8 length = 0;

	return length;
}

/*************************************************************************************
**  uint8 ClrPicture135Data(uint8 com)
**   ����: ���135ͼƬ�Ĳ��ֲ���
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 ClrPicture151Data(uint8 com)
{



	return 1;
}
/*************************************************************************************
**  uint8 PictureSwitch(uint8 com)
**   ����: �л�ͼƬ�������ʱ��û�в���Һ��������ص���̬����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
void PictureSwitch(uint8 com)
{
/************************ ��ʱ�䲻�����ص������� ***********************************/	
	if((JdyTest.g_TimerMs - JdyTest.Lcd.PageOperateTimeFlag > MAIN_PICTURE_CHANGE_TIME))
	{//�Ƿŵ�״̬��15���Ӳ������ص������棬17���Ӳ��������䰵
		JdyTest.DataYk.PictureNO = PICTURE1;//һ�����س�̬����

		if(JdyTest.g_TimerMs - JdyTest.Lcd.PageOperateTimeFlag > (MAIN_PICTURE_CHANGE_TIME+120000))
		{
			CloseLcdLed(com);		
		}
	}	
	else
	{
		OpenLcdLed(com);
	}
	if(PackedAndSendData(com, JdyTest.DataYk.PictureNO, CMD_SWITCH_PIC)==TRUE)	// �л�ͼƬ
	{
		if(JdyTest.DataYk.OldPictureNO != JdyTest.DataYk.PictureNO)
		{
			//LcdComm.BatGroupNO = 0;
			//LcdComm.DevNO = 0;
		}
	}
	OSTimeDly(100); //1000��ʱ�ӽ�����250ms��������Ҫ��ʱ�������ܽ����л���ʱ���������
}
/*************************************************************************************
**  static uint8 LcdGetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
**   ����: �����ѽ������ݰ�����
**   ����: buf--���ݽ�����ɺ󻺴��� 
**   ���: ret-������ɱ�־    buf-���ݽ��ջ�����ʼ��ַ 
**   ����: 
**************************************************************************************/
static uint8 LcdGetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8 i, len, *ptr, ret = LCD_RCV_UNDONE;
	
	if(pchan->p_head >= pchan->p_tail)
	{

		len = pchan->p_head - pchan->p_tail;           /* �ѽ��յ����ݳ��� */
	}
	else
	{

		len = pchan->p_head + LCD_RCV_QUEUE_SIZE - pchan->p_tail;
	}
	if(len >= 11)                                       /* һ֡������С����Ϊ11���ֽ� */                                       
	{
		ptr = pchan->p_tail;                     
		for(i = 0; i < 11; i++)                         /*����ת�봦�����*/
		{
			buf[i] = *ptr;
			if(++ptr >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
			{
				ptr = pchan->rcvbuf;                   /* ѭ������ */
			}
		}
		if(len >= (buf[2]+3))
		{
			for(i=11;i<buf[2]+3;i++)
			{
				buf[i] = *ptr;
				if(++ptr >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
				{
					ptr = pchan->rcvbuf;                   /* ѭ������ */
				}
			}
		}
		else
		{
			return ret;
		}
		pchan->p_tail = ptr;       
		ret = LCD_RCV_OK;                              /*���ݰ��������*/  
	}
	return ret;
}



/*************************************************************************************
**  static void LcdFixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   ����:  �������ݴ洢
**   ����:  src--modbus�еĶε�ַ   buf--��洢��������ʼ��ַ  startAddr--�ε�ַ�е�ƫ�Ƶ�ַ
**   ���:  
**   ����: 
**************************************************************************************/
static void LcdFixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i;

	for(i = 0; i < Lenth; i++)
	{
		if(i%2)         
		{
			src[StartAddr+i-1] = buf[i];
		}
		else
		{
			src[StartAddr+i+1] = buf[i];
		}	
	}
}

/*************************************************************************************
**  static uint8 LcdFillTransData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   ����:  �������ݴ洢
**   ����:  src--�ε�ַ   buf--��洢��������ʼ��ַ  startAddr--�ε�ַ�е�ƫ�Ƶ�ַ
			Lenth--���ݳ���
**   ���:  
**   ����: 
**************************************************************************************/
static uint8 LcdFillTransData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i, txlen = 0;

	//buf[txlen++] = Lenth;
	
	for(i = 0; i < Lenth; i++)
	{
		if(i%2)         
		{
			buf[txlen++] = src[StartAddr+i-1];
		}
		else
		{
			buf[txlen++] = src[StartAddr+i+1];
		}
	}
	
	return txlen;
}
/*************************************************************************************
**  static uint8 LcdFillTransData32(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   ����:  �������ݴ洢
**   ����:  src--�ε�ַ   buf--��洢��������ʼ��ַ  startAddr--�ε�ַ�е�ƫ�Ƶ�ַ
			Lenth--���ݳ���
**   ���:  
**   ����: 
**************************************************************************************/
static uint8 LcdFillTransData32(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
{
	uint8 i, txlen = 0;
	
	for(i = 0; i < Lenth; i++)
	{
		switch(i%4)
		{
			case 0:
				buf[txlen++] = src[StartAddr+i+3];
				break;
			case 1:
				buf[txlen++] = src[StartAddr+i+1];
				break;
			case 2:
				buf[txlen++] = src[StartAddr+i-1];
				break;
			case 3:
				buf[txlen++] = src[StartAddr+i-3];
				break;
		}
	}
	
	return txlen;
}
/*************************************************************************************
**   void CalAndSendData(uint8 *txbuf,uint8 length)
**   ����: У�����ݲ�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
 uint8 CalAndSendData(uint8 com,uint8 *txbuf,uint8 txlen)
 {
	uint16 CRCValue = 0; 
	txbuf[2] =  txlen - 3;                     // ������ݳ���
	CRCValue = LcdTxCRC((txbuf + 3), txbuf[2]); 
	memcpy(&txbuf[txlen], &CRCValue, 2); 
	txlen += 2; 
	txbuf[2] += 2; 
	if(Uart_Write(UartHandler[com], txbuf, txlen) == txlen)
	{
		return TRUE;
	}

	else
	{
		return FALSE;
	}
 }
/*************************************************************************************
**  void PackedAndSendData(uint8 *txbuf, uint16 pageno, uint8 cmd)
**   ����: �����������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 PackedAndSendData(uint8 com,uint16 pageno, uint8 cmd)
{
   if (cmd == CMD_SWITCH_PIC)  
   {
      /*********************�л�ͼƬ*********************/
         SwitchToPicX(com,cmd,pageno); 
   }
   else
   {
	switch(pageno)
	{
		case PICTURE0:
			WriteDataToPicture0(com);
			break;
		case PICTURE1:
			WriteDataToPicture1(com);
			break;
		case PICTURE2:
			WriteDataToPicture2(com);
			break;



		
		default:
			break;
	}
   }
	 return TRUE;
}
 /*************************************************************************************
**  void LcdProtocolAnalysis(uint8 com,uint8 *buf,uint8 SectNo,uint8 addrOffset,uint16 regNum)
**   ����: ��ͨ��Э�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 LcdProtocolAnalysis(uint8 com,uint8 *buf,uint8 SectNo,uint8 addrOffset,uint16 regNum)
{
	uint8 ret = RCV_OK;
	//uint8 txlen = 0, ret = RCV_OK, dataOffset = 4;  /*д���Ĵ���������������MODBUS���ݰ��е�ƫ�Ƶ�ַ*/
	//MSG_STRUCT msg;
	//uint8 i;
	//uint16 temp;
	//struct tm ttm;
	//#if OS_CRITICAL_METHOD == 3
	//OS_CPU_SR  cpu_sr;
	//#endif  
	
	switch (buf[3]) 
      {
	      case CMD_READ_DATA:
	         {
			if(SectNo == LCD_CMD_SECT)
			{
				DealScreenCmd(com,buf,SectNo,addrOffset,regNum);				/*���������ص�����*/
			}
			else
			{
				DealScreenData(com,buf,SectNo,addrOffset,regNum); 				/*��������ؼ����ص�����*/
			}
	         }
	         break; 
	         
	      case CMD_READ_PICNUM:
	         {
			;
	         }
	         break; 
	      default:
	         break;
      }
	return ret;
}

/*************************************************************************************
**  static uint8 LcdIrqSectAddr(uint16 startAddr,uint8 RegNum)
**   ����: �������ݶ�
**   ����:  
**   ���:  
**   ����: 
**************************************************************************************/
static uint8 LcdIrqSectAddr(uint16 startAddr,uint8 RegNum)
{
	uint16 i,AddrLimit;
	AddrLimit = startAddr + RegNum - 1;
	for(i = 1; i <= S_MAX_SECT; i++)
	{
		if((startAddr >= SSectAddrAndData[i].LowRegAddr) && (AddrLimit <= (SSectAddrAndData[i].HighRegAddr-1)))
		{
			return i;	
		}
	}
	return DEV_NON_SECT;
}

/*************************************************************************************
**  void CheckRcvLcdData(uint8 com,uint8 *buf)
**   ����: У�����ݰ�
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 CheckRcvLcdData(uint8 com,uint8 *buf)
{
	uint8  SectNo, RcvLen, ret = LCD_RCV_OK;
	uint16 StartAddr = 0, RegNum = 0, addrOffset;
	uint16 CrcValue;
	if(buf[3] == CMD_READ_DATA)
	{
		RcvLen = buf[2] + 3;
		RegNum = buf[6];
	}
	CrcValue = LcdRxCRC(&buf[3],RcvLen - 5);
	if(!(((CrcValue & 0xff) == buf[buf[2] + 1]) && (((CrcValue>>8) & 0xff) == buf[buf[2] + 2])))
	{
		ret = RCV_CRC_ERR;                 
	}
	else
	{
			StartAddr = (buf[4]<<8) | buf[5];
			if((StartAddr >= FIRST_BUTTON) && (StartAddr <= FINAL_BUTTON))
			{
				SectNo = LCD_CMD_SECT;
			}
			else
			{
				SectNo = LcdIrqSectAddr(StartAddr, RegNum);
			}
			if((SectNo == S_NON_SECT) || ((StartAddr + RegNum) > SSectAddrAndData[SectNo].HighRegAddr))
			{
				ret = LCD_RCV_REG_ADD_ERR;
			}
			else
			{
				addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*�ڶ��е�ƫ�Ƶ�ַ*/	
				ret = LcdProtocolAnalysis(com,buf,SectNo,addrOffset,RegNum);             /*���ݰ������ж�,������ȷ��Ӧ��*/
			}
	}
	return ret;
}
/************************�������������ҳ���ȡ����********************************/
uint8 CheckRcvLcdData1(uint8 com,uint8 *buf)
{
	uint8  SectNo, RcvLen, ret = LCD_RCV_OK;
	uint16 StartAddr = 0, RegNum = 0, addrOffset;
	uint16 CrcValue;
	uint16 PageID;
	if(buf[3] == CMD_READ_DATA)
	{
		RcvLen = buf[2] + 3;
		RegNum = buf[6];
	}
	else if(buf[3] == CMD_READ_PICNUM)
	{
		PageID = (uint16)(buf[6]<<8) + buf[7];
	}
	CrcValue = LcdRxCRC(&buf[3],RcvLen - 5);
	if(!(((CrcValue & 0xff) == buf[buf[2] + 1]) && (((CrcValue>>8) & 0xff) == buf[buf[2] + 2])))
	{
		ret = RCV_CRC_ERR;                 
	}
	else
	{
		if(buf[3] == CMD_READ_DATA)
		{
			StartAddr = (buf[4]<<8) | buf[5];
			if((StartAddr >= FIRST_BUTTON) && (StartAddr <= LCD_BUTTON_MAX_ADDR))
			{
				SectNo = LCD_CMD_SECT;
			}
			else
			{
				SectNo = LcdIrqSectAddr(StartAddr, RegNum);
			}
			if((SectNo == S_NON_SECT) || ((StartAddr + RegNum) > SSectAddrAndData[SectNo].HighRegAddr))
			{
				ret = LCD_RCV_REG_ADD_ERR;
			}
			else
			{
				addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*�ڶ��е�ƫ�Ƶ�ַ*/	
				ret = LcdProtocolAnalysis(com,buf,SectNo,addrOffset,RegNum);             /*���ݰ������ж�,������ȷ��Ӧ��*/
			}
		}
		else if(buf[3] == CMD_READ_PICNUM)
		{
			JdyTest.DataYk.PictureNO = PageID;
		}
	}
	return ret;
}
/*************************************************************************************
**  uint8 LcdCheckRcvDone(UART_RCV_CFG *pchan, uint8 *buf)
**   ����:�����Ƿ�������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
uint8 LcdCheckRcvDone(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8  ret = LCD_RCV_UNDONE;
	int16 temp;
	while(pchan->p_tail != pchan->p_head)                                         /*�����������*/
	{
		temp = pchan->p_head - pchan->p_tail;
		if(temp < 0)
		{
			temp = pchan->p_head + LCD_RCV_QUEUE_SIZE - pchan->p_tail;
		}
		if(temp >= 2)
		{
			if(((*(pchan->p_tail) == FRAME_HIGH) && (*(pchan->p_tail + 1) == FRAME_LOW) )|| (*(pchan->p_tail) == BROADCAST_ADDR))/*�豸��ַ��ȷ,��ʼ����*/
			{
				ret = LcdGetRcvLen(pchan,buf);                                           /*�ж�һ֡�����Ƿ������*/
				break;
			}
			else
			{
				if(++(pchan->p_tail) >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
				{
					pchan->p_tail = pchan->rcvbuf;                                     /* ѭ������ */
				}
			}
		}
		else if(temp == 1)
		{
			ret = LCD_RCV_UNDONE;
			return ret;
		}
	}
	return ret;
}
/*************************************************************************************
**  void LcdErrRespond(com, rcvtmpbuf[1], ret)
**   ����: ���ݴ�����Ӧ
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
void LcdErrRespond(void)
{

}
/*************************************************************************************
**   void DealScreenData(uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum,uint8 *TxBuf)
**   ����: ��������ؼ����ص�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
 uint8 DealScreenData(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum) 				/*��������ؼ����ص�����*/
{
	uint8 dataOffset;
	uint8 ret = RCV_OK;
	MSG_STRUCT msg;
	//uint16 temp;
	uint8 i,DEVCOM;
	uint8 recom;//��Ҫ�������õ�ͨ�Ŷ˿ں�
	uint32 address,bkpaddress;
	uint16 batgroup;
	struct tm ttm;	
	
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  

	dataOffset = 7;
	
	switch(SectNo)
	{
		case S_BAT_V_DATA_SECT:										/*��ص�ѹ���ݶ�*/
		break;
		case S_BAT_R_DATA_SECT:										/*�����������*/
		break;
		case S_CONNECT_R_DATA_SECT:								/*���ӵ������ݶ�*/
		break;
		case S_BAT_TEMP_DATA_SECT:									/*����¶����ݶ�*/
		break;
		case S_BAT_ERR_STAT_SECT:									/*��ع���״̬��*/
		break;
		case S_DEV_STAT_SECT:											/*ģ������״̬��*/
		break;
		case S_MEA_DATA_SECT:											/*�������ݶ�*/
		break;
		case S_CAL_PARA_SECT:											/*У׼������*/
		{
			//LcdComm.ReadParaFlag = ON;//����֮���ȡһ��
			//LcdFixReadData((uint8*)&MbsSlave.SysPara.CalPara[batgroup][LcdComm.DevNO].S_CellVoltageB, buf+dataOffset, addrOffset, regNum*2);
			msg.MsgType = MSG_WR_REG;
			msg.MsgLenth = 1;
			msg.MsgCOM = DEVCOM;
			msg.MsgDevAddr = 0;
			msg.MsgData[0] = MODBUS_WR_MULT_REG;
			msg.MsgData[1] = (uint8)(CAL_PARA_ADDR>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
			msg.MsgData[2] = (uint8)(CAL_PARA_ADDR & 0xFF);
			msg.MsgData[3] = regNum;
			for(i=0;i<regNum;i++)
			{
				msg.MsgData[i*2+4] = buf[i*2+dataOffset+1];//ע��洢ģʽ
				msg.MsgData[i*2+5] = buf[i*2+dataOffset];
			}
			
				//SendMsg2Queue(&msg,DevTask.TaskBat1);

		}
		break;
		case S_SYS_WORK_STAT_SECT:									/*ϵͳ����״̬��*/
		break;
		case S_ASSIST_STAT_SECT:									/*����״̬��*/
		break;
		case S_SYS_CTRL_SECT:											/*ϵͳ���ƶ�*/
		{
			//LcdComm.ReadParaFlag = ON;//����֮���ȡһ��
			LcdFixReadData((uint8*)&MbsSlave.SysPara.SysCtrl[0].S_StartCellMea, buf+dataOffset, addrOffset, regNum*2);//װ������
			msg.MsgType = MSG_WR_REG;
			msg.MsgLenth = 1;
			msg.MsgCOM = DEVCOM;
			msg.MsgDevAddr = BROADCAST_ADDR;
			msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
			msg.MsgData[1] = (uint8)(SYS_CTRL_ADDR>>8) & 0x00FF;//ע��:����ĵ�ַҪ��MBS�������涨��ĵ�ַ
			msg.MsgData[2] = (uint8)(SYS_CTRL_ADDR & 0xFF);
			msg.MsgData[3] = regNum;
			for(i=0;i<regNum;i++)
			{
				msg.MsgData[i*2+4] = buf[i*2+dataOffset+1];
				msg.MsgData[i*2+5] = buf[i*2+dataOffset];
			}
			//SendMsg2Queue(&msg,DevTask.TaskBat1);
		}
		break;
		case S_MSYS_CFG_SECT:											/*ϵͳ���ö�*/

		break;

		case S_COM_PARA_SECT:											/*ͨѶ������*/
	

		break;
		case S_HISTORY_RECORD_SECT:								/*��ʷ��¼��*/
		break;
		case S_USART_TEST_SECT:										/*���ڲ��Զ�*/
		break;
		default:
		break;
	}
	return ret;
 }

 /*************************************************************************************
**  void DealScreenCmd(uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum,uint8 *TxBuf) 
**   ����: ���������ص�����
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
 void DealScreenCmd(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum) 				/*���������ص�����*/
 {
 	uint16 cmdaddr;
	uint16 returnvalue;
	float FloatReturnValue;
	uint8 i;
	uint8 TempNO;
	uint16 batgroup;
	uint8 tmpbuf[4];
	tmpbuf[0] = buf[8];
	tmpbuf[1] = buf[7];
	tmpbuf[2] = buf[10];
	tmpbuf[3] = buf[9];

	cmdaddr = (uint16)(buf[4]<<8) + buf[5];
	returnvalue = (uint16)(buf[7]<<8) + buf[8];
	FloatReturnValue = *((float *)&tmpbuf[0]);
    JdyTest.Lcd.PageOperateTimeFlag = JdyTest.g_TimerMs;//��¼���һ�β���ʱ��
	OpenLcdLed(com);													//�������в������򿪱����
		switch(cmdaddr)
		{



			default:
			
			break;
		}
 }

/*************************************************************************************
**  void LcdProcess(uint8 com)
**   ����: LCD��ʾ�������
**   ����:   
**   ���:  
**   ����: 
**************************************************************************************/
void LcdProcess(uint8 com)
{
	uint8 rcvLen = 0, ret = LCD_RCV_UNDONE;
	uint8 rcvtmpbuf[MAX_UART_BUF_LENTH];	
	rcvLen = Uart_Read(UartHandler[com], UartRcvCfg[com].p_head, UartRcvCfg[com].rcvbuf+LCD_RCV_QUEUE_SIZE-UartRcvCfg[com].p_head);
	if(rcvLen > 0)                                                /* ���յ������� */
	{
		UartRcvCfg[com].FailCount = 0;
		UartRcvCfg[com].p_head += rcvLen;
		if(UartRcvCfg[com].p_head >= (UartRcvCfg[com].rcvbuf + LCD_RCV_QUEUE_SIZE)) 
		{
			UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;      /* ѭ�����н��� */
		}
		/* �ж��Ƿ������� */
		if(RCV_OK == LcdCheckRcvDone(&UartRcvCfg[com],rcvtmpbuf))
		{
			ret = CheckRcvLcdData(com,rcvtmpbuf);              /* У���봦������ */
			switch(ret)
			{
				case LCD_RCV_UNDONE:
					break;

				case LCD_RCV_OK:
					break;

				case LCD_RCV_COMM_ADD_ERR:
				case LCD_RCV_CRC_ERR:
				case LCD_RCV_FUCTION_ERR:
				case LCD_RCV_LEN_ERR:
				case LCD_RCV_REG_DATA_SAVE_ERR:
				case LCD_RCV_REG_DATA_READ_ERR:
				case LCD_RCV_REG_ADD_ERR:
				case LCD_RCV_REG_DATA_ERR:
					if(rcvtmpbuf[0] != BROADCAST_ADDR)            /* �㲥����ظ� */
					{
						LcdErrRespond(); /* �ظ�modbus������ */
					}
					break;

				default:
					break;
			}
		}
	}
	else
	{
		if(++UartRcvCfg[com].FailCount > DEVCOMM_FAIL_NUM)
		{
			UartRcvCfg[com].FailCount = 0;
			UartRcvCfg[com].p_tail = UartRcvCfg[com].p_head;      /* ͨѶ��ʱ,��λ����ָ�� */
		}
	}
/*************************��ǰҳ���������******************************/
   	PackedAndSendData(com, JdyTest.DataYk.PictureNO, CMD_WRITE_DATA); 
/*************���ͼƬ�б䶯���л�ͼƬ,��ʱ�䲻�����ص�������******************/
	PictureSwitch(com);
}


