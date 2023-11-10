/****************************************Copyright (c)**************************************************

********************************************************************************************************/

//#define IN_QUEUE
#include "eeprom_queue.h"
#include "config.h"
#include "i2c.h"

/*********************************************************************************************************
** 函数名称: QueueCreate
** 功能描述: 初始化数据队列
** 输　入: Buf      ：为队列分配的存储空间地址
**         SizeOfBuf：为队列分配的存储空间大小（字节）
**			dataSize: 队列每次处理数据大小 ADD
**         ReadEmpty：为队列读空时处理程序
**         WriteFull：为队列写满时处理程序
** 输　出: NOT_OK:参数错误
**         QUEUE_OK:成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------

********************************************************************************************************/
#define QUEUE_VALID_FLAG	11111115
#define QUEUE_CIRC_FLAG     0x22
uint8 EQueueCreate(				uint32 addr,
													void *Buf,
                          uint32 SizeOfBuf,
						  						uint16 dataSize,
                          uint8 (* ReadEmpty)(),
                          uint8 (* WriteFull)()
                          )
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    EDataQueue *Queue;
		uint8 ret = NOT_OK;
		EDataQueue tmpData;
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(EDataQueue)))        /* 判断参数是否有效 */
    {
        Queue = (EDataQueue *)Buf;

        OS_ENTER_CRITICAL();

			 	if(Eeprom_Read((uint8*)Queue, addr, sizeof(EDataQueue)) == TRUE)
				{
						if(Queue->validflag == QUEUE_VALID_FLAG)
						{
							Queue->WriteFull = WriteFull;
							OS_EXIT_CRITICAL();
							return QUEUE_OK;
						}
				}

				Queue->validflag = QUEUE_VALID_FLAG;
				Queue->EepAddrOffset = addr;

        Queue->DataSize = dataSize;                            /* 初始化结构体数据 */
        Queue->MaxData = (SizeOfBuf - sizeof(EDataQueue))/Queue->DataSize; /* 计算队列可以存储的数据数目 */
      	Queue->End = addr + SizeOfBuf;//Queue->MaxData*Queue->DataSize;       /* 计算数据缓冲的结束地址 */
		Queue->Buf = addr+sizeof(EDataQueue);
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;
		Queue->UpMaxData = 0;
        Queue->ReadEmpty = ReadEmpty;
        Queue->WriteFull = WriteFull;	 

			if(Eeprom_Write((uint8*)Queue, addr, sizeof(EDataQueue)) == TRUE)
			{
			  
				if(Eeprom_Read((uint8*)&tmpData, addr, sizeof(EDataQueue)) == TRUE)
			 	{
				   if(memcmp(&tmpData, Queue, sizeof(EDataQueue)) == 0)
				   {
						 ret = QUEUE_OK;
				   }	
				}
			}	

        OS_EXIT_CRITICAL();

        return ret;
    }
    else
    {
        return NOT_OK;
    }
}


/*********************************************************************************************************
** 函数名称: QueueRead
** 功能描述: 获取队列中的数据
** 输　入: Ret:存储返回的消息的地址
**         Buf:指向队列的指针
** 输　出: NOT_OK     ：参数错误
**         QUEUE_OK   ：收到消息
**         QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 EQueueRead(uint8 *Ret, VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err;
    EDataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* 队列是否为空 */
        { 
				                                                     /* 不空         */
            //memcpy(Ret,Queue->Out,Queue->DataSize);				/* 数据出队     */
						Eeprom_Read(Ret, Queue->Out, Queue->DataSize);

			//*Ret = Queue->Out[0];                               /* 数据出队     */
            Queue->Out = Queue->Out+Queue->DataSize;            /* 调整出队指针 */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* 数据减少      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* 空              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* 调用用户处理函数 */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }

			Eeprom_Write((uint8*)Queue, Queue->EepAddrOffset, sizeof(EDataQueue));

 	    OS_EXIT_CRITICAL();

    }
    return err;
}


//读取出队数据，不改变出对位置
uint8 EQueueReadNoOut(uint8 *Ret, VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err = NOT_OK;
    EDataQueue *Queue;

    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* 队列是否为空 */
        { 
			Eeprom_Read(Ret, Queue->Out, Queue->DataSize);
			err = QUEUE_OK;
        }
        else
        {                                                       /* 空              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* 调用用户处理函数 */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }
 	    OS_EXIT_CRITICAL();
    }
    return err;
}

