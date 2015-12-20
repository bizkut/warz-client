#ifndef _C_CkDh_H
#define _C_CkDh_H
#include "Chilkat_C.h"

HCkDh CkDh_Create(void);
void CkDh_Dispose(HCkDh handle);
void CkDh_getDebugLogFilePath(HCkDh cHandle, HCkString retval);
void CkDh_putDebugLogFilePath(HCkDh cHandle, const char *newVal);
HCkDhProgress CkDh_getEventCallbackObject(HCkDh cHandle);
void CkDh_putEventCallbackObject(HCkDh cHandle, HCkDhProgress newVal);
int CkDh_getG(HCkDh cHandle);
void CkDh_getLastErrorHtml(HCkDh cHandle, HCkString retval);
void CkDh_getLastErrorText(HCkDh cHandle, HCkString retval);
void CkDh_getLastErrorXml(HCkDh cHandle, HCkString retval);
void CkDh_getP(HCkDh cHandle, HCkString retval);
BOOL CkDh_getUtf8(HCkDh cHandle);
void CkDh_putUtf8(HCkDh cHandle, BOOL newVal);
BOOL CkDh_getVerboseLogging(HCkDh cHandle);
void CkDh_putVerboseLogging(HCkDh cHandle, BOOL newVal);
void CkDh_getVersion(HCkDh cHandle, HCkString retval);
BOOL CkDh_CreateE(HCkDh cHandle, int numBits, HCkString outStr);
BOOL CkDh_FindK(HCkDh cHandle, const char *e, HCkString outStr);
BOOL CkDh_GenPG(HCkDh cHandle, int numBits, int g);
BOOL CkDh_SaveLastError(HCkDh cHandle, const char *path);
BOOL CkDh_SetPG(HCkDh cHandle, const char *p, int g);
BOOL CkDh_UnlockComponent(HCkDh cHandle, const char *unlockCode);
void CkDh_UseKnownPrime(HCkDh cHandle, int index);
const char *CkDh_createE(HCkDh cHandle, int numBits);
const char *CkDh_debugLogFilePath(HCkDh cHandle);
const char *CkDh_findK(HCkDh cHandle, const char *e);
const char *CkDh_lastErrorHtml(HCkDh cHandle);
const char *CkDh_lastErrorText(HCkDh cHandle);
const char *CkDh_lastErrorXml(HCkDh cHandle);
const char *CkDh_p(HCkDh cHandle);
const char *CkDh_version(HCkDh cHandle);
#endif
