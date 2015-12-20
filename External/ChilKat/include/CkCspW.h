#ifndef _CkCspW_H
#define _CkCspW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCsp.h"

class CkStringArrayW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCspW : public CkWideCharBase
{
    private:
	CkCsp *m_impl;

        // Disallow assignment or copying this object.
	CkCspW(const CkCspW &) { }
	CkCspW &operator=(const CkCspW &) { return *this; }


    public:
	CkCspW()
	    {
	    m_impl = new CkCsp;
	    m_impl->put_Utf8(true);
	    }
	CkCspW(CkCsp *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCsp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCspW(bool bForMono)
	    {
	    m_impl = new CkCsp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCsp *getMbPtr(void) { return m_impl; }
	CkCsp &getMbRef(void) { return *m_impl; }
	const CkCsp *getMbConstPtr(void) const { return m_impl; }
	const CkCsp &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCspW() { delete m_impl; m_impl = 0; }

void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_EncryptAlgorithm(CkString &propVal);
long get_EncryptAlgorithmID(void);
long get_EncryptNumBits(void);
void get_HashAlgorithm(CkString &propVal);
long get_HashAlgorithmID(void);
long get_HashNumBits(void);
void get_KeyContainerName(CkString &propVal);
void put_KeyContainerName(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_MachineKeyset(void);
void put_MachineKeyset(bool propVal);
long get_NumEncryptAlgorithms(void);
long get_NumHashAlgorithms(void);
long get_NumKeyContainers(void);
long get_NumKeyExchangeAlgorithms(void);
long get_NumSignatureAlgorithms(void);
void get_ProviderName(CkString &propVal);
void put_ProviderName(const wchar_t *w1);
long get_ProviderType(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
CkStringArrayW *GetKeyContainerNames(void);
bool HasEncryptAlgorithm(const wchar_t *w1,long numBits);
bool HasHashAlgorithm(const wchar_t *w1,long numBits);
bool Initialize(void);
bool NthEncryptionAlgorithm(long index,CkString &outName);
long NthEncryptionNumBits(long index);
bool NthHashAlgorithmName(long index,CkString &outName);
long NthHashNumBits(long index);
bool NthKeyContainerName(long index,CkString &outName);
bool NthKeyExchangeAlgorithm(long index,CkString &outName);
long NthKeyExchangeNumBits(long index);
bool NthSignatureAlgorithm(long index,CkString &outName);
long NthSignatureNumBits(long index);
bool SaveLastError(const wchar_t *w1);
long SetEncryptAlgorithm(const wchar_t *w1);
long SetHashAlgorithm(const wchar_t *w1);
bool SetProviderMicrosoftBase(void);
bool SetProviderMicrosoftEnhanced(void);
bool SetProviderMicrosoftRsaAes(void);
bool SetProviderMicrosoftStrong(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *encryptAlgorithm(void);
const wchar_t *hashAlgorithm(void);
const wchar_t *keyContainerName(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *nthEncryptionAlgorithm(long index);
const wchar_t *nthHashAlgorithmName(long index);
const wchar_t *nthKeyContainerName(long index);
const wchar_t *nthKeyExchangeAlgorithm(long index);
const wchar_t *nthSignatureAlgorithm(long index);
const wchar_t *providerName(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
