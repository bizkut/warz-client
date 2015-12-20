#ifndef _C_CkDtObj_W_H
#define _C_CkDtObj_W_H
#include "Chilkat_C.h"

HCkDtObjW CkDtObjW_Create(void);
HCkDtObjW CkDtObjW_Create2(bool bForMono);
void CkDtObjW_Dispose(HCkDtObjW handle);
int CkDtObjW_getDay(HCkDtObjW cHandle);
void CkDtObjW_putDay(HCkDtObjW cHandle, int newVal);
void CkDtObjW_getDebugLogFilePath(HCkDtObjW cHandle, HCkStringW retval);
void CkDtObjW_putDebugLogFilePath(HCkDtObjW cHandle, const wchar_t *newVal);
int CkDtObjW_getHour(HCkDtObjW cHandle);
void CkDtObjW_putHour(HCkDtObjW cHandle, int newVal);
void CkDtObjW_getLastErrorHtml(HCkDtObjW cHandle, HCkStringW retval);
void CkDtObjW_getLastErrorText(HCkDtObjW cHandle, HCkStringW retval);
void CkDtObjW_getLastErrorXml(HCkDtObjW cHandle, HCkStringW retval);
int CkDtObjW_getMinute(HCkDtObjW cHandle);
void CkDtObjW_putMinute(HCkDtObjW cHandle, int newVal);
int CkDtObjW_getMonth(HCkDtObjW cHandle);
void CkDtObjW_putMonth(HCkDtObjW cHandle, int newVal);
int CkDtObjW_getSecond(HCkDtObjW cHandle);
void CkDtObjW_putSecond(HCkDtObjW cHandle, int newVal);
int CkDtObjW_getStructTmMonth(HCkDtObjW cHandle);
void CkDtObjW_putStructTmMonth(HCkDtObjW cHandle, int newVal);
int CkDtObjW_getStructTmYear(HCkDtObjW cHandle);
void CkDtObjW_putStructTmYear(HCkDtObjW cHandle, int newVal);
BOOL CkDtObjW_getUtc(HCkDtObjW cHandle);
void CkDtObjW_putUtc(HCkDtObjW cHandle, BOOL newVal);
BOOL CkDtObjW_getVerboseLogging(HCkDtObjW cHandle);
void CkDtObjW_putVerboseLogging(HCkDtObjW cHandle, BOOL newVal);
void CkDtObjW_getVersion(HCkDtObjW cHandle, HCkStringW retval);
int CkDtObjW_getYear(HCkDtObjW cHandle);
void CkDtObjW_putYear(HCkDtObjW cHandle, int newVal);
void CkDtObjW_DeSerialize(HCkDtObjW cHandle, const wchar_t *serializedDtObj);
BOOL CkDtObjW_SaveLastError(HCkDtObjW cHandle, const wchar_t *path);
BOOL CkDtObjW_Serialize(HCkDtObjW cHandle, HCkStringW outStr);
const wchar_t *CkDtObjW__debugLogFilePath(HCkDtObjW cHandle);
const wchar_t *CkDtObjW__lastErrorHtml(HCkDtObjW cHandle);
const wchar_t *CkDtObjW__lastErrorText(HCkDtObjW cHandle);
const wchar_t *CkDtObjW__lastErrorXml(HCkDtObjW cHandle);
const wchar_t *CkDtObjW__serialize(HCkDtObjW cHandle);
const wchar_t *CkDtObjW__version(HCkDtObjW cHandle);
#endif
