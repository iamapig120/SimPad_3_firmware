#ifndef __DATAFLASH_DEFINE__
#define __DATAFLASH_DEFINE__

#define NO_XSFR_DEFINE 1

#include "CORE_LIB\CH559.H"

extern UINT8X DataToWrite[8]; //用于复写内存储，大小为 8 字节
extern UINT8	ProgWord( UINT16, UINT16);
void WriteDataFlash(UINT16,PUINT8,UINT16);
void ReadDataFlash(UINT16,PUINT8,UINT16);

#endif