#ifndef _CkHttpResponseW_H
#define _CkHttpResponseW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkHttpResponse.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkHttpResponseW : public CkWideCharBase
{
    private:
	CkHttpResponse *m_impl;

        // Disallow assignment or copying this object.
	CkHttpResponseW(const CkHttpResponseW &) { }
	CkHttpResponseW &operator=(const CkHttpResponseW &) { return *this; }


    public:
	CkHttpResponseW()
	    {
	    m_impl = new CkHttpResponse;
	    m_impl->put_Utf8(true);
	    }
	CkHttpResponseW(CkHttpResponse *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkHttpResponse;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttpResponseW(bool bForMono)
	    {
	    m_impl = new CkHttpResponse;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttpResponse *getMbPtr(void) { return m_impl; }
	CkHttpResponse &getMbRef(void) { return *m_impl; }
	const CkHttpResponse *getMbConstPtr(void) const { return m_impl; }
	const CkHttpResponse &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkHttpResponseW() { delete m_impl; m_impl = 0; }

void get_Body(CkByteData &propVal);
void get_BodyQP(CkString &propVal);
void get_BodyStr(CkString &propVal);
void get_Charset(CkString &propVal);
unsigned long get_ContentLength(void);
__int64 get_ContentLength64(void);
void get_Date(SYSTEMTIME &propVal);
void get_DateStr(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_Domain(CkString &propVal);
void get_FullMime(CkString &propVal);
void get_Header(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumCookies(void);
int get_NumHeaderFields(void);
int get_StatusCode(void);
void get_StatusLine(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool GetCookieDomain(int index,CkString &outStr);
bool GetCookieExpires(int index,SYSTEMTIME &outSysTime);
bool GetCookieExpiresStr(int index,CkString &outStr);
bool GetCookieName(int index,CkString &outStr);
bool GetCookiePath(int index,CkString &outStr);
bool GetCookieValue(int index,CkString &outStr);
bool GetHeaderField(const wchar_t *w1,CkString &outStr);
bool GetHeaderFieldAttr(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool GetHeaderName(int index,CkString &outStr);
bool GetHeaderValue(int index,CkString &outStr);
bool SaveBodyBinary(const wchar_t *w1);
bool SaveBodyText(bool bCrlf,const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool UrlEncParamValue(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
const wchar_t *bodyQP(void);
const wchar_t *bodyStr(void);
const wchar_t *charset(void);
const wchar_t *dateStr(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *domain(void);
const wchar_t *fullMime(void);
const wchar_t *getCookieDomain(int index);
const wchar_t *getCookieExpiresStr(int index);
const wchar_t *getCookieName(int index);
const wchar_t *getCookiePath(int index);
const wchar_t *getCookieValue(int index);
const wchar_t *getHeaderField(const wchar_t *w1);
const wchar_t *getHeaderFieldAttr(const wchar_t *w1,const wchar_t *w2);
const wchar_t *getHeaderName(int index);
const wchar_t *getHeaderValue(int index);
const wchar_t *header(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *statusLine(void);
const wchar_t *urlEncParamValue(const wchar_t *w1,const wchar_t *w2);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
