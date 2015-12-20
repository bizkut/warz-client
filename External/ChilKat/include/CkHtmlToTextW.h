#ifndef _CkHtmlToTextW_H
#define _CkHtmlToTextW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkHtmlToText.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkHtmlToTextW : public CkWideCharBase
{
    private:
	CkHtmlToText *m_impl;

        // Disallow assignment or copying this object.
	CkHtmlToTextW(const CkHtmlToTextW &) { }
	CkHtmlToTextW &operator=(const CkHtmlToTextW &) { return *this; }


    public:
	CkHtmlToTextW()
	    {
	    m_impl = new CkHtmlToText;
	    m_impl->put_Utf8(true);
	    }
	CkHtmlToTextW(CkHtmlToText *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkHtmlToText;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHtmlToTextW(bool bForMono)
	    {
	    m_impl = new CkHtmlToText;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkHtmlToText *getMbPtr(void) { return m_impl; }
	CkHtmlToText &getMbRef(void) { return *m_impl; }
	const CkHtmlToText *getMbConstPtr(void) const { return m_impl; }
	const CkHtmlToText &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkHtmlToTextW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_DecodeHtmlEntities(void);
void put_DecodeHtmlEntities(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_RightMargin(void);
void put_RightMargin(int propVal);
bool get_SuppressLinks(void);
void put_SuppressLinks(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool IsUnlocked(void);
bool ReadFileToString(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool ToText(const wchar_t *w1,CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
bool WriteStringToFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *debugLogFilePath(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *readFileToString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *toText(const wchar_t *w1);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
