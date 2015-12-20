#ifndef _C_CkSocksProxy_H
#define _C_CkSocksProxy_H
#include "Chilkat_C.h"

HCkSocksProxy CkSocksProxy_Create(void);
void CkSocksProxy_Dispose(HCkSocksProxy handle);
BOOL CkSocksProxy_getAllowUnauthenticatedSocks5(HCkSocksProxy cHandle);
void CkSocksProxy_putAllowUnauthenticatedSocks5(HCkSocksProxy cHandle, BOOL newVal);
BOOL CkSocksProxy_getAuthenticatedSocks5(HCkSocksProxy cHandle);
void CkSocksProxy_getClientIp(HCkSocksProxy cHandle, HCkString retval);
int CkSocksProxy_getClientPort(HCkSocksProxy cHandle);
BOOL CkSocksProxy_getConnectionPending(HCkSocksProxy cHandle);
void CkSocksProxy_getDebugLogFilePath(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_putDebugLogFilePath(HCkSocksProxy cHandle, const char *newVal);
HCkSocksProxyProgress CkSocksProxy_getEventCallbackObject(HCkSocksProxy cHandle);
void CkSocksProxy_putEventCallbackObject(HCkSocksProxy cHandle, HCkSocksProxyProgress newVal);
void CkSocksProxy_getLastErrorHtml(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_getLastErrorText(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_getLastErrorXml(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_getListenBindIpAddress(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_putListenBindIpAddress(HCkSocksProxy cHandle, const char *newVal);
void CkSocksProxy_getLogin(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_getOutboundBindIpAddress(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_putOutboundBindIpAddress(HCkSocksProxy cHandle, const char *newVal);
int CkSocksProxy_getOutboundBindPort(HCkSocksProxy cHandle);
void CkSocksProxy_putOutboundBindPort(HCkSocksProxy cHandle, int newVal);
void CkSocksProxy_getPassword(HCkSocksProxy cHandle, HCkString retval);
void CkSocksProxy_getServerIp(HCkSocksProxy cHandle, HCkString retval);
int CkSocksProxy_getServerPort(HCkSocksProxy cHandle);
int CkSocksProxy_getSocksVersion(HCkSocksProxy cHandle);
BOOL CkSocksProxy_getUtf8(HCkSocksProxy cHandle);
void CkSocksProxy_putUtf8(HCkSocksProxy cHandle, BOOL newVal);
BOOL CkSocksProxy_getVerboseLogging(HCkSocksProxy cHandle);
void CkSocksProxy_putVerboseLogging(HCkSocksProxy cHandle, BOOL newVal);
void CkSocksProxy_getVersion(HCkSocksProxy cHandle, HCkString retval);
BOOL CkSocksProxy_AllowConnection(HCkSocksProxy cHandle);
BOOL CkSocksProxy_GetTunnelsXml(HCkSocksProxy cHandle, HCkString outStr);
BOOL CkSocksProxy_Initialize(HCkSocksProxy cHandle, int port);
BOOL CkSocksProxy_ProceedSocks5(HCkSocksProxy cHandle);
BOOL CkSocksProxy_RejectConnection(HCkSocksProxy cHandle);
BOOL CkSocksProxy_SaveLastError(HCkSocksProxy cHandle, const char *path);
BOOL CkSocksProxy_StopAllTunnels(HCkSocksProxy cHandle, int maxWaitMs);
BOOL CkSocksProxy_UnlockComponent(HCkSocksProxy cHandle, const char *unlockCode);
BOOL CkSocksProxy_WaitForConnection(HCkSocksProxy cHandle, int maxWaitMs);
const char *CkSocksProxy_clientIp(HCkSocksProxy cHandle);
const char *CkSocksProxy_debugLogFilePath(HCkSocksProxy cHandle);
const char *CkSocksProxy_getTunnelsXml(HCkSocksProxy cHandle);
const char *CkSocksProxy_lastErrorHtml(HCkSocksProxy cHandle);
const char *CkSocksProxy_lastErrorText(HCkSocksProxy cHandle);
const char *CkSocksProxy_lastErrorXml(HCkSocksProxy cHandle);
const char *CkSocksProxy_listenBindIpAddress(HCkSocksProxy cHandle);
const char *CkSocksProxy_login(HCkSocksProxy cHandle);
const char *CkSocksProxy_outboundBindIpAddress(HCkSocksProxy cHandle);
const char *CkSocksProxy_password(HCkSocksProxy cHandle);
const char *CkSocksProxy_serverIp(HCkSocksProxy cHandle);
const char *CkSocksProxy_version(HCkSocksProxy cHandle);
#endif
