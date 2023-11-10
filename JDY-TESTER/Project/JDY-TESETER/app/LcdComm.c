#include <includes.h>




const uint8 NoDev[]={"没有模块!"};
const uint8 StartRMea[]={"      启动内阻巡检!      "};
const uint8 StartRMea1[]={"非浮充状态,启动内阻巡检! "};
const uint8 StopRMea[]={"      停止内阻巡检!       "};
const uint8 SaveSuccess[]={"保存成功!"};
const uint8 SaveFail[]={"保存失败!"};
const uint8 SetSuccess[]={"设定成功!"};
const uint8 SetFail[]={"设定失败!"};
const uint8 NoTestRecord[]={"没有测量记录!"};
const uint8 SaveParaRecord[]={"         保存配置!         "};


#define MAIN_PICTURE_CHANGE_TIME		2000000

uint16 ErrBatNOAddr[5] = {0x0810,0x0840,0x0870,0x08A0,0x08D0};//故障数据电池编号在LCD中的地址
uint16 ErrTypeAddr[5] = {0x0820,0x0850,0x0880,0x08B0,0x08E0};//故障数据故障类型在LCD中的地址
uint16 ErrDataAddr[5] = {0x0830,0x0860,0x0890,0x08C0,0x08F0};//故障数据故障数据在LCD中的地址



