#ifndef _C_CkPrivateKey_H
#define _C_CkPrivateKey_H
#include "Chilkat_C.h"

HCkPrivateKey CkPrivateKey_Create(void);
void CkPrivateKey_Dispose(HCkPrivateKey handle);
void CkPrivateKey_getDebugLogFilePath(HCkPrivateKey cHandle, HCkString retval);
void CkPrivateKey_putDebugLogFilePath(HCkPrivateKey cHandle, const char *newVal);
void CkPrivateKey_getLastErrorHtml(HCkPrivateKey cHandle, HCkString retval);
void CkPrivateKey_getLastErrorText(HCkPrivateKey cHandle, HCkString retval);
void CkPrivateKey_getLastErrorXml(HCkPrivateKey cHandle, HCkString retval);
BOOL CkPrivateKey_getUtf8(HCkPrivateKey cHandle);
void CkPrivateKey_putUtf8(HCkPrivateKey cHandle, BOOL newVal);
BOOL CkPrivateKey_getVerboseLogging(HCkPrivateKey cHandle);
void CkPrivateKey_putVerboseLogging(HCkPrivateKey cHandle, BOOL newVal);
void CkPrivateKey_getVersion(HCkPrivateKey cHandle, HCkString retval);
BOOL CkPrivateKey_GetPkcs8(HCkPrivateKey cHandle, HCkByteData outData);
BOOL CkPrivateKey_GetPkcs8Encrypted(HCkPrivateKey cHandle, const char *password, HCkByteData outBytes);
BOOL CkPrivateKey_GetPkcs8EncryptedPem(HCkPrivateKey cHandle, const char *password, HCkString outStr);
BOOL CkPrivateKey_GetPkcs8Pem(HCkPrivateKey cHandle, HCkString outStr);
BOOL CkPrivateKey_GetRsaDer(HCkPrivateKey cHandle, HCkByteData outData);
BOOL CkPrivateKey_GetRsaPem(HCkPrivateKey cHandle, HCkString outStr);
BOOL CkPrivateKey_GetXml(HCkPrivateKey cHandle, HCkString outStr);
BOOL CkPrivateKey_LoadEncryptedPem(HCkPrivateKey cHandle, const char *pemStr, const char *password);
BOOL CkPrivateKey_LoadEncryptedPemFile(HCkPrivateKey cHandle, const char *path, const char *password);
BOOL CkPrivateKey_LoadPem(HCkPrivateKey cHandle, const char *str);
BOOL CkPrivateKey_LoadPemFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_LoadPkcs8(HCkPrivateKey cHandle, HCkByteData data);
BOOL CkPrivateKey_LoadPkcs8Encrypted(HCkPrivateKey cHandle, HCkByteData data, const char *password);
BOOL CkPrivateKey_LoadPkcs8EncryptedFile(HCkPrivateKey cHandle, const char *path, const char *password);
BOOL CkPrivateKey_LoadPkcs8File(HCkPrivateKey cHandle, const char *path);
#if defined(WIN32) && !defined(__MINGW32__)
#ifdef WIN32
BOOL CkPrivateKey_LoadPvk(HCkPrivateKey cHandle, HCkByteData data, const char *password);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#ifdef WIN32
BOOL CkPrivateKey_LoadPvkFile(HCkPrivateKey cHandle, const char *path, const char *password);
#endif
#endif
BOOL CkPrivateKey_LoadRsaDer(HCkPrivateKey cHandle, HCkByteData data);
BOOL CkPrivateKey_LoadRsaDerFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_LoadXml(HCkPrivateKey cHandle, const char *xml);
BOOL CkPrivateKey_LoadXmlFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SaveLastError(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SavePkcs8EncryptedFile(HCkPrivateKey cHandle, const char *password, const char *path);
BOOL CkPrivateKey_SavePkcs8EncryptedPemFile(HCkPrivateKey cHandle, const char *password, const char *path);
BOOL CkPrivateKey_SavePkcs8File(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SavePkcs8PemFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SaveRsaDerFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SaveRsaPemFile(HCkPrivateKey cHandle, const char *path);
BOOL CkPrivateKey_SaveXmlFile(HCkPrivateKey cHandle, const char *path);
const char *CkPrivateKey_debugLogFilePath(HCkPrivateKey cHandle);
const char *CkPrivateKey_getPkcs8EncryptedPem(HCkPrivateKey cHandle, const char *password);
const char *CkPrivateKey_getPkcs8Pem(HCkPrivateKey cHandle);
const char *CkPrivateKey_getRsaPem(HCkPrivateKey cHandle);
const char *CkPrivateKey_getXml(HCkPrivateKey cHandle);
const char *CkPrivateKey_lastErrorHtml(HCkPrivateKey cHandle);
const char *CkPrivateKey_lastErrorText(HCkPrivateKey cHandle);
const char *CkPrivateKey_lastErrorXml(HCkPrivateKey cHandle);
const char *CkPrivateKey_version(HCkPrivateKey cHandle);
#endif
