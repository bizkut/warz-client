#ifndef _C_CkDh_W_H
#define _C_CkDh_W_H
#include "Chilkat_C.h"

HCkDhW CkDhW_Create(void);
HCkDhW CkDhW_Create2(bool bForMono);
void CkDhW_Dispose(HCkDhW handle);
void CkDhW_getDebugLogFilePath(HCkDhW cHandle, HCkStringW retval);
void CkDhW_putDebugLogFilePath(HCkDhW cHandle, const wchar_t *newVal);
HCkDhProgressW CkDhW_getEventCallbackObject(HCkDhW cHandle);
void CkDhW_putEventCallbackObject(HCkDhW cHandle, HCkDhProgressW newVal);
int CkDhW_getG(HCkDhW cHandle);
void CkDhW_getLastErrorHtml(HCkDhW cHandle, HCkStringW retval);
void CkDhW_getLastErrorText(HCkDhW cHandle, HCkStringW retval);
void CkDhW_getLastErrorXml(HCkDhW cHandle, HCkStringW retval);
void CkDhW_getP(HCkDhW cHandle, HCkStringW retval);
BOOL CkDhW_getVerboseLogging(HCkDhW cHandle);
void CkDhW_putVerboseLogging(HCkDhW cHandle, BOOL newVal);
void CkDhW_getVersion(HCkDhW cHandle, HCkStringW retval);
BOOL CkDhW_CreateE(HCkDhW cHandle, int numBits, HCkStringW outStr);
BOOL CkDhW_FindK(HCkDhW cHandle, const wchar_t *e, HCkStringW outStr);
BOOL CkDhW_GenPG(HCkDhW cHandle, int numBits, int g);
BOOL CkDhW_SaveLastError(HCkDhW cHandle, const wchar_t *path);
BOOL CkDhW_SetPG(HCkDhW cHandle, const wchar_t *p, int g);
BOOL CkDhW_UnlockComponent(HCkDhW cHandle, const wchar_t *unlockCode);
void CkDhW_UseKnownPrime(HCkDhW cHandle, int index);
const wchar_t *CkDhW__createE(HCkDhW cHandle, int numBits);
const wchar_t *CkDhW__debugLogFilePath(HCkDhW cHandle);
const wchar_t *CkDhW__findK(HCkDhW cHandle, const wchar_t *e);
const wchar_t *CkDhW__lastErrorHtml(HCkDhW cHandle);
const wchar_t *CkDhW__lastErrorText(HCkDhW cHandle);
const wchar_t *CkDhW__lastErrorXml(HCkDhW cHandle);
const wchar_t *CkDhW__p(HCkDhW cHandle);
const wchar_t *CkDhW__version(HCkDhW cHandle);
#endif
