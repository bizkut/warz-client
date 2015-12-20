#ifndef _CkXmpW_H
#define _CkXmpW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkXmp.h"

class CkXmlW;
class CkStringArrayW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkXmpW : public CkWideCharBase
{
    private:
	CkXmp *m_impl;

        // Disallow assignment or copying this object.
	CkXmpW(const CkXmpW &) { }
	CkXmpW &operator=(const CkXmpW &) { return *this; }


    public:
	CkXmpW()
	    {
	    m_impl = new CkXmp;
	    m_impl->put_Utf8(true);
	    }
	CkXmpW(CkXmp *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkXmp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkXmpW(bool bForMono)
	    {
	    m_impl = new CkXmp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkXmp *getMbPtr(void) { return m_impl; }
	CkXmp &getMbRef(void) { return *m_impl; }
	const CkXmp *getMbConstPtr(void) const { return m_impl; }
	const CkXmp &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkXmpW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumEmbedded(void);
bool get_StructInnerDescrip(void);
void put_StructInnerDescrip(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AddArray(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2,CkStringArrayW &values);
void AddNsMapping(const wchar_t *w1,const wchar_t *w2);
bool AddSimpleDate(CkXmlW &xml,const wchar_t *w1,SYSTEMTIME &sysTime);
bool AddSimpleInt(CkXmlW &xml,const wchar_t *w1,int propVal);
bool AddSimpleStr(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2);
bool AddStructProp(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool Append(CkXmlW &xml);
bool DateToString(SYSTEMTIME &sysTime,CkString &outStr);
CkStringArrayW *GetArray(CkXmlW &xml,const wchar_t *w1);
CkXmlW *GetEmbedded(int index);
CkXmlW *GetProperty(CkXmlW &xml,const wchar_t *w1);
bool GetSimpleDate(CkXmlW &xml,const wchar_t *w1,SYSTEMTIME &outSysTime);
int GetSimpleInt(CkXmlW &xml,const wchar_t *w1);
bool GetSimpleStr(CkXmlW &xml,const wchar_t *w1,CkString &outStr);
CkStringArrayW *GetStructPropNames(CkXmlW &xml,const wchar_t *w1);
bool GetStructValue(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool LoadAppFile(const wchar_t *w1);
bool LoadFromBuffer(const CkByteData &byteData,const wchar_t *w1);
CkXmlW *NewXmp(void);
void RemoveAllEmbedded(void);
bool RemoveArray(CkXmlW &xml,const wchar_t *w1);
void RemoveEmbedded(int index);
void RemoveNsMapping(const wchar_t *w1);
bool RemoveSimple(CkXmlW &xml,const wchar_t *w1);
bool RemoveStruct(CkXmlW &xml,const wchar_t *w1);
bool RemoveStructProp(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2);
bool SaveAppFile(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SaveToBuffer(CkByteData &outBytes);
bool StringToDate(const wchar_t *w1,SYSTEMTIME &outSysTime);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *dateToString(SYSTEMTIME &sysTime);
const wchar_t *debugLogFilePath(void);
const wchar_t *getSimpleStr(CkXmlW &xml,const wchar_t *w1);
const wchar_t *getStructValue(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *simpleStr(CkXmlW &xml,const wchar_t *w1);
const wchar_t *structValue(CkXmlW &xml,const wchar_t *w1,const wchar_t *w2);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
