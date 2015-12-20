#ifndef _CkSshW_H
#define _CkSshW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSsh.h"

class CkStringArrayW;
class CkSshProgress;
class CkSshKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSshW : public CkWideCharBase
{
    private:
	CkSsh *m_impl;

        // Disallow assignment or copying this object.
	CkSshW(const CkSshW &) { }
	CkSshW &operator=(const CkSshW &) { return *this; }


    public:
	CkSshW()
	    {
	    m_impl = new CkSsh;
	    m_impl->put_Utf8(true);
	    }
	CkSshW(CkSsh *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSsh;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSshW(bool bForMono)
	    {
	    m_impl = new CkSsh;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSsh *getMbPtr(void) { return m_impl; }
	CkSsh &getMbRef(void) { return *m_impl; }
	const CkSsh *getMbConstPtr(void) const { return m_impl; }
	const CkSsh &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSshW() { delete m_impl; m_impl = 0; }

int get_ChannelOpenFailCode(void);
void get_ChannelOpenFailReason(CkString &propVal);
void get_ClientIdentifier(CkString &propVal);
void put_ClientIdentifier(const wchar_t *w1);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
int get_ConnectTimeoutMs(void);
void put_ConnectTimeoutMs(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_DisconnectCode(void);
void get_DisconnectReason(CkString &propVal);
CkSshProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkSshProgress * propVal);
void get_ForceCipher(CkString &propVal);
void put_ForceCipher(const wchar_t *w1);
int get_HeartbeatMs(void);
void put_HeartbeatMs(int propVal);
void get_HostKeyAlg(CkString &propVal);
void put_HostKeyAlg(const wchar_t *w1);
void get_HostKeyFingerprint(CkString &propVal);
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
bool get_IsConnected(void);
bool get_KeepSessionLog(void);
void put_KeepSessionLog(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_MaxPacketSize(void);
void put_MaxPacketSize(int propVal);
int get_NumOpenChannels(void);
bool get_PasswordChangeRequested(void);
int get_ReadTimeoutMs(void);
void put_ReadTimeoutMs(int propVal);
void get_SessionLog(CkString &propVal);
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
bool get_StderrToStdout(void);
void put_StderrToStdout(bool propVal);
bool get_TcpNoDelay(void);
void put_TcpNoDelay(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AuthenticatePk(const wchar_t *w1,CkSshKeyW &privateKey);
bool AuthenticatePw(const wchar_t *w1,const wchar_t *w2);
bool AuthenticatePwPk(const wchar_t *w1,const wchar_t *w2,CkSshKeyW &privateKey);
bool ChannelIsOpen(int channelNum);
int ChannelPoll(int channelNum,int pollTimeoutMs);
int ChannelRead(int channelNum);
int ChannelReadAndPoll(int channelNum,int pollTimeoutMs);
int ChannelReadAndPoll2(int channelNum,int pollTimeoutMs,int maxNumBytes);
bool ChannelReceiveToClose(int channelNum);
bool ChannelReceiveUntilMatch(int channelNum,const wchar_t *w1,const wchar_t *w2,bool caseSensitive);
bool ChannelReceiveUntilMatchN(int channelNum,CkStringArrayW &matchPatterns,const wchar_t *w1,bool caseSensitive);
bool ChannelReceivedClose(int channelNum);
bool ChannelReceivedEof(int channelNum);
bool ChannelReceivedExitStatus(int channelNum);
bool ChannelSendClose(int channelNum);
bool ChannelSendData(int channelNum,const CkByteData &data);
bool ChannelSendEof(int channelNum);
bool ChannelSendString(int channelNum,const wchar_t *w1,const wchar_t *w2);
void ClearTtyModes(void);
bool Connect(const wchar_t *w1,int port);
void Disconnect(void);
int GetChannelExitStatus(int channelNum);
int GetChannelNumber(int index);
bool GetChannelType(int index,CkString &outStr);
bool GetReceivedData(int channelNum,CkByteData &outBytes);
bool GetReceivedDataN(int channelNum,int numBytes,CkByteData &outBytes);
int GetReceivedNumBytes(int channelNum);
bool GetReceivedStderr(int channelNum,CkByteData &outBytes);
bool GetReceivedText(int channelNum,const wchar_t *w1,CkString &outStr);
bool GetReceivedTextS(int channelNum,const wchar_t *w1,const wchar_t *w2,CkString &outStr);
int OpenCustomChannel(const wchar_t *w1);
int OpenDirectTcpIpChannel(const wchar_t *w1,int port);
int OpenSessionChannel(void);
bool PeekReceivedText(int channelNum,const wchar_t *w1,CkString &outStr);
bool ReKey(void);
bool SaveLastError(const wchar_t *w1);
bool SendIgnore(void);
bool SendReqExec(int channelNum,const wchar_t *w1);
bool SendReqPty(int channelNum,const wchar_t *w1,int widthInChars,int heightInRows,int pixWidth,int pixHeight);
bool SendReqSetEnv(int channelNum,const wchar_t *w1,const wchar_t *w2);
bool SendReqShell(int channelNum);
bool SendReqSignal(int channelNum,const wchar_t *w1);
bool SendReqSubsystem(int channelNum,const wchar_t *w1);
bool SendReqWindowChange(int channelNum,int widthInChars,int heightInRows,int pixWidth,int pixHeight);
bool SendReqX11Forwarding(int channelNum,bool singleConnection,const wchar_t *w1,const wchar_t *w2,int screenNum);
bool SendReqXonXoff(int channelNum,bool clientCanDo);
bool SetTtyMode(const wchar_t *w1,int value);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *channelOpenFailReason(void);
const wchar_t *clientIdentifier(void);
const wchar_t *clientIpAddress(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *disconnectReason(void);
const wchar_t *forceCipher(void);
const wchar_t *getChannelType(int index);
const wchar_t *getReceivedText(int channelNum,const wchar_t *w1);
const wchar_t *getReceivedTextS(int channelNum,const wchar_t *w1,const wchar_t *w2);
const wchar_t *hostKeyAlg(void);
const wchar_t *hostKeyFingerprint(void);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *peekReceivedText(int channelNum,const wchar_t *w1);
const wchar_t *sessionLog(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
