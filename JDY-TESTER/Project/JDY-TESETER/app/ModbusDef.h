#ifndef _MODBUSDEF_H
#define _MODBUSDEF_H

#ifdef  __cplusplus
extern  "C" {
#endif
#include "config.h"	 

/* �����Ͷ����е����ݼ��϶��� */
typedef struct
{
    uint16 LowRegAddr;                              /* �ͼĴ�����ַ */
    uint16 HighRegAddr;                             /* �߼Ĵ�����ַ */
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
    RCV_UNDONE = 0,                                 /* ����δ��� */
    RCV_FUCTION_ERR = 1,                            /* ������������� */
    RCV_REG_ADD_ERR = 2,                            /* ���յ�ַ�� */
    RCV_REG_NUM_ERR = 3,                            /* ���������� */	
    RCV_REG_DATA_ERR = 4,                           /* �������ݴ� */	
    RCV_REG_DATA_SAVE_ERR = 5, 			/* ����洢�� */	
    RCV_REG_DATA_READ_ERR = 6, 			/* ����洢�� */
	
    RCV_COMM_ADD_ERR = 14,                          /* ����ͨѶ��ַ�� */
    RCV_LEN_ERR = 15,                               /* ���ճ��ȴ� */
    RCV_CRC_ERR = 16,                               /* ����CRC�� */
    RCV_OK = 0xff,                                  /* �������� */
	
}MODBUS_ERROR_CODE;
/*****************************************************************************************/
#define MODBUS_MAX_READ_REG         125             /*MODBUS�����������Ĵ�������*/
#define MODBUS_MAX_WRITE_REG        123             /*MODBUSд���������Ĵ�������*/
#define MODBUS_MAX_REG              123             /*MODBUS�������ݲ��������Ĵ�������*/
#define MBSSLAVE_ADDRESS            0x01            /* ��ַ */
#define MBSSLAVE_COMTIMEOUT         10000           /* 10s ͨѶ��ʱʱ�� */
#define COMM_BUF_SIZE               255             /* ͨѶ��������С */
#define COMM_ERR_COUNT              9               /* ͨѶʧ�ܼ��� */
#define RESEND_DATA_COUNT           3			/* �ط�ָ����� */


/*--------------------------------------------------------------------------------- */
//#define MODBUS_CRC_LEN              2
//#define MODBUS_QUERY_LEN            6
//#define MODBUS_HEADER_LEN           2               /* function + address */
//#define MODBUS_EXC_CODE             0x80            /* Exception code defines */ 

/* ��ʱ���� */
//#define MODBUS_OVERTIME             1000            /* 1 sec */
#define MODBUS_COMM_TIME_OUT        800             /* ͨѶ��·��ʱ */
#define XRD_MODBUS_COMM_TIME_OUT        1000             /* ͨѶ��·��ʱ */
#define RCV_OVER_TIME               20              /* �ɼ�ModBus�ֽ�֮�䳬ʱʱ�� */

//#define MODBUS_MASTER_SLAVE_OVERTIME	3000	/*3 sec*/

#define LOBYTE(w)		(w&0xFF)
#define HIBYTE(w)		((w>>8)&0xFF)	

#ifdef  __cplusplus
}
#endif
#endif

