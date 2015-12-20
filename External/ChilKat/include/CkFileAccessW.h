#ifndef _CkFileAccessW_H
#define _CkFileAccessW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkFileAccess.h"

class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkFileAccessW : public CkWideCharBase
{
    private:
	CkFileAccess *m_impl;

        // Disallow assignment or copying this object.
	CkFileAccessW(const CkFileAccessW &) { }
	CkFileAccessW &operator=(const CkFileAccessW &) { return *this; }


    public:
	CkFileAccessW()
	    {
	    m_impl = new CkFileAccess;
	    m_impl->put_Utf8(true);
	    }
	CkFileAccessW(CkFileAccess *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkFileAccess;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkFileAccessW(bool bForMono)
	    {
	    m_impl = new CkFileAccess;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkFileAccess *getMbPtr(void) { return m_impl; }
	CkFileAccess &getMbRef(void) { return *m_impl; }
	const CkFileAccess *getMbConstPtr(void) const { return m_impl; }
	const CkFileAccess &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkFileAccessW() { delete m_impl; m_impl = 0; }

void get_CurrentDir(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_EndOfFile(void);
int get_FileOpenError(void);
void get_FileOpenErrorMsg(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AppendAnsi(const wchar_t *w1);
bool AppendText(const wchar_t *w1,const wchar_t *w2);
bool AppendUnicodeBOM(void);
bool AppendUtf8BOM(void);
bool DirAutoCreate(const wchar_t *w1);
bool DirCreate(const wchar_t *w1);
bool DirDelete(const wchar_t *w1);
bool DirEnsureExists(const wchar_t *w1);
void FileClose(void);
bool FileCopy(const wchar_t *w1,const wchar_t *w2,bool failIfExists);
bool FileDelete(const wchar_t *w1);
bool FileExists(const wchar_t *w1);
bool FileRead(int numBytes,CkByteData &outBytes);
bool FileRename(const wchar_t *w1,const wchar_t *w2);
bool FileSeek(int offset,int origin);
int FileSize(const wchar_t *w1);
bool FileWrite(const CkByteData &data);
bool GetTempFilename(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool OpenForAppend(const wchar_t *w1);
bool OpenForRead(const wchar_t *w1);
bool OpenForReadWrite(const wchar_t *w1);
bool OpenForWrite(const wchar_t *w1);
bool ReadBinaryToEncoded(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool ReadEntireFile(const wchar_t *w1,CkByteData &outBytes);
bool ReadEntireTextFile(const wchar_t *w1,const wchar_t *w2,CkString &outStrFileContents);
bool ReassembleFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
int ReplaceStrings(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
bool SaveLastError(const wchar_t *w1);
bool SetCurrentDir(const wchar_t *w1);
bool SetFileTimes(const wchar_t *w1,CkDateTimeW &create,CkDateTimeW &lastAccess,CkDateTimeW &lastModified);
bool SetLastModified(const wchar_t *w1,CkDateTimeW &lastModified);
bool SplitFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,int partSize,const wchar_t *w4);
bool TreeDelete(const wchar_t *w1);
bool WriteEntireFile(const wchar_t *w1,const CkByteData &fileData);
bool WriteEntireTextFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,bool includePreamble);
const wchar_t *currentDir(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *fileOpenErrorMsg(void);
const wchar_t *getTempFilename(const wchar_t *w1,const wchar_t *w2);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *readBinaryToEncoded(const wchar_t *w1,const wchar_t *w2);
const wchar_t *readEntireTextFile(const wchar_t *w1,const wchar_t *w2);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
