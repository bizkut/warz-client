#ifndef _C_CkPublicKey_W_H
#define _C_CkPublicKey_W_H
#include "Chilkat_C.h"

HCkPublicKeyW CkPublicKeyW_Create(void);
HCkPublicKeyW CkPublicKeyW_Create2(bool bForMono);
void CkPublicKeyW_Dispose(HCkPublicKeyW handle);
void CkPublicKeyW_getDebugLogFilePath(HCkPublicKeyW cHandle, HCkStringW retval);
void CkPublicKeyW_putDebugLogFilePath(HCkPublicKeyW cHandle, const wchar_t *newVal);
void CkPublicKeyW_getLastErrorHtml(HCkPublicKeyW cHandle, HCkStringW retval);
void CkPublicKeyW_getLastErrorText(HCkPublicKeyW cHandle, HCkStringW retval);
void CkPublicKeyW_getLastErrorXml(HCkPublicKeyW cHandle, HCkStringW retval);
BOOL CkPublicKeyW_getVerboseLogging(HCkPublicKeyW cHandle);
void CkPublicKeyW_putVerboseLogging(HCkPublicKeyW cHandle, BOOL newVal);
void CkPublicKeyW_getVersion(HCkPublicKeyW cHandle, HCkStringW retval);
BOOL CkPublicKeyW_GetOpenSslDer(HCkPublicKeyW cHandle, HCkByteDataW outData);
BOOL CkPublicKeyW_GetOpenSslPem(HCkPublicKeyW cHandle, HCkStringW outStr);
BOOL CkPublicKeyW_GetRsaDer(HCkPublicKeyW cHandle, HCkByteDataW outData);
BOOL CkPublicKeyW_GetXml(HCkPublicKeyW cHandle, HCkStringW outStr);
BOOL CkPublicKeyW_LoadOpenSslDer(HCkPublicKeyW cHandle, HCkByteDataW data);
BOOL CkPublicKeyW_LoadOpenSslDerFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_LoadOpenSslPem(HCkPublicKeyW cHandle, const wchar_t *str);
BOOL CkPublicKeyW_LoadOpenSslPemFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_LoadPkcs1Pem(HCkPublicKeyW cHandle, const wchar_t *str);
BOOL CkPublicKeyW_LoadRsaDer(HCkPublicKeyW cHandle, HCkByteDataW data);
BOOL CkPublicKeyW_LoadRsaDerFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_LoadXml(HCkPublicKeyW cHandle, const wchar_t *xml);
BOOL CkPublicKeyW_LoadXmlFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_SaveLastError(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_SaveOpenSslDerFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_SaveOpenSslPemFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_SaveRsaDerFile(HCkPublicKeyW cHandle, const wchar_t *path);
BOOL CkPublicKeyW_SaveXmlFile(HCkPublicKeyW cHandle, const wchar_t *path);
const wchar_t *CkPublicKeyW__debugLogFilePath(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__getOpenSslPem(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__getXml(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__lastErrorHtml(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__lastErrorText(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__lastErrorXml(HCkPublicKeyW cHandle);
const wchar_t *CkPublicKeyW__version(HCkPublicKeyW cHandle);
#endif
