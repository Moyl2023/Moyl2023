#ifndef _CRC_H_
#define _CRC_H_

#include "config.h"	

#define CRC_SEND		0
#define CRC_REV			1

uint16 crcm(uint8 *buffer,int16 length,int16 fig);
void crcupdate(uint8 datat,uint16 *accum,const uint16 *crctab);
extern uint16 CalCS(uint8* buf, uint16 lenth);
extern uint16 ModbusCRC(uint8 *data, uint16 len);
extern uint16 RxdCrc(uint8* p,uint8 length);
extern uint16  TxdCrc(uint8* p,uint8 length);
uint16 LcdRxCRC(uint8 *data, uint16 len);
uint16 LcdTxCRC(uint8 *data, uint16 len);
unsigned short CalcCRC1021(unsigned char *pu8Buf, unsigned int u32Len);
uint16  FdyTxdCrc(uint8* p,uint8 length);
uint16 FdyRxdCrc(uint8* p,uint8 length);

#endif

