#ifndef _C_CkCert_W_H
#define _C_CkCert_W_H
#include "Chilkat_C.h"

HCkCertW CkCertW_Create(void);
HCkCertW CkCertW_Create2(bool bForMono);
void CkCertW_Dispose(HCkCertW handle);
BOOL CkCertW_getAvoidWindowsPkAccess(HCkCertW cHandle);
void CkCertW_putAvoidWindowsPkAccess(HCkCertW cHandle, BOOL newVal);
int CkCertW_getCertVersion(HCkCertW cHandle);
void CkCertW_getCspName(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getDebugLogFilePath(HCkCertW cHandle, HCkStringW retval);
void CkCertW_putDebugLogFilePath(HCkCertW cHandle, const wchar_t *newVal);
BOOL CkCertW_getExpired(HCkCertW cHandle);
BOOL CkCertW_getForClientAuthentication(HCkCertW cHandle);
BOOL CkCertW_getForCodeSigning(HCkCertW cHandle);
BOOL CkCertW_getForSecureEmail(HCkCertW cHandle);
BOOL CkCertW_getForServerAuthentication(HCkCertW cHandle);
BOOL CkCertW_getForTimeStamping(HCkCertW cHandle);
BOOL CkCertW_getHasKeyContainer(HCkCertW cHandle);
unsigned long CkCertW_getIntendedKeyUsage(HCkCertW cHandle);
BOOL CkCertW_getIsRoot(HCkCertW cHandle);
void CkCertW_getIssuerC(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerCN(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerDN(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerE(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerL(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerO(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerOU(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getIssuerS(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getKeyContainerName(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getLastErrorHtml(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getLastErrorText(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getLastErrorXml(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getMachineKeyset(HCkCertW cHandle);
void CkCertW_getOcspUrl(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getPrivateKeyExportable(HCkCertW cHandle);
BOOL CkCertW_getRevoked(HCkCertW cHandle);
void CkCertW_getRfc822Name(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getSelfSigned(HCkCertW cHandle);
void CkCertW_getSerialNumber(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSha1Thumbprint(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getSignatureVerified(HCkCertW cHandle);
BOOL CkCertW_getSilent(HCkCertW cHandle);
void CkCertW_getSubjectC(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectCN(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectDN(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectE(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectL(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectO(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectOU(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getSubjectS(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getTrustedRoot(HCkCertW cHandle);
void CkCertW_getValidFrom(HCkCertW cHandle, SYSTEMTIME *retval);
void CkCertW_getValidFromStr(HCkCertW cHandle, HCkStringW retval);
void CkCertW_getValidTo(HCkCertW cHandle, SYSTEMTIME *retval);
void CkCertW_getValidToStr(HCkCertW cHandle, HCkStringW retval);
BOOL CkCertW_getVerboseLogging(HCkCertW cHandle);
void CkCertW_putVerboseLogging(HCkCertW cHandle, BOOL newVal);
void CkCertW_getVersion(HCkCertW cHandle, HCkStringW retval);
int CkCertW_CheckRevoked(HCkCertW cHandle);
BOOL CkCertW_ExportCertDer(HCkCertW cHandle, HCkByteDataW outData);
BOOL CkCertW_ExportCertDerFile(HCkCertW cHandle, const wchar_t *path);
BOOL CkCertW_ExportCertPem(HCkCertW cHandle, HCkStringW outStr);
BOOL CkCertW_ExportCertPemFile(HCkCertW cHandle, const wchar_t *path);
BOOL CkCertW_ExportCertXml(HCkCertW cHandle, HCkStringW outStr);
HCkPrivateKeyW CkCertW_ExportPrivateKey(HCkCertW cHandle);
HCkPublicKeyW CkCertW_ExportPublicKey(HCkCertW cHandle);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertW_ExportToPfxFile(HCkCertW cHandle, const wchar_t *pfxPath, const wchar_t *password, BOOL bIncludeChain);
#endif
#endif
HCkCertW CkCertW_FindIssuer(HCkCertW cHandle);
BOOL CkCertW_GetEncoded(HCkCertW cHandle, HCkStringW outStr);
BOOL CkCertW_GetPrivateKeyPem(HCkCertW cHandle, HCkStringW outStr);
HCkDateTimeW CkCertW_GetValidFromDt(HCkCertW cHandle);
HCkDateTimeW CkCertW_GetValidToDt(HCkCertW cHandle);
BOOL CkCertW_HasPrivateKey(HCkCertW cHandle);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertW_LinkPrivateKey(HCkCertW cHandle, const wchar_t *keyContainerName, BOOL bMachineKeyset, BOOL bForSigning);
#endif
#endif
BOOL CkCertW_LoadByCommonName(HCkCertW cHandle, const wchar_t *cn);
BOOL CkCertW_LoadByEmailAddress(HCkCertW cHandle, const wchar_t *emailAddress);
BOOL CkCertW_LoadByIssuerAndSerialNumber(HCkCertW cHandle, const wchar_t *issuerCN, const wchar_t *serialNum);
BOOL CkCertW_LoadFromBase64(HCkCertW cHandle, const wchar_t *encodedCert);
BOOL CkCertW_LoadFromBinary(HCkCertW cHandle, HCkByteDataW data);
#if !defined(CHILKAT_MONO)
BOOL CkCertW_LoadFromBinary2(HCkCertW cHandle, const unsigned char *pByteData, unsigned long szByteData);
#endif
BOOL CkCertW_LoadFromFile(HCkCertW cHandle, const wchar_t *path);
BOOL CkCertW_LoadPfxData(HCkCertW cHandle, HCkByteDataW pfxData, const wchar_t *password);
#if !defined(CHILKAT_MONO)
BOOL CkCertW_LoadPfxData2(HCkCertW cHandle, const unsigned char *pByteData, unsigned long szByteData, const wchar_t *password);
#endif
BOOL CkCertW_LoadPfxFile(HCkCertW cHandle, const wchar_t *path, const wchar_t *password);
BOOL CkCertW_PemFileToDerFile(HCkCertW cHandle, const wchar_t *fromPath, const wchar_t *toPath);
BOOL CkCertW_SaveLastError(HCkCertW cHandle, const wchar_t *path);
BOOL CkCertW_SaveToFile(HCkCertW cHandle, const wchar_t *path);
BOOL CkCertW_SetFromEncoded(HCkCertW cHandle, const wchar_t *encodedCert);
BOOL CkCertW_SetPrivateKey(HCkCertW cHandle, HCkPrivateKeyW privKey);
BOOL CkCertW_SetPrivateKeyPem(HCkCertW cHandle, const wchar_t *privKeyPem);
const wchar_t *CkCertW__cspName(HCkCertW cHandle);
const wchar_t *CkCertW__debugLogFilePath(HCkCertW cHandle);
const wchar_t *CkCertW__exportCertPem(HCkCertW cHandle);
const wchar_t *CkCertW__exportCertXml(HCkCertW cHandle);
const wchar_t *CkCertW__getEncoded(HCkCertW cHandle);
const wchar_t *CkCertW__getPrivateKeyPem(HCkCertW cHandle);
const wchar_t *CkCertW__issuerC(HCkCertW cHandle);
const wchar_t *CkCertW__issuerCN(HCkCertW cHandle);
const wchar_t *CkCertW__issuerDN(HCkCertW cHandle);
const wchar_t *CkCertW__issuerE(HCkCertW cHandle);
const wchar_t *CkCertW__issuerL(HCkCertW cHandle);
const wchar_t *CkCertW__issuerO(HCkCertW cHandle);
const wchar_t *CkCertW__issuerOU(HCkCertW cHandle);
const wchar_t *CkCertW__issuerS(HCkCertW cHandle);
const wchar_t *CkCertW__keyContainerName(HCkCertW cHandle);
const wchar_t *CkCertW__lastErrorHtml(HCkCertW cHandle);
const wchar_t *CkCertW__lastErrorText(HCkCertW cHandle);
const wchar_t *CkCertW__lastErrorXml(HCkCertW cHandle);
const wchar_t *CkCertW__ocspUrl(HCkCertW cHandle);
const wchar_t *CkCertW__rfc822Name(HCkCertW cHandle);
const wchar_t *CkCertW__serialNumber(HCkCertW cHandle);
const wchar_t *CkCertW__sha1Thumbprint(HCkCertW cHandle);
const wchar_t *CkCertW__subjectC(HCkCertW cHandle);
const wchar_t *CkCertW__subjectCN(HCkCertW cHandle);
const wchar_t *CkCertW__subjectDN(HCkCertW cHandle);
const wchar_t *CkCertW__subjectE(HCkCertW cHandle);
const wchar_t *CkCertW__subjectL(HCkCertW cHandle);
const wchar_t *CkCertW__subjectO(HCkCertW cHandle);
const wchar_t *CkCertW__subjectOU(HCkCertW cHandle);
const wchar_t *CkCertW__subjectS(HCkCertW cHandle);
const wchar_t *CkCertW__validFromStr(HCkCertW cHandle);
const wchar_t *CkCertW__validToStr(HCkCertW cHandle);
const wchar_t *CkCertW__version(HCkCertW cHandle);
#endif
