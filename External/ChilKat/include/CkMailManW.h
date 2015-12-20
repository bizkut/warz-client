#ifndef _CkMailManW_H
#define _CkMailManW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkMailMan.h"

class CkEmailW;
class CkEmailBundleW;
class CkMailProgress;
class CkPrivateKeyW;
#if defined(WIN32) && !defined(__MINGW32__)
class CkCspW;
#endif
class CkCertW;
class CkStringArrayW;
class CkSshKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkMailManW : public CkWideCharBase
{
    private:
	CkMailMan *m_impl;

        // Disallow assignment or copying this object.
	CkMailManW(const CkMailManW &) { }
	CkMailManW &operator=(const CkMailManW &) { return *this; }


    public:
	CkMailManW()
	    {
	    m_impl = new CkMailMan;
	    m_impl->put_Utf8(true);
	    }
	CkMailManW(CkMailMan *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkMailMan;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMailManW(bool bForMono)
	    {
	    m_impl = new CkMailMan;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMailMan *getMbPtr(void) { return m_impl; }
	CkMailMan &getMbRef(void) { return *m_impl; }
	const CkMailMan *getMbConstPtr(void) const { return m_impl; }
	const CkMailMan &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkMailManW() { delete m_impl; m_impl = 0; }

bool get_AllOrNone(void);
void put_AllOrNone(bool propVal);
bool get_AutoFix(void);
void put_AutoFix(bool propVal);
bool get_AutoGenMessageId(void);
void put_AutoGenMessageId(bool propVal);
bool get_AutoSmtpRset(void);
void put_AutoSmtpRset(bool propVal);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
long get_ConnectTimeout(void);
void put_ConnectTimeout(long propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DsnEnvid(CkString &propVal);
void put_DsnEnvid(const wchar_t *w1);
void get_DsnNotify(CkString &propVal);
void put_DsnNotify(const wchar_t *w1);
void get_DsnRet(CkString &propVal);
void put_DsnRet(const wchar_t *w1);
bool get_EmbedCertChain(void);
void put_EmbedCertChain(bool propVal);
CkMailProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkMailProgress * propVal);
void get_Filter(CkString &propVal);
void put_Filter(const wchar_t *w1);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
void get_HeloHostname(CkString &propVal);
void put_HeloHostname(const wchar_t *w1);
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
bool get_ImmediateDelete(void);
void put_ImmediateDelete(bool propVal);
bool get_IncludeRootCert(void);
void put_IncludeRootCert(bool propVal);
bool get_IsSmtpConnected(void);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastSendQFilename(CkString &propVal);
long get_LastSmtpStatus(void);
void get_LogMailReceivedFilename(CkString &propVal);
void put_LogMailReceivedFilename(const wchar_t *w1);
void get_LogMailSentFilename(CkString &propVal);
void put_LogMailSentFilename(const wchar_t *w1);
void get_MailHost(CkString &propVal);
void put_MailHost(const wchar_t *w1);
long get_MailPort(void);
void put_MailPort(long propVal);
long get_MaxCount(void);
void put_MaxCount(long propVal);
bool get_OpaqueSigning(void);
void put_OpaqueSigning(bool propVal);
bool get_Pop3SPA(void);
void put_Pop3SPA(bool propVal);
int get_Pop3SessionId(void);
void get_Pop3SessionLog(CkString &propVal);
bool get_Pop3SslServerCertVerified(void);
bool get_Pop3Stls(void);
void put_Pop3Stls(bool propVal);
void get_PopPassword(CkString &propVal);
void put_PopPassword(const wchar_t *w1);
void get_PopPasswordBase64(CkString &propVal);
void put_PopPasswordBase64(const wchar_t *w1);
bool get_PopSsl(void);
void put_PopSsl(bool propVal);
void get_PopUsername(CkString &propVal);
void put_PopUsername(const wchar_t *w1);
long get_ReadTimeout(void);
void put_ReadTimeout(long propVal);
bool get_RequireSslCertVerify(void);
void put_RequireSslCertVerify(bool propVal);
bool get_ResetDateOnLoad(void);
void put_ResetDateOnLoad(bool propVal);
int get_SendBufferSize(void);
void put_SendBufferSize(int propVal);
bool get_SendIndividual(void);
void put_SendIndividual(bool propVal);
long get_SizeLimit(void);
void put_SizeLimit(long propVal);
void get_SmtpAuthMethod(CkString &propVal);
void put_SmtpAuthMethod(const wchar_t *w1);
void get_SmtpHost(CkString &propVal);
void put_SmtpHost(const wchar_t *w1);
void get_SmtpLoginDomain(CkString &propVal);
void put_SmtpLoginDomain(const wchar_t *w1);
void get_SmtpPassword(CkString &propVal);
void put_SmtpPassword(const wchar_t *w1);
long get_SmtpPort(void);
void put_SmtpPort(long propVal);
void get_SmtpSessionLog(CkString &propVal);
bool get_SmtpSsl(void);
void put_SmtpSsl(bool propVal);
bool get_SmtpSslServerCertVerified(void);
void get_SmtpUsername(CkString &propVal);
void put_SmtpUsername(const wchar_t *w1);
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
bool get_StartTLS(void);
void put_StartTLS(bool propVal);
bool get_UseApop(void);
void put_UseApop(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AddPfxSourceData(const CkByteData &pfxData,const wchar_t *w1);
bool AddPfxSourceFile(const wchar_t *w1,const wchar_t *w2);
long CheckMail(void);
void ClearBadEmailAddresses(void);
void ClearPop3SessionLog(void);
void ClearSmtpSessionLog(void);
bool CloseSmtpConnection(void);
CkEmailBundleW *CopyMail(void);
bool DeleteBundle(const CkEmailBundleW &bundle);
bool DeleteByMsgnum(int msgnum);
bool DeleteByUidl(const wchar_t *w1);
bool DeleteEmail(const CkEmailW &email);
bool DeleteMultiple(const CkStringArrayW &uidlArray);
CkEmailW *FetchByMsgnum(int msgnum);
CkEmailW *FetchEmail(const wchar_t *w1);
bool FetchMime(const wchar_t *w1,CkByteData &outData);
bool FetchMimeByMsgnum(int msgnum,CkByteData &outBytes);
CkEmailBundleW *FetchMultiple(const CkStringArrayW &uidlArray);
CkEmailBundleW *FetchMultipleHeaders(const CkStringArrayW &uidlArray,long numBodyLines);
CkStringArrayW *FetchMultipleMime(const CkStringArrayW &uidlArray);
CkEmailW *FetchSingleHeader(long numBodyLines,long msgNum);
CkEmailW *FetchSingleHeaderByUidl(long numBodyLines,const wchar_t *w1);
CkEmailBundleW *GetAllHeaders(long numBodyLines);
CkStringArrayW *GetBadEmailAddrs(void);
CkEmailW *GetFullEmail(const CkEmailW &email);
CkEmailBundleW *GetHeaders(long numBodyLines,long fromIndex,long toIndex);
int GetMailboxCount(void);
bool GetMailboxInfoXml(CkString &outXml);
unsigned long GetMailboxSize(void);
CkCertW *GetPop3SslServerCert(void);
int GetSizeByUidl(const wchar_t *w1);
CkCertW *GetSmtpSslServerCert(void);
CkStringArrayW *GetUidls(void);
bool IsSmtpDsnCapable(void);
bool IsUnlocked(void);
CkEmailW *LoadEml(const wchar_t *w1);
CkEmailBundleW *LoadMbx(const wchar_t *w1);
CkEmailW *LoadMime(const wchar_t *w1);
CkEmailW *LoadXmlEmail(const wchar_t *w1);
CkEmailW *LoadXmlEmailString(const wchar_t *w1);
CkEmailBundleW *LoadXmlFile(const wchar_t *w1);
CkEmailBundleW *LoadXmlString(const wchar_t *w1);
#ifndef WINCE
bool MxLookup(const wchar_t *w1,CkString &outStrHostname);
#endif
#ifndef WINCE
CkStringArrayW *MxLookupAll(const wchar_t *w1);
#endif
bool OpenSmtpConnection(void);
bool Pop3BeginSession(void);
bool Pop3EndSession(void);
bool Pop3EndSessionNoQuit(void);
bool Pop3Noop(void);
bool Pop3Reset(void);
bool Pop3SendRawCommand(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool QuickSend(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,const wchar_t *w5);
bool RenderToMime(const CkEmailW &email,CkString &outStr);
bool RenderToMimeBytes(CkEmailW &email,CkByteData &outBytes);
bool SaveLastError(const wchar_t *w1);
bool SendBundle(const CkEmailBundleW &bundle);
bool SendEmail(const CkEmailW &email);
bool SendMime(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool SendMimeBytes(const wchar_t *w1,const wchar_t *w2,const CkByteData &mimeData);
#ifndef WINCE
bool SendMimeBytesQ(const wchar_t *w1,const wchar_t *w2,const CkByteData &mimeData);
#endif
bool SendMimeQ(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool SendMimeToList(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
#ifndef WINCE
bool SendQ(const CkEmailW &email);
#endif
#ifndef WINCE
bool SendQ2(const CkEmailW &email,const wchar_t *w1);
#endif
bool SendToDistributionList(CkEmailW &email,CkStringArrayW &sa);
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
bool SetCSP(CkCspW &csp);
#endif
bool SetDecryptCert2(const CkCertW &cert,CkPrivateKeyW &key);
bool SetSslClientCert(CkCertW &cert);
bool SetSslClientCertPem(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCertPfx(const wchar_t *w1,const wchar_t *w2);
bool SmtpNoop(void);
bool SmtpReset(void);
bool SmtpSendRawCommand(const wchar_t *w1,const wchar_t *w2,bool bEncodeBase64,CkString &outStr);
bool SshAuthenticatePk(bool bSmtp,const wchar_t *w1,CkSshKeyW &privateKey);
bool SshAuthenticatePw(bool bSmtp,const wchar_t *w1,const wchar_t *w2);
bool SshCloseTunnel(bool bSmtp);
bool SshTunnel(bool bSmtp,const wchar_t *w1,int sshServerPort);
CkEmailBundleW *TransferMail(void);
CkStringArrayW *TransferMultipleMime(const CkStringArrayW &uidlArray);
bool UnlockComponent(const wchar_t *w1);
bool VerifyPopConnection(void);
bool VerifyPopLogin(void);
bool VerifyRecips(const CkEmailW &email,CkStringArrayW &saBadAddrs);
bool VerifySmtpConnection(void);
bool VerifySmtpLogin(void);
const wchar_t *clientIpAddress(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *dsnEnvid(void);
const wchar_t *dsnNotify(void);
const wchar_t *dsnRet(void);
const wchar_t *filter(void);
const wchar_t *getMailboxInfoXml(void);
const wchar_t *heloHostname(void);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastSendQFilename(void);
const wchar_t *logMailReceivedFilename(void);
const wchar_t *logMailSentFilename(void);
const wchar_t *mailHost(void);
#ifndef WINCE
const wchar_t *mxLookup(const wchar_t *w1);
#endif
const wchar_t *pop3SendRawCommand(const wchar_t *w1,const wchar_t *w2);
const wchar_t *pop3SessionLog(void);
const wchar_t *popPassword(void);
const wchar_t *popPasswordBase64(void);
const wchar_t *popUsername(void);
const wchar_t *renderToMime(const CkEmailW &email);
const wchar_t *smtpAuthMethod(void);
const wchar_t *smtpHost(void);
const wchar_t *smtpLoginDomain(void);
const wchar_t *smtpPassword(void);
const wchar_t *smtpSendRawCommand(const wchar_t *w1,const wchar_t *w2,bool bEncodeBase64);
const wchar_t *smtpSessionLog(void);
const wchar_t *smtpUsername(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
