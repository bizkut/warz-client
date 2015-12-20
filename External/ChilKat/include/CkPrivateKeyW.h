#ifndef _CkPrivateKeyW_H
#define _CkPrivateKeyW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkPrivateKey.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkPrivateKeyW : public CkWideCharBase
{
    private:
	CkPrivateKey *m_impl;

        // Disallow assignment or copying this object.
	CkPrivateKeyW(const CkPrivateKeyW &) { }
	CkPrivateKeyW &operator=(const CkPrivateKeyW &) { return *this; }


    public:
	CkPrivateKeyW()
	    {
	    m_impl = new CkPrivateKey;
	    m_impl->put_Utf8(true);
	    }
	CkPrivateKeyW(CkPrivateKey *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkPrivateKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkPrivateKeyW(bool bForMono)
	    {
	    m_impl = new CkPrivateKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkPrivateKey *getMbPtr(void) { return m_impl; }
	CkPrivateKey &getMbRef(void) { return *m_impl; }
	const CkPrivateKey *getMbConstPtr(void) const { return m_impl; }
	const CkPrivateKey &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkPrivateKeyW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool GetPkcs8(CkByteData &outData);
bool GetPkcs8Encrypted(const wchar_t *w1,CkByteData &outBytes);
bool GetPkcs8EncryptedPem(const wchar_t *w1,CkString &outStr);
bool GetPkcs8Pem(CkString &outStr);
bool GetRsaDer(CkByteData &outData);
bool GetRsaPem(CkString &outStr);
bool GetXml(CkString &outStr);
bool LoadEncryptedPem(const wchar_t *w1,const wchar_t *w2);
bool LoadEncryptedPemFile(const wchar_t *w1,const wchar_t *w2);
bool LoadPem(const wchar_t *w1);
bool LoadPemFile(const wchar_t *w1);
bool LoadPkcs8(const CkByteData &data);
bool LoadPkcs8Encrypted(const CkByteData &data,const wchar_t *w1);
bool LoadPkcs8EncryptedFile(const wchar_t *w1,const wchar_t *w2);
bool LoadPkcs8File(const wchar_t *w1);
#ifdef WIN32
bool LoadPvk(const CkByteData &data,const wchar_t *w1);
#endif
#ifdef WIN32
bool LoadPvkFile(const wchar_t *w1,const wchar_t *w2);
#endif
bool LoadRsaDer(const CkByteData &data);
bool LoadRsaDerFile(const wchar_t *w1);
bool LoadXml(const wchar_t *w1);
bool LoadXmlFile(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SavePkcs8EncryptedFile(const wchar_t *w1,const wchar_t *w2);
bool SavePkcs8EncryptedPemFile(const wchar_t *w1,const wchar_t *w2);
bool SavePkcs8File(const wchar_t *w1);
bool SavePkcs8PemFile(const wchar_t *w1);
bool SaveRsaDerFile(const wchar_t *w1);
bool SaveRsaPemFile(const wchar_t *w1);
bool SaveXmlFile(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *getPkcs8EncryptedPem(const wchar_t *w1);
const wchar_t *getPkcs8Pem(void);
const wchar_t *getRsaPem(void);
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
