#ifndef _CkCertStoreW_H
#define _CkCertStoreW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCertStore.h"

class CkCertW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCertStoreW : public CkWideCharBase
{
    private:
	CkCertStore *m_impl;

        // Disallow assignment or copying this object.
	CkCertStoreW(const CkCertStoreW &) { }
	CkCertStoreW &operator=(const CkCertStoreW &) { return *this; }


    public:
	CkCertStoreW()
	    {
	    m_impl = new CkCertStore;
	    m_impl->put_Utf8(true);
	    }
	CkCertStoreW(CkCertStore *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCertStore;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCertStoreW(bool bForMono)
	    {
	    m_impl = new CkCertStore;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCertStore *getMbPtr(void) { return m_impl; }
	CkCertStore &getMbRef(void) { return *m_impl; }
	const CkCertStore *getMbConstPtr(void) const { return m_impl; }
	const CkCertStore &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCertStoreW() { delete m_impl; m_impl = 0; }

bool get_AvoidWindowsPkAccess(void);
void put_AvoidWindowsPkAccess(bool propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
long get_NumCertificates(void);
#if defined(WIN32) && !defined(__MINGW32__)
long get_NumEmailCerts(void);
#endif
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
#if defined(WIN32) && !defined(__MINGW32__)
bool AddCertificate(const CkCertW &cert);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool CreateFileStore(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool CreateMemoryStore(void);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool CreateRegistryStore(const wchar_t *w1,const wchar_t *w2);
#endif
CkCertW *FindCertByRfc822Name(const wchar_t *w1);
CkCertW *FindCertBySerial(const wchar_t *w1);
CkCertW *FindCertBySha1Thumbprint(const wchar_t *w1);
CkCertW *FindCertBySubject(const wchar_t *w1);
CkCertW *FindCertBySubjectCN(const wchar_t *w1);
CkCertW *FindCertBySubjectE(const wchar_t *w1);
CkCertW *FindCertBySubjectO(const wchar_t *w1);
#if defined(WIN32) && !defined(__MINGW32__)
CkCertW *FindCertForEmail(const wchar_t *w1);
#endif
CkCertW *GetCertificate(long index);
#if defined(WIN32) && !defined(__MINGW32__)
CkCertW *GetEmailCert(long index);
#endif
bool LoadPfxData(const CkByteData &pfxData,const wchar_t *w1);
#if !defined(CHILKAT_MONO)
bool LoadPfxData2(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1);
#endif
bool LoadPfxFile(const wchar_t *w1,const wchar_t *w2);
#if defined(WIN32) && !defined(__MINGW32__)
bool OpenCurrentUserStore(bool readOnly);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool OpenFileStore(const wchar_t *w1,bool readOnly);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool OpenLocalSystemStore(bool readOnly);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool OpenRegistryStore(const wchar_t *w1,const wchar_t *w2,bool readOnly);
#endif
#if defined(WIN32) && !defined(__MINGW32__)
bool RemoveCertificate(const CkCertW &cert);
#endif
bool SaveLastError(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
