#ifndef _CkImapW_H
#define _CkImapW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkImap.h"

class CkMessageSetW;
class CkEmailW;
class CkEmailBundleW;
class CkPrivateKeyW;
#if defined(WIN32) && !defined(__MINGW32__)
class CkCspW;
#endif
class CkMailboxesW;
class CkCertW;
class CkImapProgress;
class CkStringArrayW;
class CkSshKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkImapW : public CkWideCharBase
{
    private:
	CkImap *m_impl;

        // Disallow assignment or copying this object.
	CkImapW(const CkImapW &) { }
	CkImapW &operator=(const CkImapW &) { return *this; }


    public:
	CkImapW()
	    {
	    m_impl = new CkImap;
	    m_impl->put_Utf8(true);
	    }
	CkImapW(CkImap *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkImap;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkImapW(bool bForMono)
	    {
	    m_impl = new CkImap;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkImap *getMbPtr(void) { return m_impl; }
	CkImap &getMbRef(void) { return *m_impl; }
	const CkImap *getMbConstPtr(void) const { return m_impl; }
	const CkImap &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkImapW() { delete m_impl; m_impl = 0; }

bool get_AppendSeen(void);
void put_AppendSeen(bool propVal);
int get_AppendUid(void);
void get_AuthMethod(CkString &propVal);
void put_AuthMethod(const wchar_t *w1);
void get_AuthzId(CkString &propVal);
void put_AuthzId(const wchar_t *w1);
bool get_AutoDownloadAttachments(void);
void put_AutoDownloadAttachments(bool propVal);
bool get_AutoFix(void);
void put_AutoFix(bool propVal);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
int get_ConnectTimeout(void);
void put_ConnectTimeout(int propVal);
void get_ConnectedToHost(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_Domain(CkString &propVal);
void put_Domain(const wchar_t *w1);
CkImapProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkImapProgress * propVal);
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
bool get_KeepSessionLog(void);
void put_KeepSessionLog(bool propVal);
void get_LastAppendedMime(CkString &propVal);
void get_LastCommand(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastIntermediateResponse(CkString &propVal);
void get_LastResponse(CkString &propVal);
void get_LoggedInUser(CkString &propVal);
long get_NumMessages(void);
bool get_PeekMode(void);
void put_PeekMode(bool propVal);
int get_Port(void);
void put_Port(int propVal);
int get_ReadTimeout(void);
void put_ReadTimeout(int propVal);
void get_SearchCharset(CkString &propVal);
void put_SearchCharset(const wchar_t *w1);
void get_SelectedMailbox(CkString &propVal);
int get_SendBufferSize(void);
void put_SendBufferSize(int propVal);
char get_SeparatorChar(void);
void put_SeparatorChar(char propVal);
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
bool get_Ssl(void);
void put_Ssl(bool propVal);
void get_SslProtocol(CkString &propVal);
void put_SslProtocol(const wchar_t *w1);
bool get_SslServerCertVerified(void);
bool get_StartTls(void);
void put_StartTls(bool propVal);
int get_UidNext(void);
int get_UidValidity(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AddPfxSourceData(const CkByteData &pfxData,const wchar_t *w1);
bool AddPfxSourceFile(const wchar_t *w1,const wchar_t *w2);
bool AppendMail(const wchar_t *w1,const CkEmailW &email);
bool AppendMime(const wchar_t *w1,const wchar_t *w2);
bool AppendMimeWithDate(const wchar_t *w1,const wchar_t *w2,SYSTEMTIME &internalDate);
bool AppendMimeWithDateStr(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool AppendMimeWithFlags(const wchar_t *w1,const wchar_t *w2,bool seen,bool flagged,bool answered,bool draft);
CkMessageSetW *CheckForNewEmail(void);
void ClearSessionLog(void);
bool CloseMailbox(const wchar_t *w1);
bool Connect(const wchar_t *w1);
bool Copy(long msgId,bool bUid,const wchar_t *w1);
bool CopyMultiple(CkMessageSetW &messageSet,const wchar_t *w1);
bool CopySequence(int startSeqNum,int count,const wchar_t *w1);
bool CreateMailbox(const wchar_t *w1);
bool DeleteMailbox(const wchar_t *w1);
bool Disconnect(void);
bool ExamineMailbox(const wchar_t *w1);
bool Expunge(void);
bool ExpungeAndClose(void);
bool FetchAttachment(CkEmailW &email,int attachIndex,const wchar_t *w1);
bool FetchAttachmentBytes(CkEmailW &email,int attachIndex,CkByteData &outBytes);
bool FetchAttachmentString(CkEmailW &email,int attachIndex,const wchar_t *w1,CkString &outStr);
CkEmailBundleW *FetchBundle(const CkMessageSetW &messageSet);
CkStringArrayW *FetchBundleAsMime(const CkMessageSetW &messageSet);
bool FetchFlags(long msgId,bool bUid,CkString &outStrFlags);
CkEmailBundleW *FetchHeaders(const CkMessageSetW &messageSet);
CkEmailBundleW *FetchSequence(int startSeqNum,int numMessages);
CkStringArrayW *FetchSequenceAsMime(int startSeqNum,int count);
CkEmailBundleW *FetchSequenceHeaders(int startSeqNum,int numMessages);
CkEmailW *FetchSingle(long msgId,bool bUid);
bool FetchSingleAsMime(long msgId,bool bUid,CkString &outStrMime);
CkEmailW *FetchSingleHeader(long msgId,bool bUid);
bool FetchSingleHeaderAsMime(int msgId,bool bUID,CkString &outStr);
CkMessageSetW *GetAllUids(void);
bool GetMailAttachFilename(const CkEmailW &email,long attachIndex,CkString &outStrFilename);
long GetMailAttachSize(const CkEmailW &email,long attachIndex);
long GetMailFlag(const CkEmailW &email,const wchar_t *w1);
long GetMailNumAttach(const CkEmailW &email);
long GetMailSize(const CkEmailW &email);
CkCertW *GetSslServerCert(void);
bool IsConnected(void);
bool IsLoggedIn(void);
bool IsUnlocked(void);
CkMailboxesW *ListMailboxes(const wchar_t *w1,const wchar_t *w2);
CkMailboxesW *ListSubscribed(const wchar_t *w1,const wchar_t *w2);
bool Login(const wchar_t *w1,const wchar_t *w2);
bool Logout(void);
bool Noop(void);
bool RefetchMailFlags(CkEmailW &emailInOut);
bool RenameMailbox(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
CkMessageSetW *Search(const wchar_t *w1,bool bUid);
bool SelectMailbox(const wchar_t *w1);
bool SendRawCommand(const wchar_t *w1,CkString &outRawResponse);
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
bool SetCSP(CkCspW &csp);
#endif
bool SetDecryptCert2(const CkCertW &cert,CkPrivateKeyW &key);
bool SetFlag(long msgId,bool bUid,const wchar_t *w1,long value);
bool SetFlags(const CkMessageSetW &messageSet,const wchar_t *w1,long value);
bool SetMailFlag(const CkEmailW &email,const wchar_t *w1,long value);
bool SetSslClientCert(CkCertW &cert);
bool SetSslClientCertPem(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCertPfx(const wchar_t *w1,const wchar_t *w2);
bool SshAuthenticatePk(const wchar_t *w1,CkSshKeyW &privateKey);
bool SshAuthenticatePw(const wchar_t *w1,const wchar_t *w2);
bool SshTunnel(const wchar_t *w1,int sshServerPort);
bool StoreFlags(long msgId,bool bUid,const wchar_t *w1,long value);
bool Subscribe(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
bool Unsubscribe(const wchar_t *w1);
const wchar_t *authMethod(void);
const wchar_t *authzId(void);
const wchar_t *clientIpAddress(void);
const wchar_t *connectedToHost(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *domain(void);
const wchar_t *fetchAttachmentString(CkEmailW &email,int attachIndex,const wchar_t *w1);
const wchar_t *fetchFlags(long msgId,bool bUid);
const wchar_t *fetchSingleAsMime(long msgId,bool bUid);
const wchar_t *fetchSingleHeaderAsMime(int msgId,bool bUID);
const wchar_t *getMailAttachFilename(const CkEmailW &email,long attachIndex);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastAppendedMime(void);
const wchar_t *lastCommand(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastIntermediateResponse(void);
const wchar_t *lastResponse(void);
const wchar_t *loggedInUser(void);
const wchar_t *searchCharset(void);
const wchar_t *selectedMailbox(void);
const wchar_t *sendRawCommand(const wchar_t *w1);
const wchar_t *sessionLog(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *sslProtocol(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
