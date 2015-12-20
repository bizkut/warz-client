#ifndef _CkHttpRequestW_H
#define _CkHttpRequestW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkHttpRequest.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkHttpRequestW : public CkWideCharBase
{
    private:
	CkHttpRequest *m_impl;

        // Disallow assignment or copying this object.
	CkHttpRequestW(const CkHttpRequestW &) { }
	CkHttpRequestW &operator=(const CkHttpRequestW &) { return *this; }


    public:
	CkHttpRequestW()
	    {
	    m_impl = new CkHttpRequest;
	    m_impl->put_Utf8(true);
	    }
	CkHttpRequestW(CkHttpRequest *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkHttpRequest;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttpRequestW(bool bForMono)
	    {
	    m_impl = new CkHttpRequest;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHttpRequest *getMbPtr(void) { return m_impl; }
	CkHttpRequest &getMbRef(void) { return *m_impl; }
	const CkHttpRequest *getMbConstPtr(void) const { return m_impl; }
	const CkHttpRequest &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkHttpRequestW() { delete m_impl; m_impl = 0; }

void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_ContentType(CkString &propVal);
void put_ContentType(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_EntireHeader(CkString &propVal);
void put_EntireHeader(const wchar_t *w1);
void get_HttpVerb(CkString &propVal);
void put_HttpVerb(const wchar_t *w1);
void get_HttpVersion(CkString &propVal);
void put_HttpVersion(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumHeaderFields(void);
int get_NumParams(void);
void get_Path(CkString &propVal);
void put_Path(const wchar_t *w1);
bool get_SendCharset(void);
void put_SendCharset(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AddBytesForUpload(const wchar_t *w1,const wchar_t *w2,const CkByteData &byteData);
bool AddBytesForUpload2(const wchar_t *w1,const wchar_t *w2,const CkByteData &byteData,const wchar_t *w3);
bool AddFileForUpload(const wchar_t *w1,const wchar_t *w2);
bool AddFileForUpload2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
void AddHeader(const wchar_t *w1,const wchar_t *w2);
void AddParam(const wchar_t *w1,const wchar_t *w2);
bool AddStringForUpload(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool AddStringForUpload2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,const wchar_t *w5);
bool GenerateRequestText(CkString &outStr);
bool GetHeaderField(const wchar_t *w1,CkString &outStr);
bool GetHeaderName(int index,CkString &outStr);
bool GetHeaderValue(int index,CkString &outStr);
bool GetParam(const wchar_t *w1,CkString &outStr);
bool GetParamName(int index,CkString &outStr);
bool GetParamValue(int index,CkString &outStr);
bool GetUrlEncodedParams(CkString &outStr);
bool LoadBodyFromBytes(const CkByteData &binaryData);
bool LoadBodyFromFile(const wchar_t *w1);
bool LoadBodyFromString(const wchar_t *w1,const wchar_t *w2);
void RemoveAllParams(void);
bool RemoveHeader(const wchar_t *w1);
void RemoveParam(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
void SetFromUrl(const wchar_t *w1);
bool StreamBodyFromFile(const wchar_t *w1);
void UseGet(void);
void UseHead(void);
void UsePost(void);
void UsePostMultipartForm(void);
void UsePut(void);
void UseUpload(void);
void UseUploadPut(void);
void UseXmlHttp(const wchar_t *w1);
const wchar_t *charset(void);
const wchar_t *contentType(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *entireHeader(void);
const wchar_t *generateRequestText(void);
const wchar_t *getHeaderField(const wchar_t *w1);
const wchar_t *getHeaderName(int index);
const wchar_t *getHeaderValue(int index);
const wchar_t *getParam(const wchar_t *w1);
const wchar_t *getParamName(int index);
const wchar_t *getParamValue(int index);
const wchar_t *getUrlEncodedParams(void);
const wchar_t *httpVerb(void);
const wchar_t *httpVersion(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *path(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
