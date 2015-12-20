// CkMht.h: interface for the CkMht class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKMHT_H
#define _CKMHT_H



class CkMime;
class CkEmail;
#include "CkString.h"
class CkMhtProgress;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkMht
class CkMht  : public CkMultiByteBase
{
    private:
	CkMhtProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkMht(const CkMht &) { } 
	CkMht &operator=(const CkMht &) { return *this; }
	CkMht(void *impl);

    public:
	CkMht();
	virtual ~CkMht();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	CkMhtProgress *get_EventCallbackObject(void) const { return m_callback; }
	void put_EventCallbackObject(CkMhtProgress *progress) { m_callback = progress; }

	bool UnpackMHT(const char *mhtPath, const char *unpackDir, const char *htmlFilename, const char *partsSubDir);
	bool UnpackMHTString(const char *mhtContents, const char *unpackDir, const char *htmlFilename, const char *partsSubDir);

	// eventCallbacks
	bool GetAndZipMHT(const char *url, const char *zipEntryFilename, const char *zipPath);
	// eventCallbacks
	bool GetAndZipEML(const char *url, const char *zipEntryFilename, const char *zipPath);
	// eventCallbacks
	CkEmail *GetEmail(const char *url);
	// eventCallbacks
	bool GetAndSaveMHT(const char *url, const char *mhtPath);
	// eventCallbacks
	bool GetAndSaveEML(const char *url, const char *emlPath);
	// eventCallbacks
	bool GetMHT(const char *url, CkString &outStr);
	// eventCallbacks
	bool GetEML(const char *url, CkString &outStr);

	// eventCallbacks
	CkEmail *HtmlToEmail(const char *htmlText);

	// eventCallbacks
	bool HtmlToMHT(const char *htmlText, CkString &outStr);
	// eventCallbacks
	bool HtmlToEML(const char *htmlText, CkString &outStr);

	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;

	void AddExternalStyleSheet(const char *url);
	void ExcludeImagesMatching(const char *pattern);
	void RestoreDefaults();

	// These times are in milliseconds.
	long get_HeartbeatMs(void);
	void put_HeartbeatMs(long newVal);

        // When rendering MHT, this flag determines what is done when the
        // HTML offers a <noscript> alternative.  The default is to include
        // both <script> and <noscript> tags.
        // If this setting is false, then the scripts (<script>...</script>)
        // are removed and the <noscript> tags are removed.
	bool get_PreferMHTScripts(void);
	void put_PreferMHTScripts(bool newVal);

	bool get_NoScripts(void);
	void put_NoScripts(bool newVal);

	bool get_UseCids(void);
	void put_UseCids(bool newVal);

	bool get_EmbedImages(void);
	void put_EmbedImages(bool newVal);

	bool get_NtlmAuth(void);
	void put_NtlmAuth(bool newVal);

	bool get_DebugTagCleaning(void);
	void put_DebugTagCleaning(bool newVal);

	void get_WebSiteLogin(CkString &str);
	void put_WebSiteLogin(const char *newVal);

	void get_WebSitePassword(CkString &str);
	void put_WebSitePassword(const char *newVal);

	void get_ProxyLogin(CkString &str);
	void put_ProxyLogin(const char *newVal);

	void get_ProxyPassword(CkString &str);
	void put_ProxyPassword(const char *newVal);

	const char *proxyLogin(void);
	const char *proxyPassword(void);

	void get_DebugHtmlBefore(CkString &str);
	void put_DebugHtmlBefore(const char *newVal);

	void get_DebugHtmlAfter(CkString &str);
	void put_DebugHtmlAfter(const char *newVal);

	void get_BaseUrl(CkString &str);
	void put_BaseUrl(const char *newVal);

	void get_Proxy(CkString &str);
	void put_Proxy(const char *newVal);

	// Timeouts are in seconds
	long get_ReadTimeout(void);
	void put_ReadTimeout(long numSeconds);

	// Timeouts are in seconds
	long get_ConnectTimeout(void);
	void put_ConnectTimeout(long numSeconds);


	void AddCustomHeader(const char *name, const char *value);
	void RemoveCustomHeader(const char *name);
	void ClearCustomHeaders(void);

	const char *getMHT(const char *url);
	const char *getEML(const char *url);
	const char *htmlToMHT(const char *htmlText);
	const char *htmlToEML(const char *htmlText);
	const char *webSiteLogin(void);
	const char *webSitePassword(void);
	const char *debugHtmlBefore(void);
	const char *debugHtmlAfter(void);
	const char *baseUrl(void);
	const char *proxy(void);


	// Cache methods/properties.
	// 0 = all files in the cache root(s).
	// 1 = 256 subdirectories off cache roots
	// 2 = 256x256 subdirs.
	void put_NumCacheLevels(long v);
	long get_NumCacheLevels(void);

	// Setting/getting cache roots.
	long get_NumCacheRoots(void);
	bool GetCacheRoot(int index, CkString &outStr);
	const char *getCacheRoot(int index);
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


	// USEIEPROXY_BEGIN
	bool get_UseIEProxy(void);
	void put_UseIEProxy(bool newVal);
	// USEIEPROXY_END
	// USEFILENAME_BEGIN
	bool get_UseFilename(void);
	void put_UseFilename(bool newVal);
	// USEFILENAME_END
	// USEINLINE_BEGIN
	bool get_UseInline(void);
	void put_UseInline(bool newVal);
	// USEINLINE_END
	// GETMIME_BEGIN
	// eventCallbacks
	CkMime *GetMime(const char *url);
	// GETMIME_END
	// HTMLTOEMLFILE_BEGIN
	// eventCallbacks
	bool HtmlToEMLFile(const char *html, const char *emlPath);
	// HTMLTOEMLFILE_END
	// HTMLTOMHTFILE_BEGIN
	// eventCallbacks
	bool HtmlToMHTFile(const char *html, const char *mhtPath);
	// HTMLTOMHTFILE_END
	// EMBEDLOCALONLY_BEGIN
	bool get_EmbedLocalOnly(void);
	void put_EmbedLocalOnly(bool newVal);
	// EMBEDLOCALONLY_END
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
	// SOCKSPORT_BEGIN
	int get_SocksPort(void);
	void put_SocksPort(int newVal);
	// SOCKSPORT_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// UNPACKUSERELPATHS_BEGIN
	bool get_UnpackUseRelPaths(void);
	void put_UnpackUseRelPaths(bool newVal);
	// UNPACKUSERELPATHS_END
	// WEBSITELOGINDOMAIN_BEGIN
	void get_WebSiteLoginDomain(CkString &str);
	const char *webSiteLoginDomain(void);
	void put_WebSiteLoginDomain(const char *newVal);
	// WEBSITELOGINDOMAIN_END

	// MHT_INSERT_POINT

	// END PUBLIC INTERFACE


};

#ifndef __sun__
#pragma pack (pop)
#endif


#endif


