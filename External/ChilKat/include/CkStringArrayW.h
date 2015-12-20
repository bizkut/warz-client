#ifndef _CkStringArrayW_H
#define _CkStringArrayW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkStringArray.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkStringArrayW : public CkWideCharBase
{
    private:
	CkStringArray *m_impl;

        // Disallow assignment or copying this object.
	CkStringArrayW(const CkStringArrayW &) { }
	CkStringArrayW &operator=(const CkStringArrayW &) { return *this; }


    public:
	CkStringArrayW()
	    {
	    m_impl = new CkStringArray;
	    m_impl->put_Utf8(true);
	    }
	CkStringArrayW(CkStringArray *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkStringArray;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkStringArrayW(bool bForMono)
	    {
	    m_impl = new CkStringArray;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkStringArray *getMbPtr(void) { return m_impl; }
	CkStringArray &getMbRef(void) { return *m_impl; }
	const CkStringArray *getMbConstPtr(void) const { return m_impl; }
	const CkStringArray &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkStringArrayW() { delete m_impl; m_impl = 0; }

int get_Count(void);
bool get_Crlf(void);
void put_Crlf(bool propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_Trim(void);
void put_Trim(bool propVal);
bool get_Unique(void);
void put_Unique(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool Append(const wchar_t *w1);
bool AppendSerialized(const wchar_t *w1);
void Clear(void);
bool Contains(const wchar_t *w1);
int Find(const wchar_t *w1,int firstIndex);
int FindFirstMatch(const wchar_t *w1,int firstIndex);
bool GetString(int index,CkString &outStr);
int GetStringLen(int index);
void InsertAt(int index,const wchar_t *w1);
bool LastString(CkString &outStr);
bool LoadFromFile(const wchar_t *w1);
bool LoadFromFile2(const wchar_t *w1,const wchar_t *w2);
void LoadFromText(const wchar_t *w1);
bool Pop(CkString &outStr);
void Prepend(const wchar_t *w1);
void Remove(const wchar_t *w1);
bool RemoveAt(int index);
bool SaveLastError(const wchar_t *w1);
bool SaveNthToFile(int index,const wchar_t *w1);
bool SaveToFile(const wchar_t *w1);
bool SaveToFile2(const wchar_t *w1,const wchar_t *w2);
bool SaveToText(CkString &outStr);
bool Serialize(CkString &outStr);
void Sort(bool ascending);
void SplitAndAppend(const wchar_t *w1,const wchar_t *w2);
void Subtract(CkStringArrayW &sa);
void Union(CkStringArrayW &sa);
const wchar_t *debugLogFilePath(void);
const wchar_t *getString(int index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastString(void);
const wchar_t *pop(void);
const wchar_t *saveToText(void);
const wchar_t *serialize(void);
const wchar_t *strAt(int index);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
