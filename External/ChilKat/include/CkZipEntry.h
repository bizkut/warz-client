// CkZipEntry.h: interface for the CkZipEntry class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKZIPENTRY__H
#define _CKZIPENTRY__H




#ifndef WIN32
#include "int64.h"
#endif

#include "CkString.h"
class CkByteData;
class CkZip;
class CkDateTime;

#include "CkMultiByteBase.h"

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

#undef Copy

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

*/
#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkZipEntry
class CkZipEntry  : public CkMultiByteBase
{
    public:
	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// Inflate to an already-open file handle.
#if defined(WIN32) | defined(WINCE)
	bool InflateToHandle(HANDLE handle);
#endif

	// BEGIN PUBLIC INTERFACE
	// UNZIPTOSTRING_BEGIN
	bool UnzipToString(int lineEndingBehavior, const char *srcCharset, CkString &outStr);
	const char *unzipToString(int lineEndingBehavior, const char *srcCharset);
	// UNZIPTOSTRING_END

	// REPLACESTRING_BEGIN
	bool ReplaceString(const char *inStr, const char *charset);
	// REPLACESTRING_END

	// APPENDSTRING_BEGIN
	bool AppendString(const char *inStr, const char *charset);
	// APPENDSTRING_END


	void get_FileDateTimeStr(CkString &str);
	const char *fileDateTimeStr(void);
	void put_FileDateTimeStr(const char *newVal);
	void SetDt(CkDateTime &dt);
	CkDateTime *GetDt(void);

	// This entry's filename.
	void get_FileName(CkString &str);
	void put_FileName(const char *pStr);

	// The uncompressed size in bytes.
	unsigned long get_UncompressedLength(void);

	// The compression level. 0 = no compression, while 9 = maximum compression. 
	// The default is 6.
	long get_CompressionLevel(void);
	void put_CompressionLevel(long newVal);

	// Set to 0 for no compression, or 8 for the Deflate algorithm. 
	// The Deflate algorithm is the default algorithm of the most popular Zip utility programs.
	long get_CompressionMethod(void);
	void put_CompressionMethod(long newVal);

	// The compressed size in bytes.
	unsigned long get_CompressedLength(void);

	// The comment associated with this file in the Zip.
	void get_Comment(CkString &str);
	void put_Comment(const char *pStr);

	// Indicates the origin of the entry. There are three possible values: 
	// (0) a file on disk, 
	// (1) a entry in an existing Zip file, and 
	// (2) an entry created directly from data in memory. 
	long get_EntryType(void);

	// The entry's file date/time.
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	void get_FileDateTime(SYSTEMTIME &outSysTime);
	void put_FileDateTime(const SYSTEMTIME &sysTime);

	// True if this entry is a directory.
	bool get_IsDirectory(void);

	// The unique ID assigned by Chilkat Zip while the object is instantiated in memory.
	long get_EntryID(void);

	// Unzips this entry into the specified base directory. 
	// The file is unzipped to the subdirectory according to the relative path 
	// stored with the entry in the Zip.
	bool Extract(const char *dirPath);

	// Unzips this entry into the specified directory, regardless of the 
	// relative directory of the entry.
	bool ExtractInto(const char *dirPath);

	// Inflates and returns the uncompressed data.
	bool Inflate(CkByteData &outData);

	// Replaces the entry's data.
	bool ReplaceData(const CkByteData &bdata);

	// Appends more data to the entry's data.
	bool AppendData(const CkByteData &bdata);

	// Returns the compressed data as a Base64-encoded string. 
	// This data can be re-added to a Zip by calling CkZip.AppendBase64.
	bool CopyToBase64(CkString &outStr);

	// Returns the compressed data as a Hex-encoded string. 
	// This data can be re-added to a Zip by calling CkZip.AppendHex.
	bool CopyToHex(CkString &outStr);

	// Returns the raw compressed data. 
	bool Copy(CkByteData &outData);

	// Returns the next entry in the Zip.  The returned object must be deleted by
	// the application.
	CkZipEntry *NextEntry(void);

	const char *fileName(void);
	const char *comment(void);
	const char *copyToBase64(void);
	const char *copyToHex(void);

	int get_Crc(void);
	__int64 get_CompressedLength64(void);
	__int64 get_UncompressedLength64(void);

	// ZIPENTRY_INSERT_POINT

	// END PUBLIC INTERFACE


    // Everything below here is for internal use only.
    private:
	void *m_impl;

	// Don't allow assignment or copying these objects.
	CkZipEntry(const CkZipEntry &) { } 
	CkZipEntry &operator=(const CkZipEntry &) { return *this; }
	CkZipEntry(void *impl);


    public:

	// The following method(s) should not be called by an application.
	// They for internal use only.
	void inject(void *impl);

	CkZipEntry(); // the default constructor should NEVER be called...
		
	virtual ~CkZipEntry();


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif

