#ifndef _CkSFtpFileW_H
#define _CkSFtpFileW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSFtpFile.h"

class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSFtpFileW : public CkWideCharBase
{
    private:
	CkSFtpFile *m_impl;

        // Disallow assignment or copying this object.
	CkSFtpFileW(const CkSFtpFileW &) { }
	CkSFtpFileW &operator=(const CkSFtpFileW &) { return *this; }


    public:
	CkSFtpFileW()
	    {
	    m_impl = new CkSFtpFile;
	    m_impl->put_Utf8(true);
	    }
	CkSFtpFileW(CkSFtpFile *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSFtpFile;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSFtpFileW(bool bForMono)
	    {
	    m_impl = new CkSFtpFile;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSFtpFile *getMbPtr(void) { return m_impl; }
	CkSFtpFile &getMbRef(void) { return *m_impl; }
	const CkSFtpFile *getMbConstPtr(void) const { return m_impl; }
	const CkSFtpFile &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSFtpFileW() { delete m_impl; m_impl = 0; }

void get_CreateTime(SYSTEMTIME &propVal);
void get_CreateTimeStr(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_FileType(CkString &propVal);
void get_Filename(CkString &propVal);
int get_Gid(void);
void get_Group(CkString &propVal);
bool get_IsAppendOnly(void);
bool get_IsArchive(void);
bool get_IsCaseInsensitive(void);
bool get_IsCompressed(void);
bool get_IsDirectory(void);
bool get_IsEncrypted(void);
bool get_IsHidden(void);
bool get_IsImmutable(void);
bool get_IsReadOnly(void);
bool get_IsRegular(void);
bool get_IsSparse(void);
bool get_IsSymLink(void);
bool get_IsSync(void);
bool get_IsSystem(void);
void get_LastAccessTime(SYSTEMTIME &propVal);
void get_LastAccessTimeStr(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LastModifiedTime(SYSTEMTIME &propVal);
void get_LastModifiedTimeStr(CkString &propVal);
void get_Owner(CkString &propVal);
int get_Permissions(void);
int get_Size32(void);
__int64 get_Size64(void);
void get_SizeStr(CkString &propVal);
int get_Uid(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
CkDateTimeW *GetCreateDt(void);
CkDateTimeW *GetLastAccessDt(void);
CkDateTimeW *GetLastModifiedDt(void);
bool SaveLastError(const wchar_t *w1);
const wchar_t *createTimeStr(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *fileType(void);
const wchar_t *filename(void);
const wchar_t *group(void);
const wchar_t *lastAccessTimeStr(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *lastModifiedTimeStr(void);
const wchar_t *owner(void);
const wchar_t *sizeStr(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
