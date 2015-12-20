#if defined(WIN32) || defined(WINCE)

#ifndef _C_CkKeyContainer_H
#define _C_CkKeyContainer_H
#include "Chilkat_C.h"

HCkKeyContainer CkKeyContainer_Create(void);
void CkKeyContainer_Dispose(HCkKeyContainer handle);
void CkKeyContainer_getContainerName(HCkKeyContainer cHandle, HCkString retval);
void CkKeyContainer_getDebugLogFilePath(HCkKeyContainer cHandle, HCkString retval);
void CkKeyContainer_putDebugLogFilePath(HCkKeyContainer cHandle, const char *newVal);
BOOL CkKeyContainer_getIsMachineKeyset(HCkKeyContainer cHandle);
BOOL CkKeyContainer_getIsOpen(HCkKeyContainer cHandle);
void CkKeyContainer_getLastErrorHtml(HCkKeyContainer cHandle, HCkString retval);
void CkKeyContainer_getLastErrorText(HCkKeyContainer cHandle, HCkString retval);
void CkKeyContainer_getLastErrorXml(HCkKeyContainer cHandle, HCkString retval);
BOOL CkKeyContainer_getUtf8(HCkKeyContainer cHandle);
void CkKeyContainer_putUtf8(HCkKeyContainer cHandle, BOOL newVal);
BOOL CkKeyContainer_getVerboseLogging(HCkKeyContainer cHandle);
void CkKeyContainer_putVerboseLogging(HCkKeyContainer cHandle, BOOL newVal);
void CkKeyContainer_getVersion(HCkKeyContainer cHandle, HCkString retval);
void CkKeyContainer_CloseContainer(HCkKeyContainer cHandle);
BOOL CkKeyContainer_CreateContainer(HCkKeyContainer cHandle, const char *name, BOOL machineKeyset);
BOOL CkKeyContainer_DeleteContainer(HCkKeyContainer cHandle);
BOOL CkKeyContainer_FetchContainerNames(HCkKeyContainer cHandle, BOOL bMachineKeyset);
BOOL CkKeyContainer_GenerateKeyPair(HCkKeyContainer cHandle, BOOL bKeyExchangePair, int keyLengthInBits);
void CkKeyContainer_GenerateUuid(HCkKeyContainer cHandle, HCkString outGuid);
void CkKeyContainer_GetNthContainerName(HCkKeyContainer cHandle, BOOL bMachineKeyset, int index, HCkString outName);
int CkKeyContainer_GetNumContainers(HCkKeyContainer cHandle, BOOL bMachineKeyset);
HCkPrivateKey CkKeyContainer_GetPrivateKey(HCkKeyContainer cHandle, BOOL bKeyExchangePair);
HCkPublicKey CkKeyContainer_GetPublicKey(HCkKeyContainer cHandle, BOOL bKeyExchangePair);
BOOL CkKeyContainer_ImportPrivateKey(HCkKeyContainer cHandle, HCkPrivateKey key, BOOL bKeyExchangePair);
BOOL CkKeyContainer_ImportPublicKey(HCkKeyContainer cHandle, HCkPublicKey key, BOOL bKeyExchangePair);
BOOL CkKeyContainer_OpenContainer(HCkKeyContainer cHandle, const char *name, BOOL needPrivateKeyAccess, BOOL machineKeyset);
BOOL CkKeyContainer_SaveLastError(HCkKeyContainer cHandle, const char *path);
const char *CkKeyContainer_containerName(HCkKeyContainer cHandle);
const char *CkKeyContainer_debugLogFilePath(HCkKeyContainer cHandle);
const char *CkKeyContainer_generateUuid(HCkKeyContainer cHandle);
const char *CkKeyContainer_getNthContainerName(HCkKeyContainer cHandle, BOOL bMachineKeyset, int index);
const char *CkKeyContainer_lastErrorHtml(HCkKeyContainer cHandle);
const char *CkKeyContainer_lastErrorText(HCkKeyContainer cHandle);
const char *CkKeyContainer_lastErrorXml(HCkKeyContainer cHandle);
const char *CkKeyContainer_version(HCkKeyContainer cHandle);
#endif

#endif // WIN32 (entire file)
