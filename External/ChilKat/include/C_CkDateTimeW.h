#ifndef _C_CkDateTime_W_H
#define _C_CkDateTime_W_H
#include "Chilkat_C.h"

HCkDateTimeW CkDateTimeW_Create(void);
HCkDateTimeW CkDateTimeW_Create2(bool bForMono);
void CkDateTimeW_Dispose(HCkDateTimeW handle);
void CkDateTimeW_getDebugLogFilePath(HCkDateTimeW cHandle, HCkStringW retval);
void CkDateTimeW_putDebugLogFilePath(HCkDateTimeW cHandle, const wchar_t *newVal);
int CkDateTimeW_getIsDst(HCkDateTimeW cHandle);
void CkDateTimeW_getLastErrorHtml(HCkDateTimeW cHandle, HCkStringW retval);
void CkDateTimeW_getLastErrorText(HCkDateTimeW cHandle, HCkStringW retval);
void CkDateTimeW_getLastErrorXml(HCkDateTimeW cHandle, HCkStringW retval);
int CkDateTimeW_getUtcOffset(HCkDateTimeW cHandle);
BOOL CkDateTimeW_getVerboseLogging(HCkDateTimeW cHandle);
void CkDateTimeW_putVerboseLogging(HCkDateTimeW cHandle, BOOL newVal);
void CkDateTimeW_getVersion(HCkDateTimeW cHandle, HCkStringW retval);
BOOL CkDateTimeW_AddDays(HCkDateTimeW cHandle, int numDays);
void CkDateTimeW_DeSerialize(HCkDateTimeW cHandle, const wchar_t *serializedDateTime);
__int64 CkDateTimeW_GetAsDateTimeTicks(HCkDateTimeW cHandle, BOOL bLocal);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned long CkDateTimeW_GetAsDosDate(HCkDateTimeW cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint32_t CkDateTimeW_GetAsDosDate(HCkDateTimeW cHandle, BOOL bLocal);
#endif
double CkDateTimeW_GetAsOleDate(HCkDateTimeW cHandle, BOOL bLocal);
BOOL CkDateTimeW_GetAsRfc822(HCkDateTimeW cHandle, BOOL bLocal, HCkStringW outStr);
void CkDateTimeW_GetAsSystemTime(HCkDateTimeW cHandle, BOOL bLocal, SYSTEMTIME *outSysTime);
void CkDateTimeW_GetAsTmStruct(HCkDateTimeW cHandle, BOOL bLocal, struct tm *tmbuf);
time_t CkDateTimeW_GetAsUnixTime(HCkDateTimeW cHandle, BOOL bLocal);
__int64 CkDateTimeW_GetAsUnixTime64(HCkDateTimeW cHandle, BOOL bLocal);
double CkDateTimeW_GetAsUnixTimeDbl(HCkDateTimeW cHandle, BOOL bLocal);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned short CkDateTimeW_GetDosDateHigh(HCkDateTimeW cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint16_t CkDateTimeW_GetDosDateHigh(HCkDateTimeW cHandle, BOOL bLocal);
#endif
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
unsigned short CkDateTimeW_GetDosDateLow(HCkDateTimeW cHandle, BOOL bLocal);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
uint16_t CkDateTimeW_GetDosDateLow(HCkDateTimeW cHandle, BOOL bLocal);
#endif
BOOL CkDateTimeW_SaveLastError(HCkDateTimeW cHandle, const wchar_t *path);
BOOL CkDateTimeW_Serialize(HCkDateTimeW cHandle, HCkStringW outStr);
void CkDateTimeW_SetFromCurrentSystemTime(HCkDateTimeW cHandle);
void CkDateTimeW_SetFromDateTimeTicks(HCkDateTimeW cHandle, BOOL bLocal, __int64 n);
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
void CkDateTimeW_SetFromDosDate(HCkDateTimeW cHandle, BOOL bLocal, unsigned long t);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
void CkDateTimeW_SetFromDosDate(HCkDateTimeW cHandle, BOOL bLocal, uint32_t t);
#endif
#if defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
void CkDateTimeW_SetFromDosDate2(HCkDateTimeW cHandle, BOOL bLocal, unsigned short d, unsigned short t);
#endif
#if !defined(WIN32) && !defined(__MINGW32__) && !defined(__BORLANDC__) && !defined(__BCPLUSPLUS__)
void CkDateTimeW_SetFromDosDate2(HCkDateTimeW cHandle, BOOL bLocal, uint16_t d, uint16_t t);
#endif
void CkDateTimeW_SetFromOleDate(HCkDateTimeW cHandle, BOOL bLocal, double dt);
BOOL CkDateTimeW_SetFromRfc822(HCkDateTimeW cHandle, const wchar_t *rfc822Str);
void CkDateTimeW_SetFromSystemTime(HCkDateTimeW cHandle, BOOL bLocal, SYSTEMTIME *sysTime);
void CkDateTimeW_SetFromTmStruct(HCkDateTimeW cHandle, BOOL bLocal, struct tm *tmbuf);
void CkDateTimeW_SetFromUnixTime(HCkDateTimeW cHandle, BOOL bLocal, time_t t);
void CkDateTimeW_SetFromUnixTime64(HCkDateTimeW cHandle, BOOL bLocal, __int64 t);
void CkDateTimeW_SetFromUnixTimeDbl(HCkDateTimeW cHandle, BOOL bLocal, double t);
const wchar_t *CkDateTimeW__debugLogFilePath(HCkDateTimeW cHandle);
const wchar_t *CkDateTimeW__getAsRfc822(HCkDateTimeW cHandle, BOOL bLocal);
const wchar_t *CkDateTimeW__lastErrorHtml(HCkDateTimeW cHandle);
const wchar_t *CkDateTimeW__lastErrorText(HCkDateTimeW cHandle);
const wchar_t *CkDateTimeW__lastErrorXml(HCkDateTimeW cHandle);
const wchar_t *CkDateTimeW__serialize(HCkDateTimeW cHandle);
const wchar_t *CkDateTimeW__version(HCkDateTimeW cHandle);
#endif
