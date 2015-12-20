#ifndef _C_CkEmailBundle_W_H
#define _C_CkEmailBundle_W_H
#include "Chilkat_C.h"

HCkEmailBundleW CkEmailBundleW_Create(void);
HCkEmailBundleW CkEmailBundleW_Create2(bool bForMono);
void CkEmailBundleW_Dispose(HCkEmailBundleW handle);
void CkEmailBundleW_getDebugLogFilePath(HCkEmailBundleW cHandle, HCkStringW retval);
void CkEmailBundleW_putDebugLogFilePath(HCkEmailBundleW cHandle, const wchar_t *newVal);
void CkEmailBundleW_getLastErrorHtml(HCkEmailBundleW cHandle, HCkStringW retval);
void CkEmailBundleW_getLastErrorText(HCkEmailBundleW cHandle, HCkStringW retval);
void CkEmailBundleW_getLastErrorXml(HCkEmailBundleW cHandle, HCkStringW retval);
long CkEmailBundleW_getMessageCount(HCkEmailBundleW cHandle);
BOOL CkEmailBundleW_getVerboseLogging(HCkEmailBundleW cHandle);
void CkEmailBundleW_putVerboseLogging(HCkEmailBundleW cHandle, BOOL newVal);
void CkEmailBundleW_getVersion(HCkEmailBundleW cHandle, HCkStringW retval);
BOOL CkEmailBundleW_AddEmail(HCkEmailBundleW cHandle, HCkEmailW email);
HCkEmailW CkEmailBundleW_FindByHeader(HCkEmailBundleW cHandle, const wchar_t *name, const wchar_t *value);
HCkEmailW CkEmailBundleW_GetEmail(HCkEmailBundleW cHandle, long index);
HCkStringArrayW CkEmailBundleW_GetUidls(HCkEmailBundleW cHandle);
BOOL CkEmailBundleW_GetXml(HCkEmailBundleW cHandle, HCkStringW outXml);
BOOL CkEmailBundleW_LoadXml(HCkEmailBundleW cHandle, const wchar_t *path);
BOOL CkEmailBundleW_LoadXmlString(HCkEmailBundleW cHandle, const wchar_t *xmlStr);
BOOL CkEmailBundleW_RemoveEmail(HCkEmailBundleW cHandle, HCkEmailW email);
BOOL CkEmailBundleW_RemoveEmailByIndex(HCkEmailBundleW cHandle, int index);
BOOL CkEmailBundleW_SaveLastError(HCkEmailBundleW cHandle, const wchar_t *path);
BOOL CkEmailBundleW_SaveXml(HCkEmailBundleW cHandle, const wchar_t *path);
void CkEmailBundleW_SortByDate(HCkEmailBundleW cHandle, BOOL ascending);
void CkEmailBundleW_SortByRecipient(HCkEmailBundleW cHandle, BOOL ascending);
void CkEmailBundleW_SortBySender(HCkEmailBundleW cHandle, BOOL ascending);
void CkEmailBundleW_SortBySubject(HCkEmailBundleW cHandle, BOOL ascending);
const wchar_t *CkEmailBundleW__debugLogFilePath(HCkEmailBundleW cHandle);
const wchar_t *CkEmailBundleW__getXml(HCkEmailBundleW cHandle);
const wchar_t *CkEmailBundleW__lastErrorHtml(HCkEmailBundleW cHandle);
const wchar_t *CkEmailBundleW__lastErrorText(HCkEmailBundleW cHandle);
const wchar_t *CkEmailBundleW__lastErrorXml(HCkEmailBundleW cHandle);
const wchar_t *CkEmailBundleW__version(HCkEmailBundleW cHandle);
#endif
