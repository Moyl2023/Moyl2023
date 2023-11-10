#include "config.h"
#include "Message.h"
#include "queue.h"

// 消息队列 
// =========  taskAppStart 消息队列 ================================
#define   APP_START_QUEUE_SIZE  20
void      *AppStartQueueTbl[APP_START_QUEUE_SIZE];
OS_EVENT  *AppStartQueue=NULL;
// =========  TaskUART1消息队列 ================================
#define   COM1_UART_QUEUE_SIZE  20
void      *COM1UARTQueueTbl[COM1_UART_QUEUE_SIZE];
OS_EVENT  *COM1UARTQueue=NULL;
// =========  TaskUART2消息队列 ================================
#define   COM2_UART_QUEUE_SIZE  20
void      *COM2UARTQueueTbl[COM2_UART_QUEUE_SIZE];
OS_EVENT  *COM2UARTQueue=NULL;
// =========  TaskUART3消息队列 ================================
#define   COM3_UART_QUEUE_SIZE  20
void      *COM3UARTQueueTbl[COM3_UART_QUEUE_SIZE];
OS_EVENT  *COM3UARTQueue=NULL;
// =========  TaskUART4消息队列 ================================
#define   COM4_UART_QUEUE_SIZE  20
void      *COM4UARTQueueTbl[COM4_UART_QUEUE_SIZE];
OS_EVENT  *COM4UARTQueue=NULL;

// =========  TaskLCD消息队列 ================================
#define   LCD_QUEUE_SIZE  20
void      *LCDQueueTbl[LCD_QUEUE_SIZE];
OS_EVENT  *LCDQueue=NULL;
// =========  TaskUSB消息队列 ================================
#define   USB_QUEUE_SIZE  20
void      *USBQueueTbl[LCD_QUEUE_SIZE];
OS_EVENT  *USBQueue=NULL;

#define MAX_MSG_NUM			64			//最大消息数
static 	MSG_STRUCT MsgArray[MAX_MSG_NUM];

 
BOOL  ATC_QueueCreat(uint8 TaskPrio)
{
	static uint8 poweron = 1;
	if(poweron==1)
	{
	 	memset(MsgArray,0,sizeof(MSG_STRUCT)*MAX_MSG_NUM);
		poweron=0;
	}
	
	switch(TaskPrio)
	{
		case APP_TASK_START_PRIO:
		{			
			AppStartQueue = OSQCreate(&AppStartQueueTbl[0], APP_START_QUEUE_SIZE);	
		}
		break;
		case APP_TASK_JDYTEST_PRIO:
		{			
			COM1UARTQueue = OSQCreate(&COM1UARTQueueTbl[0], COM1_UART_QUEUE_SIZE);	
		}
		break;
		case APP_TASK_LCD_PRIO:
		{			
			COM2UARTQueue = OSQCreate(&COM2UARTQueueTbl[0], COM2_UART_QUEUE_SIZE);	
		}
		break;
		case APP_TASK_UpdateCOM_PRIO:
		{			
			COM3UARTQueue = OSQCreate(&COM3UARTQueueTbl[0], COM3_UART_QUEUE_SIZE);	
		}
		break;
		default: 
			break;	
	}

	return TRUE;
}


// 动态分配消息 内存  将消息内容转存到内存 
uint8* Msg2Queue(MSG_STRUCT *msg)
{	
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0;
#endif    

	uint8 i = 3,j; 
	while(i--)
	{
		for(j=0;j<MAX_MSG_NUM;j++)
		{
			 if(MsgArray[j].Valid==0)
			 {
					OS_ENTER_CRITICAL();
					memcpy(&MsgArray[j],msg ,sizeof(MSG_STRUCT));
					MsgArray[j].Valid = 1;
					OS_EXIT_CRITICAL();

					return 	(uint8*)&MsgArray[j];
			 }
		}
		OSTimeDlyHMSM(0,0,0,10);	
	}																  
	return NULL;
}

