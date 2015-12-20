#if defined(WIN32) || defined(WINCE)

#ifndef _C_CkCsp_H
#define _C_CkCsp_H
#include "Chilkat_C.h"

HCkCsp CkCsp_Create(void);
void CkCsp_Dispose(HCkCsp handle);
void CkCsp_getDebugLogFilePath(HCkCsp cHandle, HCkString retval);
void CkCsp_putDebugLogFilePath(HCkCsp cHandle, const char *newVal);
void CkCsp_getEncryptAlgorithm(HCkCsp cHandle, HCkString retval);
long CkCsp_getEncryptAlgorithmID(HCkCsp cHandle);
long CkCsp_getEncryptNumBits(HCkCsp cHandle);
void CkCsp_getHashAlgorithm(HCkCsp cHandle, HCkString retval);
long CkCsp_getHashAlgorithmID(HCkCsp cHandle);
long CkCsp_getHashNumBits(HCkCsp cHandle);
void CkCsp_getKeyContainerName(HCkCsp cHandle, HCkString retval);
void CkCsp_putKeyContainerName(HCkCsp cHandle, const char *newVal);
void CkCsp_getLastErrorHtml(HCkCsp cHandle, HCkString retval);
void CkCsp_getLastErrorText(HCkCsp cHandle, HCkString retval);
void CkCsp_getLastErrorXml(HCkCsp cHandle, HCkString retval);
BOOL CkCsp_getMachineKeyset(HCkCsp cHandle);
void CkCsp_putMachineKeyset(HCkCsp cHandle, BOOL newVal);
long CkCsp_getNumEncryptAlgorithms(HCkCsp cHandle);
long CkCsp_getNumHashAlgorithms(HCkCsp cHandle);
long CkCsp_getNumKeyContainers(HCkCsp cHandle);
long CkCsp_getNumKeyExchangeAlgorithms(HCkCsp cHandle);
long CkCsp_getNumSignatureAlgorithms(HCkCsp cHandle);
void CkCsp_getProviderName(HCkCsp cHandle, HCkString retval);
void CkCsp_putProviderName(HCkCsp cHandle, const char *newVal);
long CkCsp_getProviderType(HCkCsp cHandle);
BOOL CkCsp_getUtf8(HCkCsp cHandle);
void CkCsp_putUtf8(HCkCsp cHandle, BOOL newVal);
BOOL CkCsp_getVerboseLogging(HCkCsp cHandle);
void CkCsp_putVerboseLogging(HCkCsp cHandle, BOOL newVal);
void CkCsp_getVersion(HCkCsp cHandle, HCkString retval);
HCkStringArray CkCsp_GetKeyContainerNames(HCkCsp cHandle);
BOOL CkCsp_HasEncryptAlgorithm(HCkCsp cHandle, const char *name, long numBits);
BOOL CkCsp_HasHashAlgorithm(HCkCsp cHandle, const char *name, long numBits);
BOOL CkCsp_Initialize(HCkCsp cHandle);
BOOL CkCsp_NthEncryptionAlgorithm(HCkCsp cHandle, long index, HCkString outName);
long CkCsp_NthEncryptionNumBits(HCkCsp cHandle, long index);
BOOL CkCsp_NthHashAlgorithmName(HCkCsp cHandle, long index, HCkString outName);
long CkCsp_NthHashNumBits(HCkCsp cHandle, long index);
BOOL CkCsp_NthKeyContainerName(HCkCsp cHandle, long index, HCkString outName);
BOOL CkCsp_NthKeyExchangeAlgorithm(HCkCsp cHandle, long index, HCkString outName);
long CkCsp_NthKeyExchangeNumBits(HCkCsp cHandle, long index);
BOOL CkCsp_NthSignatureAlgorithm(HCkCsp cHandle, long index, HCkString outName);
long CkCsp_NthSignatureNumBits(HCkCsp cHandle, long index);
BOOL CkCsp_SaveLastError(HCkCsp cHandle, const char *path);
long CkCsp_SetEncryptAlgorithm(HCkCsp cHandle, const char *name);
long CkCsp_SetHashAlgorithm(HCkCsp cHandle, const char *name);
BOOL CkCsp_SetProviderMicrosoftBase(HCkCsp cHandle);
BOOL CkCsp_SetProviderMicrosoftEnhanced(HCkCsp cHandle);
BOOL CkCsp_SetProviderMicrosoftRsaAes(HCkCsp cHandle);
BOOL CkCsp_SetProviderMicrosoftStrong(HCkCsp cHandle);
const char *CkCsp_debugLogFilePath(HCkCsp cHandle);
const char *CkCsp_encryptAlgorithm(HCkCsp cHandle);
const char *CkCsp_hashAlgorithm(HCkCsp cHandle);
const char *CkCsp_keyContainerName(HCkCsp cHandle);
const char *CkCsp_lastErrorHtml(HCkCsp cHandle);
const char *CkCsp_lastErrorText(HCkCsp cHandle);
const char *CkCsp_lastErrorXml(HCkCsp cHandle);
const char *CkCsp_nthEncryptionAlgorithm(HCkCsp cHandle, long index);
const char *CkCsp_nthHashAlgorithmName(HCkCsp cHandle, long index);
const char *CkCsp_nthKeyContainerName(HCkCsp cHandle, long index);
const char *CkCsp_nthKeyExchangeAlgorithm(HCkCsp cHandle, long index);
const char *CkCsp_nthSignatureAlgorithm(HCkCsp cHandle, long index);
const char *CkCsp_providerName(HCkCsp cHandle);
const char *CkCsp_version(HCkCsp cHandle);
#endif

#endif // WIN32 (entire file)
