#ifndef _CkCgiW_H
#define _CkCgiW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCgi.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCgiW : public CkWideCharBase
{
    private:
	CkCgi *m_impl;

        // Disallow assignment or copying this object.
	CkCgiW(const CkCgiW &) { }
	CkCgiW &operator=(const CkCgiW &) { return *this; }


    public:
	CkCgiW()
	    {
	    m_impl = new CkCgi;
	    m_impl->put_Utf8(true);
	    }
	CkCgiW(CkCgi *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCgi;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCgiW(bool bForMono)
	    {
	    m_impl = new CkCgi;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCgi *getMbPtr(void) { return m_impl; }
	CkCgi &getMbRef(void) { return *m_impl; }
	const CkCgi *getMbConstPtr(void) const { return m_impl; }
	const CkCgi &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCgiW() { delete m_impl; m_impl = 0; }

unsigned long get_AsyncBytesRead(void);
bool get_AsyncInProgress(void);
unsigned long get_AsyncPostSize(void);
bool get_AsyncSuccess(void);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_HeartbeatMs(void);
void put_HeartbeatMs(int propVal);
int get_IdleTimeoutMs(void);
void put_IdleTimeoutMs(int propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumParams(void);
int get_NumUploadFiles(void);
int get_ReadChunkSize(void);
void put_ReadChunkSize(int propVal);
unsigned long get_SizeLimitKB(void);
void put_SizeLimitKB(unsigned long propVal);
bool get_StreamToUploadDir(void);
void put_StreamToUploadDir(bool propVal);
void get_UploadDir(CkString &propVal);
void put_UploadDir(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void AbortAsync(void);
#if defined(WIN32) && !defined(SINGLE_THREADED)
bool AsyncReadRequest(void);
#endif
bool GetEnv(const wchar_t *w1,CkString &outStr);
bool GetParam(const wchar_t *w1,CkString &outStr);
bool GetParamName(int idx,CkString &outStr);
bool GetParamValue(int idx,CkString &outStr);
bool GetRawPostData(CkByteData &outData);
bool GetUploadData(int idx,CkByteData &outData);
bool GetUploadFilename(int idx,CkString &outStr);
unsigned long GetUploadSize(int idx);
bool IsGet(void);
bool IsHead(void);
bool IsPost(void);
bool IsUpload(void);
bool ReadRequest(void);
bool SaveLastError(const wchar_t *w1);
void SleepMs(int millisec);
const wchar_t *debugLogFilePath(void);
const wchar_t *getEnv(const wchar_t *w1);
const wchar_t *getParam(const wchar_t *w1);
const wchar_t *getParamName(int idx);
const wchar_t *getParamValue(int idx);
const wchar_t *getUploadFilename(int idx);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *uploadDir(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
