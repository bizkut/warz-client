#ifndef _CkMhtW_H
#define _CkMhtW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkMht.h"

class CkMimeW;
class CkEmailW;
class CkMhtProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkMhtW : public CkWideCharBase
{
    private:
	CkMht *m_impl;

        // Disallow assignment or copying this object.
	CkMhtW(const CkMhtW &) { }
	CkMhtW &operator=(const CkMhtW &) { return *this; }


    public:
	CkMhtW()
	    {
	    m_impl = new CkMht;
	    m_impl->put_Utf8(true);
	    }
	CkMhtW(CkMht *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkMht;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMhtW(bool bForMono)
	    {
	    m_impl = new CkMht;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMht *getMbPtr(void) { return m_impl; }
	CkMht &getMbRef(void) { return *m_impl; }
	const CkMht *getMbConstPtr(void) const { return m_impl; }
	const CkMht &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkMhtW() { delete m_impl; m_impl = 0; }

void get_BaseUrl(CkString &propVal);
void put_BaseUrl(const wchar_t *w1);
long get_ConnectTimeout(void);
void put_ConnectTimeout(long propVal);
void get_DebugHtmlAfter(CkString &propVal);
void put_DebugHtmlAfter(const wchar_t *w1);
void get_DebugHtmlBefore(CkString &propVal);
void put_DebugHtmlBefore(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_DebugTagCleaning(void);
void put_DebugTagCleaning(bool propVal);
bool get_EmbedImages(void);
void put_EmbedImages(bool propVal);
bool get_EmbedLocalOnly(void);
void put_EmbedLocalOnly(bool propVal);
CkMhtProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkMhtProgress * propVal);
bool get_FetchFromCache(void);
void put_FetchFromCache(bool propVal);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
bool get_IgnoreMustRevalidate(void);
void put_IgnoreMustRevalidate(bool propVal);
bool get_IgnoreNoCache(void);
void put_IgnoreNoCache(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_NoScripts(void);
void put_NoScripts(bool propVal);
bool get_NtlmAuth(void);
void put_NtlmAuth(bool propVal);
long get_NumCacheLevels(void);
void put_NumCacheLevels(long propVal);
long get_NumCacheRoots(void);
bool get_PreferMHTScripts(void);
void put_PreferMHTScripts(bool propVal);
void get_Proxy(CkString &propVal);
void put_Proxy(const wchar_t *w1);
void get_ProxyLogin(CkString &propVal);
void put_ProxyLogin(const wchar_t *w1);
void get_ProxyPassword(CkString &propVal);
void put_ProxyPassword(const wchar_t *w1);
long get_ReadTimeout(void);
void put_ReadTimeout(long propVal);
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
bool get_UnpackUseRelPaths(void);
void put_UnpackUseRelPaths(bool propVal);
bool get_UpdateCache(void);
void put_UpdateCache(bool propVal);
bool get_UseCids(void);
void put_UseCids(bool propVal);
bool get_UseFilename(void);
void put_UseFilename(bool propVal);
bool get_UseIEProxy(void);
void put_UseIEProxy(bool propVal);
bool get_UseInline(void);
void put_UseInline(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void get_WebSiteLogin(CkString &propVal);
void put_WebSiteLogin(const wchar_t *w1);
void get_WebSiteLoginDomain(CkString &propVal);
void put_WebSiteLoginDomain(const wchar_t *w1);
void get_WebSitePassword(CkString &propVal);
void put_WebSitePassword(const wchar_t *w1);
void AddCacheRoot(const wchar_t *w1);
void AddCustomHeader(const wchar_t *w1,const wchar_t *w2);
void AddExternalStyleSheet(const wchar_t *w1);
void ClearCustomHeaders(void);
void ExcludeImagesMatching(const wchar_t *w1);
bool GetAndSaveEML(const wchar_t *w1,const wchar_t *w2);
bool GetAndSaveMHT(const wchar_t *w1,const wchar_t *w2);
bool GetAndZipEML(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool GetAndZipMHT(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool GetCacheRoot(int index,CkString &outStr);
bool GetEML(const wchar_t *w1,CkString &outStr);
CkEmailW *GetEmail(const wchar_t *w1);
bool GetMHT(const wchar_t *w1,CkString &outStr);
CkMimeW *GetMime(const wchar_t *w1);
bool HtmlToEML(const wchar_t *w1,CkString &outStr);
bool HtmlToEMLFile(const wchar_t *w1,const wchar_t *w2);
CkEmailW *HtmlToEmail(const wchar_t *w1);
bool HtmlToMHT(const wchar_t *w1,CkString &outStr);
bool HtmlToMHTFile(const wchar_t *w1,const wchar_t *w2);
bool IsUnlocked(void);
void RemoveCustomHeader(const wchar_t *w1);
void RestoreDefaults(void);
bool SaveLastError(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
bool UnpackMHT(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool UnpackMHTString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
const wchar_t *baseUrl(void);
const wchar_t *debugHtmlAfter(void);
const wchar_t *debugHtmlBefore(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *getCacheRoot(int index);
const wchar_t *getEML(const wchar_t *w1);
const wchar_t *getMHT(const wchar_t *w1);
const wchar_t *htmlToEML(const wchar_t *w1);
const wchar_t *htmlToMHT(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *proxy(void);
const wchar_t *proxyLogin(void);
const wchar_t *proxyPassword(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *version(void);
const wchar_t *webSiteLogin(void);
const wchar_t *webSiteLoginDomain(void);
const wchar_t *webSitePassword(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
