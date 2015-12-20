#ifndef _CkCsvW_H
#define _CkCsvW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCsv.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCsvW : public CkWideCharBase
{
    private:
	CkCsv *m_impl;

        // Disallow assignment or copying this object.
	CkCsvW(const CkCsvW &) { }
	CkCsvW &operator=(const CkCsvW &) { return *this; }


    public:
	CkCsvW()
	    {
	    m_impl = new CkCsv;
	    m_impl->put_Utf8(true);
	    }
	CkCsvW(CkCsv *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCsv;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCsvW(bool bForMono)
	    {
	    m_impl = new CkCsv;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCsv *getMbPtr(void) { return m_impl; }
	CkCsv &getMbRef(void) { return *m_impl; }
	const CkCsv *getMbConstPtr(void) const { return m_impl; }
	const CkCsv &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCsvW() { delete m_impl; m_impl = 0; }

bool get_Crlf(void);
void put_Crlf(bool propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_Delimiter(CkString &propVal);
void put_Delimiter(const wchar_t *w1);
bool get_HasColumnNames(void);
void put_HasColumnNames(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumColumns(void);
int get_NumRows(void);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool GetCell(int row,int col,CkString &outStr);
bool GetColumnName(int index,CkString &outStr);
int GetIndex(const wchar_t *w1);
int GetNumCols(int row);
bool LoadFile(const wchar_t *w1);
bool LoadFile2(const wchar_t *w1,const wchar_t *w2);
bool LoadFromString(const wchar_t *w1);
bool SaveFile(const wchar_t *w1);
bool SaveFile2(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
bool SaveToString(CkString &outStr);
bool SetCell(int row,int col,const wchar_t *w1);
bool SetColumnName(int index,const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *delimiter(void);
const wchar_t *getCell(int row,int col);
const wchar_t *getColumnName(int index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *saveToString(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
