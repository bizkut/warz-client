#ifndef _CkRsaW_H
#define _CkRsaW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkRsa.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkRsaW : public CkWideCharBase
{
    private:
	CkRsa *m_impl;

        // Disallow assignment or copying this object.
	CkRsaW(const CkRsaW &) { }
	CkRsaW &operator=(const CkRsaW &) { return *this; }


    public:
	CkRsaW()
	    {
	    m_impl = new CkRsa;
	    m_impl->put_Utf8(true);
	    }
	CkRsaW(CkRsa *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkRsa;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkRsaW(bool bForMono)
	    {
	    m_impl = new CkRsa;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkRsa *getMbPtr(void) { return m_impl; }
	CkRsa &getMbRef(void) { return *m_impl; }
	const CkRsa *getMbConstPtr(void) const { return m_impl; }
	const CkRsa &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkRsaW() { delete m_impl; m_impl = 0; }

void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_EncodingMode(CkString &propVal);
void put_EncodingMode(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_LittleEndian(void);
void put_LittleEndian(bool propVal);
bool get_NoUnpad(void);
void put_NoUnpad(bool propVal);
long get_NumBits(void);
bool get_OaepPadding(void);
void put_OaepPadding(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool DecryptBytes(const CkByteData &bData,bool bUsePrivateKey,CkByteData &outData);
bool DecryptBytesENC(const wchar_t *w1,bool bUsePrivateKey,CkByteData &outData);
bool DecryptString(const CkByteData &bData,bool bUsePrivateKey,CkString &outStr);
bool DecryptStringENC(const wchar_t *w1,bool bUsePrivateKey,CkString &outStr);
bool EncryptBytes(const CkByteData &bData,bool bUsePrivateKey,CkByteData &outData);
bool EncryptBytesENC(const CkByteData &bData,bool bUsePrivateKey,CkString &outStr);
bool EncryptString(const wchar_t *w1,bool bUsePrivateKey,CkByteData &outData);
bool EncryptStringENC(const wchar_t *w1,bool bUsePrivateKey,CkString &outStr);
bool ExportPrivateKey(CkString &outStr);
bool ExportPublicKey(CkString &outStr);
bool GenerateKey(int numBits);
bool ImportPrivateKey(const wchar_t *w1);
bool ImportPublicKey(const wchar_t *w1);
bool OpenSslSignBytes(const CkByteData &data,CkByteData &outBytes);
bool OpenSslSignBytesENC(const CkByteData &data,CkString &outStr);
bool OpenSslSignString(const wchar_t *w1,CkByteData &outBytes);
bool OpenSslSignStringENC(const wchar_t *w1,CkString &outStr);
bool OpenSslVerifyBytes(const CkByteData &signature,CkByteData &outBytes);
bool OpenSslVerifyBytesENC(const wchar_t *w1,CkByteData &outBytes);
bool OpenSslVerifyString(const CkByteData &data,CkString &outStr);
bool OpenSslVerifyStringENC(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool SignBytes(const CkByteData &bData,const wchar_t *w1,CkByteData &outData);
bool SignBytesENC(const CkByteData &bData,const wchar_t *w1,CkString &outStr);
bool SignHash(const CkByteData &hashBytes,const wchar_t *w1,CkByteData &outBytes);
bool SignHashENC(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool SignString(const wchar_t *w1,const wchar_t *w2,CkByteData &outData);
bool SignStringENC(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool SnkToXml(const wchar_t *w1,CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
bool VerifyBytes(const CkByteData &bData,const wchar_t *w1,const CkByteData &sigData);
bool VerifyBytesENC(const CkByteData &bData,const wchar_t *w1,const wchar_t *w2);
bool VerifyHash(const CkByteData &hashBytes,const wchar_t *w1,const CkByteData &sigBytes);
bool VerifyHashENC(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool VerifyPrivateKey(const wchar_t *w1);
bool VerifyString(const wchar_t *w1,const wchar_t *w2,const CkByteData &sigData);
bool VerifyStringENC(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *charset(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *decryptString(const CkByteData &bData,bool bUsePrivateKey);
const wchar_t *decryptStringENC(const wchar_t *w1,bool bUsePrivateKey);
const wchar_t *encodingMode(void);
const wchar_t *encryptBytesENC(const CkByteData &bData,bool bUsePrivateKey);
const wchar_t *encryptStringENC(const wchar_t *w1,bool bUsePrivateKey);
const wchar_t *exportPrivateKey(void);
const wchar_t *exportPublicKey(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *openSslSignBytesENC(const CkByteData &data);
const wchar_t *openSslSignStringENC(const wchar_t *w1);
const wchar_t *openSslVerifyString(const CkByteData &data);
const wchar_t *openSslVerifyStringENC(const wchar_t *w1);
const wchar_t *signBytesENC(const CkByteData &bData,const wchar_t *w1);
const wchar_t *signHashENC(const wchar_t *w1,const wchar_t *w2);
const wchar_t *signStringENC(const wchar_t *w1,const wchar_t *w2);
const wchar_t *snkToXml(const wchar_t *w1);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
