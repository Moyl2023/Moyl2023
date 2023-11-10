#ifndef _MODBUSDEF_H
#define _MODBUSDEF_H

#ifdef  __cplusplus
extern  "C" {
#endif
#include "config.h"	 

/* 段区和段区中的数据集合定义 */
typedef struct
{
    uint16 LowRegAddr;                              /* 低寄存器地址 */
    uint16 HighRegAddr;                             /* 高寄存器地址 */
}SECT_ADDR_AND_DATA;

/* Function codes */
#define MODBUS_RD_COIL                  0x01        /* Address: 0XXXX */
#define MODBUS_RD_DISCRETE_IN           0x02        /* Address: 1XXXX */
#define MODBUS_RD_HOLD_REG              0x03        /* Address: 4XXXX */
#define MODBUS_RD_IN_REG                0x04        /* Address: 3XXXX */
#define MODBUS_WR_SINGLE_COIL           0x05        /* Address: 0XXXX */
#define MODBUS_WR_SINGLE_REG            0x06        /* Address: 4XXXX */

#define MODBUS_WR_MULT_COIL             0x0F        /* Address: 0XXXX */
#define MODBUS_WR_MULT_REG              0x10        /* Address: 4XXXX */
#define MODBUS_REPORT_SLAVE_ID          0x11 

#define MODBUS_RD_WR_MULT_REG           0x17
#define MODBUS_RD_FIFO                  0x18  

typedef enum
{
    RCV_UNDONE = 0,                                 /* 接收未完成 */
    RCV_FUCTION_ERR = 1,                            /* 接收命令码错误 */
    RCV_REG_ADD_ERR = 2,                            /* 接收地址错 */
    RCV_REG_NUM_ERR = 3,                            /* 接收数量错 */	
    RCV_REG_DATA_ERR = 4,                           /* 接收数据错 */	
    RCV_REG_DATA_SAVE_ERR = 5, 			/* 铁电存储错 */	
    RCV_REG_DATA_READ_ERR = 6, 			/* 铁电存储错 */
	
    RCV_COMM_ADD_ERR = 14,                          /* 接收通讯地址错 */
    RCV_LEN_ERR = 15,                               /* 接收长度错 */
    RCV_CRC_ERR = 16,                               /* 接收CRC错 */
    RCV_OK = 0xff,                                  /* 接收正常 */
	
}MODBUS_ERROR_CODE;
/*****************************************************************************************/
#define MODBUS_MAX_READ_REG         125             /*MODBUS读操作的最大寄存器数量*/
#define MODBUS_MAX_WRITE_REG        123             /*MODBUS写操作的最大寄存器数量*/
#define MODBUS_MAX_REG              123             /*MODBUS单笔数据操作的最大寄存器数量*/
#define MBSSLAVE_ADDRESS            0x01            /* 地址 */
#define MBSSLAVE_COMTIMEOUT         10000           /* 10s 通讯超时时间 */
#define COMM_BUF_SIZE               255             /* 通讯缓冲区大小 */
#define COMM_ERR_COUNT              9               /* 通讯失败计数 */
#define RESEND_DATA_COUNT           3			/* 重发指令次数 */


/*--------------------------------------------------------------------------------- */
//#define MODBUS_CRC_LEN              2
//#define MODBUS_QUERY_LEN            6
//#define MODBUS_HEADER_LEN           2               /* function + address */
//#define MODBUS_EXC_CODE             0x80            /* Exception code defines */ 

/* 超时设置 */
//#define MODBUS_OVERTIME             1000            /* 1 sec */
#define MODBUS_COMM_TIME_OUT        800             /* 通讯链路超时 */
#define XRD_MODBUS_COMM_TIME_OUT        1000             /* 通讯链路超时 */
#define RCV_OVER_TIME               20              /* 采集ModBus字节之间超时时间 */

//#define MODBUS_MASTER_SLAVE_OVERTIME	3000	/*3 sec*/

#define LOBYTE(w)		(w&0xFF)
#define HIBYTE(w)		((w>>8)&0xFF)	

#ifdef  __cplusplus
}
#endif
#endif

