#ifndef _CkSshTunnelW_H
#define _CkSshTunnelW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSshTunnel.h"

class CkSshKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSshTunnelW : public CkWideCharBase
{
    private:
	CkSshTunnel *m_impl;

        // Disallow assignment or copying this object.
	CkSshTunnelW(const CkSshTunnelW &) { }
	CkSshTunnelW &operator=(const CkSshTunnelW &) { return *this; }


    public:
	CkSshTunnelW()
	    {
	    m_impl = new CkSshTunnel;
	    m_impl->put_Utf8(true);
	    }
	CkSshTunnelW(CkSshTunnel *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSshTunnel;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSshTunnelW(bool bForMono)
	    {
	    m_impl = new CkSshTunnel;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSshTunnel *getMbPtr(void) { return m_impl; }
	CkSshTunnel &getMbRef(void) { return *m_impl; }
	const CkSshTunnel *getMbConstPtr(void) const { return m_impl; }
	const CkSshTunnel &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSshTunnelW() { delete m_impl; m_impl = 0; }

void get_AcceptThreadSessionLogPath(CkString &propVal);
void put_AcceptThreadSessionLogPath(const wchar_t *w1);
void get_ConnectLog(CkString &propVal);
void put_ConnectLog(const wchar_t *w1);
int get_ConnectTimeoutMs(void);
void put_ConnectTimeoutMs(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DestHostname(CkString &propVal);
void put_DestHostname(const wchar_t *w1);
int get_DestPort(void);
void put_DestPort(int propVal);
void get_HttpProxyAuthMethod(CkString &propVal);
void put_HttpProxyAuthMethod(const wchar_t *w1);
void get_HttpProxyDomain(CkString &propVal);
void put_HttpProxyDomain(const wchar_t *w1);
void get_HttpProxyHostname(CkString &propVal);
void put_HttpProxyHostname(const wchar_t *w1);
void get_HttpProxyPassword(CkString &propVal);
void put_HttpProxyPassword(const wchar_t *w1);
int get_HttpProxyPort(void);
void put_HttpProxyPort(int propVal);
void get_HttpProxyUsername(CkString &propVal);
void put_HttpProxyUsername(const wchar_t *w1);
int get_IdleTimeoutMs(void);
void put_IdleTimeoutMs(int propVal);
bool get_IsAccepting(void);
bool get_KeepConnectLog(void);
void put_KeepConnectLog(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_ListenBindIpAddress(CkString &propVal);
void put_ListenBindIpAddress(const wchar_t *w1);
int get_ListenPort(void);
int get_MaxPacketSize(void);
void put_MaxPacketSize(int propVal);
void get_OutboundBindIpAddress(CkString &propVal);
void put_OutboundBindIpAddress(const wchar_t *w1);
int get_OutboundBindPort(void);
void put_OutboundBindPort(int propVal);
int get_SoRcvBuf(void);
void put_SoRcvBuf(int propVal);
int get_SoSndBuf(void);
void put_SoSndBuf(int propVal);
void get_SocksHostname(CkString &propVal);
void put_SocksHostname(const wchar_t *w1);
void get_SocksPassword(CkString &propVal);
void put_SocksPassword(const wchar_t *w1);
int get_SocksPort(void);
void put_SocksPort(int propVal);
void get_SocksUsername(CkString &propVal);
void put_SocksUsername(const wchar_t *w1);
int get_SocksVersion(void);
void put_SocksVersion(int propVal);
void get_SshHostname(CkString &propVal);
void put_SshHostname(const wchar_t *w1);
void get_SshLogin(CkString &propVal);
void put_SshLogin(const wchar_t *w1);
void get_SshPassword(CkString &propVal);
void put_SshPassword(const wchar_t *w1);
int get_SshPort(void);
void put_SshPort(int propVal);
bool get_TcpNoDelay(void);
void put_TcpNoDelay(bool propVal);
void get_TunnelErrors(CkString &propVal);
void get_TunnelThreadSessionLogPath(CkString &propVal);
void put_TunnelThreadSessionLogPath(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool BeginAccepting(int listenPort);
void ClearTunnelErrors(void);
bool GetTunnelsXml(CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool SetSshAuthenticationKey(CkSshKeyW &key);
bool StopAccepting(void);
bool StopAllTunnels(int maxWaitMs);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *acceptThreadSessionLogPath(void);
const wchar_t *connectLog(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *destHostname(void);
const wchar_t *getTunnelsXml(void);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *listenBindIpAddress(void);
const wchar_t *outboundBindIpAddress(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *sshHostname(void);
const wchar_t *sshLogin(void);
const wchar_t *sshPassword(void);
const wchar_t *tunnelErrors(void);
const wchar_t *tunnelThreadSessionLogPath(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
