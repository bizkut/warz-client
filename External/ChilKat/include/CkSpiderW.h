#ifndef _CkSpiderW_H
#define _CkSpiderW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSpider.h"

class CkSpiderProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSpiderW : public CkWideCharBase
{
    private:
	CkSpider *m_impl;

        // Disallow assignment or copying this object.
	CkSpiderW(const CkSpiderW &) { }
	CkSpiderW &operator=(const CkSpiderW &) { return *this; }


    public:
	CkSpiderW()
	    {
	    m_impl = new CkSpider;
	    m_impl->put_Utf8(true);
	    }
	CkSpiderW(CkSpider *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSpider;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSpiderW(bool bForMono)
	    {
	    m_impl = new CkSpider;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSpider *getMbPtr(void) { return m_impl; }
	CkSpider &getMbRef(void) { return *m_impl; }
	const CkSpider *getMbConstPtr(void) const { return m_impl; }
	const CkSpider &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSpiderW() { delete m_impl; m_impl = 0; }

bool get_AvoidHttps(void);
void put_AvoidHttps(bool propVal);
void get_CacheDir(CkString &propVal);
void put_CacheDir(const wchar_t *w1);
bool get_ChopAtQuery(void);
void put_ChopAtQuery(bool propVal);
int get_ConnectTimeout(void);
void put_ConnectTimeout(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_Domain(CkString &propVal);
CkSpiderProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkSpiderProgress * propVal);
bool get_FetchFromCache(void);
void put_FetchFromCache(bool propVal);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_LastFromCache(void);
void get_LastHtml(CkString &propVal);
void get_LastHtmlDescription(CkString &propVal);
void get_LastHtmlKeywords(CkString &propVal);
void get_LastHtmlTitle(CkString &propVal);
void get_LastModDate(SYSTEMTIME &propVal);
void get_LastModDateStr(CkString &propVal);
void get_LastUrl(CkString &propVal);
int get_MaxResponseSize(void);
void put_MaxResponseSize(int propVal);
int get_MaxUrlLen(void);
void put_MaxUrlLen(int propVal);
int get_NumAvoidPatterns(void);
int get_NumFailed(void);
int get_NumOutboundLinks(void);
int get_NumSpidered(void);
int get_NumUnspidered(void);
void get_ProxyDomain(CkString &propVal);
void put_ProxyDomain(const wchar_t *w1);
void get_ProxyLogin(CkString &propVal);
void put_ProxyLogin(const wchar_t *w1);
void get_ProxyPassword(CkString &propVal);
void put_ProxyPassword(const wchar_t *w1);
int get_ProxyPort(void);
void put_ProxyPort(int propVal);
int get_ReadTimeout(void);
void put_ReadTimeout(int propVal);
bool get_UpdateCache(void);
void put_UpdateCache(bool propVal);
void get_UserAgent(CkString &propVal);
void put_UserAgent(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int get_WindDownCount(void);
void put_WindDownCount(int propVal);
void AddAvoidOutboundLinkPattern(const wchar_t *w1);
void AddAvoidPattern(const wchar_t *w1);
void AddMustMatchPattern(const wchar_t *w1);
void AddUnspidered(const wchar_t *w1);
bool CanonicalizeUrl(const wchar_t *w1,CkString &outStr);
void ClearFailedUrls(void);
void ClearOutboundLinks(void);
void ClearSpideredUrls(void);
bool CrawlNext(void);
bool FetchRobotsText(CkString &outStr);
bool GetAvoidPattern(long index,CkString &outStr);
bool GetBaseDomain(const wchar_t *w1,CkString &outStr);
bool GetFailedUrl(long index,CkString &outStr);
bool GetOutboundLink(long index,CkString &outStr);
bool GetSpideredUrl(long index,CkString &outStr);
bool GetUnspideredUrl(long index,CkString &outStr);
bool GetUrlDomain(const wchar_t *w1,CkString &outStr);
void Initialize(const wchar_t *w1);
bool RecrawlLast(void);
bool SaveLastError(const wchar_t *w1);
void SkipUnspidered(long index);
void SleepMs(long millisec);
const wchar_t *avoidPattern(long index);
const wchar_t *cacheDir(void);
const wchar_t *canonicalizeUrl(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *domain(void);
const wchar_t *failedUrl(long index);
const wchar_t *fetchRobotsText(void);
const wchar_t *getAvoidPattern(long index);
const wchar_t *getBaseDomain(const wchar_t *w1);
const wchar_t *getFailedUrl(long index);
const wchar_t *getOutboundLink(long index);
const wchar_t *getSpideredUrl(long index);
const wchar_t *getUnspideredUrl(long index);
const wchar_t *getUrlDomain(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastHtml(void);
const wchar_t *lastHtmlDescription(void);
const wchar_t *lastHtmlKeywords(void);
const wchar_t *lastHtmlTitle(void);
const wchar_t *lastModDateStr(void);
const wchar_t *lastUrl(void);
const wchar_t *outboundLink(long index);
const wchar_t *proxyDomain(void);
const wchar_t *proxyLogin(void);
const wchar_t *proxyPassword(void);
const wchar_t *spideredUrl(long index);
const wchar_t *unspideredUrl(long index);
const wchar_t *userAgent(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
