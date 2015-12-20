#if defined(WIN32) || defined(WINCE)

#ifndef _C_CkCsp_W_H
#define _C_CkCsp_W_H
#include "Chilkat_C.h"

HCkCspW CkCspW_Create(void);
HCkCspW CkCspW_Create2(bool bForMono);
void CkCspW_Dispose(HCkCspW handle);
void CkCspW_getDebugLogFilePath(HCkCspW cHandle, HCkStringW retval);
void CkCspW_putDebugLogFilePath(HCkCspW cHandle, const wchar_t *newVal);
void CkCspW_getEncryptAlgorithm(HCkCspW cHandle, HCkStringW retval);
long CkCspW_getEncryptAlgorithmID(HCkCspW cHandle);
long CkCspW_getEncryptNumBits(HCkCspW cHandle);
void CkCspW_getHashAlgorithm(HCkCspW cHandle, HCkStringW retval);
long CkCspW_getHashAlgorithmID(HCkCspW cHandle);
long CkCspW_getHashNumBits(HCkCspW cHandle);
void CkCspW_getKeyContainerName(HCkCspW cHandle, HCkStringW retval);
void CkCspW_putKeyContainerName(HCkCspW cHandle, const wchar_t *newVal);
void CkCspW_getLastErrorHtml(HCkCspW cHandle, HCkStringW retval);
void CkCspW_getLastErrorText(HCkCspW cHandle, HCkStringW retval);
void CkCspW_getLastErrorXml(HCkCspW cHandle, HCkStringW retval);
BOOL CkCspW_getMachineKeyset(HCkCspW cHandle);
void CkCspW_putMachineKeyset(HCkCspW cHandle, BOOL newVal);
long CkCspW_getNumEncryptAlgorithms(HCkCspW cHandle);
long CkCspW_getNumHashAlgorithms(HCkCspW cHandle);
long CkCspW_getNumKeyContainers(HCkCspW cHandle);
long CkCspW_getNumKeyExchangeAlgorithms(HCkCspW cHandle);
long CkCspW_getNumSignatureAlgorithms(HCkCspW cHandle);
void CkCspW_getProviderName(HCkCspW cHandle, HCkStringW retval);
void CkCspW_putProviderName(HCkCspW cHandle, const wchar_t *newVal);
long CkCspW_getProviderType(HCkCspW cHandle);
BOOL CkCspW_getVerboseLogging(HCkCspW cHandle);
void CkCspW_putVerboseLogging(HCkCspW cHandle, BOOL newVal);
void CkCspW_getVersion(HCkCspW cHandle, HCkStringW retval);
HCkStringArrayW CkCspW_GetKeyContainerNames(HCkCspW cHandle);
BOOL CkCspW_HasEncryptAlgorithm(HCkCspW cHandle, const wchar_t *name, long numBits);
BOOL CkCspW_HasHashAlgorithm(HCkCspW cHandle, const wchar_t *name, long numBits);
BOOL CkCspW_Initialize(HCkCspW cHandle);
BOOL CkCspW_NthEncryptionAlgorithm(HCkCspW cHandle, long index, HCkStringW outName);
long CkCspW_NthEncryptionNumBits(HCkCspW cHandle, long index);
BOOL CkCspW_NthHashAlgorithmName(HCkCspW cHandle, long index, HCkStringW outName);
long CkCspW_NthHashNumBits(HCkCspW cHandle, long index);
BOOL CkCspW_NthKeyContainerName(HCkCspW cHandle, long index, HCkStringW outName);
BOOL CkCspW_NthKeyExchangeAlgorithm(HCkCspW cHandle, long index, HCkStringW outName);
long CkCspW_NthKeyExchangeNumBits(HCkCspW cHandle, long index);
BOOL CkCspW_NthSignatureAlgorithm(HCkCspW cHandle, long index, HCkStringW outName);
long CkCspW_NthSignatureNumBits(HCkCspW cHandle, long index);
BOOL CkCspW_SaveLastError(HCkCspW cHandle, const wchar_t *path);
long CkCspW_SetEncryptAlgorithm(HCkCspW cHandle, const wchar_t *name);
long CkCspW_SetHashAlgorithm(HCkCspW cHandle, const wchar_t *name);
BOOL CkCspW_SetProviderMicrosoftBase(HCkCspW cHandle);
BOOL CkCspW_SetProviderMicrosoftEnhanced(HCkCspW cHandle);
BOOL CkCspW_SetProviderMicrosoftRsaAes(HCkCspW cHandle);
BOOL CkCspW_SetProviderMicrosoftStrong(HCkCspW cHandle);
const wchar_t *CkCspW__debugLogFilePath(HCkCspW cHandle);
const wchar_t *CkCspW__encryptAlgorithm(HCkCspW cHandle);
const wchar_t *CkCspW__hashAlgorithm(HCkCspW cHandle);
const wchar_t *CkCspW__keyContainerName(HCkCspW cHandle);
const wchar_t *CkCspW__lastErrorHtml(HCkCspW cHandle);
const wchar_t *CkCspW__lastErrorText(HCkCspW cHandle);
const wchar_t *CkCspW__lastErrorXml(HCkCspW cHandle);
const wchar_t *CkCspW__nthEncryptionAlgorithm(HCkCspW cHandle, long index);
const wchar_t *CkCspW__nthHashAlgorithmName(HCkCspW cHandle, long index);
const wchar_t *CkCspW__nthKeyContainerName(HCkCspW cHandle, long index);
const wchar_t *CkCspW__nthKeyExchangeAlgorithm(HCkCspW cHandle, long index);
const wchar_t *CkCspW__nthSignatureAlgorithm(HCkCspW cHandle, long index);
const wchar_t *CkCspW__providerName(HCkCspW cHandle);
const wchar_t *CkCspW__version(HCkCspW cHandle);
#endif

#endif // WIN32 (entire file)
