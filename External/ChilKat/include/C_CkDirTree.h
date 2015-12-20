#ifndef _C_CkDirTree_H
#define _C_CkDirTree_H
#include "Chilkat_C.h"

HCkDirTree CkDirTree_Create(void);
void CkDirTree_Dispose(HCkDirTree handle);
void CkDirTree_getBaseDir(HCkDirTree cHandle, HCkString retval);
void CkDirTree_putBaseDir(HCkDirTree cHandle, const char *newVal);
void CkDirTree_getDebugLogFilePath(HCkDirTree cHandle, HCkString retval);
void CkDirTree_putDebugLogFilePath(HCkDirTree cHandle, const char *newVal);
BOOL CkDirTree_getDoneIterating(HCkDirTree cHandle);
int CkDirTree_getFileSize32(HCkDirTree cHandle);
__int64 CkDirTree_getFileSize64(HCkDirTree cHandle);
void CkDirTree_getFullPath(HCkDirTree cHandle, HCkString retval);
void CkDirTree_getFullUncPath(HCkDirTree cHandle, HCkString retval);
BOOL CkDirTree_getIsDirectory(HCkDirTree cHandle);
void CkDirTree_getLastErrorHtml(HCkDirTree cHandle, HCkString retval);
void CkDirTree_getLastErrorText(HCkDirTree cHandle, HCkString retval);
void CkDirTree_getLastErrorXml(HCkDirTree cHandle, HCkString retval);
BOOL CkDirTree_getRecurse(HCkDirTree cHandle);
void CkDirTree_putRecurse(HCkDirTree cHandle, BOOL newVal);
void CkDirTree_getRelativePath(HCkDirTree cHandle, HCkString retval);
BOOL CkDirTree_getUtf8(HCkDirTree cHandle);
void CkDirTree_putUtf8(HCkDirTree cHandle, BOOL newVal);
BOOL CkDirTree_getVerboseLogging(HCkDirTree cHandle);
void CkDirTree_putVerboseLogging(HCkDirTree cHandle, BOOL newVal);
void CkDirTree_getVersion(HCkDirTree cHandle, HCkString retval);
BOOL CkDirTree_AdvancePosition(HCkDirTree cHandle);
BOOL CkDirTree_BeginIterate(HCkDirTree cHandle);
BOOL CkDirTree_SaveLastError(HCkDirTree cHandle, const char *path);
const char *CkDirTree_baseDir(HCkDirTree cHandle);
const char *CkDirTree_debugLogFilePath(HCkDirTree cHandle);
const char *CkDirTree_fullPath(HCkDirTree cHandle);
const char *CkDirTree_fullUncPath(HCkDirTree cHandle);
const char *CkDirTree_lastErrorHtml(HCkDirTree cHandle);
const char *CkDirTree_lastErrorText(HCkDirTree cHandle);
const char *CkDirTree_lastErrorXml(HCkDirTree cHandle);
const char *CkDirTree_relativePath(HCkDirTree cHandle);
const char *CkDirTree_version(HCkDirTree cHandle);
#endif
