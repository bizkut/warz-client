// CkSshTunnel.h: interface for the CkSshTunnel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkSshTunnel_H
#define _CkSshTunnel_H



#include "CkString.h"
class CkSshTunnelProgress;
#include "CkMultiByteBase.h"

class CkSshKey;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkSshTunnel
class CkSshTunnel  : public CkMultiByteBase
{
    private:

	// Don't allow assignment or copying these objects.
	CkSshTunnel(const CkSshTunnel &);
	CkSshTunnel &operator=(const CkSshTunnel &);


    public:
	CkSshTunnel(void *impl);

	CkSshTunnel();
	virtual ~CkSshTunnel();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// KEEPCONNECTLOG_BEGIN
	bool get_KeepConnectLog(void);
	void put_KeepConnectLog(bool newVal);
	// KEEPCONNECTLOG_END

	// SORCVBUF_BEGIN
	int get_SoRcvBuf(void);
	void put_SoRcvBuf(int newVal);
	// SORCVBUF_END

	// SOSNDBUF_BEGIN
	int get_SoSndBuf(void);
	void put_SoSndBuf(int newVal);
	// SOSNDBUF_END

	// IDLETIMEOUTMS_BEGIN
	int get_IdleTimeoutMs(void);
	void put_IdleTimeoutMs(int newVal);
	// IDLETIMEOUTMS_END

