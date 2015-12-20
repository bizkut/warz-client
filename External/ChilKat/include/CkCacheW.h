#ifndef _CkCacheW_H
#define _CkCacheW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCache.h"

class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCacheW : public CkWideCharBase
{
    private:
	CkCache *m_impl;

        // Disallow assignment or copying this object.
	CkCacheW(const CkCacheW &) { }
	CkCacheW &operator=(const CkCacheW &) { return *this; }


    public:
	CkCacheW()
	    {
	    m_impl = new CkCache;
	    m_impl->put_Utf8(true);
	    }
	CkCacheW(CkCache *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCache;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCacheW(bool bForMono)
	    {
	    m_impl = new CkCache;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCache *getMbPtr(void) { return m_impl; }
	CkCache &getMbRef(void) { return *m_impl; }
	const CkCache *getMbConstPtr(void) const { return m_impl; }
	const CkCache &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCacheW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastEtagFetched(CkString &propVal);
void get_LastExpirationFetched(SYSTEMTIME &propVal);
void get_LastExpirationFetchedStr(CkString &propVal);
bool get_LastHitExpired(void);
void get_LastKeyFetched(CkString &propVal);
int get_Level(void);
void put_Level(int propVal);
int get_NumRoots(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void AddRoot(const wchar_t *w1);
int DeleteAll(void);
int DeleteAllExpired(void);
bool DeleteFromCache(const wchar_t *w1);
int DeleteOlder(SYSTEMTIME &dt);
int DeleteOlderDt(CkDateTimeW &dt);
int DeleteOlderStr(const wchar_t *w1);
bool FetchFromCache(const wchar_t *w1,CkByteData &outBytes);
bool FetchText(const wchar_t *w1,CkString &outStr);
bool GetEtag(const wchar_t *w1,CkString &outStr);
bool GetExpiration(const wchar_t *w1,SYSTEMTIME &outSysTime);
CkDateTimeW *GetExpirationDt(const wchar_t *w1);
bool GetExpirationStr(const wchar_t *w1,CkString &outStr);
bool GetFilename(const wchar_t *w1,CkString &outStr);
bool GetRoot(int index,CkString &outStr);
bool IsCached(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SaveText(const wchar_t *w1,SYSTEMTIME &expire,const wchar_t *w2,const wchar_t *w3);
bool SaveTextDt(const wchar_t *w1,CkDateTimeW &expire,const wchar_t *w2,const wchar_t *w3);
bool SaveTextNoExpire(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool SaveTextStr(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool SaveToCache(const wchar_t *w1,SYSTEMTIME &expire,const wchar_t *w2,const CkByteData &data);
bool SaveToCacheDt(const wchar_t *w1,CkDateTimeW &expire,const wchar_t *w2,const CkByteData &data);
bool SaveToCacheNoExpire(const wchar_t *w1,const wchar_t *w2,const CkByteData &data);
bool SaveToCacheStr(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const CkByteData &data);
bool UpdateExpiration(const wchar_t *w1,SYSTEMTIME &dt);
bool UpdateExpirationDt(const wchar_t *w1,CkDateTimeW &dt);
bool UpdateExpirationStr(const wchar_t *w1,const wchar_t *w2);
const wchar_t *debugLogFilePath(void);
const wchar_t *fetchText(const wchar_t *w1);
const wchar_t *getEtag(const wchar_t *w1);
const wchar_t *getExpirationStr(const wchar_t *w1);
const wchar_t *getFilename(const wchar_t *w1);
const wchar_t *getRoot(int index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastEtagFetched(void);
const wchar_t *lastExpirationFetchedStr(void);
const wchar_t *lastKeyFetched(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
