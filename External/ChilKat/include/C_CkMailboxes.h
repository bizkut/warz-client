#ifndef _C_CkMailboxes_H
#define _C_CkMailboxes_H
#include "Chilkat_C.h"

HCkMailboxes CkMailboxes_Create(void);
void CkMailboxes_Dispose(HCkMailboxes handle);
long CkMailboxes_getCount(HCkMailboxes cHandle);
void CkMailboxes_getDebugLogFilePath(HCkMailboxes cHandle, HCkString retval);
void CkMailboxes_putDebugLogFilePath(HCkMailboxes cHandle, const char *newVal);
void CkMailboxes_getLastErrorHtml(HCkMailboxes cHandle, HCkString retval);
void CkMailboxes_getLastErrorText(HCkMailboxes cHandle, HCkString retval);
void CkMailboxes_getLastErrorXml(HCkMailboxes cHandle, HCkString retval);
BOOL CkMailboxes_getUtf8(HCkMailboxes cHandle);
void CkMailboxes_putUtf8(HCkMailboxes cHandle, BOOL newVal);
BOOL CkMailboxes_getVerboseLogging(HCkMailboxes cHandle);
void CkMailboxes_putVerboseLogging(HCkMailboxes cHandle, BOOL newVal);
void CkMailboxes_getVersion(HCkMailboxes cHandle, HCkString retval);
BOOL CkMailboxes_GetName(HCkMailboxes cHandle, long index, HCkString outStrName);
BOOL CkMailboxes_HasInferiors(HCkMailboxes cHandle, long index);
BOOL CkMailboxes_IsMarked(HCkMailboxes cHandle, long index);
BOOL CkMailboxes_IsSelectable(HCkMailboxes cHandle, long index);
BOOL CkMailboxes_SaveLastError(HCkMailboxes cHandle, const char *path);
const char *CkMailboxes_debugLogFilePath(HCkMailboxes cHandle);
const char *CkMailboxes_getName(HCkMailboxes cHandle, long index);
const char *CkMailboxes_lastErrorHtml(HCkMailboxes cHandle);
const char *CkMailboxes_lastErrorText(HCkMailboxes cHandle);
const char *CkMailboxes_lastErrorXml(HCkMailboxes cHandle);
const char *CkMailboxes_version(HCkMailboxes cHandle);
#endif
