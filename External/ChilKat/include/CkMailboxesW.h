#ifndef _CkMailboxesW_H
#define _CkMailboxesW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkMailboxes.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkMailboxesW : public CkWideCharBase
{
    private:
	CkMailboxes *m_impl;

        // Disallow assignment or copying this object.
	CkMailboxesW(const CkMailboxesW &) { }
	CkMailboxesW &operator=(const CkMailboxesW &) { return *this; }


    public:
	CkMailboxesW()
	    {
	    m_impl = new CkMailboxes;
	    m_impl->put_Utf8(true);
	    }
	CkMailboxesW(CkMailboxes *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkMailboxes;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMailboxesW(bool bForMono)
	    {
	    m_impl = new CkMailboxes;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMailboxes *getMbPtr(void) { return m_impl; }
	CkMailboxes &getMbRef(void) { return *m_impl; }
	const CkMailboxes *getMbConstPtr(void) const { return m_impl; }
	const CkMailboxes &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkMailboxesW() { delete m_impl; m_impl = 0; }

long get_Count(void);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool GetName(long index,CkString &outStrName);
bool HasInferiors(long index);
bool IsMarked(long index);
bool IsSelectable(long index);
bool SaveLastError(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *getName(long index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
