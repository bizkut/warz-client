// CkFileAccess.h: interface for the CkFileAccess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKFILEACCESS_H
#define _CKFILEACCESS_H




class CkByteData;
class CkDateTime;

#include "CkString.h"
#include "CkMultiByteBase.h"

#ifdef WIN32
#pragma warning( disable : 4068 )
#pragma unmanaged
#endif


/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkFileAccess
class CkFileAccess  : public CkMultiByteBase
{
    public:
	CkFileAccess();
	virtual ~CkFileAccess();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE


        bool ReadEntireTextFile(const char *path, const char *charset, CkString &outStrFileContents);

        const char *readEntireTextFile(const char *path, const char *charset);
	// CURRENTDIR_BEGIN
	void get_CurrentDir(CkString &str);
	const char *currentDir(void);
	// CURRENTDIR_END
	// ENDOFFILE_BEGIN
	bool get_EndOfFile(void);
	// ENDOFFILE_END
	// SETCURRENTDIR_BEGIN
	bool SetCurrentDir(const char *path);
	// SETCURRENTDIR_END
	// APPENDANSI_BEGIN
	bool AppendAnsi(const char *text);
	// APPENDANSI_END
	// APPENDTEXT_BEGIN
	bool AppendText(const char *text, const char *charset);
	// APPENDTEXT_END
	// APPENDUNICODEBOM_BEGIN
	bool AppendUnicodeBOM(void);
	// APPENDUNICODEBOM_END
	// APPENDUTF8BOM_BEGIN
	bool AppendUtf8BOM(void);
	// APPENDUTF8BOM_END
	// DIRAUTOCREATE_BEGIN
	bool DirAutoCreate(const char *path);
	// DIRAUTOCREATE_END
	// DIRCREATE_BEGIN
	bool DirCreate(const char *path);
	// DIRCREATE_END
	// DIRDELETE_BEGIN
	bool DirDelete(const char *path);
	// DIRDELETE_END
	// DIRENSUREEXISTS_BEGIN
	bool DirEnsureExists(const char *filePath);
	// DIRENSUREEXISTS_END
	// FILECLOSE_BEGIN
	void FileClose(void);
	// FILECLOSE_END
	// FILECOPY_BEGIN
	bool FileCopy(const char *existingPath, const char *newPath, bool failIfExists);
	// FILECOPY_END
	// FILEDELETE_BEGIN
	bool FileDelete(const char *path);
	// FILEDELETE_END
	// FILEEXISTS_BEGIN
	bool FileExists(const char *path);
	// FILEEXISTS_END
	// FILEOPEN_BEGIN
	bool FileOpen(const char *path, unsigned int accessMode, 
	    unsigned int shareMode, unsigned int createDisp, unsigned int attr);
	// FILEOPEN_END
	// FILEREAD_BEGIN
	bool FileRead(int numBytes, CkByteData &outBytes);
	// FILEREAD_END
	// FILERENAME_BEGIN
	bool FileRename(const char *existingPath, const char *newPath);
	// FILERENAME_END
	// FILESEEK_BEGIN
	bool FileSeek(int offset, int origin);
	// FILESEEK_END
	// FILESIZE_BEGIN
	int FileSize(const char *path);
	// FILESIZE_END
	// FILEWRITE_BEGIN
	bool FileWrite(const CkByteData &data);
	// FILEWRITE_END
	// GETTEMPFILENAME_BEGIN
	bool GetTempFilename(const char *dirName, const char *prefix, CkString &outStr);
	const char *getTempFilename(const char *dirName, const char *prefix);
	// GETTEMPFILENAME_END
	// READENTIREFILE_BEGIN
	bool ReadEntireFile(const char *path, CkByteData &outBytes);
	// READENTIREFILE_END
	// TREEDELETE_BEGIN
	bool TreeDelete(const char *path);
	// TREEDELETE_END
	// WRITEENTIREFILE_BEGIN
	bool WriteEntireFile(const char *path, const CkByteData &fileData);
	// WRITEENTIREFILE_END
	// WRITEENTIRETEXTFILE_BEGIN
	bool WriteEntireTextFile(const char *path, const char *fileData, const char *charset, bool includePreamble);
	// WRITEENTIRETEXTFILE_END
	// SPLITFILE_BEGIN
	bool SplitFile(const char *fileToSplit, const char *partPrefix, const char *partExtension, int partSize, const char *destDir);
	// SPLITFILE_END
	// REASSEMBLEFILE_BEGIN
	bool ReassembleFile(const char *partsDirPath, const char *partPrefix, const char *partExtension, const char *reassembledFilename);
	// REASSEMBLEFILE_END
	// READBINARYTOENCODED_BEGIN
	bool ReadBinaryToEncoded(const char *path, const char *encoding, CkString &outStr);
	const char *readBinaryToEncoded(const char *path, const char *encoding);
	// READBINARYTOENCODED_END
	// FILEOPENERROR_BEGIN
	int get_FileOpenError(void);
	// FILEOPENERROR_END
	// FILEOPENERRORMSG_BEGIN
	void get_FileOpenErrorMsg(CkString &str);
	const char *fileOpenErrorMsg(void);
	// FILEOPENERRORMSG_END
	// OPENFORREAD_BEGIN
	bool OpenForRead(const char *filePath);
	// OPENFORREAD_END
	// OPENFORWRITE_BEGIN
	bool OpenForWrite(const char *filePath);
	// OPENFORWRITE_END
	// OPENFORREADWRITE_BEGIN
	bool OpenForReadWrite(const char *filePath);
	// OPENFORREADWRITE_END
	// OPENFORAPPEND_BEGIN
	bool OpenForAppend(const char *filePath);
	// OPENFORAPPEND_END
	// REPLACESTRINGS_BEGIN
	int ReplaceStrings(const char *path, const char *charset, const char *existingString, const char *replacementString);
	// REPLACESTRINGS_END
	// SETFILETIMES_BEGIN
	bool SetFileTimes(const char *path, CkDateTime &create, CkDateTime &lastAccess, CkDateTime &lastModified);
	bool SetLastModified(const char *path, CkDateTime &lastModified);
	// SETFILETIMES_END

	// FILEACCESS_INSERT_POINT

	// END PUBLIC INTERFACE



    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkFileAccess(const CkFileAccess &) { } 
	CkFileAccess &operator=(const CkFileAccess &) { return *this; }
	CkFileAccess(void *impl);



};

#ifndef __sun__
#pragma pack (pop)
#endif


#endif


