#ifndef _C_CkXmp_H
#define _C_CkXmp_H
#include "Chilkat_C.h"

HCkXmp CkXmp_Create(void);
void CkXmp_Dispose(HCkXmp handle);
void CkXmp_getDebugLogFilePath(HCkXmp cHandle, HCkString retval);
void CkXmp_putDebugLogFilePath(HCkXmp cHandle, const char *newVal);
void CkXmp_getLastErrorHtml(HCkXmp cHandle, HCkString retval);
void CkXmp_getLastErrorText(HCkXmp cHandle, HCkString retval);
void CkXmp_getLastErrorXml(HCkXmp cHandle, HCkString retval);
int CkXmp_getNumEmbedded(HCkXmp cHandle);
BOOL CkXmp_getStructInnerDescrip(HCkXmp cHandle);
void CkXmp_putStructInnerDescrip(HCkXmp cHandle, BOOL newVal);
BOOL CkXmp_getUtf8(HCkXmp cHandle);
void CkXmp_putUtf8(HCkXmp cHandle, BOOL newVal);
BOOL CkXmp_getVerboseLogging(HCkXmp cHandle);
void CkXmp_putVerboseLogging(HCkXmp cHandle, BOOL newVal);
void CkXmp_getVersion(HCkXmp cHandle, HCkString retval);
BOOL CkXmp_AddArray(HCkXmp cHandle, HCkXml xml, const char *arrType, const char *propName, HCkStringArray values);
void CkXmp_AddNsMapping(HCkXmp cHandle, const char *ns, const char *uri);
BOOL CkXmp_AddSimpleDate(HCkXmp cHandle, HCkXml xml, const char *propName, SYSTEMTIME *sysTime);
BOOL CkXmp_AddSimpleInt(HCkXmp cHandle, HCkXml xml, const char *propName, int propVal);
BOOL CkXmp_AddSimpleStr(HCkXmp cHandle, HCkXml xml, const char *propName, const char *propVal);
BOOL CkXmp_AddStructProp(HCkXmp cHandle, HCkXml xml, const char *structName, const char *propName, const char *propVal);
BOOL CkXmp_Append(HCkXmp cHandle, HCkXml xml);
BOOL CkXmp_DateToString(HCkXmp cHandle, SYSTEMTIME *sysTime, HCkString outStr);
HCkStringArray CkXmp_GetArray(HCkXmp cHandle, HCkXml xml, const char *propName);
HCkXml CkXmp_GetEmbedded(HCkXmp cHandle, int index);
HCkXml CkXmp_GetProperty(HCkXmp cHandle, HCkXml xml, const char *propName);
BOOL CkXmp_GetSimpleDate(HCkXmp cHandle, HCkXml xml, const char *propName, SYSTEMTIME *outSysTime);
int CkXmp_GetSimpleInt(HCkXmp cHandle, HCkXml xml, const char *propName);
BOOL CkXmp_GetSimpleStr(HCkXmp cHandle, HCkXml xml, const char *propName, HCkString outStr);
HCkStringArray CkXmp_GetStructPropNames(HCkXmp cHandle, HCkXml xml, const char *structName);
BOOL CkXmp_GetStructValue(HCkXmp cHandle, HCkXml xml, const char *structName, const char *propName, HCkString outStr);
BOOL CkXmp_LoadAppFile(HCkXmp cHandle, const char *path);
BOOL CkXmp_LoadFromBuffer(HCkXmp cHandle, HCkByteData byteData, const char *ext);
HCkXml CkXmp_NewXmp(HCkXmp cHandle);
void CkXmp_RemoveAllEmbedded(HCkXmp cHandle);
BOOL CkXmp_RemoveArray(HCkXmp cHandle, HCkXml xml, const char *propName);
#if defined(WIN32) && !defined(__MINGW32__)
void CkXmp_RemoveEmbedded(HCkXmp cHandle, int index);
#endif
void CkXmp_RemoveNsMapping(HCkXmp cHandle, const char *ns);
BOOL CkXmp_RemoveSimple(HCkXmp cHandle, HCkXml xml, const char *propName);
BOOL CkXmp_RemoveStruct(HCkXmp cHandle, HCkXml xml, const char *structName);
BOOL CkXmp_RemoveStructProp(HCkXmp cHandle, HCkXml xml, const char *structName, const char *propName);
BOOL CkXmp_SaveAppFile(HCkXmp cHandle, const char *path);
BOOL CkXmp_SaveLastError(HCkXmp cHandle, const char *path);
BOOL CkXmp_SaveToBuffer(HCkXmp cHandle, HCkByteData outBytes);
BOOL CkXmp_StringToDate(HCkXmp cHandle, const char *str, SYSTEMTIME *outSysTime);
BOOL CkXmp_UnlockComponent(HCkXmp cHandle, const char *unlockCode);
const char *CkXmp_dateToString(HCkXmp cHandle, SYSTEMTIME *sysTime);
const char *CkXmp_debugLogFilePath(HCkXmp cHandle);
const char *CkXmp_getSimpleStr(HCkXmp cHandle, HCkXml xml, const char *propName);
const char *CkXmp_getStructValue(HCkXmp cHandle, HCkXml xml, const char *structName, const char *propName);
const char *CkXmp_lastErrorHtml(HCkXmp cHandle);
const char *CkXmp_lastErrorText(HCkXmp cHandle);
const char *CkXmp_lastErrorXml(HCkXmp cHandle);
const char *CkXmp_simpleStr(HCkXmp cHandle, HCkXml xml, const char *propName);
const char *CkXmp_structValue(HCkXmp cHandle, HCkXml xml, const char *structName, const char *propName);
const char *CkXmp_version(HCkXmp cHandle);
#endif
