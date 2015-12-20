#ifndef _C_CkDateTime_H
#define _C_CkDateTime_H
#include "Chilkat_C.h"

HCkDateTime CkDateTime_Create(void);
void CkDateTime_Dispose(HCkDateTime handle);
void CkDateTime_getDebugLogFilePath(HCkDateTime cHandle, HCkString retval);
void CkDateTime_putDebugLogFilePath(HCkDateTime cHandle, const char *newVal);
int CkDateTime_getIsDst(HCkDateTime cHandle);
void CkDateTime_getLastErrorHtml(HCkDateTime cHandle, HCkString retval);
void CkDateTime_getLastErrorText(HCkDateTime cHandle, HCkString retval);
void CkDateTime_getLastErrorXml(HCkDateTime cHandle, HCkString retval);
int CkDateTime_getUtcOffset(HCkDateTime cHandle);
BOOL CkDateTime_getUtf8(HCkDateTime cHandle);
void CkDateTime_putUtf8(HCkDateTime cHandle, BOOL newVal);
BOOL CkDateTime_getVerboseLogging(HCkDateTime cHandle);
void CkDateTime_putVerboseLogging(HCkDateTime cHandle, BOOL newVal);
void CkDateTime_getVersion(HCkDateTime cHandle, HCkString retval);
BOOL CkDateTime_AddDays(HCkDateTime cHandle, int numDays);
void CkDateTime_DeSerialize(HCkDateTime cHandle, const char *serializedDateTime);
__int64 CkDateTime_GetAsDateTimeTicks(HCkDateTime cHandle, BOOL bLocal);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned long CkDateTime_GetAsDosDate(HCkDateTime cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint32_t CkDateTime_GetAsDosDate(HCkDateTime cHandle, BOOL bLocal);
#endif
double CkDateTime_GetAsOleDate(HCkDateTime cHandle, BOOL bLocal);
BOOL CkDateTime_GetAsRfc822(HCkDateTime cHandle, BOOL bLocal, HCkString outStr);
void CkDateTime_GetAsSystemTime(HCkDateTime cHandle, BOOL bLocal, SYSTEMTIME *outSysTime);
void CkDateTime_GetAsTmStruct(HCkDateTime cHandle, BOOL bLocal, struct tm *tmbuf);
time_t CkDateTime_GetAsUnixTime(HCkDateTime cHandle, BOOL bLocal);
__int64 CkDateTime_GetAsUnixTime64(HCkDateTime cHandle, BOOL bLocal);
double CkDateTime_GetAsUnixTimeDbl(HCkDateTime cHandle, BOOL bLocal);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned short CkDateTime_GetDosDateHigh(HCkDateTime cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint16_t CkDateTime_GetDosDateHigh(HCkDateTime cHandle, BOOL bLocal);
#endif
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned short CkDateTime_GetDosDateLow(HCkDateTime cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint16_t CkDateTime_GetDosDateLow(HCkDateTime cHandle, BOOL bLocal);
#endif
BOOL CkDateTime_SaveLastError(HCkDateTime cHandle, const char *path);
BOOL CkDateTime_Serialize(HCkDateTime cHandle, HCkString outStr);
void CkDateTime_SetFromCurrentSystemTime(HCkDateTime cHandle);
void CkDateTime_SetFromDateTimeTicks(HCkDateTime cHandle, BOOL bLocal, __int64 n);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
void CkDateTime_SetFromDosDate(HCkDateTime cHandle, BOOL bLocal, unsigned long t);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
void CkDateTime_SetFromDosDate(HCkDateTime cHandle, BOOL bLocal, uint32_t t);
#endif
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
void CkDateTime_SetFromDosDate2(HCkDateTime cHandle, BOOL bLocal, unsigned short d, unsigned short t);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
void CkDateTime_SetFromDosDate2(HCkDateTime cHandle, BOOL bLocal, uint16_t d, uint16_t t);
#endif
void CkDateTime_SetFromOleDate(HCkDateTime cHandle, BOOL bLocal, double dt);
BOOL CkDateTime_SetFromRfc822(HCkDateTime cHandle, const char *rfc822Str);
void CkDateTime_SetFromSystemTime(HCkDateTime cHandle, BOOL bLocal, SYSTEMTIME *sysTime);
void CkDateTime_SetFromTmStruct(HCkDateTime cHandle, BOOL bLocal, struct tm *tmbuf);
void CkDateTime_SetFromUnixTime(HCkDateTime cHandle, BOOL bLocal, time_t t);
void CkDateTime_SetFromUnixTime64(HCkDateTime cHandle, BOOL bLocal, __int64 t);
void CkDateTime_SetFromUnixTimeDbl(HCkDateTime cHandle, BOOL bLocal, double t);
const char *CkDateTime_debugLogFilePath(HCkDateTime cHandle);
const char *CkDateTime_getAsRfc822(HCkDateTime cHandle, BOOL bLocal);
const char *CkDateTime_lastErrorHtml(HCkDateTime cHandle);
const char *CkDateTime_lastErrorText(HCkDateTime cHandle);
const char *CkDateTime_lastErrorXml(HCkDateTime cHandle);
const char *CkDateTime_serialize(HCkDateTime cHandle);
const char *CkDateTime_version(HCkDateTime cHandle);
#endif
