// CkSpider.h: interface for the CkSpider class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSPIDER_H
#define _CKSPIDER_H



#include "CkString.h"
class CkSpiderProgress;

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkSpider
class CkSpider  : public CkMultiByteBase
{
    private:
	CkSpiderProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkSpider(const CkSpider &) { } 
	CkSpider &operator=(const CkSpider &) { return *this; }
	CkSpider(void *impl);


    public:
	CkSpider();
	virtual ~CkSpider();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);


	// BEGIN PUBLIC INTERFACE

	CkSpiderProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkSpiderProgress *progress);

	void SleepMs(long millisec);

	void get_LastHtmlDescription(CkString &outStr);
	void get_LastHtmlKeywords(CkString &outStr);
	void get_LastHtmlTitle(CkString &outStr);
	void get_LastHtml(CkString &outStr);

	bool get_LastFromCache(void);
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	void get_LastModDate(SYSTEMTIME &outSysTime);
	void get_LastUrl(CkString &outStr);
	void get_LastModDateStr(CkString &outStr);

	void SkipUnspidered(long index);
	// eventCallbacks
	bool FetchRobotsText(CkString &outStr);

	void get_Domain(CkString &outStr);
	void AddMustMatchPattern(const char *pattern);
	void AddAvoidOutboundLinkPattern(const char *pattern);
	bool GetAvoidPattern(long index, CkString &outStr);
	void AddAvoidPattern(const char *pattern);
	bool GetOutboundLink(long index, CkString &outStr);
	bool GetFailedUrl(long index, CkString &outStr);
	bool GetSpideredUrl(long index, CkString &outStr);
	bool GetUnspideredUrl(long index, CkString &outStr);
	// eventCallbacks
	bool RecrawlLast(void);

	void ClearOutboundLinks();
	void ClearFailedUrls();
	void ClearSpideredUrls();

	int get_WindDownCount(void);
	void put_WindDownCount(long newVal);

	// These times are in milliseconds.
	long get_HeartbeatMs(void);
	void put_HeartbeatMs(long newVal);

	int get_NumAvoidPatterns(void);
	int get_NumOutboundLinks(void);
	int get_NumFailed(void);
	int get_NumSpidered(void);
	int get_NumUnspidered(void);
	
	// eventCallbacks
	bool CrawlNext(void);

	bool get_ChopAtQuery(void);
	void put_ChopAtQuery(bool newVal);

	bool get_AvoidHttps(void);
	void put_AvoidHttps(bool newVal);

	int get_MaxResponseSize(void);
	void put_MaxResponseSize(long newVal);

	int get_MaxUrlLen(void);
	void put_MaxUrlLen(long newVal);

	void get_CacheDir(CkString &outStr);
	void put_CacheDir(const char *dir);

	bool get_UpdateCache(void);
	void put_UpdateCache(bool newVal);

	bool get_FetchFromCache(void);
	void put_FetchFromCache(bool newVal);

	int get_ConnectTimeout(void);
	void put_ConnectTimeout(long numSeconds);

	void put_UserAgent(const char *ua);
	const char *userAgent(void);
	void get_UserAgent(CkString &outStr);

	int get_ReadTimeout(void);
	void put_ReadTimeout(long numSeconds);

	void AddUnspidered(const char *url);
	void Initialize(const char *domain);

	bool GetUrlDomain(const char *url, CkString &outStr);
	bool GetBaseDomain(const char *domain, CkString &outStr);
	bool CanonicalizeUrl(const char *url, CkString &outStr);

	const char *getUrlDomain(const char *url);
	const char *getBaseDomain(const char *domain);
	const char *canonicalizeUrl(const char *url);

	const char *getAvoidPattern(long index);
	const char *getOutboundLink(long index);
	const char *getFailedUrl(long index);
	const char *getSpideredUrl(long index);
	const char *getUnspideredUrl(long index);

	const char *cacheDir(void);
	const char *avoidPattern(long index);
	const char *outboundLink(long index);
	const char *failedUrl(long index);
	const char *spideredUrl(long index);
	const char *unspideredUrl(long index);
	const char *domain(void);
	const char *lastHtmlDescription(void);
	const char *lastHtmlKeywords(void);
	const char *lastHtmlTitle(void);
	const char *lastHtml(void);
	const char *lastUrl(void);
	const char *lastModDateStr(void);
	const char *fetchRobotsText(void);

	// PROXYDOMAIN_BEGIN
	void get_ProxyDomain(CkString &str);
	const char *proxyDomain(void);
	void put_ProxyDomain(const char *newVal);
	// PROXYDOMAIN_END
	// PROXYLOGIN_BEGIN
	void get_ProxyLogin(CkString &str);
	const char *proxyLogin(void);
	void put_ProxyLogin(const char *newVal);
	// PROXYLOGIN_END
	// PROXYPASSWORD_BEGIN
	void get_ProxyPassword(CkString &str);
	const char *proxyPassword(void);
	void put_ProxyPassword(const char *newVal);
	// PROXYPASSWORD_END
	// PROXYPORT_BEGIN
	int get_ProxyPort(void);
	void put_ProxyPort(int newVal);
	// PROXYPORT_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END

	// SPIDER_INSERT_POINT

	// END PUBLIC INTERFACE

};

#ifndef __sun__
#pragma pack (pop)
#endif


#endif


