#ifndef _CkHttpW_H
#define _CkHttpW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkHttp.h"

class CkHttpRequestW;
class CkCertW;
class CkHttpResponseW;
class CkHttpProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkHttpW : public CkWideCharBase
{
    private:
	CkHttp *m_impl;

        // Disallow assignment or copying this object.
	CkHttpW(const CkHttpW &) { }
	CkHttpW &operator=(const CkHttpW &) { return *this; }


    public:
	CkHttpW()
	    {
	    m_impl = new CkHttp;
	    m_impl->put_Utf8(true);
	    }
	CkHttpW(CkHttp *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkHttp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttpW(bool bForMono)
	    {
	    m_impl = new CkHttp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttp *getMbPtr(void) { return m_impl; }
	CkHttp &getMbRef(void) { return *m_impl; }
	const CkHttp *getMbConstPtr(void) const { return m_impl; }
	const CkHttp &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkHttpW() { delete m_impl; m_impl = 0; }

void get_Accept(CkString &propVal);
void put_Accept(const wchar_t *w1);
void get_AcceptCharset(CkString &propVal);
void put_AcceptCharset(const wchar_t *w1);
void get_AcceptLanguage(CkString &propVal);
void put_AcceptLanguage(const wchar_t *w1);
bool get_AllowGzip(void);
void put_AllowGzip(bool propVal);
bool get_AutoAddHostHeader(void);
void put_AutoAddHostHeader(bool propVal);
void get_AwsAccessKey(CkString &propVal);
void put_AwsAccessKey(const wchar_t *w1);
void get_AwsSecretKey(CkString &propVal);
void put_AwsSecretKey(const wchar_t *w1);
void get_AwsSubResources(CkString &propVal);
void put_AwsSubResources(const wchar_t *w1);
void get_BgLastErrorText(CkString &propVal);
int get_BgPercentDone(void);
void get_BgResultData(CkByteData &propVal);
int get_BgResultInt(void);
void get_BgResultString(CkString &propVal);
bool get_BgTaskFinished(void);
bool get_BgTaskRunning(void);
bool get_BgTaskSuccess(void);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
long get_ConnectTimeout(void);
void put_ConnectTimeout(long propVal);
void get_Connection(CkString &propVal);
void put_Connection(const wchar_t *w1);
void get_CookieDir(CkString &propVal);
void put_CookieDir(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
long get_DefaultFreshPeriod(void);
void put_DefaultFreshPeriod(long propVal);
bool get_DigestAuth(void);
void put_DigestAuth(bool propVal);
CkHttpProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkHttpProgress * propVal);
int get_EventLogCount(void);
bool get_FetchFromCache(void);
void put_FetchFromCache(bool propVal);
void get_FinalRedirectUrl(CkString &propVal);
bool get_FollowRedirects(void);
void put_FollowRedirects(bool propVal);
long get_FreshnessAlgorithm(void);
void put_FreshnessAlgorithm(long propVal);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
bool get_IgnoreMustRevalidate(void);
void put_IgnoreMustRevalidate(bool propVal);
bool get_IgnoreNoCache(void);
void put_IgnoreNoCache(bool propVal);
bool get_KeepEventLog(void);
void put_KeepEventLog(bool propVal);
long get_LMFactor(void);
void put_LMFactor(long propVal);
void get_LastContentType(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastHeader(CkString &propVal);
void get_LastModDate(CkString &propVal);
void get_LastResponseHeader(CkString &propVal);
long get_LastStatus(void);
void get_Login(CkString &propVal);
void put_Login(const wchar_t *w1);
void get_LoginDomain(CkString &propVal);
void put_LoginDomain(const wchar_t *w1);
long get_MaxConnections(void);
void put_MaxConnections(long propVal);
long get_MaxFreshPeriod(void);
void put_MaxFreshPeriod(long propVal);
unsigned long get_MaxResponseSize(void);
void put_MaxResponseSize(unsigned long propVal);
long get_MaxUrlLen(void);
void put_MaxUrlLen(long propVal);
bool get_MimicFireFox(void);
void put_MimicFireFox(bool propVal);
bool get_MimicIE(void);
void put_MimicIE(bool propVal);
long get_MinFreshPeriod(void);
void put_MinFreshPeriod(long propVal);
bool get_NegotiateAuth(void);
void put_NegotiateAuth(bool propVal);
bool get_NtlmAuth(void);
void put_NtlmAuth(bool propVal);
long get_NumCacheLevels(void);
void put_NumCacheLevels(long propVal);
long get_NumCacheRoots(void);
bool get_OAuth1(void);
void put_OAuth1(bool propVal);
void get_OAuthConsumerKey(CkString &propVal);
void put_OAuthConsumerKey(const wchar_t *w1);
void get_OAuthConsumerSecret(CkString &propVal);
void put_OAuthConsumerSecret(const wchar_t *w1);
void get_OAuthRealm(CkString &propVal);
void put_OAuthRealm(const wchar_t *w1);
void get_OAuthSigMethod(CkString &propVal);
void put_OAuthSigMethod(const wchar_t *w1);
void get_OAuthToken(CkString &propVal);
void put_OAuthToken(const wchar_t *w1);
void get_OAuthTokenSecret(CkString &propVal);
void put_OAuthTokenSecret(const wchar_t *w1);
void get_OAuthVerifier(CkString &propVal);
void put_OAuthVerifier(const wchar_t *w1);
void get_Password(CkString &propVal);
void put_Password(const wchar_t *w1);
void get_ProxyAuthMethod(CkString &propVal);
void put_ProxyAuthMethod(const wchar_t *w1);
void get_ProxyDomain(CkString &propVal);
void put_ProxyDomain(const wchar_t *w1);
void get_ProxyLogin(CkString &propVal);
void put_ProxyLogin(const wchar_t *w1);
void get_ProxyLoginDomain(CkString &propVal);
void put_ProxyLoginDomain(const wchar_t *w1);
void get_ProxyPassword(CkString &propVal);
void put_ProxyPassword(const wchar_t *w1);
long get_ProxyPort(void);
void put_ProxyPort(long propVal);
long get_ReadTimeout(void);
void put_ReadTimeout(long propVal);
void get_RedirectVerb(CkString &propVal);
void put_RedirectVerb(const wchar_t *w1);
void get_Referer(CkString &propVal);
void put_Referer(const wchar_t *w1);
void get_RequiredContentType(CkString &propVal);
void put_RequiredContentType(const wchar_t *w1);
bool get_S3Ssl(void);
void put_S3Ssl(bool propVal);
bool get_SaveCookies(void);
void put_SaveCookies(bool propVal);
int get_SendBufferSize(void);
void put_SendBufferSize(int propVal);
bool get_SendCookies(void);
void put_SendCookies(bool propVal);
void get_SessionLogFilename(CkString &propVal);
void put_SessionLogFilename(const wchar_t *w1);
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
void get_SslProtocol(CkString &propVal);
void put_SslProtocol(const wchar_t *w1);
bool get_UpdateCache(void);
void put_UpdateCache(bool propVal);
bool get_UseBgThread(void);
void put_UseBgThread(bool propVal);
bool get_UseIEProxy(void);
void put_UseIEProxy(bool propVal);
void get_UserAgent(CkString &propVal);
void put_UserAgent(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool get_WasRedirected(void);
void AddCacheRoot(const wchar_t *w1);
bool AddQuickHeader(const wchar_t *w1,const wchar_t *w2);
CkHttpResponseW *BgResponseObject(void);
void BgTaskAbort(void);
void ClearBgEventLog(void);
void ClearInMemoryCookies(void);
bool CloseAllConnections(void);
bool Download(const wchar_t *w1,const wchar_t *w2);
bool DownloadAppend(const wchar_t *w1,const wchar_t *w2);
bool DownloadHash(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
bool EventLogName(int index,CkString &outStr);
bool EventLogValue(int index,CkString &outStr);
bool ExtractMetaRefreshUrl(const wchar_t *w1,CkString &outStr);
bool GenTimeStamp(CkString &outStr);
bool GetCacheRoot(int index,CkString &outStr);
bool GetCookieXml(const wchar_t *w1,CkString &outStr);
bool GetDomain(const wchar_t *w1,CkString &outStr);
CkHttpResponseW *GetHead(const wchar_t *w1);
bool GetRequestHeader(const wchar_t *w1,CkString &outStr);
CkCertW *GetServerSslCert(const wchar_t *w1,int port);
bool GetUrlPath(const wchar_t *w1,CkString &outStr);
bool HasRequestHeader(const wchar_t *w1);
bool IsUnlocked(void);
bool PostBinary(const wchar_t *w1,const CkByteData &byteData,const wchar_t *w2,bool md5,bool gzip,CkString &outStr);
CkHttpResponseW *PostJson(const wchar_t *w1,const wchar_t *w2);
CkHttpResponseW *PostMime(const wchar_t *w1,const wchar_t *w2);
CkHttpResponseW *PostUrlEncoded(const wchar_t *w1,CkHttpRequestW &req);
CkHttpResponseW *PostXml(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool PutBinary(const wchar_t *w1,const CkByteData &byteData,const wchar_t *w2,bool md5,bool gzip,CkString &outStr);
bool PutText(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,bool md5,bool gzip,CkString &outStr);
bool QuickDeleteStr(const wchar_t *w1,CkString &outStr);
bool QuickGet(const wchar_t *w1,CkByteData &outData);
CkHttpResponseW *QuickGetObj(const wchar_t *w1);
bool QuickGetStr(const wchar_t *w1,CkString &outStr);
bool QuickPutStr(const wchar_t *w1,CkString &outStr);
bool RemoveQuickHeader(const wchar_t *w1);
void RemoveRequestHeader(const wchar_t *w1);
bool RenderGet(const wchar_t *w1,CkString &outStr);
bool ResumeDownload(const wchar_t *w1,const wchar_t *w2);
bool S3_CreateBucket(const wchar_t *w1);
bool S3_DeleteBucket(const wchar_t *w1);
bool S3_DeleteObject(const wchar_t *w1,const wchar_t *w2);
bool S3_DownloadBytes(const wchar_t *w1,const wchar_t *w2,CkByteData &outBytes);
bool S3_DownloadFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool S3_DownloadString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
bool S3_ListBucketObjects(const wchar_t *w1,CkString &outStr);
bool S3_ListBuckets(CkString &outStr);
bool S3_UploadBytes(const CkByteData &objectContent,const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool S3_UploadFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool S3_UploadString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,const wchar_t *w5);
bool SaveLastError(const wchar_t *w1);
bool SetCookieXml(const wchar_t *w1,const wchar_t *w2);
void SetRequestHeader(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCert(CkCertW &cert);
bool SetSslClientCertPem(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCertPfx(const wchar_t *w1,const wchar_t *w2);
void SleepMs(int millisec);
CkHttpResponseW *SynchronousRequest(const wchar_t *w1,long port,bool ssl,const CkHttpRequestW &req);
bool UnlockComponent(const wchar_t *w1);
bool UrlDecode(const wchar_t *w1,CkString &outStr);
bool UrlEncode(const wchar_t *w1,CkString &outStr);
bool XmlRpc(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool XmlRpcPut(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
const wchar_t *acceptCharset(void);
const wchar_t *acceptLanguage(void);
const wchar_t *awsAccessKey(void);
const wchar_t *awsSecretKey(void);
const wchar_t *awsSubResources(void);
const wchar_t *bgLastErrorText(void);
const wchar_t *bgResultString(void);
const wchar_t *ck_accept(void);
const wchar_t *clientIpAddress(void);
const wchar_t *connection(void);
const wchar_t *cookieDir(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *downloadHash(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *eventLogName(int index);
const wchar_t *eventLogValue(int index);
const wchar_t *extractMetaRefreshUrl(const wchar_t *w1);
const wchar_t *finalRedirectUrl(void);
const wchar_t *genTimeStamp(void);
const wchar_t *getCacheRoot(int index);
const wchar_t *getCookieXml(const wchar_t *w1);
const wchar_t *getDomain(const wchar_t *w1);
const wchar_t *getRequestHeader(const wchar_t *w1);
const wchar_t *getUrlPath(const wchar_t *w1);
const wchar_t *lastContentType(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastHeader(void);
const wchar_t *lastModDate(void);
const wchar_t *lastResponseHeader(void);
const wchar_t *login(void);
const wchar_t *loginDomain(void);
const wchar_t *oAuthConsumerKey(void);
const wchar_t *oAuthConsumerSecret(void);
const wchar_t *oAuthRealm(void);
const wchar_t *oAuthSigMethod(void);
const wchar_t *oAuthToken(void);
const wchar_t *oAuthTokenSecret(void);
const wchar_t *oAuthVerifier(void);
const wchar_t *password(void);
const wchar_t *postBinary(const wchar_t *w1,const CkByteData &byteData,const wchar_t *w2,bool md5,bool gzip);
const wchar_t *proxyAuthMethod(void);
const wchar_t *proxyDomain(void);
const wchar_t *proxyLogin(void);
const wchar_t *proxyLoginDomain(void);
const wchar_t *proxyPassword(void);
const wchar_t *putBinary(const wchar_t *w1,const CkByteData &byteData,const wchar_t *w2,bool md5,bool gzip);
const wchar_t *putText(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,bool md5,bool gzip);
const wchar_t *quickDeleteStr(const wchar_t *w1);
const wchar_t *quickGetStr(const wchar_t *w1);
const wchar_t *quickPutStr(const wchar_t *w1);
const wchar_t *redirectVerb(void);
const wchar_t *referer(void);
const wchar_t *renderGet(const wchar_t *w1);
const wchar_t *requiredContentType(void);
const wchar_t *s3_DownloadString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *s3_ListBucketObjects(const wchar_t *w1);
const wchar_t *s3_ListBuckets(void);
const wchar_t *sessionLogFilename(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *sslProtocol(void);
const wchar_t *urlDecode(const wchar_t *w1);
const wchar_t *urlEncode(const wchar_t *w1);
const wchar_t *userAgent(void);
const wchar_t *version(void);
const wchar_t *xmlRpc(const wchar_t *w1,const wchar_t *w2);
const wchar_t *xmlRpcPut(const wchar_t *w1,const wchar_t *w2);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
