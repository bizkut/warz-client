#ifndef _CkAtomW_H
#define _CkAtomW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkAtom.h"

class CkAtomProgress;
class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkAtomW : public CkWideCharBase
{
    private:
	CkAtom *m_impl;

        // Disallow assignment or copying this object.
	CkAtomW(const CkAtomW &) { }
	CkAtomW &operator=(const CkAtomW &) { return *this; }


    public:
	CkAtomW()
	    {
	    m_impl = new CkAtom;
	    m_impl->put_Utf8(true);
	    }
	CkAtomW(CkAtom *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkAtom;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkAtomW(bool bForMono)
	    {
	    m_impl = new CkAtom;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkAtom *getMbPtr(void) { return m_impl; }
	CkAtom &getMbRef(void) { return *m_impl; }
	const CkAtom *getMbConstPtr(void) const { return m_impl; }
	const CkAtom &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkAtomW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
CkAtomProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkAtomProgress * propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumEntries(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int AddElement(const wchar_t *w1,const wchar_t *w2);
int AddElementDate(const wchar_t *w1,SYSTEMTIME &dateTime);
int AddElementDateStr(const wchar_t *w1,const wchar_t *w2);
int AddElementDt(const wchar_t *w1,CkDateTimeW &dateTime);
int AddElementHtml(const wchar_t *w1,const wchar_t *w2);
int AddElementXHtml(const wchar_t *w1,const wchar_t *w2);
int AddElementXml(const wchar_t *w1,const wchar_t *w2);
void AddEntry(const wchar_t *w1);
void AddLink(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
void AddPerson(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
void DeleteElement(const wchar_t *w1,int index);
void DeleteElementAttr(const wchar_t *w1,int index,const wchar_t *w2);
void DeletePerson(const wchar_t *w1,int index);
bool DownloadAtom(const wchar_t *w1);
bool GetElement(const wchar_t *w1,int index,CkString &outStr);
bool GetElementAttr(const wchar_t *w1,int index,const wchar_t *w2,CkString &outStr);
int GetElementCount(const wchar_t *w1);
bool GetElementDate(const wchar_t *w1,int index,SYSTEMTIME &outSysTime);
bool GetElementDateStr(const wchar_t *w1,int index,CkString &outStr);
CkDateTimeW *GetElementDt(const wchar_t *w1,int index);
CkAtomW *GetEntry(int index);
bool GetLinkHref(const wchar_t *w1,CkString &outStr);
bool GetPersonInfo(const wchar_t *w1,int index,const wchar_t *w2,CkString &outStr);
bool GetTopAttr(const wchar_t *w1,CkString &outStr);
bool HasElement(const wchar_t *w1);
bool LoadXml(const wchar_t *w1);
void NewEntry(void);
void NewFeed(void);
bool SaveLastError(const wchar_t *w1);
void SetElementAttr(const wchar_t *w1,int index,const wchar_t *w2,const wchar_t *w3);
void SetTopAttr(const wchar_t *w1,const wchar_t *w2);
bool ToXmlString(CkString &outStr);
void UpdateElement(const wchar_t *w1,int index,const wchar_t *w2);
void UpdateElementDate(const wchar_t *w1,int index,SYSTEMTIME &dateTime);
void UpdateElementDateStr(const wchar_t *w1,int index,const wchar_t *w2);
void UpdateElementDt(const wchar_t *w1,int index,CkDateTimeW &dateTime);
void UpdateElementHtml(const wchar_t *w1,int index,const wchar_t *w2);
void UpdateElementXHtml(const wchar_t *w1,int index,const wchar_t *w2);
void UpdateElementXml(const wchar_t *w1,int index,const wchar_t *w2);
void UpdatePerson(const wchar_t *w1,int index,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
const wchar_t *debugLogFilePath(void);
const wchar_t *getElement(const wchar_t *w1,int index);
const wchar_t *getElementAttr(const wchar_t *w1,int index,const wchar_t *w2);
const wchar_t *getElementDateStr(const wchar_t *w1,int index);
const wchar_t *getLinkHref(const wchar_t *w1);
const wchar_t *getPersonInfo(const wchar_t *w1,int index,const wchar_t *w2);
const wchar_t *getTopAttr(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *toXmlString(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
