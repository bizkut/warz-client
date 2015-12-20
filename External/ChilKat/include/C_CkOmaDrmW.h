#ifndef _C_CkOmaDrm_W_H
#define _C_CkOmaDrm_W_H
#include "Chilkat_C.h"

HCkOmaDrmW CkOmaDrmW_Create(void);
HCkOmaDrmW CkOmaDrmW_Create2(bool bForMono);
void CkOmaDrmW_Dispose(HCkOmaDrmW handle);
void CkOmaDrmW_getBase64Key(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_putBase64Key(HCkOmaDrmW cHandle, const wchar_t *newVal);
void CkOmaDrmW_getContentType(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_putContentType(HCkOmaDrmW cHandle, const wchar_t *newVal);
void CkOmaDrmW_getContentUri(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_putContentUri(HCkOmaDrmW cHandle, const wchar_t *newVal);
void CkOmaDrmW_getDebugLogFilePath(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_putDebugLogFilePath(HCkOmaDrmW cHandle, const wchar_t *newVal);
void CkOmaDrmW_getDecryptedData(HCkOmaDrmW cHandle, HCkByteDataW retval);
int CkOmaDrmW_getDrmContentVersion(HCkOmaDrmW cHandle);
void CkOmaDrmW_getEncryptedData(HCkOmaDrmW cHandle, HCkByteDataW retval);
void CkOmaDrmW_getHeaders(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_putHeaders(HCkOmaDrmW cHandle, const wchar_t *newVal);
void CkOmaDrmW_getIV(HCkOmaDrmW cHandle, HCkByteDataW retval);
void CkOmaDrmW_putIV(HCkOmaDrmW cHandle, HCkByteDataW  newVal);
void CkOmaDrmW_getLastErrorHtml(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_getLastErrorText(HCkOmaDrmW cHandle, HCkStringW retval);
void CkOmaDrmW_getLastErrorXml(HCkOmaDrmW cHandle, HCkStringW retval);
BOOL CkOmaDrmW_getVerboseLogging(HCkOmaDrmW cHandle);
void CkOmaDrmW_putVerboseLogging(HCkOmaDrmW cHandle, BOOL newVal);
void CkOmaDrmW_getVersion(HCkOmaDrmW cHandle, HCkStringW retval);
BOOL CkOmaDrmW_CreateDcfFile(HCkOmaDrmW cHandle, const wchar_t *path);
BOOL CkOmaDrmW_GetHeaderField(HCkOmaDrmW cHandle, const wchar_t *fieldName, HCkStringW outVal);
BOOL CkOmaDrmW_LoadDcfData(HCkOmaDrmW cHandle, HCkByteDataW data);
BOOL CkOmaDrmW_LoadDcfFile(HCkOmaDrmW cHandle, const wchar_t *path);
void CkOmaDrmW_LoadUnencryptedData(HCkOmaDrmW cHandle, HCkByteDataW data);
BOOL CkOmaDrmW_LoadUnencryptedFile(HCkOmaDrmW cHandle, const wchar_t *path);
BOOL CkOmaDrmW_SaveDecrypted(HCkOmaDrmW cHandle, const wchar_t *path);
BOOL CkOmaDrmW_SaveLastError(HCkOmaDrmW cHandle, const wchar_t *path);
void CkOmaDrmW_SetEncodedIV(HCkOmaDrmW cHandle, const wchar_t *encodedIv, const wchar_t *encoding);
BOOL CkOmaDrmW_UnlockComponent(HCkOmaDrmW cHandle, const wchar_t *unlockCode);
const wchar_t *CkOmaDrmW__base64Key(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__contentType(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__contentUri(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__debugLogFilePath(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__getHeaderField(HCkOmaDrmW cHandle, const wchar_t *fieldName);
const wchar_t *CkOmaDrmW__headers(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__lastErrorHtml(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__lastErrorText(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__lastErrorXml(HCkOmaDrmW cHandle);
const wchar_t *CkOmaDrmW__version(HCkOmaDrmW cHandle);
#endif
