#ifndef _CkTarW_H
#define _CkTarW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkTar.h"

class CkTarProgress;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkTarW : public CkWideCharBase
{
    private:
	CkTar *m_impl;

        // Disallow assignment or copying this object.
	CkTarW(const CkTarW &) { }
	CkTarW &operator=(const CkTarW &) { return *this; }


    public:
	CkTarW()
	    {
	    m_impl = new CkTar;
	    m_impl->put_Utf8(true);
	    }
	CkTarW(CkTar *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkTar;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkTarW(bool bForMono)
	    {
	    m_impl = new CkTar;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkTar *getMbPtr(void) { return m_impl; }
	CkTar &getMbRef(void) { return *m_impl; }
	const CkTar *getMbConstPtr(void) const { return m_impl; }
	const CkTar &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkTarW() { delete m_impl; m_impl = 0; }

void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_DirMode(void);
void put_DirMode(int propVal);
void get_DirPrefix(CkString &propVal);
void put_DirPrefix(const wchar_t *w1);
CkTarProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkTarProgress * propVal);
int get_FileMode(void);
void put_FileMode(int propVal);
int get_GroupId(void);
void put_GroupId(int propVal);
void get_GroupName(CkString &propVal);
void put_GroupName(const wchar_t *w1);
int get_HeartbeatMs(void);
void put_HeartbeatMs(int propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_NoAbsolutePaths(void);
void put_NoAbsolutePaths(bool propVal);
int get_NumDirRoots(void);
int get_ScriptFileMode(void);
void put_ScriptFileMode(int propVal);
bool get_UntarCaseSensitive(void);
void put_UntarCaseSensitive(bool propVal);
bool get_UntarDebugLog(void);
void put_UntarDebugLog(bool propVal);
bool get_UntarDiscardPaths(void);
void put_UntarDiscardPaths(bool propVal);
void get_UntarFromDir(CkString &propVal);
void put_UntarFromDir(const wchar_t *w1);
void get_UntarMatchPattern(CkString &propVal);
void put_UntarMatchPattern(const wchar_t *w1);
int get_UntarMaxCount(void);
void put_UntarMaxCount(int propVal);
int get_UserId(void);
void put_UserId(int propVal);
void get_UserName(CkString &propVal);
void put_UserName(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void get_WriteFormat(CkString &propVal);
void put_WriteFormat(const wchar_t *w1);
bool AddDirRoot(const wchar_t *w1);
bool GetDirRoot(int index,CkString &outStr);
bool ListXml(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
int Untar(const wchar_t *w1);
bool UntarBz2(const wchar_t *w1);
bool UntarFirstMatchingToMemory(const CkByteData &tarFileBytes,const wchar_t *w1,CkByteData &outBytes);
int UntarFromMemory(const CkByteData &tarFileBytes);
bool UntarGz(const wchar_t *w1);
bool UntarZ(const wchar_t *w1);
bool VerifyTar(const wchar_t *w1);
bool WriteTar(const wchar_t *w1);
bool WriteTarBz2(const wchar_t *w1);
bool WriteTarGz(const wchar_t *w1);
const wchar_t *charset(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *dirPrefix(void);
const wchar_t *getDirRoot(int index);
const wchar_t *groupName(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *listXml(const wchar_t *w1);
const wchar_t *untarFromDir(void);
const wchar_t *untarMatchPattern(void);
const wchar_t *userName(void);
const wchar_t *version(void);
const wchar_t *writeFormat(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
