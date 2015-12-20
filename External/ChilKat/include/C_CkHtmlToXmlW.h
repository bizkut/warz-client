#ifndef _C_CkHtmlToXml_W_H
#define _C_CkHtmlToXml_W_H
#include "Chilkat_C.h"

HCkHtmlToXmlW CkHtmlToXmlW_Create(void);
HCkHtmlToXmlW CkHtmlToXmlW_Create2(bool bForMono);
void CkHtmlToXmlW_Dispose(HCkHtmlToXmlW handle);
void CkHtmlToXmlW_getDebugLogFilePath(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_putDebugLogFilePath(HCkHtmlToXmlW cHandle, const wchar_t *newVal);
BOOL CkHtmlToXmlW_getDropCustomTags(HCkHtmlToXmlW cHandle);
void CkHtmlToXmlW_putDropCustomTags(HCkHtmlToXmlW cHandle, BOOL newVal);
void CkHtmlToXmlW_getHtml(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_putHtml(HCkHtmlToXmlW cHandle, const wchar_t *newVal);
void CkHtmlToXmlW_getLastErrorHtml(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_getLastErrorText(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_getLastErrorXml(HCkHtmlToXmlW cHandle, HCkStringW retval);
long CkHtmlToXmlW_getNbsp(HCkHtmlToXmlW cHandle);
void CkHtmlToXmlW_putNbsp(HCkHtmlToXmlW cHandle, long newVal);
BOOL CkHtmlToXmlW_getVerboseLogging(HCkHtmlToXmlW cHandle);
void CkHtmlToXmlW_putVerboseLogging(HCkHtmlToXmlW cHandle, BOOL newVal);
void CkHtmlToXmlW_getVersion(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_getXmlCharset(HCkHtmlToXmlW cHandle, HCkStringW retval);
void CkHtmlToXmlW_putXmlCharset(HCkHtmlToXmlW cHandle, const wchar_t *newVal);
BOOL CkHtmlToXmlW_ConvertFile(HCkHtmlToXmlW cHandle, const wchar_t *inHtmlPath, const wchar_t *destXmlPath);
void CkHtmlToXmlW_DropTagType(HCkHtmlToXmlW cHandle, const wchar_t *tagName);
void CkHtmlToXmlW_DropTextFormattingTags(HCkHtmlToXmlW cHandle);
BOOL CkHtmlToXmlW_IsUnlocked(HCkHtmlToXmlW cHandle);
BOOL CkHtmlToXmlW_ReadFileToString(HCkHtmlToXmlW cHandle, const wchar_t *path, const wchar_t *srcCharset, HCkStringW outStr);
BOOL CkHtmlToXmlW_SaveLastError(HCkHtmlToXmlW cHandle, const wchar_t *path);
void CkHtmlToXmlW_SetHtmlBytes(HCkHtmlToXmlW cHandle, HCkByteDataW inData);
BOOL CkHtmlToXmlW_SetHtmlFromFile(HCkHtmlToXmlW cHandle, const wchar_t *path);
BOOL CkHtmlToXmlW_ToXml(HCkHtmlToXmlW cHandle, HCkStringW outStr);
void CkHtmlToXmlW_UndropTagType(HCkHtmlToXmlW cHandle, const wchar_t *tagName);
void CkHtmlToXmlW_UndropTextFormattingTags(HCkHtmlToXmlW cHandle);
BOOL CkHtmlToXmlW_UnlockComponent(HCkHtmlToXmlW cHandle, const wchar_t *code);
BOOL CkHtmlToXmlW_WriteStringToFile(HCkHtmlToXmlW cHandle, const wchar_t *str, const wchar_t *path, const wchar_t *charset);
BOOL CkHtmlToXmlW_Xml(HCkHtmlToXmlW cHandle, HCkStringW outStr);
const wchar_t *CkHtmlToXmlW__debugLogFilePath(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__html(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__lastErrorHtml(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__lastErrorText(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__lastErrorXml(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__readFileToString(HCkHtmlToXmlW cHandle, const wchar_t *path, const wchar_t *srcCharset);
const wchar_t *CkHtmlToXmlW__toXml(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__version(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__xml(HCkHtmlToXmlW cHandle);
const wchar_t *CkHtmlToXmlW__xmlCharset(HCkHtmlToXmlW cHandle);
#endif
