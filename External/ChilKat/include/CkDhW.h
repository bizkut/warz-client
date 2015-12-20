#ifndef _CkDhW_H
#define _CkDhW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkDh.h"

class CkDhProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkDhW : public CkWideCharBase
{
    private:
	CkDh *m_impl;

        // Disallow assignment or copying this object.
	CkDhW(const CkDhW &) { }
	CkDhW &operator=(const CkDhW &) { return *this; }


    public:
	CkDhW()
	    {
	    m_impl = new CkDh;
	    m_impl->put_Utf8(true);
	    }
	CkDhW(CkDh *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkDh;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDhW(bool bForMono)
	    {
	    m_impl = new CkDh;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDh *getMbPtr(void) { return m_impl; }
	CkDh &getMbRef(void) { return *m_impl; }
	const CkDh *getMbConstPtr(void) const { return m_impl; }
	const CkDh &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkDhW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
CkDhProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkDhProgress * propVal);
int get_G(void);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_P(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool CreateE(int numBits,CkString &outStr);
bool FindK(const wchar_t *w1,CkString &outStr);
bool GenPG(int numBits,int g);
bool SaveLastError(const wchar_t *w1);
bool SetPG(const wchar_t *w1,int g);
bool UnlockComponent(const wchar_t *w1);
void UseKnownPrime(int index);
const wchar_t *createE(int numBits);
const wchar_t *debugLogFilePath(void);
const wchar_t *findK(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *p(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
