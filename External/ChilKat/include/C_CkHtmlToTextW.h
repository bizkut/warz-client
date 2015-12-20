#ifndef _C_CkHtmlToText_W_H
#define _C_CkHtmlToText_W_H
#include "Chilkat_C.h"

HCkHtmlToTextW CkHtmlToTextW_Create(void);
HCkHtmlToTextW CkHtmlToTextW_Create2(bool bForMono);
void CkHtmlToTextW_Dispose(HCkHtmlToTextW handle);
void CkHtmlToTextW_getDebugLogFilePath(HCkHtmlToTextW cHandle, HCkStringW retval);
void CkHtmlToTextW_putDebugLogFilePath(HCkHtmlToTextW cHandle, const wchar_t *newVal);
BOOL CkHtmlToTextW_getDecodeHtmlEntities(HCkHtmlToTextW cHandle);
void CkHtmlToTextW_putDecodeHtmlEntities(HCkHtmlToTextW cHandle, BOOL newVal);
void CkHtmlToTextW_getLastErrorHtml(HCkHtmlToTextW cHandle, HCkStringW retval);
void CkHtmlToTextW_getLastErrorText(HCkHtmlToTextW cHandle, HCkStringW retval);
void CkHtmlToTextW_getLastErrorXml(HCkHtmlToTextW cHandle, HCkStringW retval);
int CkHtmlToTextW_getRightMargin(HCkHtmlToTextW cHandle);
void CkHtmlToTextW_putRightMargin(HCkHtmlToTextW cHandle, int newVal);
BOOL CkHtmlToTextW_getSuppressLinks(HCkHtmlToTextW cHandle);
void CkHtmlToTextW_putSuppressLinks(HCkHtmlToTextW cHandle, BOOL newVal);
BOOL CkHtmlToTextW_getVerboseLogging(HCkHtmlToTextW cHandle);
void CkHtmlToTextW_putVerboseLogging(HCkHtmlToTextW cHandle, BOOL newVal);
void CkHtmlToTextW_getVersion(HCkHtmlToTextW cHandle, HCkStringW retval);
BOOL CkHtmlToTextW_IsUnlocked(HCkHtmlToTextW cHandle);
BOOL CkHtmlToTextW_ReadFileToString(HCkHtmlToTextW cHandle, const wchar_t *path, const wchar_t *srcCharset, HCkStringW outStr);
BOOL CkHtmlToTextW_SaveLastError(HCkHtmlToTextW cHandle, const wchar_t *path);
BOOL CkHtmlToTextW_ToText(HCkHtmlToTextW cHandle, const wchar_t *html, HCkStringW outStr);
BOOL CkHtmlToTextW_UnlockComponent(HCkHtmlToTextW cHandle, const wchar_t *code);
BOOL CkHtmlToTextW_WriteStringToFile(HCkHtmlToTextW cHandle, const wchar_t *str, const wchar_t *path, const wchar_t *charset);
const wchar_t *CkHtmlToTextW__debugLogFilePath(HCkHtmlToTextW cHandle);
const wchar_t *CkHtmlToTextW__lastErrorHtml(HCkHtmlToTextW cHandle);
const wchar_t *CkHtmlToTextW__lastErrorText(HCkHtmlToTextW cHandle);
const wchar_t *CkHtmlToTextW__lastErrorXml(HCkHtmlToTextW cHandle);
const wchar_t *CkHtmlToTextW__readFileToString(HCkHtmlToTextW cHandle, const wchar_t *path, const wchar_t *srcCharset);
const wchar_t *CkHtmlToTextW__toText(HCkHtmlToTextW cHandle, const wchar_t *html);
const wchar_t *CkHtmlToTextW__version(HCkHtmlToTextW cHandle);
#endif
