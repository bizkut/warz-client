#ifndef _C_CkBounce_W_H
#define _C_CkBounce_W_H
#include "Chilkat_C.h"

HCkBounceW CkBounceW_Create(void);
HCkBounceW CkBounceW_Create2(bool bForMono);
void CkBounceW_Dispose(HCkBounceW handle);
void CkBounceW_getBounceAddress(HCkBounceW cHandle, HCkStringW retval);
void CkBounceW_getBounceData(HCkBounceW cHandle, HCkStringW retval);
int CkBounceW_getBounceType(HCkBounceW cHandle);
void CkBounceW_getDebugLogFilePath(HCkBounceW cHandle, HCkStringW retval);
void CkBounceW_putDebugLogFilePath(HCkBounceW cHandle, const wchar_t *newVal);
void CkBounceW_getLastErrorHtml(HCkBounceW cHandle, HCkStringW retval);
void CkBounceW_getLastErrorText(HCkBounceW cHandle, HCkStringW retval);
void CkBounceW_getLastErrorXml(HCkBounceW cHandle, HCkStringW retval);
BOOL CkBounceW_getVerboseLogging(HCkBounceW cHandle);
void CkBounceW_putVerboseLogging(HCkBounceW cHandle, BOOL newVal);
void CkBounceW_getVersion(HCkBounceW cHandle, HCkStringW retval);
BOOL CkBounceW_ExamineEmail(HCkBounceW cHandle, HCkEmailW email);
BOOL CkBounceW_ExamineEml(HCkBounceW cHandle, const wchar_t *emlPath);
BOOL CkBounceW_ExamineMime(HCkBounceW cHandle, const wchar_t *mimeString);
BOOL CkBounceW_SaveLastError(HCkBounceW cHandle, const wchar_t *path);
BOOL CkBounceW_UnlockComponent(HCkBounceW cHandle, const wchar_t *unlockCode);
const wchar_t *CkBounceW__bounceAddress(HCkBounceW cHandle);
const wchar_t *CkBounceW__bounceData(HCkBounceW cHandle);
const wchar_t *CkBounceW__debugLogFilePath(HCkBounceW cHandle);
const wchar_t *CkBounceW__lastErrorHtml(HCkBounceW cHandle);
const wchar_t *CkBounceW__lastErrorText(HCkBounceW cHandle);
const wchar_t *CkBounceW__lastErrorXml(HCkBounceW cHandle);
const wchar_t *CkBounceW__version(HCkBounceW cHandle);
#endif
