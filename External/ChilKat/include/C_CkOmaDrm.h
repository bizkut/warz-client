#ifndef _C_CkOmaDrm_H
#define _C_CkOmaDrm_H
#include "Chilkat_C.h"

HCkOmaDrm CkOmaDrm_Create(void);
void CkOmaDrm_Dispose(HCkOmaDrm handle);
void CkOmaDrm_getBase64Key(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_putBase64Key(HCkOmaDrm cHandle, const char *newVal);
void CkOmaDrm_getContentType(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_putContentType(HCkOmaDrm cHandle, const char *newVal);
void CkOmaDrm_getContentUri(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_putContentUri(HCkOmaDrm cHandle, const char *newVal);
void CkOmaDrm_getDebugLogFilePath(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_putDebugLogFilePath(HCkOmaDrm cHandle, const char *newVal);
void CkOmaDrm_getDecryptedData(HCkOmaDrm cHandle, HCkByteData retval);
int CkOmaDrm_getDrmContentVersion(HCkOmaDrm cHandle);
void CkOmaDrm_getEncryptedData(HCkOmaDrm cHandle, HCkByteData retval);
void CkOmaDrm_getHeaders(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_putHeaders(HCkOmaDrm cHandle, const char *newVal);
void CkOmaDrm_getIV(HCkOmaDrm cHandle, HCkByteData retval);
void CkOmaDrm_putIV(HCkOmaDrm cHandle, HCkByteData  newVal);
void CkOmaDrm_getLastErrorHtml(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_getLastErrorText(HCkOmaDrm cHandle, HCkString retval);
void CkOmaDrm_getLastErrorXml(HCkOmaDrm cHandle, HCkString retval);
BOOL CkOmaDrm_getUtf8(HCkOmaDrm cHandle);
void CkOmaDrm_putUtf8(HCkOmaDrm cHandle, BOOL newVal);
BOOL CkOmaDrm_getVerboseLogging(HCkOmaDrm cHandle);
void CkOmaDrm_putVerboseLogging(HCkOmaDrm cHandle, BOOL newVal);
void CkOmaDrm_getVersion(HCkOmaDrm cHandle, HCkString retval);
BOOL CkOmaDrm_CreateDcfFile(HCkOmaDrm cHandle, const char *path);
BOOL CkOmaDrm_GetHeaderField(HCkOmaDrm cHandle, const char *fieldName, HCkString outVal);
BOOL CkOmaDrm_LoadDcfData(HCkOmaDrm cHandle, HCkByteData data);
BOOL CkOmaDrm_LoadDcfFile(HCkOmaDrm cHandle, const char *path);
void CkOmaDrm_LoadUnencryptedData(HCkOmaDrm cHandle, HCkByteData data);
BOOL CkOmaDrm_LoadUnencryptedFile(HCkOmaDrm cHandle, const char *path);
BOOL CkOmaDrm_SaveDecrypted(HCkOmaDrm cHandle, const char *path);
BOOL CkOmaDrm_SaveLastError(HCkOmaDrm cHandle, const char *path);
void CkOmaDrm_SetEncodedIV(HCkOmaDrm cHandle, const char *encodedIv, const char *encoding);
BOOL CkOmaDrm_UnlockComponent(HCkOmaDrm cHandle, const char *unlockCode);
const char *CkOmaDrm_base64Key(HCkOmaDrm cHandle);
const char *CkOmaDrm_contentType(HCkOmaDrm cHandle);
const char *CkOmaDrm_contentUri(HCkOmaDrm cHandle);
const char *CkOmaDrm_debugLogFilePath(HCkOmaDrm cHandle);
const char *CkOmaDrm_getHeaderField(HCkOmaDrm cHandle, const char *fieldName);
const char *CkOmaDrm_headers(HCkOmaDrm cHandle);
const char *CkOmaDrm_lastErrorHtml(HCkOmaDrm cHandle);
const char *CkOmaDrm_lastErrorText(HCkOmaDrm cHandle);
const char *CkOmaDrm_lastErrorXml(HCkOmaDrm cHandle);
const char *CkOmaDrm_version(HCkOmaDrm cHandle);
#endif
