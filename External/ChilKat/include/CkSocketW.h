#ifndef _CkSocketW_H
#define _CkSocketW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSocket.h"

class CkSocketProgress;
class CkCertW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSocketW : public CkWideCharBase
{
    private:
	CkSocket *m_impl;

        // Disallow assignment or copying this object.
	CkSocketW(const CkSocketW &) { }
	CkSocketW &operator=(const CkSocketW &) { return *this; }


    public:
	CkSocketW()
	    {
	    m_impl = new CkSocket;
	    m_impl->put_Utf8(true);
	    }
	CkSocketW(CkSocket *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSocket;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSocketW(bool bForMono)
	    {
	    m_impl = new CkSocket;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSocket *getMbPtr(void) { return m_impl; }
	CkSocket &getMbRef(void) { return *m_impl; }
	const CkSocket *getMbConstPtr(void) const { return m_impl; }
	const CkSocket &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSocketW() { delete m_impl; m_impl = 0; }

bool get_AsyncAcceptFinished(void);
void get_AsyncAcceptLog(CkString &propVal);
bool get_AsyncAcceptSuccess(void);
bool get_AsyncConnectFinished(void);
void get_AsyncConnectLog(CkString &propVal);
bool get_AsyncConnectSuccess(void);
bool get_AsyncDnsFinished(void);
void get_AsyncDnsLog(CkString &propVal);
void get_AsyncDnsResult(CkString &propVal);
bool get_AsyncDnsSuccess(void);
bool get_AsyncReceiveFinished(void);
void get_AsyncReceiveLog(CkString &propVal);
bool get_AsyncReceiveSuccess(void);
void get_AsyncReceivedBytes(CkByteData &propVal);
void get_AsyncReceivedString(CkString &propVal);
bool get_AsyncSendFinished(void);
void get_AsyncSendLog(CkString &propVal);
bool get_AsyncSendSuccess(void);
bool get_BigEndian(void);
void put_BigEndian(bool propVal);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
int get_ClientPort(void);
void put_ClientPort(int propVal);
long get_ConnectFailReason(void);
long get_DebugConnectDelayMs(void);
void put_DebugConnectDelayMs(long propVal);
long get_DebugDnsDelayMs(void);
void put_DebugDnsDelayMs(long propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_ElapsedSeconds(void);
CkSocketProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkSocketProgress * propVal);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
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
bool get_IsConnected(void);
bool get_KeepSessionLog(void);
void put_KeepSessionLog(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_LastMethodFailed(void);
bool get_ListenIpv6(void);
void put_ListenIpv6(bool propVal);
void get_LocalIpAddress(CkString &propVal);
int get_LocalPort(void);
long get_MaxReadIdleMs(void);
void put_MaxReadIdleMs(long propVal);
long get_MaxSendIdleMs(void);
void put_MaxSendIdleMs(long propVal);
void get_MyIpAddress(CkString &propVal);
int get_NumReceivedClientCerts(void);
int get_NumSocketsInSet(void);
int get_NumSslAcceptableClientCAs(void);
long get_ObjectId(void);
long get_ReceivePacketSize(void);
void put_ReceivePacketSize(long propVal);
int get_ReceivedCount(void);
void put_ReceivedCount(int propVal);
void get_RemoteIpAddress(CkString &propVal);
long get_RemotePort(void);
int get_SelectorIndex(void);
void put_SelectorIndex(int propVal);
int get_SelectorReadIndex(void);
void put_SelectorReadIndex(int propVal);
int get_SelectorWriteIndex(void);
void put_SelectorWriteIndex(int propVal);
long get_SendPacketSize(void);
void put_SendPacketSize(long propVal);
void get_SessionLog(CkString &propVal);
void get_SessionLogEncoding(CkString &propVal);
void put_SessionLogEncoding(const wchar_t *w1);
int get_SoRcvBuf(void);
void put_SoRcvBuf(int propVal);
bool get_SoReuseAddr(void);
void put_SoReuseAddr(bool propVal);
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
bool get_Ssl(void);
void put_Ssl(bool propVal);
void get_SslProtocol(CkString &propVal);
void put_SslProtocol(const wchar_t *w1);
void get_StringCharset(CkString &propVal);
void put_StringCharset(const wchar_t *w1);
bool get_TcpNoDelay(void);
void put_TcpNoDelay(bool propVal);
void get_UserData(CkString &propVal);
void put_UserData(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
CkSocketW *AcceptNextConnection(int maxWaitMs);
bool AddSslAcceptableClientCaDn(const wchar_t *w1);
void AsyncAcceptAbort(void);
CkSocketW *AsyncAcceptSocket(void);
bool AsyncAcceptStart(int maxWaitMs);
void AsyncConnectAbort(void);
bool AsyncConnectStart(const wchar_t *w1,int port,bool ssl,int maxWaitMs);
void AsyncDnsAbort(void);
bool AsyncDnsStart(const wchar_t *w1,int maxWaitMs);
void AsyncReceiveAbort(void);
bool AsyncReceiveBytes(void);
bool AsyncReceiveBytesN(unsigned long numBytes);
bool AsyncReceiveString(void);
bool AsyncReceiveToCRLF(void);
bool AsyncReceiveUntilMatch(const wchar_t *w1);
void AsyncSendAbort(void);
bool AsyncSendByteData(const CkByteData &data);
bool AsyncSendBytes(const unsigned char *pByteData,unsigned long szByteData);
bool AsyncSendString(const wchar_t *w1);
bool BindAndListen(int port,int backlog);
bool BuildHttpGetRequest(const wchar_t *w1,CkString &outStr);
int CheckWriteable(int maxWaitMs);
void ClearSessionLog(void);
void Close(int maxWaitMs);
bool Connect(const wchar_t *w1,int port,bool ssl,int maxWaitMs);
bool ConvertFromSsl(void);
bool ConvertToSsl(void);
bool DnsLookup(const wchar_t *w1,int maxWaitMs,CkString &outStr);
CkCertW *GetMyCert(void);
CkCertW *GetReceivedClientCert(int index);
bool GetSslAcceptableClientCaDn(int index,CkString &outStr);
CkCertW *GetSslServerCert(void);
bool InitSslServer(CkCertW &cert);
bool IsUnlocked(void);
bool PollDataAvailable(void);
bool ReceiveBytes(CkByteData &outData);
bool ReceiveBytesN(unsigned long numBytes,CkByteData &outData);
bool ReceiveBytesToFile(const wchar_t *w1);
int ReceiveCount(void);
bool ReceiveString(CkString &outStr);
bool ReceiveStringMaxN(int maxBytes,CkString &outStr);
bool ReceiveStringUntilByte(int byteValue,CkString &outStr);
bool ReceiveToCRLF(CkString &outStr);
bool ReceiveUntilByte(int byteValue,CkByteData &outBytes);
bool ReceiveUntilMatch(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
int SelectForReading(int timeoutMs);
int SelectForWriting(int timeoutMs);
bool SendByteData(const CkByteData &data);
bool SendBytes(const unsigned char *pByteData,unsigned long szByteData);
bool SendCount(int byteCount);
bool SendString(const wchar_t *w1);
bool SetSslClientCert(CkCertW &cert);
bool SetSslClientCertPem(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCertPfx(const wchar_t *w1,const wchar_t *w2);
void SleepMs(int millisec);
void StartTiming(void);
bool TakeSocket(CkSocketW &sock);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *asyncAcceptLog(void);
const wchar_t *asyncConnectLog(void);
const wchar_t *asyncDnsLog(void);
const wchar_t *asyncDnsResult(void);
const wchar_t *asyncReceiveLog(void);
const wchar_t *asyncReceivedString(void);
const wchar_t *asyncSendLog(void);
const wchar_t *buildHttpGetRequest(const wchar_t *w1);
const wchar_t *clientIpAddress(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *dnsLookup(const wchar_t *w1,int maxWaitMs);
const wchar_t *getSslAcceptableClientCaDn(int index);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *localIpAddress(void);
const wchar_t *myIpAddress(void);
const wchar_t *receiveString(void);
const wchar_t *receiveStringMaxN(int maxBytes);
const wchar_t *receiveStringUntilByte(int byteValue);
const wchar_t *receiveToCRLF(void);
const wchar_t *receiveUntilMatch(const wchar_t *w1);
const wchar_t *remoteIpAddress(void);
const wchar_t *sessionLog(void);
const wchar_t *sessionLogEncoding(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *sslProtocol(void);
const wchar_t *stringCharset(void);
const wchar_t *userData(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
