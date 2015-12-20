#ifndef _CkUploadW_H
#define _CkUploadW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkUpload.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkUploadW : public CkWideCharBase
{
    private:
	CkUpload *m_impl;

        // Disallow assignment or copying this object.
	CkUploadW(const CkUploadW &) { }
	CkUploadW &operator=(const CkUploadW &) { return *this; }


    public:
	CkUploadW()
	    {
	    m_impl = new CkUpload;
	    m_impl->put_Utf8(true);
	    }
	CkUploadW(CkUpload *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkUpload;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkUploadW(bool bForMono)
	    {
	    m_impl = new CkUpload;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkUpload *getMbPtr(void) { return m_impl; }
	CkUpload &getMbRef(void) { return *m_impl; }
	const CkUpload *getMbConstPtr(void) const { return m_impl; }
	const CkUpload &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkUploadW() { delete m_impl; m_impl = 0; }

int get_ChunkSize(void);
void put_ChunkSize(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_HeartbeatMs(void);
void put_HeartbeatMs(int propVal);
void get_Hostname(CkString &propVal);
void put_Hostname(const wchar_t *w1);
int get_IdleTimeoutMs(void);
void put_IdleTimeoutMs(int propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_Login(CkString &propVal);
void put_Login(const wchar_t *w1);
unsigned long get_NumBytesSent(void);
void get_Password(CkString &propVal);
void put_Password(const wchar_t *w1);
void get_Path(CkString &propVal);
void put_Path(const wchar_t *w1);
unsigned long get_PercentUploaded(void);
int get_Port(void);
void put_Port(int propVal);
void get_ProxyDomain(CkString &propVal);
void put_ProxyDomain(const wchar_t *w1);
void get_ProxyLogin(CkString &propVal);
void put_ProxyLogin(const wchar_t *w1);
void get_ProxyPassword(CkString &propVal);
void put_ProxyPassword(const wchar_t *w1);
long get_ProxyPort(void);
void put_ProxyPort(long propVal);
void get_ResponseBody(CkByteData &propVal);
void get_ResponseHeader(CkString &propVal);
int get_ResponseStatus(void);
bool get_Ssl(void);
void put_Ssl(bool propVal);
unsigned long get_TotalUploadSize(void);
bool get_UploadInProgress(void);
bool get_UploadSuccess(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
#ifndef SINGLE_THREADED
void AbortUpload(void);
#endif
void AddCustomHeader(const wchar_t *w1,const wchar_t *w2);
void AddFileReference(const wchar_t *w1,const wchar_t *w2);
void AddParam(const wchar_t *w1,const wchar_t *w2);
#ifndef SINGLE_THREADED
bool BeginUpload(void);
#endif
bool BlockingUpload(void);
void ClearFileReferences(void);
void ClearParams(void);
bool SaveLastError(const wchar_t *w1);
void SleepMs(int millisec);
bool UploadToMemory(CkByteData &outData);
const wchar_t *debugLogFilePath(void);
const wchar_t *hostname(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *login(void);
const wchar_t *password(void);
const wchar_t *path(void);
const wchar_t *proxyDomain(void);
const wchar_t *proxyLogin(void);
const wchar_t *proxyPassword(void);
const wchar_t *responseHeader(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
