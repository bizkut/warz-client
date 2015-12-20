#ifndef _C_CkXmp_W_H
#define _C_CkXmp_W_H
#include "Chilkat_C.h"

HCkXmpW CkXmpW_Create(void);
HCkXmpW CkXmpW_Create2(bool bForMono);
void CkXmpW_Dispose(HCkXmpW handle);
void CkXmpW_getDebugLogFilePath(HCkXmpW cHandle, HCkStringW retval);
void CkXmpW_putDebugLogFilePath(HCkXmpW cHandle, const wchar_t *newVal);
void CkXmpW_getLastErrorHtml(HCkXmpW cHandle, HCkStringW retval);
void CkXmpW_getLastErrorText(HCkXmpW cHandle, HCkStringW retval);
void CkXmpW_getLastErrorXml(HCkXmpW cHandle, HCkStringW retval);
int CkXmpW_getNumEmbedded(HCkXmpW cHandle);
BOOL CkXmpW_getStructInnerDescrip(HCkXmpW cHandle);
void CkXmpW_putStructInnerDescrip(HCkXmpW cHandle, BOOL newVal);
BOOL CkXmpW_getVerboseLogging(HCkXmpW cHandle);
void CkXmpW_putVerboseLogging(HCkXmpW cHandle, BOOL newVal);
void CkXmpW_getVersion(HCkXmpW cHandle, HCkStringW retval);
BOOL CkXmpW_AddArray(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *arrType, const wchar_t *propName, HCkStringArrayW values);
void CkXmpW_AddNsMapping(HCkXmpW cHandle, const wchar_t *ns, const wchar_t *uri);
BOOL CkXmpW_AddSimpleDate(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName, SYSTEMTIME *sysTime);
BOOL CkXmpW_AddSimpleInt(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName, int propVal);
BOOL CkXmpW_AddSimpleStr(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName, const wchar_t *propVal);
BOOL CkXmpW_AddStructProp(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName, const wchar_t *propName, const wchar_t *propVal);
BOOL CkXmpW_Append(HCkXmpW cHandle, HCkXmlW xml);
BOOL CkXmpW_DateToString(HCkXmpW cHandle, SYSTEMTIME *sysTime, HCkStringW outStr);
HCkStringArrayW CkXmpW_GetArray(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
HCkXmlW CkXmpW_GetEmbedded(HCkXmpW cHandle, int index);
HCkXmlW CkXmpW_GetProperty(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
BOOL CkXmpW_GetSimpleDate(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName, SYSTEMTIME *outSysTime);
int CkXmpW_GetSimpleInt(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
BOOL CkXmpW_GetSimpleStr(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName, HCkStringW outStr);
HCkStringArrayW CkXmpW_GetStructPropNames(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName);
BOOL CkXmpW_GetStructValue(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName, const wchar_t *propName, HCkStringW outStr);
BOOL CkXmpW_LoadAppFile(HCkXmpW cHandle, const wchar_t *path);
BOOL CkXmpW_LoadFromBuffer(HCkXmpW cHandle, HCkByteDataW byteData, const wchar_t *ext);
HCkXmlW CkXmpW_NewXmp(HCkXmpW cHandle);
void CkXmpW_RemoveAllEmbedded(HCkXmpW cHandle);
BOOL CkXmpW_RemoveArray(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
#if defined(WIN32) && !defined(__MINGW32__)
void CkXmpW_RemoveEmbedded(HCkXmpW cHandle, int index);
#endif
void CkXmpW_RemoveNsMapping(HCkXmpW cHandle, const wchar_t *ns);
BOOL CkXmpW_RemoveSimple(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
BOOL CkXmpW_RemoveStruct(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName);
BOOL CkXmpW_RemoveStructProp(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName, const wchar_t *propName);
BOOL CkXmpW_SaveAppFile(HCkXmpW cHandle, const wchar_t *path);
BOOL CkXmpW_SaveLastError(HCkXmpW cHandle, const wchar_t *path);
BOOL CkXmpW_SaveToBuffer(HCkXmpW cHandle, HCkByteDataW outBytes);
BOOL CkXmpW_StringToDate(HCkXmpW cHandle, const wchar_t *str, SYSTEMTIME *outSysTime);
BOOL CkXmpW_UnlockComponent(HCkXmpW cHandle, const wchar_t *unlockCode);
const wchar_t *CkXmpW__dateToString(HCkXmpW cHandle, SYSTEMTIME *sysTime);
const wchar_t *CkXmpW__debugLogFilePath(HCkXmpW cHandle);
const wchar_t *CkXmpW__getSimpleStr(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
const wchar_t *CkXmpW__getStructValue(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName, const wchar_t *propName);
const wchar_t *CkXmpW__lastErrorHtml(HCkXmpW cHandle);
const wchar_t *CkXmpW__lastErrorText(HCkXmpW cHandle);
const wchar_t *CkXmpW__lastErrorXml(HCkXmpW cHandle);
const wchar_t *CkXmpW__simpleStr(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *propName);
const wchar_t *CkXmpW__structValue(HCkXmpW cHandle, HCkXmlW xml, const wchar_t *structName, const wchar_t *propName);
const wchar_t *CkXmpW__version(HCkXmpW cHandle);
#endif
