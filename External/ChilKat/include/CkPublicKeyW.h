#ifndef _CkPublicKeyW_H
#define _CkPublicKeyW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkPublicKey.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkPublicKeyW : public CkWideCharBase
{
    private:
	CkPublicKey *m_impl;

        // Disallow assignment or copying this object.
	CkPublicKeyW(const CkPublicKeyW &) { }
	CkPublicKeyW &operator=(const CkPublicKeyW &) { return *this; }


    public:
	CkPublicKeyW()
	    {
	    m_impl = new CkPublicKey;
	    m_impl->put_Utf8(true);
	    }
	CkPublicKeyW(CkPublicKey *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkPublicKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkPublicKeyW(bool bForMono)
	    {
	    m_impl = new CkPublicKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkPublicKey *getMbPtr(void) { return m_impl; }
	CkPublicKey &getMbRef(void) { return *m_impl; }
	const CkPublicKey *getMbConstPtr(void) const { return m_impl; }
	const CkPublicKey &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkPublicKeyW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool GetOpenSslDer(CkByteData &outData);
bool GetOpenSslPem(CkString &outStr);
bool GetRsaDer(CkByteData &outData);
bool GetXml(CkString &outStr);
bool LoadOpenSslDer(const CkByteData &data);
bool LoadOpenSslDerFile(const wchar_t *w1);
bool LoadOpenSslPem(const wchar_t *w1);
bool LoadOpenSslPemFile(const wchar_t *w1);
bool LoadPkcs1Pem(const wchar_t *w1);
bool LoadRsaDer(const CkByteData &data);
bool LoadRsaDerFile(const wchar_t *w1);
bool LoadXml(const wchar_t *w1);
bool LoadXmlFile(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SaveOpenSslDerFile(const wchar_t *w1);
bool SaveOpenSslPemFile(const wchar_t *w1);
bool SaveRsaDerFile(const wchar_t *w1);
bool SaveXmlFile(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *getOpenSslPem(void);
const wchar_t *getXml(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
