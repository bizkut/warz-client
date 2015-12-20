#ifndef _CkNtlmW_H
#define _CkNtlmW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkNtlm.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkNtlmW : public CkWideCharBase
{
    private:
	CkNtlm *m_impl;

        // Disallow assignment or copying this object.
	CkNtlmW(const CkNtlmW &) { }
	CkNtlmW &operator=(const CkNtlmW &) { return *this; }


    public:
	CkNtlmW()
	    {
	    m_impl = new CkNtlm;
	    m_impl->put_Utf8(true);
	    }
	CkNtlmW(CkNtlm *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkNtlm;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkNtlmW(bool bForMono)
	    {
	    m_impl = new CkNtlm;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkNtlm *getMbPtr(void) { return m_impl; }
	CkNtlm &getMbRef(void) { return *m_impl; }
	const CkNtlm *getMbConstPtr(void) const { return m_impl; }
	const CkNtlm &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkNtlmW() { delete m_impl; m_impl = 0; }

void get_ClientChallenge(CkString &propVal);
void put_ClientChallenge(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DnsComputerName(CkString &propVal);
void put_DnsComputerName(const wchar_t *w1);
void get_DnsDomainName(CkString &propVal);
void put_DnsDomainName(const wchar_t *w1);
void get_Domain(CkString &propVal);
void put_Domain(const wchar_t *w1);
void get_EncodingMode(CkString &propVal);
void put_EncodingMode(const wchar_t *w1);
void get_Flags(CkString &propVal);
void put_Flags(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_NetBiosComputerName(CkString &propVal);
void put_NetBiosComputerName(const wchar_t *w1);
void get_NetBiosDomainName(CkString &propVal);
void put_NetBiosDomainName(const wchar_t *w1);
int get_NtlmVersion(void);
void put_NtlmVersion(int propVal);
int get_OemCodePage(void);
void put_OemCodePage(int propVal);
void get_Password(CkString &propVal);
void put_Password(const wchar_t *w1);
void get_ServerChallenge(CkString &propVal);
void put_ServerChallenge(const wchar_t *w1);
void get_TargetName(CkString &propVal);
void put_TargetName(const wchar_t *w1);
void get_UserName(CkString &propVal);
void put_UserName(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void get_Workstation(CkString &propVal);
void put_Workstation(const wchar_t *w1);
bool CompareType3(const wchar_t *w1,const wchar_t *w2);
bool GenType1(CkString &outStr);
bool GenType2(const wchar_t *w1,CkString &outStr);
bool GenType3(const wchar_t *w1,CkString &outStr);
bool LoadType3(const wchar_t *w1);
bool ParseType1(const wchar_t *w1,CkString &outStr);
bool ParseType2(const wchar_t *w1,CkString &outStr);
bool ParseType3(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool SetFlag(const wchar_t *w1,bool on);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *clientChallenge(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *dnsComputerName(void);
const wchar_t *dnsDomainName(void);
const wchar_t *domain(void);
const wchar_t *encodingMode(void);
const wchar_t *flags(void);
const wchar_t *genType1(void);
const wchar_t *genType2(const wchar_t *w1);
const wchar_t *genType3(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *netBiosComputerName(void);
const wchar_t *netBiosDomainName(void);
const wchar_t *parseType1(const wchar_t *w1);
const wchar_t *parseType2(const wchar_t *w1);
const wchar_t *parseType3(const wchar_t *w1);
const wchar_t *password(void);
const wchar_t *serverChallenge(void);
const wchar_t *targetName(void);
const wchar_t *userName(void);
const wchar_t *version(void);
const wchar_t *workstation(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
