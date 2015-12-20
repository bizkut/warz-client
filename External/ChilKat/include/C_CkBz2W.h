#ifndef _C_CkBz2_W_H
#define _C_CkBz2_W_H
#include "Chilkat_C.h"

HCkBz2W CkBz2W_Create(void);
HCkBz2W CkBz2W_Create2(bool bForMono);
void CkBz2W_Dispose(HCkBz2W handle);
void CkBz2W_getDebugLogFilePath(HCkBz2W cHandle, HCkStringW retval);
void CkBz2W_putDebugLogFilePath(HCkBz2W cHandle, const wchar_t *newVal);
HCkBz2ProgressW CkBz2W_getEventCallbackObject(HCkBz2W cHandle);
void CkBz2W_putEventCallbackObject(HCkBz2W cHandle, HCkBz2ProgressW newVal);
void CkBz2W_getLastErrorHtml(HCkBz2W cHandle, HCkStringW retval);
void CkBz2W_getLastErrorText(HCkBz2W cHandle, HCkStringW retval);
void CkBz2W_getLastErrorXml(HCkBz2W cHandle, HCkStringW retval);
BOOL CkBz2W_getVerboseLogging(HCkBz2W cHandle);
void CkBz2W_putVerboseLogging(HCkBz2W cHandle, BOOL newVal);
void CkBz2W_getVersion(HCkBz2W cHandle, HCkStringW retval);
BOOL CkBz2W_CompressFile(HCkBz2W cHandle, const wchar_t *inPath, const wchar_t *toPath);
BOOL CkBz2W_CompressFileToMem(HCkBz2W cHandle, const wchar_t *inPath, HCkByteDataW outBytes);
BOOL CkBz2W_CompressMemToFile(HCkBz2W cHandle, HCkByteDataW inData, const wchar_t *toPath);
BOOL CkBz2W_CompressMemory(HCkBz2W cHandle, HCkByteDataW inData, HCkByteDataW outBytes);
BOOL CkBz2W_SaveLastError(HCkBz2W cHandle, const wchar_t *path);
BOOL CkBz2W_UncompressFile(HCkBz2W cHandle, const wchar_t *inPath, const wchar_t *toPath);
BOOL CkBz2W_UncompressFileToMem(HCkBz2W cHandle, const wchar_t *inPath, HCkByteDataW outBytes);
BOOL CkBz2W_UncompressMemToFile(HCkBz2W cHandle, HCkByteDataW inData, const wchar_t *toPath);
BOOL CkBz2W_UncompressMemory(HCkBz2W cHandle, HCkByteDataW inData, HCkByteDataW outBytes);
BOOL CkBz2W_UnlockComponent(HCkBz2W cHandle, const wchar_t *unlockCode);
const wchar_t *CkBz2W__debugLogFilePath(HCkBz2W cHandle);
const wchar_t *CkBz2W__lastErrorHtml(HCkBz2W cHandle);
const wchar_t *CkBz2W__lastErrorText(HCkBz2W cHandle);
const wchar_t *CkBz2W__lastErrorXml(HCkBz2W cHandle);
const wchar_t *CkBz2W__version(HCkBz2W cHandle);
#endif
