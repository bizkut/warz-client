#ifndef _C_CkPublicKey_H
#define _C_CkPublicKey_H
#include "Chilkat_C.h"

HCkPublicKey CkPublicKey_Create(void);
void CkPublicKey_Dispose(HCkPublicKey handle);
void CkPublicKey_getDebugLogFilePath(HCkPublicKey cHandle, HCkString retval);
void CkPublicKey_putDebugLogFilePath(HCkPublicKey cHandle, const char *newVal);
void CkPublicKey_getLastErrorHtml(HCkPublicKey cHandle, HCkString retval);
void CkPublicKey_getLastErrorText(HCkPublicKey cHandle, HCkString retval);
void CkPublicKey_getLastErrorXml(HCkPublicKey cHandle, HCkString retval);
BOOL CkPublicKey_getUtf8(HCkPublicKey cHandle);
void CkPublicKey_putUtf8(HCkPublicKey cHandle, BOOL newVal);
BOOL CkPublicKey_getVerboseLogging(HCkPublicKey cHandle);
void CkPublicKey_putVerboseLogging(HCkPublicKey cHandle, BOOL newVal);
void CkPublicKey_getVersion(HCkPublicKey cHandle, HCkString retval);
BOOL CkPublicKey_GetOpenSslDer(HCkPublicKey cHandle, HCkByteData outData);
BOOL CkPublicKey_GetOpenSslPem(HCkPublicKey cHandle, HCkString outStr);
BOOL CkPublicKey_GetRsaDer(HCkPublicKey cHandle, HCkByteData outData);
BOOL CkPublicKey_GetXml(HCkPublicKey cHandle, HCkString outStr);
BOOL CkPublicKey_LoadOpenSslDer(HCkPublicKey cHandle, HCkByteData data);
BOOL CkPublicKey_LoadOpenSslDerFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_LoadOpenSslPem(HCkPublicKey cHandle, const char *str);
BOOL CkPublicKey_LoadOpenSslPemFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_LoadPkcs1Pem(HCkPublicKey cHandle, const char *str);
BOOL CkPublicKey_LoadRsaDer(HCkPublicKey cHandle, HCkByteData data);
BOOL CkPublicKey_LoadRsaDerFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_LoadXml(HCkPublicKey cHandle, const char *xml);
BOOL CkPublicKey_LoadXmlFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_SaveLastError(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_SaveOpenSslDerFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_SaveOpenSslPemFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_SaveRsaDerFile(HCkPublicKey cHandle, const char *path);
BOOL CkPublicKey_SaveXmlFile(HCkPublicKey cHandle, const char *path);
const char *CkPublicKey_debugLogFilePath(HCkPublicKey cHandle);
const char *CkPublicKey_getOpenSslPem(HCkPublicKey cHandle);
const char *CkPublicKey_getXml(HCkPublicKey cHandle);
const char *CkPublicKey_lastErrorHtml(HCkPublicKey cHandle);
const char *CkPublicKey_lastErrorText(HCkPublicKey cHandle);
const char *CkPublicKey_lastErrorXml(HCkPublicKey cHandle);
const char *CkPublicKey_version(HCkPublicKey cHandle);
#endif
