/****************************************Copyright (c)**************************************************

**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef _EEPROM_QUEUE_H
#define _EEPROM_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif	
#include "config.h"
#ifndef NOT_OK
#define NOT_OK              0xff                        /* 参数错误                                     */
#endif

#define QUEUE_FULL          8                           /* 队列满                                       */
#define QUEUE_EMPTY         4                           /* 无数据                                       */
#define QUEUE_OK            1                           /* 操作成功                                     */

#define Q_WRITE_MODE        1                           /* 操作成功                                     */
#define Q_WRITE_FRONT_MODE  2                           /* 操作成功                                     */

//#endif

typedef struct {
    uint32    validflag;						  /* 队列有效标记*/
	uint32    EepAddrOffset;					/* 铁电基地址 */

    uint32     Out;                   /* 指向数据输出位置         */
    uint32     In;                    /* 指向数据输入位置         */
    uint32     End;                   /* 指向Buf的结束位置        */
    uint16    NData;                  /* 队列中数据个数           */
    uint16    MaxData;                /* 队列中允许存储的数据个数 */
    uint16	  DataSize;			      //处理队列元素大小
	uint8	  UpMaxData;			  //数据循环标志，0x22；表示已循环，即：指向数据输入位置In 重新回到起始位置，其他：未循环
    uint8     (* ReadEmpty)();        /* 读空处理函数             */
    uint8     (* WriteFull)();        /* 写满处理函数             */
    uint32     Buf;                 /* 存储数据的空间           */
} EDataQueue;



#ifndef IN_QUEUE



        uint8 EQueueCreate(uint32 addr,
													void *Buf,
                          uint32 SizeOfBuf,
						  						uint16 dataSize,
                          uint8 (* ReadEmpty)(),
                          uint8 (* WriteFull)()
                          );
/*********************************************************************************************************
** 函数名称: QueueCreate
** 功能描述: 初始化数据队列
** 输　入: Buf      ：为队列分配的存储空间地址
**         SizeOfBuf：为队列分配的存储空间大小（字节）
**         ReadEmpty：为队列读空时处理程序
**         WriteFull：为队列写满时处理程序
** 输　出: NOT_OK  :参数错误
**         QUEUE_OK:成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern uint8 EQueueRead(uint8 *Ret, void *Buf);
/*********************************************************************************************************
** 函数名称: QueueRead
** 功能描述: 获取队列中的数据
** 输　入: Ret:存储返回的消息的地址
**         Buf:指向队列的指针
** 输　出: NOT_OK     ：参数错误
**         QUEUE_OK   ：收到消息
**         QUEUE_EMPTY：队列空
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern uint8 EQueueWrite(void *Buf, uint8* Data);
/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: FIFO方式发送数据
** 输　入: Buf :指向队列的指针
**         Data:发送的数据
** 输　出: NOT_OK   ：参数错误
**         QUEUE_FULL:队列满
**         QUEUE_OK  :发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern uint8 EQueueWriteFront(void *Buf, uint8* Data);
/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: LIFO方式发送数据
** 输　入: Buf:指向队列的指针
**         Data:消息数据
** 输　出: QUEUE_FULL:队列满
**         QUEUE_OK:发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern uint16 EQueueNData(void *Buf);
/*********************************************************************************************************
** 函数名称: QueueNData
** 功能描述: 取得队列中数据数
** 输　入: Buf:指向队列的指针
** 输　出: 消息数
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern uint16 EQueueSize(void *Buf);
/*********************************************************************************************************
** 函数名称: QueueSize
** 功能描述: 取得队列总容量
** 输　入: Buf:指向队列的指针
** 输　出: 队列总容量
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/

extern void EQueueFlush(void *Buf);
/*********************************************************************************************************
** 函数名称: OSQFlush
** 功能描述: 清空队列
** 输　入: Buf:指向队列的指针
** 输　出: 无
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
extern uint8 WriteFull(VOID *Buf, uint8 *Data);

//读取出队数据，不改变出对位置
extern uint8 EQueueReadNoOut(uint8 *Ret, VOID *Buf);
extern uint8 EQueueReadByIndex(uint32 index,uint32 num,uint8 *Ret, VOID *Buf);
//仅改变出对位置
extern uint8 ChangeQueueOutPos(VOID *Buf);

//rm add start
//中断中写操作
extern uint8 EQueueWriteInInt(void *Buf, uint8* Data);


//中断中读操作
extern uint8 EQueueReadInInt(uint8 *Ret, void *Buf);
//rm add end

#endif



#define EN_QUEUE_WRITE            1     /* 禁止(0)或允许(1)FIFO发送数据       */
//#define EN_QUEUE_WRITE_FRONT      0     /* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_QUEUE_NDATA            1     /* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_QUEUE_SIZE             1     /* 禁止(0)或允许(1)取得队列数据总容量 */
//#define EN_QUEUE_FLUSH            0     /* 禁止(0)或允许(1)清空队列           */

#ifdef __cplusplus
}
#endif	

#endif /*end _QUEUE_H*/

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

