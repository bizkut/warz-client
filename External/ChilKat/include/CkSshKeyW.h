#ifndef _CkSshKeyW_H
#define _CkSshKeyW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSshKey.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSshKeyW : public CkWideCharBase
{
    private:
	CkSshKey *m_impl;

        // Disallow assignment or copying this object.
	CkSshKeyW(const CkSshKeyW &) { }
	CkSshKeyW &operator=(const CkSshKeyW &) { return *this; }


    public:
	CkSshKeyW()
	    {
	    m_impl = new CkSshKey;
	    m_impl->put_Utf8(true);
	    }
	CkSshKeyW(CkSshKey *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSshKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSshKeyW(bool bForMono)
	    {
	    m_impl = new CkSshKey;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSshKey *getMbPtr(void) { return m_impl; }
	CkSshKey &getMbRef(void) { return *m_impl; }
	const CkSshKey *getMbConstPtr(void) const { return m_impl; }
	const CkSshKey &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSshKeyW() { delete m_impl; m_impl = 0; }

void get_Comment(CkString &propVal);
void put_Comment(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_IsDsaKey(void);
bool get_IsPrivateKey(void);
bool get_IsRsaKey(void);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_Password(CkString &propVal);
void put_Password(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool FromOpenSshPrivateKey(const wchar_t *w1);
bool FromOpenSshPublicKey(const wchar_t *w1);
bool FromPuttyPrivateKey(const wchar_t *w1);
bool FromRfc4716PublicKey(const wchar_t *w1);
bool FromXml(const wchar_t *w1);
bool GenFingerprint(CkString &outStr);
bool GenerateDsaKey(int numBits);
bool GenerateRsaKey(int numBits,int exponent);
bool LoadText(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool SaveText(const wchar_t *w1,const wchar_t *w2);
bool ToOpenSshPrivateKey(bool bEncrypt,CkString &outStr);
bool ToOpenSshPublicKey(CkString &outStr);
bool ToPuttyPrivateKey(bool bEncrypt,CkString &outStr);
bool ToRfc4716PublicKey(CkString &outStr);
bool ToXml(CkString &outStr);
const wchar_t *comment(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *genFingerprint(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *loadText(const wchar_t *w1);
const wchar_t *password(void);
const wchar_t *toOpenSshPrivateKey(bool bEncrypt);
const wchar_t *toOpenSshPublicKey(void);
const wchar_t *toPuttyPrivateKey(bool bEncrypt);
const wchar_t *toRfc4716PublicKey(void);
const wchar_t *toXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
