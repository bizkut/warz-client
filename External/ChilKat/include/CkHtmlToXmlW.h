#ifndef _CkHtmlToXmlW_H
#define _CkHtmlToXmlW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkHtmlToXml.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkHtmlToXmlW : public CkWideCharBase
{
    private:
	CkHtmlToXml *m_impl;

        // Disallow assignment or copying this object.
	CkHtmlToXmlW(const CkHtmlToXmlW &) { }
	CkHtmlToXmlW &operator=(const CkHtmlToXmlW &) { return *this; }


    public:
	CkHtmlToXmlW()
	    {
	    m_impl = new CkHtmlToXml;
	    m_impl->put_Utf8(true);
	    }
	CkHtmlToXmlW(CkHtmlToXml *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkHtmlToXml;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHtmlToXmlW(bool bForMono)
	    {
	    m_impl = new CkHtmlToXml;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHtmlToXml *getMbPtr(void) { return m_impl; }
	CkHtmlToXml &getMbRef(void) { return *m_impl; }
	const CkHtmlToXml *getMbConstPtr(void) const { return m_impl; }
	const CkHtmlToXml &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkHtmlToXmlW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_DropCustomTags(void);
void put_DropCustomTags(bool propVal);
void get_Html(CkString &propVal);
void put_Html(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
long get_Nbsp(void);
void put_Nbsp(long propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void get_XmlCharset(CkString &propVal);
void put_XmlCharset(const wchar_t *w1);
bool ConvertFile(const wchar_t *w1,const wchar_t *w2);
void DropTagType(const wchar_t *w1);
void DropTextFormattingTags(void);
bool IsUnlocked(void);
bool ReadFileToString(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
void SetHtmlBytes(const CkByteData &inData);
bool SetHtmlFromFile(const wchar_t *w1);
bool ToXml(CkString &outStr);
void UndropTagType(const wchar_t *w1);
void UndropTextFormattingTags(void);
bool UnlockComponent(const wchar_t *w1);
bool WriteStringToFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool Xml(CkString &outStr);
const wchar_t *debugLogFilePath(void);
const wchar_t *html(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *readFileToString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *toXml(void);
const wchar_t *version(void);
const wchar_t *xml(void);
const wchar_t *xmlCharset(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
