#ifndef _CkCompressionW_H
#define _CkCompressionW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCompression.h"




#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCompressionW : public CkWideCharBase
{
    private:
	CkCompression *m_impl;

        // Disallow assignment or copying this object.
	CkCompressionW(const CkCompressionW &) { }
	CkCompressionW &operator=(const CkCompressionW &) { return *this; }


    public:
	CkCompressionW()
	    {
	    m_impl = new CkCompression;
	    m_impl->put_Utf8(true);
	    }
	CkCompressionW(CkCompression *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCompression;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCompressionW(bool bForMono)
	    {
	    m_impl = new CkCompression;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCompression *getMbPtr(void) { return m_impl; }
	CkCompression &getMbRef(void) { return *m_impl; }
	const CkCompression *getMbConstPtr(void) const { return m_impl; }
	const CkCompression &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCompressionW() { delete m_impl; m_impl = 0; }

void get_Algorithm(CkString &propVal);
void put_Algorithm(const wchar_t *w1);
void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_EncodingMode(CkString &propVal);
void put_EncodingMode(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool BeginCompressBytes(const CkByteData &bData,CkByteData &outData);
bool BeginCompressBytesENC(const CkByteData &bData,CkString &outStr);
bool BeginCompressString(const wchar_t *w1,CkByteData &outData);
bool BeginCompressStringENC(const wchar_t *w1,CkString &outStr);
bool BeginDecompressBytes(const CkByteData &bData,CkByteData &outData);
bool BeginDecompressBytesENC(const wchar_t *w1,CkByteData &outData);
bool BeginDecompressString(const CkByteData &bData,CkString &outStr);
bool BeginDecompressStringENC(const wchar_t *w1,CkString &outStr);
bool CompressBytes(const CkByteData &bData,CkByteData &outData);
bool CompressBytesENC(const CkByteData &bData,CkString &outStr);
bool CompressFile(const wchar_t *w1,const wchar_t *w2);
bool CompressString(const wchar_t *w1,CkByteData &outData);
bool CompressStringENC(const wchar_t *w1,CkString &outStr);
bool DecompressBytes(const CkByteData &bData,CkByteData &outData);
bool DecompressBytesENC(const wchar_t *w1,CkByteData &outData);
bool DecompressFile(const wchar_t *w1,const wchar_t *w2);
bool DecompressString(const CkByteData &bData,CkString &outStr);
bool DecompressStringENC(const wchar_t *w1,CkString &outStr);
bool EndCompressBytes(CkByteData &outData);
bool EndCompressBytesENC(CkString &outStr);
bool EndCompressString(CkByteData &outData);
bool EndCompressStringENC(CkString &outStr);
bool EndDecompressBytes(CkByteData &outData);
bool EndDecompressBytesENC(CkByteData &outData);
bool EndDecompressString(CkString &outStr);
bool EndDecompressStringENC(CkString &outStr);
bool MoreCompressBytes(const CkByteData &bData,CkByteData &outData);
bool MoreCompressBytesENC(const CkByteData &bData,CkString &outStr);
bool MoreCompressString(const wchar_t *w1,CkByteData &outData);
bool MoreCompressStringENC(const wchar_t *w1,CkString &outStr);
bool MoreDecompressBytes(const CkByteData &bData,CkByteData &outData);
bool MoreDecompressBytesENC(const wchar_t *w1,CkByteData &outData);
bool MoreDecompressString(const CkByteData &bData,CkString &outStr);
bool MoreDecompressStringENC(const wchar_t *w1,CkString &outStr);
bool SaveLastError(const wchar_t *w1);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *algorithm(void);
const wchar_t *beginCompressBytesENC(const CkByteData &bData);
const wchar_t *beginCompressStringENC(const wchar_t *w1);
const wchar_t *beginDecompressString(const CkByteData &bData);
const wchar_t *beginDecompressStringENC(const wchar_t *w1);
const wchar_t *charset(void);
const wchar_t *compressBytesENC(const CkByteData &bData);
const wchar_t *compressStringENC(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *decompressString(const CkByteData &bData);
const wchar_t *decompressStringENC(const wchar_t *w1);
const wchar_t *encodingMode(void);
const wchar_t *endCompressBytesENC(void);
const wchar_t *endCompressStringENC(void);
const wchar_t *endDecompressString(void);
const wchar_t *endDecompressStringENC(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *moreCompressBytesENC(const CkByteData &bData);
const wchar_t *moreCompressStringENC(const wchar_t *w1);
const wchar_t *moreDecompressString(const CkByteData &bData);
const wchar_t *moreDecompressStringENC(const wchar_t *w1);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
