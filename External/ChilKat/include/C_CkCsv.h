#ifndef _C_CkCsv_H
#define _C_CkCsv_H
#include "Chilkat_C.h"

HCkCsv CkCsv_Create(void);
void CkCsv_Dispose(HCkCsv handle);
BOOL CkCsv_getCrlf(HCkCsv cHandle);
void CkCsv_putCrlf(HCkCsv cHandle, BOOL newVal);
void CkCsv_getDebugLogFilePath(HCkCsv cHandle, HCkString retval);
void CkCsv_putDebugLogFilePath(HCkCsv cHandle, const char *newVal);
void CkCsv_getDelimiter(HCkCsv cHandle, HCkString retval);
void CkCsv_putDelimiter(HCkCsv cHandle, const char *newVal);
BOOL CkCsv_getHasColumnNames(HCkCsv cHandle);
void CkCsv_putHasColumnNames(HCkCsv cHandle, BOOL newVal);
void CkCsv_getLastErrorHtml(HCkCsv cHandle, HCkString retval);
void CkCsv_getLastErrorText(HCkCsv cHandle, HCkString retval);
void CkCsv_getLastErrorXml(HCkCsv cHandle, HCkString retval);
int CkCsv_getNumColumns(HCkCsv cHandle);
int CkCsv_getNumRows(HCkCsv cHandle);
BOOL CkCsv_getUtf8(HCkCsv cHandle);
void CkCsv_putUtf8(HCkCsv cHandle, BOOL newVal);
BOOL CkCsv_getVerboseLogging(HCkCsv cHandle);
void CkCsv_putVerboseLogging(HCkCsv cHandle, BOOL newVal);
void CkCsv_getVersion(HCkCsv cHandle, HCkString retval);
BOOL CkCsv_GetCell(HCkCsv cHandle, int row, int col, HCkString outStr);
BOOL CkCsv_GetColumnName(HCkCsv cHandle, int index, HCkString outStr);
int CkCsv_GetIndex(HCkCsv cHandle, const char *columnName);
int CkCsv_GetNumCols(HCkCsv cHandle, int row);
BOOL CkCsv_LoadFile(HCkCsv cHandle, const char *path);
BOOL CkCsv_LoadFile2(HCkCsv cHandle, const char *path, const char *charset);
BOOL CkCsv_LoadFromString(HCkCsv cHandle, const char *csvData);
BOOL CkCsv_SaveFile(HCkCsv cHandle, const char *path);
BOOL CkCsv_SaveFile2(HCkCsv cHandle, const char *path, const char *charset);
BOOL CkCsv_SaveLastError(HCkCsv cHandle, const char *path);
BOOL CkCsv_SaveToString(HCkCsv cHandle, HCkString outStr);
BOOL CkCsv_SetCell(HCkCsv cHandle, int row, int col, const char *content);
BOOL CkCsv_SetColumnName(HCkCsv cHandle, int index, const char *columnName);
const char *CkCsv_debugLogFilePath(HCkCsv cHandle);
const char *CkCsv_delimiter(HCkCsv cHandle);
const char *CkCsv_getCell(HCkCsv cHandle, int row, int col);
const char *CkCsv_getColumnName(HCkCsv cHandle, int index);
const char *CkCsv_lastErrorHtml(HCkCsv cHandle);
const char *CkCsv_lastErrorText(HCkCsv cHandle);
const char *CkCsv_lastErrorXml(HCkCsv cHandle);
const char *CkCsv_saveToString(HCkCsv cHandle);
const char *CkCsv_version(HCkCsv cHandle);
#endif
