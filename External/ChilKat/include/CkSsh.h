// CkSsh.h: interface for the CkSsh class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkSsh_H
#define _CkSsh_H



#include "CkString.h"
class CkByteData;
class CkSshProgress;
class CkSshKey;
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkSsh
class CkSsh  : public CkMultiByteBase
{
    private:
	CkSshProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkSsh(const CkSsh &);
	CkSsh &operator=(const CkSsh &);


    public:
	CkSsh(void *impl);

	CkSsh();
	virtual ~CkSsh();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// HOSTKEYALG_BEGIN
	void get_HostKeyAlg(CkString &str);
	const char *hostKeyAlg(void);
	void put_HostKeyAlg(const char *newVal);
	// HOSTKEYALG_END

	// SOSNDBUF_BEGIN
	int get_SoSndBuf(void);
	void put_SoSndBuf(int newVal);
	// SOSNDBUF_END

	// SORCVBUF_BEGIN
	int get_SoRcvBuf(void);
	void put_SoRcvBuf(int newVal);
	// SORCVBUF_END

	// HTTPPROXYDOMAIN_BEGIN
	void get_HttpProxyDomain(CkString &str);
	const char *httpProxyDomain(void);
	void put_HttpProxyDomain(const char *newVal);
	// HTTPPROXYDOMAIN_END


	// Error log retrieval and saving (these methods are common to all Chilkat VC++ classes.)
	
	CkSshProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkSshProgress *progress);

	// CONNECT_BEGIN
	// eventCallbacks
	bool Connect(const char *hostname, int port);
	// CONNECT_END
	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END
	// AUTHENTICATEPW_BEGIN
	// eventCallbacks
	bool AuthenticatePw(const char *login, const char *password);
	// AUTHENTICATEPW_END
	// KEEPSESSIONLOG_BEGIN
	bool get_KeepSessionLog(void);
	void put_KeepSessionLog(bool newVal);
	// KEEPSESSIONLOG_END
	// SESSIONLOG_BEGIN
	void get_SessionLog(CkString &str);
	const char *sessionLog(void);
	// SESSIONLOG_END
	// IDLETIMEOUTMS_BEGIN
	int get_IdleTimeoutMs(void);
	void put_IdleTimeoutMs(int newVal);
	// IDLETIMEOUTMS_END
	// CONNECTTIMEOUTMS_BEGIN
	int get_ConnectTimeoutMs(void);
	void put_ConnectTimeoutMs(int newVal);
	// CONNECTTIMEOUTMS_END
	// CHANNELOPENFAILCODE_BEGIN
	int get_ChannelOpenFailCode(void);
	// CHANNELOPENFAILCODE_END
	// DISCONNECTCODE_BEGIN
	int get_DisconnectCode(void);
	// DISCONNECTCODE_END
	// DISCONNECTREASON_BEGIN
	void get_DisconnectReason(CkString &str);
	const char *disconnectReason(void);
	// DISCONNECTREASON_END
	// CHANNELOPENFAILREASON_BEGIN
	void get_ChannelOpenFailReason(CkString &str);
	const char *channelOpenFailReason(void);
	// CHANNELOPENFAILREASON_END
	// MAXPACKETSIZE_BEGIN
	int get_MaxPacketSize(void);
	void put_MaxPacketSize(int newVal);
	// MAXPACKETSIZE_END
	// DISCONNECT_BEGIN
	void Disconnect(void);
	// DISCONNECT_END
	// OPENSESSIONCHANNEL_BEGIN
	// eventCallbacks
	int OpenSessionChannel(void);
	// OPENSESSIONCHANNEL_END
	// OPENX11CHANNEL_BEGIN
	//int OpenX11Channel(const char *xAddress, int port);
	// OPENX11CHANNEL_END
	// OPENFORWARDEDTCPIPCHANNEL_BEGIN
	//int OpenForwardedTcpIpChannel(void);
	// OPENFORWARDEDTCPIPCHANNEL_END
	// OPENDIRECTTCPIPCHANNEL_BEGIN
	//int OpenDirectTcpIpChannel(void);
	// OPENDIRECTTCPIPCHANNEL_END
	// OPENCUSTOMCHANNEL_BEGIN
	// eventCallbacks
	int OpenCustomChannel(const char *channelType);
	// OPENCUSTOMCHANNEL_END
	// NUMOPENCHANNELS_BEGIN
	int get_NumOpenChannels(void);
	// NUMOPENCHANNELS_END
	// GETCHANNELNUMBER_BEGIN
	int GetChannelNumber(int index);
	// GETCHANNELNUMBER_END
	// GETCHANNELTYPE_BEGIN
	bool GetChannelType(int index, CkString &outStr);
	const char *getChannelType(int index);
	// GETCHANNELTYPE_END
	// SENDREQPTY_BEGIN
	// eventCallbacks
	bool SendReqPty(int channelNum, const char *xTermEnvVar, int widthInChars, int heightInRows, int pixWidth, int pixHeight);
	// SENDREQPTY_END
	// SENDREQX11FORWARDING_BEGIN
	// eventCallbacks
	bool SendReqX11Forwarding(int channelNum, bool singleConnection, const char *authProt, const char *authCookie, int screenNum);
	// SENDREQX11FORWARDING_END
	// SENDREQSETENV_BEGIN
	// eventCallbacks
	bool SendReqSetEnv(int channelNum, const char *name, const char *value);
	// SENDREQSETENV_END
	// SENDREQSHELL_BEGIN
	// eventCallbacks
	bool SendReqShell(int channelNum);
	// SENDREQSHELL_END
	// SENDREQEXEC_BEGIN
	// eventCallbacks
	bool SendReqExec(int channelNum, const char *command);
	// SENDREQEXEC_END
	// SENDREQSUBSYSTEM_BEGIN
	// eventCallbacks
	bool SendReqSubsystem(int channelNum, const char *subsystemName);
	// SENDREQSUBSYSTEM_END
	// SENDREQWINDOWCHANGE_BEGIN
	// eventCallbacks
	bool SendReqWindowChange(int channelNum, int widthInChars, int heightInRows, int pixWidth, int pixHeight);
	// SENDREQWINDOWCHANGE_END
	// SENDREQXONXOFF_BEGIN
	// eventCallbacks
	bool SendReqXonXoff(int channelNum, bool clientCanDo);
	// SENDREQXONXOFF_END
	// SENDREQSIGNAL_BEGIN
	// eventCallbacks
	bool SendReqSignal(int channelNum, const char *signalName);
	// SENDREQSIGNAL_END
	// CHANNELSENDDATA_BEGIN
	// eventCallbacks
	bool ChannelSendData(int channelNum, const CkByteData &data);
	// CHANNELSENDDATA_END
	// CHANNELSENDSTRING_BEGIN
	// eventCallbacks
	bool ChannelSendString(int channelNum, const char *strData, const char *charset);
	// CHANNELSENDSTRING_END
	// CHANNELPOLL_BEGIN
	// eventCallbacks
	int ChannelPoll(int channelNum, int pollTimeoutMs);
	// CHANNELPOLL_END
	// CHANNELREADANDPOLL_BEGIN
	// eventCallbacks
	int ChannelReadAndPoll(int channelNum, int pollTimeoutMs);
	// CHANNELREADANDPOLL_END
	// CHANNELREAD_BEGIN
	// eventCallbacks
	int ChannelRead(int channelNum);
	// CHANNELREAD_END
	// GETRECEIVEDDATA_BEGIN
	bool GetReceivedData(int channelNum, CkByteData &outBytes);
	// GETRECEIVEDDATA_END
	// GETRECEIVEDSTDERR_BEGIN
	bool GetReceivedStderr(int channelNum, CkByteData &outBytes);
	// GETRECEIVEDSTDERR_END
	// CHANNELRECEIVEDEOF_BEGIN
	bool ChannelReceivedEof(int channelNum);
	// CHANNELRECEIVEDEOF_END
	// CHANNELRECEIVEDCLOSE_BEGIN
	bool ChannelReceivedClose(int channelNum);
	// CHANNELRECEIVEDCLOSE_END
	// CHANNELSENDCLOSE_BEGIN
	// eventCallbacks
	bool ChannelSendClose(int channelNum);
	// CHANNELSENDCLOSE_END
	// CHANNELSENDEOF_BEGIN
	// eventCallbacks
	bool ChannelSendEof(int channelNum);
	// CHANNELSENDEOF_END
	// CHANNELISOPEN_BEGIN
	bool ChannelIsOpen(int channelNum);
	// CHANNELISOPEN_END
	// CHANNELRECEIVETOCLOSE_BEGIN
	// eventCallbacks
	bool ChannelReceiveToClose(int channelNum);
	// CHANNELRECEIVETOCLOSE_END
	// CLEARTTYMODES_BEGIN
	void ClearTtyModes(void);
	// CLEARTTYMODES_END
	// SETTTYMODE_BEGIN
	bool SetTtyMode(const char *name, int value);
	// SETTTYMODE_END
	// ISCONNECTED_BEGIN
	bool get_IsConnected(void);
	// ISCONNECTED_END
	// REKEY_BEGIN
	// eventCallbacks
	bool ReKey(void);
	// REKEY_END
	// AUTHENTICATEPK_BEGIN
	// eventCallbacks
	bool AuthenticatePk(const char *username, CkSshKey &privateKey);
	// AUTHENTICATEPK_END
	// GETRECEIVEDTEXT_BEGIN
	bool GetReceivedText(int channelNum, const char *charset, CkString &outStr);
	const char *getReceivedText(int channelNum, const char *charset);
	// GETRECEIVEDTEXT_END
	// GETRECEIVEDNUMBYTES_BEGIN
	int GetReceivedNumBytes(int channelNum);
	// GETRECEIVEDNUMBYTES_END
	// CHANNELRECEIVEUNTILMATCH_BEGIN
	// eventCallbacks
	bool ChannelReceiveUntilMatch(int channelNum, const char *matchPattern, const char *charset, bool caseSensitive);
	// CHANNELRECEIVEUNTILMATCH_END
	// SENDIGNORE_BEGIN
	// eventCallbacks
	bool SendIgnore(void);
	//bool SendReKey(void);
	// SENDIGNORE_END
	// OPENDIRECTTCPIPCHANNEL_BEGIN
	// eventCallbacks
	int OpenDirectTcpIpChannel(const char *hostname, int port);
	// OPENDIRECTTCPIPCHANNEL_END
	// GETRECEIVEDTEXTS_BEGIN
	bool GetReceivedTextS(int channelNum, const char *substr, const char *charset, CkString &outStr);
	const char *getReceivedTextS(int channelNum, const char *substr, const char *charset);
	// GETRECEIVEDTEXTS_END
	// GETRECEIVEDDATAN_BEGIN
	bool GetReceivedDataN(int channelNum, int numBytes, CkByteData &outBytes);
	// GETRECEIVEDDATAN_END
	// PEEKRECEIVEDTEXT_BEGIN
	bool PeekReceivedText(int channelNum, const char *charset, CkString &outStr);
	const char *peekReceivedText(int channelNum, const char *charset);
	// PEEKRECEIVEDTEXT_END
	// HEARTBEATMS_BEGIN
	int get_HeartbeatMs(void);
	void put_HeartbeatMs(int newVal);
	// HEARTBEATMS_END
	// CHANNELRECEIVEDEXITSTATUS_BEGIN
	bool ChannelReceivedExitStatus(int channelNum);
	// CHANNELRECEIVEDEXITSTATUS_END
	// GETCHANNELEXITSTATUS_BEGIN
	int GetChannelExitStatus(int channelNum);
	// GETCHANNELEXITSTATUS_END
	// CLIENTIDENTIFIER_BEGIN
	void get_ClientIdentifier(CkString &str);
	const char *clientIdentifier(void);
	void put_ClientIdentifier(const char *newVal);
	// CLIENTIDENTIFIER_END
	// READTIMEOUTMS_BEGIN
	int get_ReadTimeoutMs(void);
	void put_ReadTimeoutMs(int newVal);
	// READTIMEOUTMS_END
	// TCPNODELAY_BEGIN
	bool get_TcpNoDelay(void);
	void put_TcpNoDelay(bool newVal);
	// TCPNODELAY_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// HOSTKEYFINGERPRINT_BEGIN
	void get_HostKeyFingerprint(CkString &str);
	const char *hostKeyFingerprint(void);
	// HOSTKEYFINGERPRINT_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
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
	// HTTPPROXYAUTHMETHOD_BEGIN
	void get_HttpProxyAuthMethod(CkString &str);
	const char *httpProxyAuthMethod(void);
	void put_HttpProxyAuthMethod(const char *newVal);
	// HTTPPROXYAUTHMETHOD_END
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
	// HTTPPROXYHOSTNAME_BEGIN
	void get_HttpProxyHostname(CkString &str);
	const char *httpProxyHostname(void);
	void put_HttpProxyHostname(const char *newVal);
	// HTTPPROXYHOSTNAME_END
	// HTTPPROXYPORT_BEGIN
	int get_HttpProxyPort(void);
	void put_HttpProxyPort(int newVal);
	// HTTPPROXYPORT_END
	// CHANNELRECEIVEUNTILMATCHN_BEGIN
	// eventCallbacks
	bool ChannelReceiveUntilMatchN(int channelNum, CkStringArray &matchPatterns, const char *charset, bool caseSensitive);
	// CHANNELRECEIVEUNTILMATCHN_END
	// CHANNELREADANDPOLL2_BEGIN
	// eventCallbacks
	int ChannelReadAndPoll2(int channelNum, int pollTimeoutMs, int maxNumBytes);
	// CHANNELREADANDPOLL2_END
	// PASSWORDCHANGEREQUESTED_BEGIN
	bool get_PasswordChangeRequested(void);
	// PASSWORDCHANGEREQUESTED_END
	// AUTHENTICATEPWPK_BEGIN
	// eventCallbacks
	bool AuthenticatePwPk(const char *username, const char *password, CkSshKey &privateKey);
	// AUTHENTICATEPWPK_END
	// CLIENTIPADDRESS_BEGIN
	void get_ClientIpAddress(CkString &str);
	const char *clientIpAddress(void);
	void put_ClientIpAddress(const char *newVal);
	// CLIENTIPADDRESS_END
	// STDERRTOSTDOUT_BEGIN
	bool get_StderrToStdout(void);
	void put_StderrToStdout(bool newVal);
	// STDERRTOSTDOUT_END
	// FORCECIPHER_BEGIN
	void get_ForceCipher(CkString &str);
	const char *forceCipher(void);
	void put_ForceCipher(const char *newVal);
	// FORCECIPHER_END

	// SSH_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


