#ifndef _CkDtObjW_H
#define _CkDtObjW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkDtObj.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkDtObjW : public CkWideCharBase
{
    private:
	CkDtObj *m_impl;

        // Disallow assignment or copying this object.
	CkDtObjW(const CkDtObjW &) { }
	CkDtObjW &operator=(const CkDtObjW &) { return *this; }


    public:
	CkDtObjW()
	    {
	    m_impl = new CkDtObj;
	    m_impl->put_Utf8(true);
	    }
	CkDtObjW(CkDtObj *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkDtObj;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDtObjW(bool bForMono)
	    {
	    m_impl = new CkDtObj;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDtObj *getMbPtr(void) { return m_impl; }
	CkDtObj &getMbRef(void) { return *m_impl; }
	const CkDtObj *getMbConstPtr(void) const { return m_impl; }
	const CkDtObj &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkDtObjW() { delete m_impl; m_impl = 0; }

int get_Day(void);
void put_Day(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_Hour(void);
void put_Hour(int propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_Minute(void);
void put_Minute(int propVal);
int get_Month(void);
void put_Month(int propVal);
int get_Second(void);
void put_Second(int propVal);
int get_StructTmMonth(void);
void put_StructTmMonth(int propVal);
int get_StructTmYear(void);
void put_StructTmYear(int propVal);
bool get_Utc(void);
void put_Utc(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int get_Year(void);
void put_Year(int propVal);
void DeSerialize(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool Serialize(CkString &outStr);
const wchar_t *debugLogFilePath(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *serialize(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