//读取出队列中索引位置的数据，不改变出队位置
uint8 EQueueReadByIndex(uint32 index,uint32 num,uint8 *Ret, VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err = NOT_OK;
	uint32 addr;
    EDataQueue *Queue;

	if(num <= 0)
		return err;
    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (EDataQueue *)Buf;
//       	if(index > Queue->In)//索引超出范围
//			return err;
		if(Queue->UpMaxData < Queue->MaxData)//队列未循环过
		{
			if((Queue->In - ((index/* + num-1*/)*Queue->DataSize))>=Queue->Buf)	
			{
				addr = Queue->In - index*(Queue->DataSize);
		        OS_ENTER_CRITICAL();	
		        { 
					Eeprom_Read(Ret, addr, Queue->DataSize);
					err = QUEUE_OK;
		        }
		 	    OS_EXIT_CRITICAL();			
			}
		}
		else
		{
			if((Queue->In - ((index/* + num-1*/)*Queue->DataSize))>=Queue->Buf)
			{
				addr = Queue->In - index*(Queue->DataSize);
		        OS_ENTER_CRITICAL();	
		        { 
					Eeprom_Read(Ret, addr, Queue->DataSize);
					err = QUEUE_OK;
		        }
		 	    OS_EXIT_CRITICAL();				
			}
			else
			{
				addr = Queue->End - (index*(Queue->DataSize) - (Queue->In - Queue->Buf));
		        OS_ENTER_CRITICAL();	
		        { 
					Eeprom_Read(Ret, addr, Queue->DataSize);
					err = QUEUE_OK;
		        }
		 	    OS_EXIT_CRITICAL();	
			}
		}

    }
    return err;
}

//仅改变出对位置
uint8 ChangeQueueOutPos(VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err = NOT_OK;
    EDataQueue *Queue;

    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* 队列是否为空 */
        { 
            Queue->Out = Queue->Out+Queue->DataSize;            /* 调整出队指针 */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* 数据减少      */
			Eeprom_Write((uint8*)Queue, Queue->EepAddrOffset, sizeof(EDataQueue));
            err = QUEUE_OK;
        }

 	    OS_EXIT_CRITICAL();
    }
		return err;
}




uint8 WriteFull(VOID *Buf, uint8 *Data)
{
    uint8 err;
    EDataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (EDataQueue *)Buf;
        
        Queue->Out = Queue->Out+Queue->DataSize;            /* 调整出队指针 */
        if (Queue->Out >= Queue->End)
        {
            Queue->Out = Queue->Buf;
        }
        Queue->NData--;                                     /* 数据减少      */

        Queue = (EDataQueue *)Buf;

				Eeprom_Write(Data, Queue->In, Queue->DataSize);
        Queue->In =Queue->In+Queue->DataSize;                        /* 调整入队指针*/
        if (Queue->In >= Queue->End)
        {
            Queue->In = Queue->Buf;
        }
        Queue->NData++;                                             /* 数据增加    */

        err = QUEUE_OK;
    }
    return err;
}


/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: FIFO方式发送数据
** 输　入: Buf :指向队列的指针
**         Data:消息数据
** 输　出: NOT_OK   :参数错误
**         QUEUE_FULL:队列满
**         QUEUE_OK  :发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE
#define EN_QUEUE_WRITE      0
#endif

#if EN_QUEUE_WRITE > 0

uint8 EQueueWrite(VOID *Buf,uint8* Data)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err;
    EDataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* 队列是否有效 */
    {
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();

        if (Queue->NData < Queue->MaxData)                              /* 队列是否满  */
        {                                                               /* 不满        */
            //memcpy(Queue->In,Data,Queue->DataSize);	/* 数据入队    */
						Eeprom_Write(Data, Queue->In, Queue->DataSize);
			//Queue->In[0] = Data;                                        /* 数据入队    */
            Queue->In =(Queue->In)+(Queue->DataSize);                        /* 调整入队指针*/
            if (Queue->In >= Queue->End)
            {
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* 数据增加    */
			if(Queue->UpMaxData < Queue->MaxData)//当Queue->UpMaxData大于Queue->MaxData时，则队列已循环
				Queue->UpMaxData++;	
            err = QUEUE_OK;
        }
        else
        {                                                               /* 满           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* 调用用户处理函数 */
            {
				//Queue->UpMaxData = QUEUE_CIRC_FLAG;//置循环标志
                err = Queue->WriteFull(Queue, Data, Q_WRITE_MODE);
            }
        }
				Eeprom_Write((uint8*)Queue, Queue->EepAddrOffset, sizeof(EDataQueue));
	    OS_EXIT_CRITICAL();

    }
    return err;
}
#endif



/*********************************************************************************************************
** 函数名称: QueueNData
** 功能描述: 取得队列中数据数
** 输　入: Buf:指向队列的指针
** 输　出: 消息数
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_NDATA
#define EN_QUEUE_NDATA    0
#endif

#if EN_QUEUE_NDATA > 0

uint16 EQueueNData(VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint16 temp;
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((EDataQueue *)Buf)->NData;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: QueueSize
** 功能描述: 取得队列总容量
** 输　入: Buf:指向队列的指针
** 输　出: 队列总容量
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
** 作　者: 陈明计
** 日　期: 2003年7月2日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_SIZE
#define EN_QUEUE_SIZE    0
#endif

#if EN_QUEUE_SIZE > 0

        uint16 EQueueSize(VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint16 temp;
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((EDataQueue *)Buf)->MaxData;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: OSQFlush
** 功能描述: 清空队列
** 输　入: Buf:指向队列的指针
** 输　出: 无
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_FLUSH
#define EN_QUEUE_FLUSH    0
#endif

#if EN_QUEUE_FLUSH > 0

        void EQueueFlush(VOID *Buf)
{
    EEDataQueue *Queue;
    
    if (Buf != NULL)                                                /* 队列是否有效 */
    {                                                               /* 有效         */
        Queue = (EDataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* 数据数目为0 */
        OS_EXIT_CRITICAL();
    }
}

#endif



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

