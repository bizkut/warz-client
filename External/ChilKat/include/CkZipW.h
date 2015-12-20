#ifndef _CkZipW_H
#define _CkZipW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkZip.h"

class CkStringArrayW;
class CkZipProgress;
class CkZipEntryW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkZipW : public CkWideCharBase
{
    private:
	CkZip *m_impl;

        // Disallow assignment or copying this object.
	CkZipW(const CkZipW &) { }
	CkZipW &operator=(const CkZipW &) { return *this; }


    public:
	CkZipW()
	    {
	    m_impl = new CkZip;
	    m_impl->put_Utf8(true);
	    }
	CkZipW(CkZip *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkZip;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkZipW(bool bForMono)
	    {
	    m_impl = new CkZip;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkZip *getMbPtr(void) { return m_impl; }
	CkZip &getMbRef(void) { return *m_impl; }
	const CkZip *getMbConstPtr(void) const { return m_impl; }
	const CkZip &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkZipW() { delete m_impl; m_impl = 0; }

void get_AppendFromDir(CkString &propVal);
void put_AppendFromDir(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE)
void get_AutoRun(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_AutoRun(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_AutoRunParams(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_AutoRunParams(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
bool get_AutoTemp(void);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_AutoTemp(bool propVal);
#endif
bool get_CaseSensitive(void);
void put_CaseSensitive(bool propVal);
bool get_ClearArchiveAttribute(void);
void put_ClearArchiveAttribute(bool propVal);
bool get_ClearReadOnlyAttr(void);
void put_ClearReadOnlyAttr(bool propVal);
void get_Comment(CkString &propVal);
void put_Comment(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DecryptPassword(CkString &propVal);
void put_DecryptPassword(const wchar_t *w1);
bool get_DiscardPaths(void);
void put_DiscardPaths(bool propVal);
long get_EncryptKeyLength(void);
void put_EncryptKeyLength(long propVal);
void get_EncryptPassword(CkString &propVal);
void put_EncryptPassword(const wchar_t *w1);
long get_Encryption(void);
void put_Encryption(long propVal);
CkZipProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkZipProgress * propVal);
#if defined(WIN32) && !defined(WINCE)
void get_ExeDefaultDir(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeDefaultDir(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
bool get_ExeFinishNotifier(void);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeFinishNotifier(bool propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_ExeIconFile(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeIconFile(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
bool get_ExeNoInterface(void);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeNoInterface(bool propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
bool get_ExeSilentProgress(void);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeSilentProgress(bool propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_ExeTitle(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeTitle(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_ExeUnzipCaption(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeUnzipCaption(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_ExeUnzipDir(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeUnzipDir(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
bool get_ExeWaitForSetup(void);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeWaitForSetup(bool propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void get_ExeXmlConfig(CkString &propVal);
#endif
#if defined(WIN32) && !defined(WINCE)
void put_ExeXmlConfig(const wchar_t *w1);
#endif
int get_FileCount(void);
void get_FileName(CkString &propVal);
void put_FileName(const wchar_t *w1);
bool get_HasZipFormatErrors(void);
bool get_IgnoreAccessDenied(void);
void put_IgnoreAccessDenied(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
long get_NumEntries(void);
long get_OemCodePage(void);
void put_OemCodePage(long propVal);
bool get_OverwriteExisting(void);
void put_OverwriteExisting(bool propVal);
bool get_PasswordProtect(void);
void put_PasswordProtect(bool propVal);
void get_PathPrefix(CkString &propVal);
void put_PathPrefix(const wchar_t *w1);
void get_Proxy(CkString &propVal);
void put_Proxy(const wchar_t *w1);
void get_TempDir(CkString &propVal);
void put_TempDir(const wchar_t *w1);
bool get_TextFlag(void);
void put_TextFlag(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
#if defined(WIN32) && !defined(WINCE)
bool AddEmbedded(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
#endif
void AddNoCompressExtension(const wchar_t *w1);
CkZipEntryW *AppendBase64(const wchar_t *w1,const wchar_t *w2);
CkZipEntryW *AppendCompressed(const wchar_t *w1,const CkByteData &data);
CkZipEntryW *AppendData(const wchar_t *w1,const CkByteData &data);
#if !defined(CHILKAT_MONO)
CkZipEntryW *AppendData2(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData);
#endif
bool AppendFiles(const wchar_t *w1,bool recurse);
CkZipEntryW *AppendHex(const wchar_t *w1,const wchar_t *w2);
CkZipEntryW *AppendNew(const wchar_t *w1);
CkZipEntryW *AppendNewDir(const wchar_t *w1);
bool AppendOneFileOrDir(const wchar_t *w1,bool saveExtraPath);
CkZipEntryW *AppendString(const wchar_t *w1,const wchar_t *w2);
CkZipEntryW *AppendString2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool AppendZip(const wchar_t *w1);
void CloseZip(void);
void ExcludeDir(const wchar_t *w1);
bool Extract(const wchar_t *w1);
bool ExtractInto(const wchar_t *w1);
bool ExtractMatching(const wchar_t *w1,const wchar_t *w2);
bool ExtractNewer(const wchar_t *w1);
CkZipEntryW *FirstEntry(void);
CkZipEntryW *FirstMatchingEntry(const wchar_t *w1);
bool GetDirectoryAsXML(CkString &outXml);
CkZipEntryW *GetEntryByID(long entryID);
CkZipEntryW *GetEntryByIndex(long index);
CkZipEntryW *GetEntryByName(const wchar_t *w1);
CkStringArrayW *GetExclusions(void);
#if defined(WIN32) && !defined(WINCE)
bool GetExeConfigParam(const wchar_t *w1,CkString &outStr);
#endif
CkZipEntryW *InsertNew(const wchar_t *w1,long beforeIndex);
bool IsNoCompressExtension(const wchar_t *w1);
bool IsPasswordProtected(const wchar_t *w1);
bool IsUnlocked(void);
bool NewZip(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE)
bool OpenEmbedded(const wchar_t *w1,const wchar_t *w2);
#endif
bool OpenFromByteData(const CkByteData &byteData);
bool OpenFromMemory(const unsigned char *pByteData,unsigned long szByteData);
bool OpenFromWeb(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE)
bool OpenMyEmbedded(const wchar_t *w1);
#endif
bool OpenZip(const wchar_t *w1);
bool QuickAppend(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE)
bool RemoveEmbedded(const wchar_t *w1,const wchar_t *w2);
#endif
void RemoveNoCompressExtension(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE)
bool ReplaceEmbedded(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
#endif
bool SaveLastError(const wchar_t *w1);
void SetCompressionLevel(int level);
void SetExclusions(const CkStringArrayW &excludePatterns);
#if defined(WIN32) && !defined(WINCE)
void SetExeConfigParam(const wchar_t *w1,const wchar_t *w2);
#endif
void SetPassword(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
int Unzip(const wchar_t *w1);
int UnzipInto(const wchar_t *w1);
int UnzipMatching(const wchar_t *w1,const wchar_t *w2,bool verbose);
int UnzipMatchingInto(const wchar_t *w1,const wchar_t *w2,bool verbose);
int UnzipNewer(const wchar_t *w1);
bool VerifyPassword(void);
#if defined(WIN32) && !defined(WINCE)
bool WriteExe(const wchar_t *w1);
#endif
#if defined(WIN32) && !defined(WINCE)
bool WriteExeToMemory(CkByteData &outBytes);
#endif
bool WriteToMemory(CkByteData &outData);
bool WriteZip(void);
bool WriteZipAndClose(void);
const wchar_t *appendFromDir(void);
#if defined(WIN32) && !defined(WINCE)
const wchar_t *autoRun(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *autoRunParams(void);
#endif
const wchar_t *comment(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *decryptPassword(void);
const wchar_t *encryptPassword(void);
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeDefaultDir(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeIconFile(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeTitle(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeUnzipCaption(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeUnzipDir(void);
#endif
#if defined(WIN32) && !defined(WINCE)
const wchar_t *exeXmlConfig(void);
#endif
const wchar_t *fileName(void);
const wchar_t *getDirectoryAsXML(void);
#if defined(WIN32) && !defined(WINCE)
const wchar_t *getExeConfigParam(const wchar_t *w1);
#endif
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *pathPrefix(void);
const wchar_t *proxy(void);
const wchar_t *tempDir(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
