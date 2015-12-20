#ifndef _CkDkimW_H
#define _CkDkimW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkDkim.h"

class CkDkimProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkDkimW : public CkWideCharBase
{
    private:
	CkDkim *m_impl;

        // Disallow assignment or copying this object.
	CkDkimW(const CkDkimW &) { }
	CkDkimW &operator=(const CkDkimW &) { return *this; }


    public:
	CkDkimW()
	    {
	    m_impl = new CkDkim;
	    m_impl->put_Utf8(true);
	    }
	CkDkimW(CkDkim *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkDkim;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDkimW(bool bForMono)
	    {
	    m_impl = new CkDkim;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDkim *getMbPtr(void) { return m_impl; }
	CkDkim &getMbRef(void) { return *m_impl; }
	const CkDkim *getMbConstPtr(void) const { return m_impl; }
	const CkDkim &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkDkimW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DkimAlg(CkString &propVal);
void put_DkimAlg(const wchar_t *w1);
int get_DkimBodyLengthCount(void);
void put_DkimBodyLengthCount(int propVal);
void get_DkimCanon(CkString &propVal);
void put_DkimCanon(const wchar_t *w1);
void get_DkimDomain(CkString &propVal);
void put_DkimDomain(const wchar_t *w1);
void get_DkimHeaders(CkString &propVal);
void put_DkimHeaders(const wchar_t *w1);
void get_DkimSelector(CkString &propVal);
void put_DkimSelector(const wchar_t *w1);
void get_DomainKeyAlg(CkString &propVal);
void put_DomainKeyAlg(const wchar_t *w1);
void get_DomainKeyCanon(CkString &propVal);
void put_DomainKeyCanon(const wchar_t *w1);
void get_DomainKeyDomain(CkString &propVal);
void put_DomainKeyDomain(const wchar_t *w1);
void get_DomainKeyHeaders(CkString &propVal);
void put_DomainKeyHeaders(const wchar_t *w1);
void get_DomainKeySelector(CkString &propVal);
void put_DomainKeySelector(const wchar_t *w1);
CkDkimProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkDkimProgress * propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AddDkimSignature(const CkByteData &mimeIn,CkByteData &outBytes);
bool AddDomainKeySignature(const CkByteData &mimeIn,CkByteData &outBytes);
bool LoadDkimPk(const wchar_t *w1,const wchar_t *w2);
bool LoadDkimPkBytes(const CkByteData &privateKeyDer,const wchar_t *w1);
bool LoadDkimPkFile(const wchar_t *w1,const wchar_t *w2);
bool LoadDomainKeyPk(const wchar_t *w1,const wchar_t *w2);
bool LoadDomainKeyPkBytes(const CkByteData &privateKeyDer,const wchar_t *w1);
bool LoadDomainKeyPkFile(const wchar_t *w1,const wchar_t *w2);
bool LoadPublicKey(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool LoadPublicKeyFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
int NumDkimSignatures(const CkByteData &mimeData);
int NumDomainKeySignatures(const CkByteData &mimeData);
bool PrefetchPublicKey(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
bool VerifyDkimSignature(int sigIdx,const CkByteData &mimeData);
bool VerifyDomainKeySignature(int sigIdx,const CkByteData &mimeData);
const wchar_t *debugLogFilePath(void);
const wchar_t *dkimAlg(void);
const wchar_t *dkimCanon(void);
const wchar_t *dkimDomain(void);
const wchar_t *dkimHeaders(void);
const wchar_t *dkimSelector(void);
const wchar_t *domainKeyAlg(void);
const wchar_t *domainKeyCanon(void);
const wchar_t *domainKeyDomain(void);
const wchar_t *domainKeyHeaders(void);
const wchar_t *domainKeySelector(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
