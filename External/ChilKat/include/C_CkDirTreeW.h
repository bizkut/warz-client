#ifndef _C_CkDirTree_W_H
#define _C_CkDirTree_W_H
#include "Chilkat_C.h"

HCkDirTreeW CkDirTreeW_Create(void);
HCkDirTreeW CkDirTreeW_Create2(bool bForMono);
void CkDirTreeW_Dispose(HCkDirTreeW handle);
void CkDirTreeW_getBaseDir(HCkDirTreeW cHandle, HCkStringW retval);
void CkDirTreeW_putBaseDir(HCkDirTreeW cHandle, const wchar_t *newVal);
void CkDirTreeW_getDebugLogFilePath(HCkDirTreeW cHandle, HCkStringW retval);
void CkDirTreeW_putDebugLogFilePath(HCkDirTreeW cHandle, const wchar_t *newVal);
BOOL CkDirTreeW_getDoneIterating(HCkDirTreeW cHandle);
int CkDirTreeW_getFileSize32(HCkDirTreeW cHandle);
__int64 CkDirTreeW_getFileSize64(HCkDirTreeW cHandle);
void CkDirTreeW_getFullPath(HCkDirTreeW cHandle, HCkStringW retval);
void CkDirTreeW_getFullUncPath(HCkDirTreeW cHandle, HCkStringW retval);
BOOL CkDirTreeW_getIsDirectory(HCkDirTreeW cHandle);
void CkDirTreeW_getLastErrorHtml(HCkDirTreeW cHandle, HCkStringW retval);
void CkDirTreeW_getLastErrorText(HCkDirTreeW cHandle, HCkStringW retval);
void CkDirTreeW_getLastErrorXml(HCkDirTreeW cHandle, HCkStringW retval);
BOOL CkDirTreeW_getRecurse(HCkDirTreeW cHandle);
void CkDirTreeW_putRecurse(HCkDirTreeW cHandle, BOOL newVal);
void CkDirTreeW_getRelativePath(HCkDirTreeW cHandle, HCkStringW retval);
BOOL CkDirTreeW_getVerboseLogging(HCkDirTreeW cHandle);
void CkDirTreeW_putVerboseLogging(HCkDirTreeW cHandle, BOOL newVal);
void CkDirTreeW_getVersion(HCkDirTreeW cHandle, HCkStringW retval);
BOOL CkDirTreeW_AdvancePosition(HCkDirTreeW cHandle);
BOOL CkDirTreeW_BeginIterate(HCkDirTreeW cHandle);
BOOL CkDirTreeW_SaveLastError(HCkDirTreeW cHandle, const wchar_t *path);
const wchar_t *CkDirTreeW__baseDir(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__debugLogFilePath(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__fullPath(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__fullUncPath(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__lastErrorHtml(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__lastErrorText(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__lastErrorXml(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__relativePath(HCkDirTreeW cHandle);
const wchar_t *CkDirTreeW__version(HCkDirTreeW cHandle);
#endif
