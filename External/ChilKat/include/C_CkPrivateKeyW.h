#ifndef _C_CkPrivateKey_W_H
#define _C_CkPrivateKey_W_H
#include "Chilkat_C.h"

HCkPrivateKeyW CkPrivateKeyW_Create(void);
HCkPrivateKeyW CkPrivateKeyW_Create2(bool bForMono);
void CkPrivateKeyW_Dispose(HCkPrivateKeyW handle);
void CkPrivateKeyW_getDebugLogFilePath(HCkPrivateKeyW cHandle, HCkStringW retval);
void CkPrivateKeyW_putDebugLogFilePath(HCkPrivateKeyW cHandle, const wchar_t *newVal);
void CkPrivateKeyW_getLastErrorHtml(HCkPrivateKeyW cHandle, HCkStringW retval);
void CkPrivateKeyW_getLastErrorText(HCkPrivateKeyW cHandle, HCkStringW retval);
void CkPrivateKeyW_getLastErrorXml(HCkPrivateKeyW cHandle, HCkStringW retval);
BOOL CkPrivateKeyW_getVerboseLogging(HCkPrivateKeyW cHandle);
void CkPrivateKeyW_putVerboseLogging(HCkPrivateKeyW cHandle, BOOL newVal);
void CkPrivateKeyW_getVersion(HCkPrivateKeyW cHandle, HCkStringW retval);
BOOL CkPrivateKeyW_GetPkcs8(HCkPrivateKeyW cHandle, HCkByteDataW outData);
BOOL CkPrivateKeyW_GetPkcs8Encrypted(HCkPrivateKeyW cHandle, const wchar_t *password, HCkByteDataW outBytes);
BOOL CkPrivateKeyW_GetPkcs8EncryptedPem(HCkPrivateKeyW cHandle, const wchar_t *password, HCkStringW outStr);
BOOL CkPrivateKeyW_GetPkcs8Pem(HCkPrivateKeyW cHandle, HCkStringW outStr);
BOOL CkPrivateKeyW_GetRsaDer(HCkPrivateKeyW cHandle, HCkByteDataW outData);
BOOL CkPrivateKeyW_GetRsaPem(HCkPrivateKeyW cHandle, HCkStringW outStr);
BOOL CkPrivateKeyW_GetXml(HCkPrivateKeyW cHandle, HCkStringW outStr);
BOOL CkPrivateKeyW_LoadEncryptedPem(HCkPrivateKeyW cHandle, const wchar_t *pemStr, const wchar_t *password);
BOOL CkPrivateKeyW_LoadEncryptedPemFile(HCkPrivateKeyW cHandle, const wchar_t *path, const wchar_t *password);
BOOL CkPrivateKeyW_LoadPem(HCkPrivateKeyW cHandle, const wchar_t *str);
BOOL CkPrivateKeyW_LoadPemFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_LoadPkcs8(HCkPrivateKeyW cHandle, HCkByteDataW data);
BOOL CkPrivateKeyW_LoadPkcs8Encrypted(HCkPrivateKeyW cHandle, HCkByteDataW data, const wchar_t *password);
BOOL CkPrivateKeyW_LoadPkcs8EncryptedFile(HCkPrivateKeyW cHandle, const wchar_t *path, const wchar_t *password);
BOOL CkPrivateKeyW_LoadPkcs8File(HCkPrivateKeyW cHandle, const wchar_t *path);
#if defined(WIN32) && !defined(__MINGW32__)
#ifdef WIN32
BOOL CkPrivateKeyW_LoadPvk(HCkPrivateKeyW cHandle, HCkByteDataW data, const wchar_t *password);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#ifdef WIN32
BOOL CkPrivateKeyW_LoadPvkFile(HCkPrivateKeyW cHandle, const wchar_t *path, const wchar_t *password);
#endif
#endif
BOOL CkPrivateKeyW_LoadRsaDer(HCkPrivateKeyW cHandle, HCkByteDataW data);
BOOL CkPrivateKeyW_LoadRsaDerFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_LoadXml(HCkPrivateKeyW cHandle, const wchar_t *xml);
BOOL CkPrivateKeyW_LoadXmlFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SaveLastError(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SavePkcs8EncryptedFile(HCkPrivateKeyW cHandle, const wchar_t *password, const wchar_t *path);
BOOL CkPrivateKeyW_SavePkcs8EncryptedPemFile(HCkPrivateKeyW cHandle, const wchar_t *password, const wchar_t *path);
BOOL CkPrivateKeyW_SavePkcs8File(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SavePkcs8PemFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SaveRsaDerFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SaveRsaPemFile(HCkPrivateKeyW cHandle, const wchar_t *path);
BOOL CkPrivateKeyW_SaveXmlFile(HCkPrivateKeyW cHandle, const wchar_t *path);
const wchar_t *CkPrivateKeyW__debugLogFilePath(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__getPkcs8EncryptedPem(HCkPrivateKeyW cHandle, const wchar_t *password);
const wchar_t *CkPrivateKeyW__getPkcs8Pem(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__getRsaPem(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__getXml(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__lastErrorHtml(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__lastErrorText(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__lastErrorXml(HCkPrivateKeyW cHandle);
const wchar_t *CkPrivateKeyW__version(HCkPrivateKeyW cHandle);
#endif
