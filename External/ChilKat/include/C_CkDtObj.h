#ifndef _C_CkDtObj_H
#define _C_CkDtObj_H
#include "Chilkat_C.h"

HCkDtObj CkDtObj_Create(void);
void CkDtObj_Dispose(HCkDtObj handle);
int CkDtObj_getDay(HCkDtObj cHandle);
void CkDtObj_putDay(HCkDtObj cHandle, int newVal);
void CkDtObj_getDebugLogFilePath(HCkDtObj cHandle, HCkString retval);
void CkDtObj_putDebugLogFilePath(HCkDtObj cHandle, const char *newVal);
int CkDtObj_getHour(HCkDtObj cHandle);
void CkDtObj_putHour(HCkDtObj cHandle, int newVal);
void CkDtObj_getLastErrorHtml(HCkDtObj cHandle, HCkString retval);
void CkDtObj_getLastErrorText(HCkDtObj cHandle, HCkString retval);
void CkDtObj_getLastErrorXml(HCkDtObj cHandle, HCkString retval);
int CkDtObj_getMinute(HCkDtObj cHandle);
void CkDtObj_putMinute(HCkDtObj cHandle, int newVal);
int CkDtObj_getMonth(HCkDtObj cHandle);
void CkDtObj_putMonth(HCkDtObj cHandle, int newVal);
int CkDtObj_getSecond(HCkDtObj cHandle);
void CkDtObj_putSecond(HCkDtObj cHandle, int newVal);
int CkDtObj_getStructTmMonth(HCkDtObj cHandle);
void CkDtObj_putStructTmMonth(HCkDtObj cHandle, int newVal);
int CkDtObj_getStructTmYear(HCkDtObj cHandle);
void CkDtObj_putStructTmYear(HCkDtObj cHandle, int newVal);
BOOL CkDtObj_getUtc(HCkDtObj cHandle);
void CkDtObj_putUtc(HCkDtObj cHandle, BOOL newVal);
BOOL CkDtObj_getUtf8(HCkDtObj cHandle);
void CkDtObj_putUtf8(HCkDtObj cHandle, BOOL newVal);
BOOL CkDtObj_getVerboseLogging(HCkDtObj cHandle);
void CkDtObj_putVerboseLogging(HCkDtObj cHandle, BOOL newVal);
void CkDtObj_getVersion(HCkDtObj cHandle, HCkString retval);
int CkDtObj_getYear(HCkDtObj cHandle);
void CkDtObj_putYear(HCkDtObj cHandle, int newVal);
void CkDtObj_DeSerialize(HCkDtObj cHandle, const char *serializedDtObj);
BOOL CkDtObj_SaveLastError(HCkDtObj cHandle, const char *path);
BOOL CkDtObj_Serialize(HCkDtObj cHandle, HCkString outStr);
const char *CkDtObj_debugLogFilePath(HCkDtObj cHandle);
const char *CkDtObj_lastErrorHtml(HCkDtObj cHandle);
const char *CkDtObj_lastErrorText(HCkDtObj cHandle);
const char *CkDtObj_lastErrorXml(HCkDtObj cHandle);
const char *CkDtObj_serialize(HCkDtObj cHandle);
const char *CkDtObj_version(HCkDtObj cHandle);
#endif
