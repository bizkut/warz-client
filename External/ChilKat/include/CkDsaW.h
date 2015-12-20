#ifndef _CkDsaW_H
#define _CkDsaW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkDsa.h"

class CkDsaProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkDsaW : public CkWideCharBase
{
    private:
	CkDsa *m_impl;

        // Disallow assignment or copying this object.
	CkDsaW(const CkDsaW &) { }
	CkDsaW &operator=(const CkDsaW &) { return *this; }


    public:
	CkDsaW()
	    {
	    m_impl = new CkDsa;
	    m_impl->put_Utf8(true);
	    }
	CkDsaW(CkDsa *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkDsa;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDsaW(bool bForMono)
	    {
	    m_impl = new CkDsa;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDsa *getMbPtr(void) { return m_impl; }
	CkDsa &getMbRef(void) { return *m_impl; }
	const CkDsa *getMbConstPtr(void) const { return m_impl; }
	const CkDsa &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkDsaW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
CkDsaProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkDsaProgress * propVal);
int get_GroupSize(void);
void put_GroupSize(int propVal);
void get_Hash(CkByteData &propVal);
void put_Hash(CkByteData & propVal);
void get_HexG(CkString &propVal);
void get_HexP(CkString &propVal);
void get_HexQ(CkString &propVal);
void get_HexX(CkString &propVal);
void get_HexY(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_Signature(CkByteData &propVal);
void put_Signature(CkByteData & propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool FromDer(const CkByteData &derData);
bool FromDerFile(const wchar_t *w1);
bool FromEncryptedPem(const wchar_t *w1,const wchar_t *w2);
bool FromPem(const wchar_t *w1);
bool FromPublicDer(const CkByteData &derData);
bool FromPublicDerFile(const wchar_t *w1);
bool FromPublicPem(const wchar_t *w1);
bool FromXml(const wchar_t *w1);
bool GenKey(int numBits);
bool GenKeyFromParamsDer(const CkByteData &derBytes);
bool GenKeyFromParamsDerFile(const wchar_t *w1);
bool GenKeyFromParamsPem(const wchar_t *w1);
bool GenKeyFromParamsPemFile(const wchar_t *w1);
bool GetEncodedHash(const wchar_t *w1,CkString &outStr);
bool GetEncodedSignature(const wchar_t *w1,CkString &outStr);
bool LoadText(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool SaveText(const wchar_t *w1,const wchar_t *w2);
bool SetEncodedHash(const wchar_t *w1,const wchar_t *w2);
bool SetEncodedSignature(const wchar_t *w1,const wchar_t *w2);
bool SetKeyExplicit(int groupSizeInBytes,const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool SetPubKeyExplicit(int groupSizeInBytes,const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool SignHash(void);
bool ToDer(CkByteData &outBytes);
bool ToDerFile(const wchar_t *w1);
bool ToEncryptedPem(const wchar_t *w1,CkString &outStr);
bool ToPem(CkString &outStr);
bool ToPublicDer(CkByteData &outBytes);
bool ToPublicDerFile(const wchar_t *w1);
bool ToPublicPem(CkString &outStr);
bool ToXml(bool bPublicOnly,CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
bool Verify(void);
bool VerifyKey(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *getEncodedHash(const wchar_t *w1);
const wchar_t *getEncodedSignature(const wchar_t *w1);
const wchar_t *hexG(void);
const wchar_t *hexP(void);
const wchar_t *hexQ(void);
const wchar_t *hexX(void);
const wchar_t *hexY(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *loadText(const wchar_t *w1);
const wchar_t *toEncryptedPem(const wchar_t *w1);
const wchar_t *toPem(void);
const wchar_t *toPublicPem(void);
const wchar_t *toXml(bool bPublicOnly);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