	// HTTPPROXYDOMAIN_BEGIN
	void get_HttpProxyDomain(CkString &str);
	const char *httpProxyDomain(void);
	void put_HttpProxyDomain(const char *newVal);
	// HTTPPROXYDOMAIN_END



	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END
	// SSHLOGIN_BEGIN
	void get_SshLogin(CkString &str);
	const char *sshLogin(void);
	void put_SshLogin(const char *newVal);
	// SSHLOGIN_END
	// SSHPASSWORD_BEGIN
	void get_SshPassword(CkString &str);
	const char *sshPassword(void);
	void put_SshPassword(const char *newVal);
	// SSHPASSWORD_END
	// CONNECTLOG_BEGIN
	void get_ConnectLog(CkString &str);
	const char *connectLog(void);
	void put_ConnectLog(const char *newVal);
	// CONNECTLOG_END
	// DESTHOSTNAME_BEGIN
	void get_DestHostname(CkString &str);
	const char *destHostname(void);
	void put_DestHostname(const char *newVal);
	// DESTHOSTNAME_END
	// SSHSERVERHOSTNAME_BEGIN
	void get_SshHostname(CkString &str);
	const char *sshHostname(void);
	void put_SshHostname(const char *newVal);
	// SSHSERVERHOSTNAME_END
	// SSHSERVERPORT_BEGIN
	int get_SshPort(void);
	void put_SshPort(int newVal);
	// SSHSERVERPORT_END
	// DESTPORT_BEGIN
	int get_DestPort(void);
	void put_DestPort(int newVal);
	// DESTPORT_END
	// ISACCEPTING_BEGIN
	bool get_IsAccepting(void);
	// ISACCEPTING_END
	// STOPALLTUNNELS_BEGIN
	bool StopAllTunnels(int maxWaitMs);
	// STOPALLTUNNELS_END
	// GETTUNNELSXML_BEGIN
	bool GetTunnelsXml(CkString &outStr);
	const char *getTunnelsXml(void);
	// GETTUNNELSXML_END
	// STOPACCEPTING_BEGIN
	bool StopAccepting(void);
	// STOPACCEPTING_END
	// BEGINACCEPTING_BEGIN
	bool BeginAccepting(int listenPort);
	// BEGINACCEPTING_END
	// SETSSHAUTHENTICATIONKEY_BEGIN
	bool SetSshAuthenticationKey(CkSshKey &key);
	// SETSSHAUTHENTICATIONKEY_END
	// LISTENBINDIPADDRESS_BEGIN
	void get_ListenBindIpAddress(CkString &str);
	const char *listenBindIpAddress(void);
	void put_ListenBindIpAddress(const char *newVal);
	// LISTENBINDIPADDRESS_END
	// OUTBOUNDBINDIPADDRESS_BEGIN
	void get_OutboundBindIpAddress(CkString &str);
	const char *outboundBindIpAddress(void);
	void put_OutboundBindIpAddress(const char *newVal);
	// OUTBOUNDBINDIPADDRESS_END
	// OUTBOUNDBINDPORT_BEGIN
	int get_OutboundBindPort(void);
	void put_OutboundBindPort(int newVal);
	// OUTBOUNDBINDPORT_END
	// MAXPACKETSIZE_BEGIN
	int get_MaxPacketSize(void);
	void put_MaxPacketSize(int newVal);
	// MAXPACKETSIZE_END
	// TCPNODELAY_BEGIN
	bool get_TcpNoDelay(void);
	void put_TcpNoDelay(bool newVal);
	// TCPNODELAY_END
	// HTTPPROXYHOSTNAME_BEGIN
	void get_HttpProxyHostname(CkString &str);
	const char *httpProxyHostname(void);
	void put_HttpProxyHostname(const char *newVal);
	// HTTPPROXYHOSTNAME_END
	// HTTPPROXYUSERNAME_BEGIN
	void get_HttpProxyUsername(CkString &str);
	const char *httpProxyUsername(void);
	void put_HttpProxyUsername(const char *newVal);
	// HTTPPROXYUSERNAME_END
	// HTTPPROXYPASSWORD_BEGIN
	void get_HttpProxyPassword(CkString &str);
	const char *httpProxyPassword(void);
	void put_HttpProxyPassword(const char *newVal);
	// HTTPPROXYPASSWORD_END
	// HTTPPROXYAUTHMETHOD_BEGIN
	void get_HttpProxyAuthMethod(CkString &str);
	const char *httpProxyAuthMethod(void);
	void put_HttpProxyAuthMethod(const char *newVal);
	// HTTPPROXYAUTHMETHOD_END
	// HTTPPROXYPORT_BEGIN
	int get_HttpProxyPort(void);
	void put_HttpProxyPort(int newVal);
	// HTTPPROXYPORT_END
	// SOCKSPORT_BEGIN
	int get_SocksPort(void);
	void put_SocksPort(int newVal);
	// SOCKSPORT_END
	// SOCKSHOSTNAME_BEGIN
	void get_SocksHostname(CkString &str);
	const char *socksHostname(void);
	void put_SocksHostname(const char *newVal);
	// SOCKSHOSTNAME_END
	// SOCKSUSERNAME_BEGIN
	void get_SocksUsername(CkString &str);
	const char *socksUsername(void);
	void put_SocksUsername(const char *newVal);
	// SOCKSUSERNAME_END
	// SOCKSPASSWORD_BEGIN
	void get_SocksPassword(CkString &str);
	const char *socksPassword(void);
	void put_SocksPassword(const char *newVal);
	// SOCKSPASSWORD_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
	// CONNECTTIMEOUTMS_BEGIN
	int get_ConnectTimeoutMs(void);
	void put_ConnectTimeoutMs(int newVal);
	// CONNECTTIMEOUTMS_END
	// LISTENPORT_BEGIN
	int get_ListenPort(void);
	// LISTENPORT_END
	// TUNNELTHREADSESSIONLOGPATH_BEGIN
	void get_TunnelThreadSessionLogPath(CkString &str);
	const char *tunnelThreadSessionLogPath(void);
	void put_TunnelThreadSessionLogPath(const char *newVal);
	// TUNNELTHREADSESSIONLOGPATH_END
	// ACCEPTTHREADSESSIONLOGPATH_BEGIN
	void get_AcceptThreadSessionLogPath(CkString &str);
	const char *acceptThreadSessionLogPath(void);
	void put_AcceptThreadSessionLogPath(const char *newVal);
	// ACCEPTTHREADSESSIONLOGPATH_END
	// CLEARTUNNELERRORS_BEGIN
	void ClearTunnelErrors(void);
	// CLEARTUNNELERRORS_END
	// TUNNELERRORS_BEGIN
	void get_TunnelErrors(CkString &str);
	const char *tunnelErrors(void);
	// TUNNELERRORS_END

	// SSHTUNNEL_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


