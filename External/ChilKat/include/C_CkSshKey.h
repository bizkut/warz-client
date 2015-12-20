#ifndef _C_CkSshKey_H
#define _C_CkSshKey_H
#include "Chilkat_C.h"

HCkSshKey CkSshKey_Create(void);
void CkSshKey_Dispose(HCkSshKey handle);
void CkSshKey_getComment(HCkSshKey cHandle, HCkString retval);
void CkSshKey_putComment(HCkSshKey cHandle, const char *newVal);
void CkSshKey_getDebugLogFilePath(HCkSshKey cHandle, HCkString retval);
void CkSshKey_putDebugLogFilePath(HCkSshKey cHandle, const char *newVal);
BOOL CkSshKey_getIsDsaKey(HCkSshKey cHandle);
BOOL CkSshKey_getIsPrivateKey(HCkSshKey cHandle);
BOOL CkSshKey_getIsRsaKey(HCkSshKey cHandle);
void CkSshKey_getLastErrorHtml(HCkSshKey cHandle, HCkString retval);
void CkSshKey_getLastErrorText(HCkSshKey cHandle, HCkString retval);
void CkSshKey_getLastErrorXml(HCkSshKey cHandle, HCkString retval);
void CkSshKey_getPassword(HCkSshKey cHandle, HCkString retval);
void CkSshKey_putPassword(HCkSshKey cHandle, const char *newVal);
BOOL CkSshKey_getUtf8(HCkSshKey cHandle);
void CkSshKey_putUtf8(HCkSshKey cHandle, BOOL newVal);
BOOL CkSshKey_getVerboseLogging(HCkSshKey cHandle);
void CkSshKey_putVerboseLogging(HCkSshKey cHandle, BOOL newVal);
void CkSshKey_getVersion(HCkSshKey cHandle, HCkString retval);
BOOL CkSshKey_FromOpenSshPrivateKey(HCkSshKey cHandle, const char *keyStr);
BOOL CkSshKey_FromOpenSshPublicKey(HCkSshKey cHandle, const char *keyStr);
BOOL CkSshKey_FromPuttyPrivateKey(HCkSshKey cHandle, const char *keyStr);
BOOL CkSshKey_FromRfc4716PublicKey(HCkSshKey cHandle, const char *keyStr);
BOOL CkSshKey_FromXml(HCkSshKey cHandle, const char *xmlKey);
BOOL CkSshKey_GenFingerprint(HCkSshKey cHandle, HCkString outStr);
BOOL CkSshKey_GenerateDsaKey(HCkSshKey cHandle, int numBits);
BOOL CkSshKey_GenerateRsaKey(HCkSshKey cHandle, int numBits, int exponent);
BOOL CkSshKey_LoadText(HCkSshKey cHandle, const char *path, HCkString outStr);
BOOL CkSshKey_SaveLastError(HCkSshKey cHandle, const char *path);
BOOL CkSshKey_SaveText(HCkSshKey cHandle, const char *strToSave, const char *path);
BOOL CkSshKey_ToOpenSshPrivateKey(HCkSshKey cHandle, BOOL bEncrypt, HCkString outStr);
BOOL CkSshKey_ToOpenSshPublicKey(HCkSshKey cHandle, HCkString outStr);
BOOL CkSshKey_ToPuttyPrivateKey(HCkSshKey cHandle, BOOL bEncrypt, HCkString outStr);
BOOL CkSshKey_ToRfc4716PublicKey(HCkSshKey cHandle, HCkString outStr);
BOOL CkSshKey_ToXml(HCkSshKey cHandle, HCkString outStr);
const char *CkSshKey_comment(HCkSshKey cHandle);
const char *CkSshKey_debugLogFilePath(HCkSshKey cHandle);
const char *CkSshKey_genFingerprint(HCkSshKey cHandle);
const char *CkSshKey_lastErrorHtml(HCkSshKey cHandle);
const char *CkSshKey_lastErrorText(HCkSshKey cHandle);
const char *CkSshKey_lastErrorXml(HCkSshKey cHandle);
const char *CkSshKey_loadText(HCkSshKey cHandle, const char *path);
const char *CkSshKey_password(HCkSshKey cHandle);
const char *CkSshKey_toOpenSshPrivateKey(HCkSshKey cHandle, BOOL bEncrypt);
const char *CkSshKey_toOpenSshPublicKey(HCkSshKey cHandle);
const char *CkSshKey_toPuttyPrivateKey(HCkSshKey cHandle, BOOL bEncrypt);
const char *CkSshKey_toRfc4716PublicKey(HCkSshKey cHandle);
const char *CkSshKey_toXml(HCkSshKey cHandle);
const char *CkSshKey_version(HCkSshKey cHandle);
#endif
