#ifndef _C_CkSocksProxy_W_H
#define _C_CkSocksProxy_W_H
#include "Chilkat_C.h"

HCkSocksProxyW CkSocksProxyW_Create(void);
HCkSocksProxyW CkSocksProxyW_Create2(bool bForMono);
void CkSocksProxyW_Dispose(HCkSocksProxyW handle);
BOOL CkSocksProxyW_getAllowUnauthenticatedSocks5(HCkSocksProxyW cHandle);
void CkSocksProxyW_putAllowUnauthenticatedSocks5(HCkSocksProxyW cHandle, BOOL newVal);
BOOL CkSocksProxyW_getAuthenticatedSocks5(HCkSocksProxyW cHandle);
void CkSocksProxyW_getClientIp(HCkSocksProxyW cHandle, HCkStringW retval);
int CkSocksProxyW_getClientPort(HCkSocksProxyW cHandle);
BOOL CkSocksProxyW_getConnectionPending(HCkSocksProxyW cHandle);
void CkSocksProxyW_getDebugLogFilePath(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_putDebugLogFilePath(HCkSocksProxyW cHandle, const wchar_t *newVal);
HCkSocksProxyProgressW CkSocksProxyW_getEventCallbackObject(HCkSocksProxyW cHandle);
void CkSocksProxyW_putEventCallbackObject(HCkSocksProxyW cHandle, HCkSocksProxyProgressW newVal);
void CkSocksProxyW_getLastErrorHtml(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_getLastErrorText(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_getLastErrorXml(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_getListenBindIpAddress(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_putListenBindIpAddress(HCkSocksProxyW cHandle, const wchar_t *newVal);
void CkSocksProxyW_getLogin(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_getOutboundBindIpAddress(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_putOutboundBindIpAddress(HCkSocksProxyW cHandle, const wchar_t *newVal);
int CkSocksProxyW_getOutboundBindPort(HCkSocksProxyW cHandle);
void CkSocksProxyW_putOutboundBindPort(HCkSocksProxyW cHandle, int newVal);
void CkSocksProxyW_getPassword(HCkSocksProxyW cHandle, HCkStringW retval);
void CkSocksProxyW_getServerIp(HCkSocksProxyW cHandle, HCkStringW retval);
int CkSocksProxyW_getServerPort(HCkSocksProxyW cHandle);
int CkSocksProxyW_getSocksVersion(HCkSocksProxyW cHandle);
BOOL CkSocksProxyW_getVerboseLogging(HCkSocksProxyW cHandle);
void CkSocksProxyW_putVerboseLogging(HCkSocksProxyW cHandle, BOOL newVal);
void CkSocksProxyW_getVersion(HCkSocksProxyW cHandle, HCkStringW retval);
BOOL CkSocksProxyW_AllowConnection(HCkSocksProxyW cHandle);
BOOL CkSocksProxyW_GetTunnelsXml(HCkSocksProxyW cHandle, HCkStringW outStr);
BOOL CkSocksProxyW_Initialize(HCkSocksProxyW cHandle, int port);
BOOL CkSocksProxyW_ProceedSocks5(HCkSocksProxyW cHandle);
BOOL CkSocksProxyW_RejectConnection(HCkSocksProxyW cHandle);
BOOL CkSocksProxyW_SaveLastError(HCkSocksProxyW cHandle, const wchar_t *path);
BOOL CkSocksProxyW_StopAllTunnels(HCkSocksProxyW cHandle, int maxWaitMs);
BOOL CkSocksProxyW_UnlockComponent(HCkSocksProxyW cHandle, const wchar_t *unlockCode);
BOOL CkSocksProxyW_WaitForConnection(HCkSocksProxyW cHandle, int maxWaitMs);
const wchar_t *CkSocksProxyW__clientIp(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__debugLogFilePath(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__getTunnelsXml(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__lastErrorHtml(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__lastErrorText(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__lastErrorXml(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__listenBindIpAddress(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__login(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__outboundBindIpAddress(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__password(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__serverIp(HCkSocksProxyW cHandle);
const wchar_t *CkSocksProxyW__version(HCkSocksProxyW cHandle);
#endif
