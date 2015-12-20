#ifndef _C_CkDkim_H
#define _C_CkDkim_H
#include "Chilkat_C.h"

HCkDkim CkDkim_Create(void);
void CkDkim_Dispose(HCkDkim handle);
void CkDkim_getDebugLogFilePath(HCkDkim cHandle, HCkString retval);
void CkDkim_putDebugLogFilePath(HCkDkim cHandle, const char *newVal);
void CkDkim_getDkimAlg(HCkDkim cHandle, HCkString retval);
void CkDkim_putDkimAlg(HCkDkim cHandle, const char *newVal);
int CkDkim_getDkimBodyLengthCount(HCkDkim cHandle);
void CkDkim_putDkimBodyLengthCount(HCkDkim cHandle, int newVal);
void CkDkim_getDkimCanon(HCkDkim cHandle, HCkString retval);
void CkDkim_putDkimCanon(HCkDkim cHandle, const char *newVal);
void CkDkim_getDkimDomain(HCkDkim cHandle, HCkString retval);
void CkDkim_putDkimDomain(HCkDkim cHandle, const char *newVal);
void CkDkim_getDkimHeaders(HCkDkim cHandle, HCkString retval);
void CkDkim_putDkimHeaders(HCkDkim cHandle, const char *newVal);
void CkDkim_getDkimSelector(HCkDkim cHandle, HCkString retval);
void CkDkim_putDkimSelector(HCkDkim cHandle, const char *newVal);
void CkDkim_getDomainKeyAlg(HCkDkim cHandle, HCkString retval);
void CkDkim_putDomainKeyAlg(HCkDkim cHandle, const char *newVal);
void CkDkim_getDomainKeyCanon(HCkDkim cHandle, HCkString retval);
void CkDkim_putDomainKeyCanon(HCkDkim cHandle, const char *newVal);
void CkDkim_getDomainKeyDomain(HCkDkim cHandle, HCkString retval);
void CkDkim_putDomainKeyDomain(HCkDkim cHandle, const char *newVal);
void CkDkim_getDomainKeyHeaders(HCkDkim cHandle, HCkString retval);
void CkDkim_putDomainKeyHeaders(HCkDkim cHandle, const char *newVal);
void CkDkim_getDomainKeySelector(HCkDkim cHandle, HCkString retval);
void CkDkim_putDomainKeySelector(HCkDkim cHandle, const char *newVal);
HCkDkimProgress CkDkim_getEventCallbackObject(HCkDkim cHandle);
void CkDkim_putEventCallbackObject(HCkDkim cHandle, HCkDkimProgress newVal);
void CkDkim_getLastErrorHtml(HCkDkim cHandle, HCkString retval);
void CkDkim_getLastErrorText(HCkDkim cHandle, HCkString retval);
void CkDkim_getLastErrorXml(HCkDkim cHandle, HCkString retval);
BOOL CkDkim_getUtf8(HCkDkim cHandle);
void CkDkim_putUtf8(HCkDkim cHandle, BOOL newVal);
BOOL CkDkim_getVerboseLogging(HCkDkim cHandle);
void CkDkim_putVerboseLogging(HCkDkim cHandle, BOOL newVal);
void CkDkim_getVersion(HCkDkim cHandle, HCkString retval);
BOOL CkDkim_AddDkimSignature(HCkDkim cHandle, HCkByteData mimeIn, HCkByteData outBytes);
BOOL CkDkim_AddDomainKeySignature(HCkDkim cHandle, HCkByteData mimeIn, HCkByteData outBytes);
BOOL CkDkim_LoadDkimPk(HCkDkim cHandle, const char *privateKey, const char *optionalPassword);
BOOL CkDkim_LoadDkimPkBytes(HCkDkim cHandle, HCkByteData privateKeyDer, const char *optionalPassword);
BOOL CkDkim_LoadDkimPkFile(HCkDkim cHandle, const char *privateKeyFilepath, const char *optionalPassword);
BOOL CkDkim_LoadDomainKeyPk(HCkDkim cHandle, const char *privateKey, const char *optionalPassword);
BOOL CkDkim_LoadDomainKeyPkBytes(HCkDkim cHandle, HCkByteData privateKeyDer, const char *optionalPassword);
BOOL CkDkim_LoadDomainKeyPkFile(HCkDkim cHandle, const char *privateKeyFilepath, const char *optionalPassword);
BOOL CkDkim_LoadPublicKey(HCkDkim cHandle, const char *selector, const char *domain, const char *publicKey);
BOOL CkDkim_LoadPublicKeyFile(HCkDkim cHandle, const char *selector, const char *domain, const char *publicKeyFilepath);
int CkDkim_NumDkimSignatures(HCkDkim cHandle, HCkByteData mimeData);
int CkDkim_NumDomainKeySignatures(HCkDkim cHandle, HCkByteData mimeData);
BOOL CkDkim_PrefetchPublicKey(HCkDkim cHandle, const char *selector, const char *domain);
BOOL CkDkim_SaveLastError(HCkDkim cHandle, const char *path);
BOOL CkDkim_UnlockComponent(HCkDkim cHandle, const char *unlockCode);
BOOL CkDkim_VerifyDkimSignature(HCkDkim cHandle, int sigIdx, HCkByteData mimeData);
BOOL CkDkim_VerifyDomainKeySignature(HCkDkim cHandle, int sigIdx, HCkByteData mimeData);
const char *CkDkim_debugLogFilePath(HCkDkim cHandle);
const char *CkDkim_dkimAlg(HCkDkim cHandle);
const char *CkDkim_dkimCanon(HCkDkim cHandle);
const char *CkDkim_dkimDomain(HCkDkim cHandle);
const char *CkDkim_dkimHeaders(HCkDkim cHandle);
const char *CkDkim_dkimSelector(HCkDkim cHandle);
const char *CkDkim_domainKeyAlg(HCkDkim cHandle);
const char *CkDkim_domainKeyCanon(HCkDkim cHandle);
const char *CkDkim_domainKeyDomain(HCkDkim cHandle);
const char *CkDkim_domainKeyHeaders(HCkDkim cHandle);
const char *CkDkim_domainKeySelector(HCkDkim cHandle);
const char *CkDkim_lastErrorHtml(HCkDkim cHandle);
const char *CkDkim_lastErrorText(HCkDkim cHandle);
const char *CkDkim_lastErrorXml(HCkDkim cHandle);
const char *CkDkim_version(HCkDkim cHandle);
#endif