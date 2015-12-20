#ifndef _CkDirTreeW_H
#define _CkDirTreeW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkDirTree.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkDirTreeW : public CkWideCharBase
{
    private:
	CkDirTree *m_impl;

        // Disallow assignment or copying this object.
	CkDirTreeW(const CkDirTreeW &) { }
	CkDirTreeW &operator=(const CkDirTreeW &) { return *this; }


    public:
	CkDirTreeW()
	    {
	    m_impl = new CkDirTree;
	    m_impl->put_Utf8(true);
	    }
	CkDirTreeW(CkDirTree *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkDirTree;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDirTreeW(bool bForMono)
	    {
	    m_impl = new CkDirTree;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkDirTree *getMbPtr(void) { return m_impl; }
	CkDirTree &getMbRef(void) { return *m_impl; }
	const CkDirTree *getMbConstPtr(void) const { return m_impl; }
	const CkDirTree &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkDirTreeW() { delete m_impl; m_impl = 0; }

void get_BaseDir(CkString &propVal);
void put_BaseDir(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_DoneIterating(void);
int get_FileSize32(void);
__int64 get_FileSize64(void);
void get_FullPath(CkString &propVal);
void get_FullUncPath(CkString &propVal);
bool get_IsDirectory(void);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_Recurse(void);
void put_Recurse(bool propVal);
void get_RelativePath(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AdvancePosition(void);
bool BeginIterate(void);
bool SaveLastError(const wchar_t *w1);
const wchar_t *baseDir(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *fullPath(void);
const wchar_t *fullUncPath(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *relativePath(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
