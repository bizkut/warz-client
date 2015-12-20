#ifndef _CkCharsetW_H
#define _CkCharsetW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCharset.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCharsetW : public CkWideCharBase
{
    private:
	CkCharset *m_impl;

        // Disallow assignment or copying this object.
	CkCharsetW(const CkCharsetW &) { }
	CkCharsetW &operator=(const CkCharsetW &) { return *this; }


    public:
	CkCharsetW()
	    {
	    m_impl = new CkCharset;
	    m_impl->put_Utf8(true);
	    }
	CkCharsetW(CkCharset *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCharset;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCharsetW(bool bForMono)
	    {
	    m_impl = new CkCharset;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCharset *getMbPtr(void) { return m_impl; }
	CkCharset &getMbRef(void) { return *m_impl; }
	const CkCharset *getMbConstPtr(void) const { return m_impl; }
	const CkCharset &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCharsetW() { delete m_impl; m_impl = 0; }

void get_AltToCharset(CkString &propVal);
void put_AltToCharset(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_ErrorAction(void);
void put_ErrorAction(int propVal);
void get_FromCharset(CkString &propVal);
void put_FromCharset(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastInputAsHex(CkString &propVal);
void get_LastInputAsQP(CkString &propVal);
void get_LastOutputAsHex(CkString &propVal);
void get_LastOutputAsQP(CkString &propVal);
bool get_SaveLast(void);
void put_SaveLast(bool propVal);
void get_ToCharset(CkString &propVal);
void put_ToCharset(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int CharsetToCodePage(const wchar_t *w1);
bool CodePageToCharset(int codePage,CkString &outCharset);
bool ConvertData(const CkByteData &inData,CkByteData &outData);
bool ConvertFile(const wchar_t *w1,const wchar_t *w2);
bool ConvertFileNoPreamble(const wchar_t *w1,const wchar_t *w2);
bool ConvertFromUnicode(const CkByteData &uniData,CkByteData &outMbData);
bool ConvertHtml(const CkByteData &htmlIn,CkByteData &outHtml);
bool ConvertHtmlFile(const wchar_t *w1,const wchar_t *w2);
bool ConvertToUnicode(const CkByteData &mbData,CkByteData &outUniData);
bool EntityEncodeDec(const wchar_t *w1,CkString &outStr);
bool EntityEncodeHex(const wchar_t *w1,CkString &outStr);
bool GetHtmlCharset(const CkByteData &htmlData,CkString &outCharset);
bool GetHtmlFileCharset(const wchar_t *w1,CkString &outCharset);
bool HtmlDecodeToStr(const wchar_t *w1,CkString &outStr);
bool HtmlEntityDecode(const CkByteData &inData,CkByteData &outData);
bool HtmlEntityDecodeFile(const wchar_t *w1,const wchar_t *w2);
bool IsUnlocked(void);
bool LowerCase(const wchar_t *w1,CkString &outStr);
bool ReadFile(const wchar_t *w1,CkByteData &outData);
bool ReadFileToString(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
void SetErrorBytes(const unsigned char *pByteData,unsigned long szByteData);
void SetErrorString(const wchar_t *w1,const wchar_t *w2);
bool UnlockComponent(const wchar_t *w1);
bool UpperCase(const wchar_t *w1,CkString &outStr);
bool UrlDecodeStr(const wchar_t *w1,CkString &outStr);
bool VerifyData(const wchar_t *w1,const CkByteData &charData);
bool VerifyFile(const wchar_t *w1,const wchar_t *w2);
bool WriteFile(const wchar_t *w1,const CkByteData &dataBuf);
bool WriteStringToFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *altToCharset(void);
const wchar_t *codePageToCharset(int codePage);
const wchar_t *debugLogFilePath(void);
const wchar_t *entityEncodeDec(const wchar_t *w1);
const wchar_t *entityEncodeHex(const wchar_t *w1);
const wchar_t *fromCharset(void);
const wchar_t *getHtmlCharset(const CkByteData &htmlData);
const wchar_t *getHtmlFileCharset(const wchar_t *w1);
const wchar_t *htmlDecodeToStr(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastInputAsHex(void);
const wchar_t *lastInputAsQP(void);
const wchar_t *lastOutputAsHex(void);
const wchar_t *lastOutputAsQP(void);
const wchar_t *lowerCase(const wchar_t *w1);
const wchar_t *readFileToString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *toCharset(void);
const wchar_t *upperCase(const wchar_t *w1);
const wchar_t *urlDecodeStr(const wchar_t *w1);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
