#if defined(WIN32) || defined(WINCE)

#ifndef _C_CkKeyContainer_W_H
#define _C_CkKeyContainer_W_H
#include "Chilkat_C.h"

HCkKeyContainerW CkKeyContainerW_Create(void);
HCkKeyContainerW CkKeyContainerW_Create2(bool bForMono);
void CkKeyContainerW_Dispose(HCkKeyContainerW handle);
void CkKeyContainerW_getContainerName(HCkKeyContainerW cHandle, HCkStringW retval);
void CkKeyContainerW_getDebugLogFilePath(HCkKeyContainerW cHandle, HCkStringW retval);
void CkKeyContainerW_putDebugLogFilePath(HCkKeyContainerW cHandle, const wchar_t *newVal);
BOOL CkKeyContainerW_getIsMachineKeyset(HCkKeyContainerW cHandle);
BOOL CkKeyContainerW_getIsOpen(HCkKeyContainerW cHandle);
void CkKeyContainerW_getLastErrorHtml(HCkKeyContainerW cHandle, HCkStringW retval);
void CkKeyContainerW_getLastErrorText(HCkKeyContainerW cHandle, HCkStringW retval);
void CkKeyContainerW_getLastErrorXml(HCkKeyContainerW cHandle, HCkStringW retval);
BOOL CkKeyContainerW_getVerboseLogging(HCkKeyContainerW cHandle);
void CkKeyContainerW_putVerboseLogging(HCkKeyContainerW cHandle, BOOL newVal);
void CkKeyContainerW_getVersion(HCkKeyContainerW cHandle, HCkStringW retval);
void CkKeyContainerW_CloseContainer(HCkKeyContainerW cHandle);
BOOL CkKeyContainerW_CreateContainer(HCkKeyContainerW cHandle, const wchar_t *name, BOOL machineKeyset);
BOOL CkKeyContainerW_DeleteContainer(HCkKeyContainerW cHandle);
BOOL CkKeyContainerW_FetchContainerNames(HCkKeyContainerW cHandle, BOOL bMachineKeyset);
BOOL CkKeyContainerW_GenerateKeyPair(HCkKeyContainerW cHandle, BOOL bKeyExchangePair, int keyLengthInBits);
void CkKeyContainerW_GenerateUuid(HCkKeyContainerW cHandle, HCkStringW outGuid);
void CkKeyContainerW_GetNthContainerName(HCkKeyContainerW cHandle, BOOL bMachineKeyset, int index, HCkStringW outName);
int CkKeyContainerW_GetNumContainers(HCkKeyContainerW cHandle, BOOL bMachineKeyset);
HCkPrivateKeyW CkKeyContainerW_GetPrivateKey(HCkKeyContainerW cHandle, BOOL bKeyExchangePair);
HCkPublicKeyW CkKeyContainerW_GetPublicKey(HCkKeyContainerW cHandle, BOOL bKeyExchangePair);
BOOL CkKeyContainerW_ImportPrivateKey(HCkKeyContainerW cHandle, HCkPrivateKeyW key, BOOL bKeyExchangePair);
BOOL CkKeyContainerW_ImportPublicKey(HCkKeyContainerW cHandle, HCkPublicKeyW key, BOOL bKeyExchangePair);
BOOL CkKeyContainerW_OpenContainer(HCkKeyContainerW cHandle, const wchar_t *name, BOOL needPrivateKeyAccess, BOOL machineKeyset);
BOOL CkKeyContainerW_SaveLastError(HCkKeyContainerW cHandle, const wchar_t *path);
const wchar_t *CkKeyContainerW__containerName(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__debugLogFilePath(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__generateUuid(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__getNthContainerName(HCkKeyContainerW cHandle, BOOL bMachineKeyset, int index);
const wchar_t *CkKeyContainerW__lastErrorHtml(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__lastErrorText(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__lastErrorXml(HCkKeyContainerW cHandle);
const wchar_t *CkKeyContainerW__version(HCkKeyContainerW cHandle);
#endif

#endif // WIN32 (entire file)
