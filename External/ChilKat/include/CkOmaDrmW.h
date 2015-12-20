#ifndef _CkOmaDrmW_H
#define _CkOmaDrmW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkOmaDrm.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkOmaDrmW : public CkWideCharBase
{
    private:
	CkOmaDrm *m_impl;

        // Disallow assignment or copying this object.
	CkOmaDrmW(const CkOmaDrmW &) { }
	CkOmaDrmW &operator=(const CkOmaDrmW &) { return *this; }


    public:
	CkOmaDrmW()
	    {
	    m_impl = new CkOmaDrm;
	    m_impl->put_Utf8(true);
	    }
	CkOmaDrmW(CkOmaDrm *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkOmaDrm;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkOmaDrmW(bool bForMono)
	    {
	    m_impl = new CkOmaDrm;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkOmaDrm *getMbPtr(void) { return m_impl; }
	CkOmaDrm &getMbRef(void) { return *m_impl; }
	const CkOmaDrm *getMbConstPtr(void) const { return m_impl; }
	const CkOmaDrm &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkOmaDrmW() { delete m_impl; m_impl = 0; }

void get_Base64Key(CkString &propVal);
void put_Base64Key(const wchar_t *w1);
void get_ContentType(CkString &propVal);
void put_ContentType(const wchar_t *w1);
void get_ContentUri(CkString &propVal);
void put_ContentUri(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DecryptedData(CkByteData &propVal);
int get_DrmContentVersion(void);
void get_EncryptedData(CkByteData &propVal);
void get_Headers(CkString &propVal);
void put_Headers(const wchar_t *w1);
void get_IV(CkByteData &propVal);
void put_IV(CkByteData & propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool CreateDcfFile(const wchar_t *w1);
bool GetHeaderField(const wchar_t *w1,CkString &outVal);
bool LoadDcfData(const CkByteData &data);
bool LoadDcfFile(const wchar_t *w1);
void LoadUnencryptedData(const CkByteData &data);
bool LoadUnencryptedFile(const wchar_t *w1);
bool SaveDecrypted(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
void SetEncodedIV(const wchar_t *w1,const wchar_t *w2);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *base64Key(void);
const wchar_t *contentType(void);
const wchar_t *contentUri(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *getHeaderField(const wchar_t *w1);
const wchar_t *headers(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
