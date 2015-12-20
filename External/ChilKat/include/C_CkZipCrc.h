#ifndef _CkZipCrc_H
#define _CkZipCrc_H
#include "Chilkat_C.h"

HCkZipCrc CkZipCrc_Create(void);
void CkZipCrc_Dispose(HCkZipCrc handle);
BOOL CkZipCrc_getUtf8(HCkZipCrc cHandle);
void CkZipCrc_putUtf8(HCkZipCrc cHandle, BOOL newVal);
void CkZipCrc_BeginStream(HCkZipCrc cHandle);

#ifdef WIN32
unsigned long CkZipCrc_CalculateCrc(HCkZipCrc cHandle, HCkByteData byteData);
unsigned long CkZipCrc_EndStream(HCkZipCrc cHandle);
unsigned long CkZipCrc_FileCrc(HCkZipCrc cHandle, const char *filename);
void CkZipCrc_ToHex(HCkZipCrc cHandle, unsigned long crc, HCkString strHex);
const char *CkZipCrc_toHex(HCkZipCrc cHandle, unsigned long crc);
#else
uint32_t CkZipCrc_CalculateCrc(HCkZipCrc cHandle, HCkByteData byteData);
uint32_t CkZipCrc_EndStream(HCkZipCrc cHandle);
uint32_t CkZipCrc_FileCrc(HCkZipCrc cHandle, const char *filename);
void CkZipCrc_ToHex(HCkZipCrc cHandle, uint32_t crc, HCkString strHex);
const char *CkZipCrc_toHex(HCkZipCrc cHandle, uint32_t crc);
#endif

void CkZipCrc_MoreData(HCkZipCrc cHandle, HCkByteData byteData);
#endif
