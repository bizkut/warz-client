#ifndef _CkUnixCompressW_H
#define _CkUnixCompressW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkUnixCompress.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkUnixCompressW : public CkWideCharBase
{
    private:
	CkUnixCompress *m_impl;

        // Disallow assignment or copying this object.
	CkUnixCompressW(const CkUnixCompressW &) { }
	CkUnixCompressW &operator=(const CkUnixCompressW &) { return *this; }


    public:
	CkUnixCompressW()
	    {
	    m_impl = new CkUnixCompress;
	    m_impl->put_Utf8(true);
	    }
	CkUnixCompressW(CkUnixCompress *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkUnixCompress;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkUnixCompressW(bool bForMono)
	    {
	    m_impl = new CkUnixCompress;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkUnixCompress *getMbPtr(void) { return m_impl; }
	CkUnixCompress &getMbRef(void) { return *m_impl; }
	const CkUnixCompress *getMbConstPtr(void) const { return m_impl; }
	const CkUnixCompress &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkUnixCompressW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool CompressFile(const wchar_t *w1,const wchar_t *w2);
bool CompressFileToMem(const wchar_t *w1,CkByteData &outData);
bool CompressMemToFile(const CkByteData &db,const wchar_t *w1);
bool CompressMemory(const CkByteData &dbIn,CkByteData &outData);
bool CompressString(const wchar_t *w1,const wchar_t *w2,CkByteData &outBytes);
bool CompressStringToFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool IsUnlocked(void);
bool SaveLastError(const wchar_t *w1);
bool UnTarZ(const wchar_t *w1,const wchar_t *w2,bool bNoAbsolute);
bool UncompressFile(const wchar_t *w1,const wchar_t *w2);
bool UncompressFileToMem(const wchar_t *w1,CkByteData &outData);
bool UncompressFileToString(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool UncompressMemToFile(const CkByteData &db,const wchar_t *w1);
bool UncompressMemory(const CkByteData &dbIn,CkByteData &outData);
bool UncompressString(const CkByteData &inData,const wchar_t *w1,CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *uncompressFileToString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *uncompressString(const CkByteData &inData,const wchar_t *w1);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
