// CkHttpResponse.h: interface for the CkHttpResponse class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkHttpResponse_H
#define _CkHttpResponse_H



#ifndef WIN32
#include "int64.h"
#endif

#include "CkString.h"
class CkByteData;

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
 

// CLASS: CkHttpResponse
class CkHttpResponse  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkHttpResponse(const CkHttpResponse &) { } 
	CkHttpResponse &operator=(const CkHttpResponse &) { return *this; }
	CkHttpResponse(void *impl);

    public:
	void inject(void *impl);

	CkHttpResponse();
	virtual ~CkHttpResponse();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// URLENCPARAMVALUE_BEGIN
	bool UrlEncParamValue(const char *encodedParams, const char *paramName, CkString &outStr);
	const char *urlEncParamValue(const char *encodedParams, const char *paramName);
	// URLENCPARAMVALUE_END

	// FULLMIME_BEGIN
	void get_FullMime(CkString &str);
	const char *fullMime(void);
	// FULLMIME_END

	// DATESTR_BEGIN
	void get_DateStr(CkString &str);
	const char *dateStr(void);
	// DATESTR_END


	void get_Header(CkString &str);
	void get_Body(CkByteData &data);
	void get_BodyStr(CkString &str);
	void get_StatusLine(CkString &str);
	int get_StatusCode(void);
	void get_Charset(CkString &str);
	void get_Domain(CkString &str);
	unsigned long get_ContentLength(void);
	__int64 get_ContentLength64(void);
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	void get_Date(SYSTEMTIME &outSysTime);

	// Get header field by name.
	bool GetHeaderField(const char *fieldName, CkString &outStr);
	bool GetHeaderFieldAttr(const char *fieldName, const char *attrName, CkString &outStr);
	
	int get_NumHeaderFields(void);
	bool GetHeaderName(int index, CkString &outStr);
	bool GetHeaderValue(int index, CkString &outStr);

	// Methods to get Set-Cookie headers.
	int get_NumCookies(void);
	bool GetCookieDomain(int index, CkString &outStr);
	bool GetCookiePath(int index, CkString &outStr);
	bool GetCookieExpiresStr(int index, CkString &outStr);
	bool GetCookieExpires(int index, SYSTEMTIME &outSysTime);
	bool GetCookieName(int index, CkString &outStr);
	bool GetCookieValue(int index, CkString &outStr);

	const char *header(void);
	const char *bodyStr(void);
	const char *statusLine(void);
	const char *charset(void);
	const char *domain(void);
	const char *getHeaderField(const char *fieldName);
	const char *getHeaderFieldAttr(const char *fieldName, const char *attrName);
	const char *getHeaderName(int index);
	const char *getHeaderValue(int index);
	const char *getCookieDomain(int index);
	const char *getCookiePath(int index);
	const char *getCookieExpiresStr(int index);
	const char *getCookieName(int index);
	const char *getCookieValue(int index);
	// BODYQP_BEGIN
	void get_BodyQP(CkString &str);
	const char *bodyQP(void);
	// BODYQP_END
	// SAVEBODYBINARY_BEGIN
	bool SaveBodyBinary(const char *path);
	// SAVEBODYBINARY_END
	// SAVEBODYTEXT_BEGIN
	bool SaveBodyText(bool bCrlf, const char *path);
	// SAVEBODYTEXT_END

	// HTTPRESPONSE_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


