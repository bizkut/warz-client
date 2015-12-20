// CkGzip.h: interface for the CkGzip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CK_GZIP_H)
#define _CK_GZIP_H


#include "CkMultiByteBase.h"

#include "CkString.h"
class CkByteData;
class CkDateTime;

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

class CkGzipProgress;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkGzip
class CkGzip  : public CkMultiByteBase 
{
public:
    private:
	CkGzipProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkGzip(const CkGzip &) { } 
	CkGzip &operator=(const CkGzip &) { return *this; }
	CkGzip(void *impl);

    public:
	CkGzipProgress *get_EventCallbackObject(void) const { return m_callback; }
	void put_EventCallbackObject(CkGzipProgress *progress) { m_callback = progress; }

	CkGzip();
	virtual ~CkGzip();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// UNCOMPRESSSTRINGENC_BEGIN
	bool UncompressStringENC(const char *strIn, const char *charset, const char *encoding, CkString &outStr);
	const char *uncompressStringENC(const char *strIn, const char *charset, const char *encoding);
	// UNCOMPRESSSTRINGENC_END

	// COMPRESSSTRINGENC_BEGIN
	bool CompressStringENC(const char *strIn, const char *charset, const char *encoding, CkString &outStr);
	const char *compressStringENC(const char *strIn, const char *charset, const char *encoding);
	// COMPRESSSTRINGENC_END

	// LASTMODSTR_BEGIN
	void get_LastModStr(CkString &str);
	const char *lastModStr(void);
	void put_LastModStr(const char *newVal);
	// LASTMODSTR_END

	// SETDT_BEGIN
	bool SetDt(CkDateTime &dt);
	// SETDT_END

	// GETDT_BEGIN
	CkDateTime *GetDt(void);
	// GETDT_END


	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;


	// File-to-file
	// eventCallbacks
	bool CompressFile(const char *srcPath, const char *destPath);
	// eventCallbacks
	bool UncompressFile(const char *srcPath, const char *destPath);

	// File-to-memory
	// eventCallbacks
	bool CompressFileToMem(const char *srcPath, CkByteData &outData);
	// eventCallbacks
	bool UncompressFileToMem(const char *srcPath, CkByteData &outData);

	// Memory-to-file
	// eventCallbacks
	bool CompressMemToFile(const CkByteData &db, const char *destPath);
	// eventCallbacks
	bool UncompressMemToFile(const CkByteData &db, const char *destPath);
	
	// Memory-to-memory
	// eventCallbacks
	bool CompressMemory(const CkByteData &dbIn, CkByteData &outData);
	// eventCallbacks
	bool UncompressMemory(const CkByteData &dbIn, CkByteData &outData);

	// String deflate/inflate
	bool DeflateStringENC(const char *str, const char *charset, const char *encoding, CkString &outStr);
	bool InflateStringENC(const char *str, const char *charset, const char *encoding, CkString &outStr);

	const char *deflateStringENC(const char *str, const char *charset, const char *encoding);
	const char *inflateStringENC(const char *str, const char *charset, const char *encoding);

	void get_Filename(CkString &str);
	void put_Filename(const char *str);

	void get_Comment(CkString &str);
	void put_Comment(const char *str);

	void get_ExtraData(CkByteData &data);
	void put_ExtraData(const CkByteData &data);

	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	void get_LastMod(SYSTEMTIME &outSysTime);
	void put_LastMod(SYSTEMTIME &sysTime);

	// eventCallbacks
	bool UnTarGz(const char *gzPath, const char *destDir, bool bNoAbsolute);

	const char *comment(void);
	const char *filename(void);
	// READFILE_BEGIN
	bool ReadFile(const char *path, CkByteData &outBytes);
	// READFILE_END
	// WRITEFILE_BEGIN
	bool WriteFile(const char *path, const CkByteData &binaryData);
	// WRITEFILE_END
	// COMPRESSFILE2_BEGIN
	// eventCallbacks
	bool CompressFile2(const char *srcPath, const char *embeddedFilename, const char *destPath);
	// COMPRESSFILE2_END
	// COMPRESSSTRING_BEGIN
	// eventCallbacks
	bool CompressString(const char *inStr, const char *destCharset, CkByteData &outBytes);
	// COMPRESSSTRING_END
	// COMPRESSSTRINGTOFILE_BEGIN
	// eventCallbacks
	bool CompressStringToFile(const char *inStr, const char *destCharset, const char *destPath);
	// COMPRESSSTRINGTOFILE_END
	// UNCOMPRESSFILETOSTRING_BEGIN
	// eventCallbacks
	bool UncompressFileToString(const char *srcPath, const char *inCharset, CkString &outStr);
	const char *uncompressFileToString(const char *srcPath, const char *inCharset);
	// UNCOMPRESSFILETOSTRING_END
	// UNCOMPRESSSTRING_BEGIN
	// eventCallbacks
	bool UncompressString(const CkByteData &inData, const char *inCharset, CkString &outStr);
	const char *uncompressString(const CkByteData &inData, const char *inCharset);
	// UNCOMPRESSSTRING_END
	// EXAMINEFILE_BEGIN
	bool ExamineFile(const char *inGzPath);
	// EXAMINEFILE_END
	// EXAMINEMEMORY_BEGIN
	bool ExamineMemory(const CkByteData &inGzData);
	// EXAMINEMEMORY_END
	// DECODE_BEGIN
	bool Decode(const char *str, const char *encoding, CkByteData &outBytes);
	// DECODE_END
	// USECURRENTDATE_BEGIN
	bool get_UseCurrentDate(void);
	void put_UseCurrentDate(bool newVal);
	// USECURRENTDATE_END
	// XFDLTOXML_BEGIN
	bool XfdlToXml(const char *xfdl, CkString &outStr);
	const char *xfdlToXml(const char *xfdl);
	// XFDLTOXML_END
	// ENCODE_BEGIN
	bool Encode(const CkByteData &byteData, const char *encoding, CkString &outStr);
	const char *encode(const CkByteData &byteData, const char *encoding);
	// ENCODE_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END

	// GZIP_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif // !defined(_CK_GZIP_H)
