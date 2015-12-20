#ifndef _C_CkCertStore_H
#define _C_CkCertStore_H
#include "Chilkat_C.h"

HCkCertStore CkCertStore_Create(void);
void CkCertStore_Dispose(HCkCertStore handle);
BOOL CkCertStore_getAvoidWindowsPkAccess(HCkCertStore cHandle);
void CkCertStore_putAvoidWindowsPkAccess(HCkCertStore cHandle, BOOL newVal);
void CkCertStore_getDebugLogFilePath(HCkCertStore cHandle, HCkString retval);
void CkCertStore_putDebugLogFilePath(HCkCertStore cHandle, const char *newVal);
void CkCertStore_getLastErrorHtml(HCkCertStore cHandle, HCkString retval);
void CkCertStore_getLastErrorText(HCkCertStore cHandle, HCkString retval);
void CkCertStore_getLastErrorXml(HCkCertStore cHandle, HCkString retval);
long CkCertStore_getNumCertificates(HCkCertStore cHandle);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
long CkCertStore_getNumEmailCerts(HCkCertStore cHandle);
#endif
#endif
BOOL CkCertStore_getUtf8(HCkCertStore cHandle);
void CkCertStore_putUtf8(HCkCertStore cHandle, BOOL newVal);
BOOL CkCertStore_getVerboseLogging(HCkCertStore cHandle);
void CkCertStore_putVerboseLogging(HCkCertStore cHandle, BOOL newVal);
void CkCertStore_getVersion(HCkCertStore cHandle, HCkString retval);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_AddCertificate(HCkCertStore cHandle, HCkCert cert);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_CreateFileStore(HCkCertStore cHandle, const char *path);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_CreateMemoryStore(HCkCertStore cHandle);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_CreateRegistryStore(HCkCertStore cHandle, const char *regRoot, const char *regPath);
#endif
#endif
HCkCert CkCertStore_FindCertByRfc822Name(HCkCertStore cHandle, const char *name);
HCkCert CkCertStore_FindCertBySerial(HCkCertStore cHandle, const char *serialNumber);
HCkCert CkCertStore_FindCertBySha1Thumbprint(HCkCertStore cHandle, const char *str);
HCkCert CkCertStore_FindCertBySubject(HCkCertStore cHandle, const char *subject);
HCkCert CkCertStore_FindCertBySubjectCN(HCkCertStore cHandle, const char *commonName);
HCkCert CkCertStore_FindCertBySubjectE(HCkCertStore cHandle, const char *emailAddress);
HCkCert CkCertStore_FindCertBySubjectO(HCkCertStore cHandle, const char *organization);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
HCkCert CkCertStore_FindCertForEmail(HCkCertStore cHandle, const char *emailAddress);
#endif
#endif
HCkCert CkCertStore_GetCertificate(HCkCertStore cHandle, long index);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
HCkCert CkCertStore_GetEmailCert(HCkCertStore cHandle, long index);
#endif
#endif
BOOL CkCertStore_LoadPfxData(HCkCertStore cHandle, HCkByteData pfxData, const char *password);
#if !defined(CHILKAT_MONO)
BOOL CkCertStore_LoadPfxData2(HCkCertStore cHandle, const unsigned char *pByteData, unsigned long szByteData, const char *password);
#endif
BOOL CkCertStore_LoadPfxFile(HCkCertStore cHandle, const char *path, const char *password);
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_OpenCurrentUserStore(HCkCertStore cHandle, BOOL readOnly);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_OpenFileStore(HCkCertStore cHandle, const char *path, BOOL readOnly);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_OpenLocalSystemStore(HCkCertStore cHandle, BOOL readOnly);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_OpenRegistryStore(HCkCertStore cHandle, const char *regRoot, const char *regPath, BOOL readOnly);
#endif
#endif
#if defined(WIN32) && !defined(__MINGW32__)
#if defined(WIN32) && !defined(__MINGW32__)
BOOL CkCertStore_RemoveCertificate(HCkCertStore cHandle, HCkCert cert);
#endif
#endif
BOOL CkCertStore_SaveLastError(HCkCertStore cHandle, const char *path);
const char *CkCertStore_debugLogFilePath(HCkCertStore cHandle);
const char *CkCertStore_lastErrorHtml(HCkCertStore cHandle);
const char *CkCertStore_lastErrorText(HCkCertStore cHandle);
const char *CkCertStore_lastErrorXml(HCkCertStore cHandle);
const char *CkCertStore_version(HCkCertStore cHandle);
#endif