// ============================================================================= 
// 发送消息msg到DestPrio任务的队列 
// 参数：msg 待发送的消息内容   DestPrio 待发送的目的任务优先级  
// 返回：TRUE 成功 FALSE 失败 
//==============================================================================
BOOL  SendMsg2Queue(MSG_STRUCT *msg, uint8 DestPrio)
{
	uint8* ptrMem = NULL;
	BOOL   ret = FALSE;
	if(msg == NULL)
		return ret;
	if((ptrMem = Msg2Queue(msg)) != NULL)
	{
		switch(DestPrio)
		{
			case APP_TASK_START_PRIO:
			{			
				OSQPost(AppStartQueue, (void*)ptrMem);
				ret = TRUE;	
			}	
			break;	
			case APP_TASK_JDYTEST_PRIO:
			{			
				OSQPost(COM1UARTQueue, (void*)ptrMem);
				ret = TRUE;	
			}	
			break;
			case APP_TASK_LCD_PRIO:
			{			
				OSQPost(COM2UARTQueue, (void*)ptrMem);
				ret = TRUE;	
			}	
			break;		
			case APP_TASK_UpdateCOM_PRIO:
			{			
				OSQPost(COM3UARTQueue, (void*)ptrMem);
				ret = TRUE;	
			}	
			break;
			default: 
			break;	
		}
	}				
	return ret;
}
// ============================================================================= 
// 接收消息  
// 无  
// 返回：msg 接收消息的指针 FALSE 失败 
//==============================================================================
BOOL RcvMsgFromQueue(MSG_STRUCT* outmsg)
{
	
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0;
#endif   

	BOOL ret = FALSE;
	MSG_STRUCT* msg = NULL;
	uint8 CurrentPrio, err;

	CurrentPrio = OSTCBCur->OSTCBPrio;
	switch(CurrentPrio)
	{
		case APP_TASK_START_PRIO:
		{			
			 msg = (MSG_STRUCT*)OSQAccept(AppStartQueue, &err);	 
		}
		break;
		case APP_TASK_JDYTEST_PRIO:
		{			
			msg = (MSG_STRUCT*)OSQAccept(COM1UARTQueue, &err);	
		}
		break;
		case APP_TASK_LCD_PRIO:
		{			
			msg = (MSG_STRUCT*)OSQAccept(COM2UARTQueue, &err);	
		}	
		break;
		case APP_TASK_UpdateCOM_PRIO:
		{			
			msg = (MSG_STRUCT*)OSQAccept(COM3UARTQueue, &err);	
		}	
		break;		
		default: 
			break;	
	}

	if(msg!=NULL)
	{	 
		OS_ENTER_CRITICAL();
		memcpy(outmsg,msg,sizeof(MSG_STRUCT));
		msg->Valid = 0;
		OS_EXIT_CRITICAL();
		ret = TRUE;
	}
	return ret;
}

void ClearMsgQueue(uint8 TaskPrio)
{
	MSG_STRUCT* msg = NULL;
	uint8 err;
	switch(TaskPrio)
	{
		case APP_TASK_START_PRIO:
			do
			{
				msg = (MSG_STRUCT*)OSQAccept(AppStartQueue, &err);
				if(msg!=NULL)
				{	 
					msg->Valid = 0;
				}
			}while(msg!=NULL);
			break;

		case APP_TASK_JDYTEST_PRIO:
			do
			{
				msg = (MSG_STRUCT*)OSQAccept(COM1UARTQueue, &err);
				if(msg!=NULL)
				{	 
					msg->Valid = 0;
				}
			}while(msg!=NULL);
			break;
		case APP_TASK_UpdateCOM_PRIO:
			do
			{
				msg = (MSG_STRUCT*)OSQAccept(COM2UARTQueue, &err);
				if(msg!=NULL)
				{	 
					msg->Valid = 0;
				}
			}while(msg!=NULL);
			break;
		case APP_TASK_LCD_PRIO:
			do
			{
				msg = (MSG_STRUCT*)OSQAccept(LCDQueue, &err);
				if(msg!=NULL)
				{	 
					msg->Valid = 0;
				}
			}while(msg!=NULL);
			break;
		default:
			break;
	}
}


void DelQueue(uint8 com)
{
	switch(com)
	{
		case APP_TASK_JDYTEST_PRIO:
			OSQDel(COM1UARTQueue,OS_DEL_ALWAYS,OS_ERR_NONE);
		break;
		case APP_TASK_LCD_PRIO:
			OSQDel(COM2UARTQueue,OS_DEL_ALWAYS,OS_ERR_NONE);
		break;
		case APP_TASK_UpdateCOM_PRIO:
			OSQDel(COM3UARTQueue,OS_DEL_ALWAYS,OS_ERR_NONE);
		break;
		default:
			break;
	}
}