/*************************************************************************************
**  uint16 Big2Samll(uint16 temp) 
**   功能: 大小端模式变换
**   参数:   
**   输出:  
**   描述: 
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
**   功能: 切换界面
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 SwitchToPicX(uint8 com,uint8 cmd, uint16 pageN) // 切换到某界面
{ 
   uint8 length = 0; 
   uint8 Txbuf[20];
   
   Txbuf[length++] = FRAME_HIGH; 
   Txbuf[length++] = FRAME_LOW; 
   Txbuf[length++] = 0;      // length
   Txbuf[length++] = cmd;    // cmd
   Txbuf[length++] = 0x03;   // register add
   
   pageN = Big2Samll(pageN); 
   memcpy(&Txbuf[length], &pageN, sizeof(pageN));    //图片序号
   length += sizeof(pageN); 

	 CalAndSendData(com,Txbuf,length);
   return length;
}


/*************************************************************************************
**  uint8 ReadPicID(uint8 com)
**   功能: 切换界面
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
/***********************************************************/
uint8 ReadPicID(uint8 com) // 切换到某界面
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
**   功能: 发送时间到LCD
**   参数:   通信端口号
**   输出:  
**   描述: 
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
**   功能: 关闭背光灯
**   参数:   
**   输出:  
**   描述: 
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
**   功能: 打开背光灯
**   参数:   
**   输出:  
**   描述: 
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
**   功能: 填充数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 WriteDataToPicture0(uint8 com)
{

	return 1;
}
/*************************************************************************************
**  uint8 WriteDataToPicture1(uint8 com)
**   功能: 填充数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 WriteDataToPicture1(uint8 com)
{
		uint8 length = 0;

	return length;
}

/*************************************************************************************
**  uint8 WriteDataToPicture2(uint8 com)
**   功能: 填充数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 WriteDataToPicture2(uint8 com)
{
	uint8 length = 0;

	
	return length;
}
/*************************************************************************************
**  uint8 WriteDataToPicture3(uint8 com)
**   功能: 填充数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 WriteDataToPicture3(uint8 com)
{
	uint8 length = 0;

	return length;
}

/*************************************************************************************
**  uint8 ClrPicture135Data(uint8 com)
**   功能: 清除135图片的部分参数
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 ClrPicture151Data(uint8 com)
{



	return 1;
}
/*************************************************************************************
**  uint8 PictureSwitch(uint8 com)
**   功能: 切换图片，如果长时间没有操作液晶屏，则回到常态界面
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void PictureSwitch(uint8 com)
{
/************************ 长时间不操作回到主界面 ***********************************/	
	if((JdyTest.g_TimerMs - JdyTest.Lcd.PageOperateTimeFlag > MAIN_PICTURE_CHANGE_TIME))
	{//非放电状态，15分钟不操作回到主界面，17分钟不操作，变暗
		JdyTest.DataYk.PictureNO = PICTURE1;//一组蓄电池常态界面

		if(JdyTest.g_TimerMs - JdyTest.Lcd.PageOperateTimeFlag > (MAIN_PICTURE_CHANGE_TIME+120000))
		{
			CloseLcdLed(com);		
		}
	}	
	else
	{
		OpenLcdLed(com);
	}
	if(PackedAndSendData(com, JdyTest.DataYk.PictureNO, CMD_SWITCH_PIC)==TRUE)	// 切换图片
	{
		if(JdyTest.DataYk.OldPictureNO != JdyTest.DataYk.PictureNO)
		{
			//LcdComm.BatGroupNO = 0;
			//LcdComm.DevNO = 0;
		}
	}
	OSTimeDly(100); //1000个时钟节拍是250ms，这里需要延时，否则不能界面切换的时候填充数据
}
/*************************************************************************************
**  static uint8 LcdGetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
**   功能: 计算已接收数据包长度
**   参数: buf--数据接收完成后缓存区 
**   输出: ret-接收完成标志    buf-数据接收缓存起始地址 
**   描述: 
**************************************************************************************/
static uint8 LcdGetRcvLen(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8 i, len, *ptr, ret = LCD_RCV_UNDONE;
	
	if(pchan->p_head >= pchan->p_tail)
	{

		len = pchan->p_head - pchan->p_tail;           /* 已接收的数据长度 */
	}
	else
	{

		len = pchan->p_head + LCD_RCV_QUEUE_SIZE - pchan->p_tail;
	}
	if(len >= 11)                                       /* 一帧数据最小长度为11个字节 */                                       
	{
		ptr = pchan->p_tail;                     
		for(i = 0; i < 11; i++)                         /*数据转入处理队列*/
		{
			buf[i] = *ptr;
			if(++ptr >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
			{
				ptr = pchan->rcvbuf;                   /* 循环队列 */
			}
		}
		if(len >= (buf[2]+3))
		{
			for(i=11;i<buf[2]+3;i++)
			{
				buf[i] = *ptr;
				if(++ptr >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
				{
					ptr = pchan->rcvbuf;                   /* 循环队列 */
				}
			}
		}
		else
		{
			return ret;
		}
		pchan->p_tail = ptr;       
		ret = LCD_RCV_OK;                              /*数据包接收完成*/  
	}
	return ret;
}



/*************************************************************************************
**  static void LcdFixReadData(uint8 *src,uint8 *buf,uint16 StartAddr,uint8 Lenth)
**   功能:  接收数据存储
**   参数:  src--modbus中的段地址   buf--需存储的数据起始地址  startAddr--段地址中的偏移地址
**   输出:  
**   描述: 
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
**   功能:  接收数据存储
**   参数:  src--段地址   buf--需存储的数据起始地址  startAddr--段地址中的偏移地址
			Lenth--数据长度
**   输出:  
**   描述: 
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
**   功能:  接收数据存储
**   参数:  src--段地址   buf--需存储的数据起始地址  startAddr--段地址中的偏移地址
			Lenth--数据长度
**   输出:  
**   描述: 
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
**   功能: 校验数据并发送
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
 uint8 CalAndSendData(uint8 com,uint8 *txbuf,uint8 txlen)
 {
	uint16 CRCValue = 0; 
	txbuf[2] =  txlen - 3;                     // 填充数据长度
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
**   功能: 打包发送数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 PackedAndSendData(uint8 com,uint16 pageno, uint8 cmd)
{
   if (cmd == CMD_SWITCH_PIC)  
   {
      /*********************切换图片*********************/
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
**   功能: 屏通信协议解析
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 LcdProtocolAnalysis(uint8 com,uint8 *buf,uint8 SectNo,uint8 addrOffset,uint16 regNum)
{
	uint8 ret = RCV_OK;
	//uint8 txlen = 0, ret = RCV_OK, dataOffset = 4;  /*写单寄存器命令，填充数据在MODBUS数据包中的偏移地址*/
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
				DealScreenCmd(com,buf,SectNo,addrOffset,regNum);				/*处理按键返回的数据*/
			}
			else
			{
				DealScreenData(com,buf,SectNo,addrOffset,regNum); 				/*处理输入控件返回的数据*/
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
**   功能: 查找数据段
**   参数:  
**   输出:  
**   描述: 
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
**   功能: 校验数据包
**   参数:   
**   输出:  
**   描述: 
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
				addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*在段中的偏移地址*/	
				ret = LcdProtocolAnalysis(com,buf,SectNo,addrOffset,RegNum);             /*数据包命令判断,接收正确后应答*/
			}
	}
	return ret;
}
/************************下面程序增加有页面读取功能********************************/
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
				addrOffset = (StartAddr - SSectAddrAndData[SectNo].LowRegAddr)*2;   /*在段中的偏移地址*/	
				ret = LcdProtocolAnalysis(com,buf,SectNo,addrOffset,RegNum);             /*数据包命令判断,接收正确后应答*/
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
**   功能:检验是否接收完成
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
uint8 LcdCheckRcvDone(UART_RCV_CFG *pchan, uint8 *buf)
{
	uint8  ret = LCD_RCV_UNDONE;
	int16 temp;
	while(pchan->p_tail != pchan->p_head)                                         /*有新数据入队*/
	{
		temp = pchan->p_head - pchan->p_tail;
		if(temp < 0)
		{
			temp = pchan->p_head + LCD_RCV_QUEUE_SIZE - pchan->p_tail;
		}
		if(temp >= 2)
		{
			if(((*(pchan->p_tail) == FRAME_HIGH) && (*(pchan->p_tail + 1) == FRAME_LOW) )|| (*(pchan->p_tail) == BROADCAST_ADDR))/*设备地址正确,开始接收*/
			{
				ret = LcdGetRcvLen(pchan,buf);                                           /*判断一帧数据是否接收完*/
				break;
			}
			else
			{
				if(++(pchan->p_tail) >= (pchan->rcvbuf+LCD_RCV_QUEUE_SIZE)) 
				{
					pchan->p_tail = pchan->rcvbuf;                                     /* 循环队列 */
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
**   功能: 数据错误响应
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void LcdErrRespond(void)
{

}
/*************************************************************************************
**   void DealScreenData(uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum,uint8 *TxBuf)
**   功能: 处理输入控件返回的数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
 uint8 DealScreenData(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum) 				/*处理输入控件返回的数据*/
{
	uint8 dataOffset;
	uint8 ret = RCV_OK;
	MSG_STRUCT msg;
	//uint16 temp;
	uint8 i,DEVCOM;
	uint8 recom;//需要重新配置的通信端口号
	uint32 address,bkpaddress;
	uint16 batgroup;
	struct tm ttm;	
	
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  

	dataOffset = 7;
	
	switch(SectNo)
	{
		case S_BAT_V_DATA_SECT:										/*电池电压数据段*/
		break;
		case S_BAT_R_DATA_SECT:										/*电池内阻数段*/
		break;
		case S_CONNECT_R_DATA_SECT:								/*连接电阻数据段*/
		break;
		case S_BAT_TEMP_DATA_SECT:									/*电池温度数据段*/
		break;
		case S_BAT_ERR_STAT_SECT:									/*电池故障状态段*/
		break;
		case S_DEV_STAT_SECT:											/*模块运行状态段*/
		break;
		case S_MEA_DATA_SECT:											/*测量数据段*/
		break;
		case S_CAL_PARA_SECT:											/*校准参数段*/
		{
			//LcdComm.ReadParaFlag = ON;//保存之后读取一遍
			//LcdFixReadData((uint8*)&MbsSlave.SysPara.CalPara[batgroup][LcdComm.DevNO].S_CellVoltageB, buf+dataOffset, addrOffset, regNum*2);
			msg.MsgType = MSG_WR_REG;
			msg.MsgLenth = 1;
			msg.MsgCOM = DEVCOM;
			msg.MsgDevAddr = 0;
			msg.MsgData[0] = MODBUS_WR_MULT_REG;
			msg.MsgData[1] = (uint8)(CAL_PARA_ADDR>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
			msg.MsgData[2] = (uint8)(CAL_PARA_ADDR & 0xFF);
			msg.MsgData[3] = regNum;
			for(i=0;i<regNum;i++)
			{
				msg.MsgData[i*2+4] = buf[i*2+dataOffset+1];//注意存储模式
				msg.MsgData[i*2+5] = buf[i*2+dataOffset];
			}
			
				//SendMsg2Queue(&msg,DevTask.TaskBat1);

		}
		break;
		case S_SYS_WORK_STAT_SECT:									/*系统工作状态段*/
		break;
		case S_ASSIST_STAT_SECT:									/*辅助状态段*/
		break;
		case S_SYS_CTRL_SECT:											/*系统控制段*/
		{
			//LcdComm.ReadParaFlag = ON;//保存之后读取一遍
			LcdFixReadData((uint8*)&MbsSlave.SysPara.SysCtrl[0].S_StartCellMea, buf+dataOffset, addrOffset, regNum*2);//装填数据
			msg.MsgType = MSG_WR_REG;
			msg.MsgLenth = 1;
			msg.MsgCOM = DEVCOM;
			msg.MsgDevAddr = BROADCAST_ADDR;
			msg.MsgData[0] = MODBUS_WR_SINGLE_REG;
			msg.MsgData[1] = (uint8)(SYS_CTRL_ADDR>>8) & 0x00FF;//注意:这里的地址要用MBS主机里面定义的地址
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
		case S_MSYS_CFG_SECT:											/*系统配置段*/

		break;

		case S_COM_PARA_SECT:											/*通讯参数段*/
	

		break;
		case S_HISTORY_RECORD_SECT:								/*历史记录段*/
		break;
		case S_USART_TEST_SECT:										/*串口测试段*/
		break;
		default:
		break;
	}
	return ret;
 }

 /*************************************************************************************
**  void DealScreenCmd(uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum,uint8 *TxBuf) 
**   功能: 处理按键返回的数据
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
 void DealScreenCmd(uint8 com,uint8 *buf,uint8 SectNo,uint16 addrOffset,uint16 regNum) 				/*处理按键返回的数据*/
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
    JdyTest.Lcd.PageOperateTimeFlag = JdyTest.g_TimerMs;//记录最近一次操作时间
	OpenLcdLed(com);													//对屏进行操作，打开背光灯
		switch(cmdaddr)
		{



			default:
			
			break;
		}
 }

/*************************************************************************************
**  void LcdProcess(uint8 com)
**   功能: LCD显示处理程序
**   参数:   
**   输出:  
**   描述: 
**************************************************************************************/
void LcdProcess(uint8 com)
{
	uint8 rcvLen = 0, ret = LCD_RCV_UNDONE;
	uint8 rcvtmpbuf[MAX_UART_BUF_LENTH];	
	rcvLen = Uart_Read(UartHandler[com], UartRcvCfg[com].p_head, UartRcvCfg[com].rcvbuf+LCD_RCV_QUEUE_SIZE-UartRcvCfg[com].p_head);
	if(rcvLen > 0)                                                /* 接收到新数据 */
	{
		UartRcvCfg[com].FailCount = 0;
		UartRcvCfg[com].p_head += rcvLen;
		if(UartRcvCfg[com].p_head >= (UartRcvCfg[com].rcvbuf + LCD_RCV_QUEUE_SIZE)) 
		{
			UartRcvCfg[com].p_head = UartRcvCfg[com].rcvbuf;      /* 循环队列接收 */
		}
		/* 判断是否接收完成 */
		if(RCV_OK == LcdCheckRcvDone(&UartRcvCfg[com],rcvtmpbuf))
		{
			ret = CheckRcvLcdData(com,rcvtmpbuf);              /* 校验与处理命令 */
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
					if(rcvtmpbuf[0] != BROADCAST_ADDR)            /* 广播命令不回复 */
					{
						LcdErrRespond(); /* 回复modbus错误码 */
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
			UartRcvCfg[com].p_tail = UartRcvCfg[com].p_head;      /* 通讯超时,复位接收指针 */
		}
	}
/*************************向当前页面填充数据******************************/
   	PackedAndSendData(com, JdyTest.DataYk.PictureNO, CMD_WRITE_DATA); 
/*************如果图片有变动就切换图片,长时间不操作回到主界面******************/
	PictureSwitch(com);
}


