#ifndef _C_CkSshKey_W_H
#define _C_CkSshKey_W_H
#include "Chilkat_C.h"

HCkSshKeyW CkSshKeyW_Create(void);
HCkSshKeyW CkSshKeyW_Create2(bool bForMono);
void CkSshKeyW_Dispose(HCkSshKeyW handle);
void CkSshKeyW_getComment(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_putComment(HCkSshKeyW cHandle, const wchar_t *newVal);
void CkSshKeyW_getDebugLogFilePath(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_putDebugLogFilePath(HCkSshKeyW cHandle, const wchar_t *newVal);
BOOL CkSshKeyW_getIsDsaKey(HCkSshKeyW cHandle);
BOOL CkSshKeyW_getIsPrivateKey(HCkSshKeyW cHandle);
BOOL CkSshKeyW_getIsRsaKey(HCkSshKeyW cHandle);
void CkSshKeyW_getLastErrorHtml(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_getLastErrorText(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_getLastErrorXml(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_getPassword(HCkSshKeyW cHandle, HCkStringW retval);
void CkSshKeyW_putPassword(HCkSshKeyW cHandle, const wchar_t *newVal);
BOOL CkSshKeyW_getVerboseLogging(HCkSshKeyW cHandle);
void CkSshKeyW_putVerboseLogging(HCkSshKeyW cHandle, BOOL newVal);
void CkSshKeyW_getVersion(HCkSshKeyW cHandle, HCkStringW retval);
BOOL CkSshKeyW_FromOpenSshPrivateKey(HCkSshKeyW cHandle, const wchar_t *keyStr);
BOOL CkSshKeyW_FromOpenSshPublicKey(HCkSshKeyW cHandle, const wchar_t *keyStr);
BOOL CkSshKeyW_FromPuttyPrivateKey(HCkSshKeyW cHandle, const wchar_t *keyStr);
BOOL CkSshKeyW_FromRfc4716PublicKey(HCkSshKeyW cHandle, const wchar_t *keyStr);
BOOL CkSshKeyW_FromXml(HCkSshKeyW cHandle, const wchar_t *xmlKey);
BOOL CkSshKeyW_GenFingerprint(HCkSshKeyW cHandle, HCkStringW outStr);
BOOL CkSshKeyW_GenerateDsaKey(HCkSshKeyW cHandle, int numBits);
BOOL CkSshKeyW_GenerateRsaKey(HCkSshKeyW cHandle, int numBits, int exponent);
BOOL CkSshKeyW_LoadText(HCkSshKeyW cHandle, const wchar_t *path, HCkStringW outStr);
BOOL CkSshKeyW_SaveLastError(HCkSshKeyW cHandle, const wchar_t *path);
BOOL CkSshKeyW_SaveText(HCkSshKeyW cHandle, const wchar_t *strToSave, const wchar_t *path);
BOOL CkSshKeyW_ToOpenSshPrivateKey(HCkSshKeyW cHandle, BOOL bEncrypt, HCkStringW outStr);
BOOL CkSshKeyW_ToOpenSshPublicKey(HCkSshKeyW cHandle, HCkStringW outStr);
BOOL CkSshKeyW_ToPuttyPrivateKey(HCkSshKeyW cHandle, BOOL bEncrypt, HCkStringW outStr);
BOOL CkSshKeyW_ToRfc4716PublicKey(HCkSshKeyW cHandle, HCkStringW outStr);
BOOL CkSshKeyW_ToXml(HCkSshKeyW cHandle, HCkStringW outStr);
const wchar_t *CkSshKeyW__comment(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__debugLogFilePath(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__genFingerprint(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__lastErrorHtml(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__lastErrorText(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__lastErrorXml(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__loadText(HCkSshKeyW cHandle, const wchar_t *path);
const wchar_t *CkSshKeyW__password(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__toOpenSshPrivateKey(HCkSshKeyW cHandle, BOOL bEncrypt);
const wchar_t *CkSshKeyW__toOpenSshPublicKey(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__toPuttyPrivateKey(HCkSshKeyW cHandle, BOOL bEncrypt);
const wchar_t *CkSshKeyW__toRfc4716PublicKey(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__toXml(HCkSshKeyW cHandle);
const wchar_t *CkSshKeyW__version(HCkSshKeyW cHandle);
#endif
