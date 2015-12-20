#ifndef _CkRssW_H
#define _CkRssW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkRss.h"

class CkRssProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkRssW : public CkWideCharBase
{
    private:
	CkRss *m_impl;

        // Disallow assignment or copying this object.
	CkRssW(const CkRssW &) { }
	CkRssW &operator=(const CkRssW &) { return *this; }


    public:
	CkRssW()
	    {
	    m_impl = new CkRss;
	    m_impl->put_Utf8(true);
	    }
	CkRssW(CkRss *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkRss;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkRssW(bool bForMono)
	    {
	    m_impl = new CkRss;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkRss *getMbPtr(void) { return m_impl; }
	CkRss &getMbRef(void) { return *m_impl; }
	const CkRss *getMbConstPtr(void) const { return m_impl; }
	const CkRss &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkRssW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
CkRssProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkRssProgress * propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumChannels(void);
int get_NumItems(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
CkRssW *AddNewChannel(void);
CkRssW *AddNewImage(void);
CkRssW *AddNewItem(void);
bool DownloadRss(const wchar_t *w1);
bool GetAttr(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
CkRssW *GetChannel(int index);
int GetCount(const wchar_t *w1);
bool GetDate(const wchar_t *w1,SYSTEMTIME &outSysTime);
bool GetDateStr(const wchar_t *w1,CkString &outStr);
CkRssW *GetImage(void);
int GetInt(const wchar_t *w1);
CkRssW *GetItem(int index);
bool GetString(const wchar_t *w1,CkString &outStr);
bool LoadRssFile(const wchar_t *w1);
bool LoadRssString(const wchar_t *w1);
bool MGetAttr(const wchar_t *w1,int index,const wchar_t *w2,CkString &outStr);
bool MGetString(const wchar_t *w1,int index,CkString &outStr);
bool MSetAttr(const wchar_t *w1,int index,const wchar_t *w2,const wchar_t *w3);
bool MSetString(const wchar_t *w1,int index,const wchar_t *w2);
void NewRss(void);
void Remove(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
void SetAttr(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
void SetDate(const wchar_t *w1,SYSTEMTIME &dateTime);
void SetDateNow(const wchar_t *w1);
void SetDateStr(const wchar_t *w1,const wchar_t *w2);
void SetInt(const wchar_t *w1,int value);
void SetString(const wchar_t *w1,const wchar_t *w2);
bool ToXmlString(CkString &outStr);
const wchar_t *debugLogFilePath(void);
const wchar_t *getAttr(const wchar_t *w1,const wchar_t *w2);
const wchar_t *getDateStr(const wchar_t *w1);
const wchar_t *getString(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *mGetAttr(const wchar_t *w1,int index,const wchar_t *w2);
const wchar_t *mGetString(const wchar_t *w1,int index);
const wchar_t *toXmlString(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
