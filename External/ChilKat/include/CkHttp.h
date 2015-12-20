// CkHttp.h: interface for the CkHttp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKHTTP_H
#define _CKHTTP_H



#include "CkString.h"
class CkByteData;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

class CkHttpRequest;
class CkHttpResponse;
class CkHttpProgress;
class CkCert;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

//#undef ClearEventLog

// CLASS: CkHttp
class CkHttp  : public CkMultiByteBase
{
    private:
	CkHttpProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkHttp(const CkHttp &) { } 
	CkHttp &operator=(const CkHttp &) { return *this; }
	CkHttp(void *impl);


    public:
	CkHttp();
	virtual ~CkHttp();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// SORCVBUF_BEGIN
	int get_SoRcvBuf(void);
	void put_SoRcvBuf(int newVal);
	// SORCVBUF_END

	// SOSNDBUF_BEGIN
	int get_SoSndBuf(void);
	void put_SoSndBuf(int newVal);
	// SOSNDBUF_END

	// OAUTHVERIFIER_BEGIN
	void get_OAuthVerifier(CkString &str);
	const char *oAuthVerifier(void);
	void put_OAuthVerifier(const char *newVal);
	// OAUTHVERIFIER_END

	// OAUTHTOKEN_BEGIN
	void get_OAuthToken(CkString &str);
	const char *oAuthToken(void);
	void put_OAuthToken(const char *newVal);
	// OAUTHTOKEN_END

	// OAUTHTOKENSECRET_BEGIN
	void get_OAuthTokenSecret(CkString &str);
	const char *oAuthTokenSecret(void);
	void put_OAuthTokenSecret(const char *newVal);
	// OAUTHTOKENSECRET_END

	// OAUTHCONSUMERSECRET_BEGIN
	void get_OAuthConsumerSecret(CkString &str);
	const char *oAuthConsumerSecret(void);
	void put_OAuthConsumerSecret(const char *newVal);
	// OAUTHCONSUMERSECRET_END

	// OAUTHSIGMETHOD_BEGIN
	void get_OAuthSigMethod(CkString &str);
	const char *oAuthSigMethod(void);
	void put_OAuthSigMethod(const char *newVal);
	// OAUTHSIGMETHOD_END

	// OAUTHCONSUMERKEY_BEGIN
	void get_OAuthConsumerKey(CkString &str);
	const char *oAuthConsumerKey(void);
	void put_OAuthConsumerKey(const char *newVal);
	// OAUTHCONSUMERKEY_END

	// OAUTHREALM_BEGIN
	void get_OAuthRealm(CkString &str);
	const char *oAuthRealm(void);
	void put_OAuthRealm(const char *newVal);
	// OAUTHREALM_END

	// OAUTH1_BEGIN
	bool get_OAuth1(void);
	void put_OAuth1(bool newVal);
	// OAUTH1_END


	CkHttpProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkHttpProgress *progress);


	// PROXYLOGINDOMAIN_BEGIN
	void get_ProxyLoginDomain(CkString &str);
	const char *proxyLoginDomain(void);
	void put_ProxyLoginDomain(const char *newVal);
	// PROXYLOGINDOMAIN_END

	// S3SSL_BEGIN
	bool get_S3Ssl(void);
	void put_S3Ssl(bool newVal);
	// S3SSL_END


	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;

	long get_MaxConnections(void);
	void put_MaxConnections(long n);
	
	// Accumulate cookies here.
	void put_CookieDir(const char *dir);
	void get_CookieDir(CkString &str);
	
	void put_SaveCookies(bool b);
	bool get_SaveCookies(void);

	void put_SendCookies(bool b);
	bool get_SendCookies(void);

	void put_UseIEProxy(bool b);
	bool get_UseIEProxy(void);

	// eventCallbacks
	bool Download(const char *url, const char *saveToPath);
	// eventCallbacks
	bool ResumeDownload(const char *url, const char *appendToPath);

	// Fetch any kind of object (images, html, etc.)
	// eventCallbacks
	bool QuickGet(const char *url, CkByteData &outData);

	// Fetch to HTML, XML, etc.
	// eventCallbacks
	bool QuickGetStr(const char *url, CkString &outStr);

	bool SetSslClientCert(CkCert &cert);

	// General request/response.
	// eventCallbacks
	CkHttpResponse *SynchronousRequest(const char *domain, long port, bool ssl, const CkHttpRequest &req);

	// eventCallbacks
	CkHttpResponse *PostMime(const char *url, const char *mime);

	void get_LastHeader(CkString &str);
	const char *lastHeader(void);

	void put_Login(const char *v);
	void get_Login(CkString &str);

	void put_Password(const char *v);
	void get_Password(CkString &str);

	void put_NtlmAuth(bool b);
	bool get_NtlmAuth(void);


	long get_ProxyPort(void);
	void put_ProxyPort(long n);
	
	void put_ProxyDomain(const char *v);
	void get_ProxyDomain(CkString &str);

	void get_ProxyLogin(CkString &str);
	void put_ProxyLogin(const char *newVal);

	void get_ProxyPassword(CkString &str);
	void put_ProxyPassword(const char *newVal);

	const char *proxyLogin(void);
	const char *proxyPassword(void);
	const char *proxyDomain(void);


	// These times are in milliseconds.
	long get_HeartbeatMs(void);
	void put_HeartbeatMs(long newVal);


	// Manage request headers added for "QuickGet" methods.
	void SetRequestHeader(const char *name, const char *value);
	bool GetRequestHeader(const char *name, CkString &outStr);
	bool HasRequestHeader(const char *name);
	void RemoveRequestHeader(const char *name);

	// For "QuickGet" URL fetches.
	// UserAgent request header
	void put_UserAgent(const char *v);
	void get_UserAgent(CkString &str);

	// For "QuickGet" URL fetches.
	// Referer request header
	void put_Referer(const char *v);
	void get_Referer(CkString &str);

	// Accept request header
	void put_Accept(const char *v);
	void get_Accept(CkString &str);

	// AcceptCharset request header
	void put_AcceptCharset(const char *v);
	void get_AcceptCharset(CkString &str);

	// AcceptLanguage request header
	void put_AcceptLanguage(const char *v);
	void get_AcceptLanguage(CkString &str);

	// Connection request header field.
	void put_Connection(const char *v);
	void get_Connection(CkString &str);

	// For "QuickGet" URL fetches.
	long get_MaxUrlLen(void);
	void put_MaxUrlLen(long n);

	// For "QuickGet" URL fetches.
	unsigned long get_MaxResponseSize(void);
	void put_MaxResponseSize(unsigned long n);

	// For "QuickGet" URL fetches.
	void put_MimicIE(bool b);
	bool get_MimicIE(void);
	void put_MimicFireFox(bool b);
	bool get_MimicFireFox(void);

	// For all requests...
	void put_AutoAddHostHeader(bool b);
	bool get_AutoAddHostHeader(void);

	long get_ConnectTimeout(void);
	void put_ConnectTimeout(long numSeconds);

	long get_ReadTimeout(void);
	void put_ReadTimeout(long numSeconds);

	bool GetDomain(const char *url, CkString &outStr);

	bool get_WasRedirected(void);
	void get_FinalRedirectUrl(CkString &str);
	
	void put_FollowRedirects(bool b);
	bool get_FollowRedirects(void);

	// Cache methods/properties.
	// 0 = all files in the cache root(s).
	// 1 = 256 subdirectories off cache roots
	// 2 = 256x256 subdirs.
	void put_NumCacheLevels(long v);
	long get_NumCacheLevels(void);

	// Setting/getting cache roots.
	long get_NumCacheRoots(void);
	bool GetCacheRoot(int index, CkString &outStr);
	void AddCacheRoot(const char *dir);

	// Cache Control properties.
	bool get_UpdateCache(void);
	void put_UpdateCache(bool b);

	bool get_FetchFromCache(void);
	void put_FetchFromCache(bool b);

	bool get_IgnoreNoCache(void);
	void put_IgnoreNoCache(bool b);

	bool get_IgnoreMustRevalidate(void);
	void put_IgnoreMustRevalidate(bool b);

	// 1-week is default.
 	long get_DefaultFreshPeriod(void);
	void put_DefaultFreshPeriod(long numMinutes);

	// 0 = constant (use default freshness period)
	// 1 = LM Factor
	long get_FreshnessAlgorithm(void);
	void put_FreshnessAlgorithm(long v);

	// 1 - 100% (percentage of time between fetch-time and
	// last-mod date used for the freshness period)
	long get_LMFactor(void);
	void put_LMFactor(long v);

	long get_MaxFreshPeriod(void);
	void put_MaxFreshPeriod(long numMinutes);

	long get_MinFreshPeriod(void);
	void put_MinFreshPeriod(long numMinutes);
	
	bool GetUrlPath(const char *url, CkString &outStr);

	long get_LastStatus(void);
	void get_LastContentType(CkString &strContentType);
	void get_LastResponseHeader(CkString &strHeader);
	// Get the last-mod date in format yyyy-mm-dd
	void get_LastModDate(CkString &str);


	const char *cookieDir(void);
	const char *quickGetStr(const char *url);
	const char *password(void);
	const char *login(void);
	const char *getRequestHeader(const char *name);
	const char *userAgent(void);
	const char *referer(void);
	const char *ck_accept(void);
	const char *acceptCharset(void);
	const char *acceptLanguage(void);
	const char *connection(void);
	const char *getDomain(const char *url);
	const char *finalRedirectUrl(void);
	const char *getCacheRoot(int index);
	const char *getUrlPath(const char *url);
	const char *lastContentType(void);
	const char *lastResponseHeader(void);
	const char *lastModDate(void);
	// RENDERGET_BEGIN
	bool RenderGet(const char *url, CkString &outStr);
	const char *renderGet(const char *url);
	// RENDERGET_END
	// GETCOOKIEXML_BEGIN
	bool GetCookieXml(const char *domain, CkString &outStr);
	const char *getCookieXml(const char *domain);
	// GETCOOKIEXML_END
	// REQUIREDCONTENTTYPE_BEGIN
	void get_RequiredContentType(CkString &str);
	const char *requiredContentType(void);
	void put_RequiredContentType(const char *newVal);
	// REQUIREDCONTENTTYPE_END
	// DOWNLOADAPPEND_BEGIN
	// eventCallbacks
	bool DownloadAppend(const char *url, const char *appendToPath);
	// DOWNLOADAPPEND_END
	// URLDECODE_BEGIN
	bool UrlDecode(const char *str, CkString &outStr);
	const char *urlDecode(const char *str);
	// URLDECODE_END
	// URLENCODE_BEGIN
	bool UrlEncode(const char *str, CkString &outStr);
	const char *urlEncode(const char *str);
	// URLENCODE_END
	// SSLPROTOCOL_BEGIN
	void get_SslProtocol(CkString &str);
	const char *sslProtocol(void);
	void put_SslProtocol(const char *newVal);
	// SSLPROTOCOL_END
	// GETHEAD_BEGIN
	// eventCallbacks
	CkHttpResponse *GetHead(const char *url);
	// GETHEAD_END
	// XMLRPC_BEGIN
	// eventCallbacks
	bool XmlRpc(const char *urlEndpoint, const char *xmlIn, CkString &outStr);
	const char *xmlRpc(const char *urlEndpoint, const char *xmlIn);
	// XMLRPC_END
	// GENTIMESTAMP_BEGIN
	bool GenTimeStamp(CkString &outStr);
	const char *genTimeStamp(void);
	// GENTIMESTAMP_END
	// XMLRPCPUT_BEGIN
	// eventCallbacks
	bool XmlRpcPut(const char *urlEndpoint, const char *xmlIn, CkString &outStr);
	const char *xmlRpcPut(const char *urlEndpoint, const char *xmlIn);
	// XMLRPCPUT_END
	// QUICKPUTSTR_BEGIN
	// eventCallbacks
	bool QuickPutStr(const char *url, CkString &outStr);
	const char *quickPutStr(const char *url);
	// QUICKPUTSTR_END
	// QUICKDELETESTR_BEGIN
	// eventCallbacks
	bool QuickDeleteStr(const char *url, CkString &outStr);
	const char *quickDeleteStr(const char *url);
	// QUICKDELETESTR_END
	// PUTTEXT_BEGIN
	// eventCallbacks
	bool PutText(const char *url, const char *textData, const char *charset, const char *contentType, bool md5, bool gzip, CkString &outStr);
	const char *putText(const char *url, const char *textData, const char *charset, const char *contentType, bool md5, bool gzip);
	// PUTTEXT_END
	// PUTBINARY_BEGIN
	// eventCallbacks
	bool PutBinary(const char *url, const CkByteData &byteData, const char *contentType, bool md5, bool gzip, CkString &outStr);
	const char *putBinary(const char *url, const CkByteData &byteData, const char *contentType, bool md5, bool gzip);
	// PUTBINARY_END
	// SENDBUFFERSIZE_BEGIN
	int get_SendBufferSize(void);
	void put_SendBufferSize(int newVal);
	// SENDBUFFERSIZE_END
	// NEGOTIATEAUTH_BEGIN
	bool get_NegotiateAuth(void);
	void put_NegotiateAuth(bool newVal);
	// NEGOTIATEAUTH_END
	// LOGINDOMAIN_BEGIN
	void get_LoginDomain(CkString &str);
	const char *loginDomain(void);
	void put_LoginDomain(const char *newVal);
	// LOGINDOMAIN_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
	// SOCKSPORT_BEGIN
	int get_SocksPort(void);
	void put_SocksPort(int newVal);
	// SOCKSPORT_END
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
	// SOCKSHOSTNAME_BEGIN
	void get_SocksHostname(CkString &str);
	const char *socksHostname(void);
	void put_SocksHostname(const char *newVal);
	// SOCKSHOSTNAME_END
	// POSTXML_BEGIN
	// eventCallbacks
	CkHttpResponse *PostXml(const char *url, const char *xmlDoc, const char *charset);
	// POSTXML_END
	// GETSERVERSSLCERT_BEGIN
	// eventCallbacks
	CkCert *GetServerSslCert(const char *domain, int port);
	// GETSERVERSSLCERT_END
	// DIGESTAUTH_BEGIN
	bool get_DigestAuth(void);
	void put_DigestAuth(bool newVal);
	// DIGESTAUTH_END
	// SETSSLCLIENTCERTPFX_BEGIN
	bool SetSslClientCertPfx(const char *pfxPath, const char *pfxPassword);
	// SETSSLCLIENTCERTPFX_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// CLEARINMEMORYCOOKIES_BEGIN
	void ClearInMemoryCookies(void);
	// CLEARINMEMORYCOOKIES_END
	// SETCOOKIEXML_BEGIN
	bool SetCookieXml(const char *domain, const char *cookieXml);
	// SETCOOKIEXML_END
	// POSTURLENCODED_BEGIN
	// eventCallbacks
	CkHttpResponse *PostUrlEncoded(const char *url, CkHttpRequest &req);
	// POSTURLENCODED_END
	// POSTBINARY_BEGIN
	// eventCallbacks
	bool PostBinary(const char *url, const CkByteData &byteData, const char *contentType, bool md5, bool gzip, CkString &outStr);
	const char *postBinary(const char *url, const CkByteData &byteData, const char *contentType, bool md5, bool gzip);
	// POSTBINARY_END
	// QUICKGETOBJ_BEGIN
	// eventCallbacks
	CkHttpResponse *QuickGetObj(const char *url);
	// QUICKGETOBJ_END
	// SESSIONLOGFILENAME_BEGIN
	void get_SessionLogFilename(CkString &str);
	const char *sessionLogFilename(void);
	void put_SessionLogFilename(const char *newVal);
	// SESSIONLOGFILENAME_END
	// USEBGTHREAD_BEGIN
	bool get_UseBgThread(void);
	void put_UseBgThread(bool newVal);
	// USEBGTHREAD_END
	// BGTASKRUNNING_BEGIN
	bool get_BgTaskRunning(void);
	// BGTASKRUNNING_END
	// BGTASKFINISHED_BEGIN
	bool get_BgTaskFinished(void);
	// BGTASKFINISHED_END
	// BGTASKSUCCESS_BEGIN
	bool get_BgTaskSuccess(void);
	// BGTASKSUCCESS_END
	// BGLASTERRORTEXT_BEGIN
	void get_BgLastErrorText(CkString &str);
	const char *bgLastErrorText(void);
	// BGLASTERRORTEXT_END
	// BGTASKABORT_BEGIN
	void BgTaskAbort(void);
	// BGTASKABORT_END
	// KEEPEVENTLOG_BEGIN
	bool get_KeepEventLog(void);
	void put_KeepEventLog(bool newVal);
	// KEEPEVENTLOG_END
	// EVENTLOGCOUNT_BEGIN
	int get_EventLogCount(void);
	// EVENTLOGCOUNT_END
	// EVENTLOGNAME_BEGIN
	bool EventLogName(int index, CkString &outStr);
	const char *eventLogName(int index);
	// EVENTLOGNAME_END
	// EVENTLOGVALUE_BEGIN
	bool EventLogValue(int index, CkString &outStr);
	const char *eventLogValue(int index);
	// EVENTLOGVALUE_END
	// CLEAREVENTLOG_BEGIN
	void ClearBgEventLog(void);
	// CLEAREVENTLOG_END
	// BGRESULTSTRING_BEGIN
	void get_BgResultString(CkString &str);
	const char *bgResultString(void);
	// BGRESULTSTRING_END
	// BGRESULTINT_BEGIN
	int get_BgResultInt(void);
	// BGRESULTINT_END
	// BGRESPONSEOBJECT_BEGIN
	CkHttpResponse *BgResponseObject(void);
	// BGRESPONSEOBJECT_END
	// BGRESULTDATA_BEGIN
	void get_BgResultData(CkByteData &data);
	// BGRESULTDATA_END
	// SLEEPMS_BEGIN
	void SleepMs(int millisec);
	// SLEEPMS_END
	// CLIENTIPADDRESS_BEGIN
	void get_ClientIpAddress(CkString &str);
	const char *clientIpAddress(void);
	void put_ClientIpAddress(const char *newVal);
	// CLIENTIPADDRESS_END
	// PROXYAUTHMETHOD_BEGIN
	void get_ProxyAuthMethod(CkString &str);
	const char *proxyAuthMethod(void);
	void put_ProxyAuthMethod(const char *newVal);
	// PROXYAUTHMETHOD_END
	// ADDQUICKHEADER_BEGIN
	bool AddQuickHeader(const char *name, const char *value);
	// ADDQUICKHEADER_END
	// REMOVEQUICKHEADER_BEGIN
	bool RemoveQuickHeader(const char *name);
	// REMOVEQUICKHEADER_END
	// SETSSLCLIENTCERTPEM_BEGIN
	bool SetSslClientCertPem(const char *pemDataOrPath, const char *pemPassword);
	// SETSSLCLIENTCERTPEM_END
	// ALLOWGZIP_BEGIN
	bool get_AllowGzip(void);
	void put_AllowGzip(bool newVal);
	// ALLOWGZIP_END
	// BGPERCENTDONE_BEGIN
	int get_BgPercentDone(void);
	// BGPERCENTDONE_END
	// REDIRECTVERB_BEGIN
	void get_RedirectVerb(CkString &str);
	const char *redirectVerb(void);
	void put_RedirectVerb(const char *newVal);
	// REDIRECTVERB_END
	// EXTRACTMETAREFRESHURL_BEGIN
	bool ExtractMetaRefreshUrl(const char *html, CkString &outStr);
	const char *extractMetaRefreshUrl(const char *html);
	// EXTRACTMETAREFRESHURL_END
	// AWSACCESSKEY_BEGIN
	void get_AwsAccessKey(CkString &str);
	const char *awsAccessKey(void);
	void put_AwsAccessKey(const char *newVal);
	// AWSACCESSKEY_END
	// AWSSECRETKEY_BEGIN
	void get_AwsSecretKey(CkString &str);
	const char *awsSecretKey(void);
	void put_AwsSecretKey(const char *newVal);
	// AWSSECRETKEY_END
	// S3_LISTBUCKETS_BEGIN
	// eventCallbacks
	bool S3_ListBuckets(CkString &outStr);
	const char *s3_ListBuckets(void);
	// S3_LISTBUCKETS_END
	// S3_UPLOADSTRING_BEGIN
	// eventCallbacks
	bool S3_UploadString(const char *objectContent, const char *charset, const char *contentType, const char *bucketName, const char *ObjectName);
	// S3_UPLOADSTRING_END
	// S3_DELETEOBJECT_BEGIN
	// eventCallbacks
	bool S3_DeleteObject(const char *bucketName, const char *objectName);
	// S3_DELETEOBJECT_END
	// S3_UPLOADBYTES_BEGIN
	// eventCallbacks
	bool S3_UploadBytes(const CkByteData &objectContent, const char *contentType, const char *bucketName, const char *objectName);
	// S3_UPLOADBYTES_END
	// S3_CREATEBUCKET_BEGIN
	// eventCallbacks
	bool S3_CreateBucket(const char *bucketName);
	// S3_CREATEBUCKET_END
	// S3_DELETEBUCKET_BEGIN
	// eventCallbacks
	bool S3_DeleteBucket(const char *bucketName);
	// S3_DELETEBUCKET_END
	// S3_DOWNLOADBYTES_BEGIN
	// eventCallbacks
	bool S3_DownloadBytes(const char *bucketName, const char *objectName, CkByteData &outBytes);
	// S3_DOWNLOADBYTES_END
	// S3_DOWNLOADSTRING_BEGIN
	// eventCallbacks
	bool S3_DownloadString(const char *bucketName, const char *objectName, const char *charset, CkString &outStr);
	const char *s3_DownloadString(const char *bucketName, const char *objectName, const char *charset);
	// S3_DOWNLOADSTRING_END
	// S3_LISTBUCKETOBJECTS_BEGIN
	// eventCallbacks
	bool S3_ListBucketObjects(const char *bucketName, CkString &outStr);
	const char *s3_ListBucketObjects(const char *bucketName);
	// S3_LISTBUCKETOBJECTS_END
	// S3_DOWNLOADFILE_BEGIN
	// eventCallbacks
	bool S3_DownloadFile(const char *bucketName, const char *objectName, const char *localFilePath);
	// S3_DOWNLOADFILE_END
	// S3_UPLOADFILE_BEGIN
	// eventCallbacks
	bool S3_UploadFile(const char *localFilePath, const char *contentType, const char *bucketName, const char *ObjectName);
	// S3_UPLOADFILE_END
	// CLOSEALLCONNECTIONS_BEGIN
	bool CloseAllConnections(void);
	// CLOSEALLCONNECTIONS_END
	// DOWNLOADHASH_BEGIN
	// eventCallbacks
	bool DownloadHash(const char *url, const char *hashAlgorithm, const char *encoding, CkString &outStr);
	const char *downloadHash(const char *url, const char *hashAlgorithm, const char *encoding);
	// DOWNLOADHASH_END
	// POSTJSON_BEGIN
	// eventCallbacks
	CkHttpResponse *PostJson(const char *url, const char *jsonText);
	// POSTJSON_END
	// AWSSUBRESOURCES_BEGIN
	void get_AwsSubResources(CkString &str);
	const char *awsSubResources(void);
	void put_AwsSubResources(const char *newVal);
	// AWSSUBRESOURCES_END

	// HTTP_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


