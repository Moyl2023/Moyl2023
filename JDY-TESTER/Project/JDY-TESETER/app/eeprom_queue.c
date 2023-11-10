/****************************************Copyright (c)**************************************************

********************************************************************************************************/

//#define IN_QUEUE
#include "eeprom_queue.h"
#include "config.h"
#include "i2c.h"

/*********************************************************************************************************
** ��������: QueueCreate
** ��������: ��ʼ�����ݶ���
** �䡡��: Buf      ��Ϊ���з���Ĵ洢�ռ��ַ
**         SizeOfBuf��Ϊ���з���Ĵ洢�ռ��С���ֽڣ�
**			dataSize: ����ÿ�δ������ݴ�С ADD
**         ReadEmpty��Ϊ���ж���ʱ�������
**         WriteFull��Ϊ����д��ʱ�������
** �䡡��: NOT_OK:��������
**         QUEUE_OK:�ɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(EDataQueue)))        /* �жϲ����Ƿ���Ч */
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

        Queue->DataSize = dataSize;                            /* ��ʼ���ṹ������ */
        Queue->MaxData = (SizeOfBuf - sizeof(EDataQueue))/Queue->DataSize; /* ������п��Դ洢��������Ŀ */
      	Queue->End = addr + SizeOfBuf;//Queue->MaxData*Queue->DataSize;       /* �������ݻ���Ľ�����ַ */
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
** ��������: QueueRead
** ��������: ��ȡ�����е�����
** �䡡��: Ret:�洢���ص���Ϣ�ĵ�ַ
**         Buf:ָ����е�ָ��
** �䡡��: NOT_OK     ����������
**         QUEUE_OK   ���յ���Ϣ
**         QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        { 
				                                                     /* ����         */
            //memcpy(Ret,Queue->Out,Queue->DataSize);				/* ���ݳ���     */
						Eeprom_Read(Ret, Queue->Out, Queue->DataSize);

			//*Ret = Queue->Out[0];                               /* ���ݳ���     */
            Queue->Out = Queue->Out+Queue->DataSize;            /* ��������ָ�� */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* �����û������� */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }

			Eeprom_Write((uint8*)Queue, Queue->EepAddrOffset, sizeof(EDataQueue));

 	    OS_EXIT_CRITICAL();

    }
    return err;
}


//��ȡ�������ݣ����ı����λ��
uint8 EQueueReadNoOut(uint8 *Ret, VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err = NOT_OK;
    EDataQueue *Queue;

    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        { 
			Eeprom_Read(Ret, Queue->Out, Queue->DataSize);
			err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* �����û������� */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }
 	    OS_EXIT_CRITICAL();
    }
    return err;
}

//��ȡ������������λ�õ����ݣ����ı����λ��
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
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (EDataQueue *)Buf;
//       	if(index > Queue->In)//����������Χ
//			return err;
		if(Queue->UpMaxData < Queue->MaxData)//����δѭ����
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

//���ı����λ��
uint8 ChangeQueueOutPos(VOID *Buf)
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif  
    uint8 err = NOT_OK;
    EDataQueue *Queue;

    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();	
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        { 
            Queue->Out = Queue->Out+Queue->DataSize;            /* ��������ָ�� */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
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
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (EDataQueue *)Buf;
        
        Queue->Out = Queue->Out+Queue->DataSize;            /* ��������ָ�� */
        if (Queue->Out >= Queue->End)
        {
            Queue->Out = Queue->Buf;
        }
        Queue->NData--;                                     /* ���ݼ���      */

        Queue = (EDataQueue *)Buf;

				Eeprom_Write(Data, Queue->In, Queue->DataSize);
        Queue->In =Queue->In+Queue->DataSize;                        /* �������ָ��*/
        if (Queue->In >= Queue->End)
        {
            Queue->In = Queue->Buf;
        }
        Queue->NData++;                                             /* ��������    */

        err = QUEUE_OK;
    }
    return err;
}


/*********************************************************************************************************
** ��������: QueueWrite
** ��������: FIFO��ʽ��������
** �䡡��: Buf :ָ����е�ָ��
**         Data:��Ϣ����
** �䡡��: NOT_OK   :��������
**         QUEUE_FULL:������
**         QUEUE_OK  :���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (EDataQueue *)Buf;
        
        OS_ENTER_CRITICAL();

        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ����        */
            //memcpy(Queue->In,Data,Queue->DataSize);	/* �������    */
						Eeprom_Write(Data, Queue->In, Queue->DataSize);
			//Queue->In[0] = Data;                                        /* �������    */
            Queue->In =(Queue->In)+(Queue->DataSize);                        /* �������ָ��*/
            if (Queue->In >= Queue->End)
            {
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* ��������    */
			if(Queue->UpMaxData < Queue->MaxData)//��Queue->UpMaxData����Queue->MaxDataʱ���������ѭ��
				Queue->UpMaxData++;	
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
            {
				//Queue->UpMaxData = QUEUE_CIRC_FLAG;//��ѭ����־
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
** ��������: QueueNData
** ��������: ȡ�ö�����������
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ��Ϣ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: QueueSize
** ��������: ȡ�ö���������
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ����������
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
** ������: ������
** �ա���: 2003��7��2��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: OSQFlush
** ��������: ��ն���
** �䡡��: Buf:ָ����е�ָ��
** �䡡��: ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_FLUSH
#define EN_QUEUE_FLUSH    0
#endif

#if EN_QUEUE_FLUSH > 0

        void EQueueFlush(VOID *Buf)
{
    EEDataQueue *Queue;
    
    if (Buf != NULL)                                                /* �����Ƿ���Ч */
    {                                                               /* ��Ч         */
        Queue = (EDataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* ������ĿΪ0 */
        OS_EXIT_CRITICAL();
    }
}

#endif



/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

