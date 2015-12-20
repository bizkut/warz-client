// CkSFtpFile.h: interface for the CkSFtpFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkSFtpFile_H
#define _CkSFtpFile_H



#ifndef WIN32
#include "int64.h"
#endif

#include "CkString.h"
#include "CkMultiByteBase.h"

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

#ifndef __sun__
#pragma pack (push, 8)
#endif
 
class CkDateTime;

// CLASS: CkSFtpFile
class CkSFtpFile  : public CkMultiByteBase
{
    private:

	// Don't allow assignment or copying these objects.
	CkSFtpFile(const CkSFtpFile &);
	CkSFtpFile &operator=(const CkSFtpFile &);


    public:
	CkSFtpFile(void *impl);

	CkSFtpFile();
	virtual ~CkSFtpFile();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// LASTMODIFIEDTIMESTR_BEGIN
	void get_LastModifiedTimeStr(CkString &str);
	const char *lastModifiedTimeStr(void);
	// LASTMODIFIEDTIMESTR_END

	// LASTACCESSTIMESTR_BEGIN
	void get_LastAccessTimeStr(CkString &str);
	const char *lastAccessTimeStr(void);
	// LASTACCESSTIMESTR_END

	// CREATETIMESTR_BEGIN
	void get_CreateTimeStr(CkString &str);
	const char *createTimeStr(void);
	// CREATETIMESTR_END

	// GETLASTACCESSDT_BEGIN
	CkDateTime *GetLastAccessDt(void);
	// GETLASTACCESSDT_END

	// GETLASTMODIFIEDDT_BEGIN
	CkDateTime *GetLastModifiedDt(void);
	// GETLASTMODIFIEDDT_END

	// GETCREATEDT_BEGIN
	CkDateTime *GetCreateDt(void);
	// GETCREATEDT_END


		// FILENAME_BEGIN
	void get_Filename(CkString &str);
	const char *filename(void);
	// FILENAME_END
	// OWNER_BEGIN
	void get_Owner(CkString &str);
	const char *owner(void);
	// OWNER_END
	// GROUP_BEGIN
	void get_Group(CkString &str);
	const char *group(void);
	// GROUP_END
	// FILETYPE_BEGIN
	void get_FileType(CkString &str);
	const char *fileType(void);
	// FILETYPE_END
	// ISREGULAR_BEGIN
	bool get_IsRegular(void);
	// ISREGULAR_END
	// ISDIRECTORY_BEGIN
	bool get_IsDirectory(void);
	// ISDIRECTORY_END
	// ISSYMLINK_BEGIN
	bool get_IsSymLink(void);
	// ISSYMLINK_END
	// ISREADONLY_BEGIN
	bool get_IsReadOnly(void);
	// ISREADONLY_END
	// ISSYSTEM_BEGIN
	bool get_IsSystem(void);
	// ISSYSTEM_END
	// ISHIDDEN_BEGIN
	bool get_IsHidden(void);
	// ISHIDDEN_END
	// ISCASEINSENSITIVE_BEGIN
	bool get_IsCaseInsensitive(void);
	// ISCASEINSENSITIVE_END
	// ISARCHIVE_BEGIN
	bool get_IsArchive(void);
	// ISARCHIVE_END
	// ISENCRYPTED_BEGIN
	bool get_IsEncrypted(void);
	// ISENCRYPTED_END
	// ISCOMPRESSED_BEGIN
	bool get_IsCompressed(void);
	// ISCOMPRESSED_END
	// ISSPARSE_BEGIN
	bool get_IsSparse(void);
	// ISSPARSE_END
	// ISAPPENDONLY_BEGIN
	bool get_IsAppendOnly(void);
	// ISAPPENDONLY_END
	// ISIMMUTABLE_BEGIN
	bool get_IsImmutable(void);
	// ISIMMUTABLE_END
	// ISSYNC_BEGIN
	bool get_IsSync(void);
	// ISSYNC_END
	// PERMISSIONS_BEGIN
	int get_Permissions(void);
	// PERMISSIONS_END
	// SIZE32_BEGIN
	int get_Size32(void);
	// SIZE32_END
	// SIZE64_BEGIN
	__int64 get_Size64(void);
	// SIZE64_END

	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"

	void get_LastModifiedTime(SYSTEMTIME &outSysTime);
	void get_LastAccessTime(SYSTEMTIME &outSysTime);
	void get_CreateTime(SYSTEMTIME &outSysTime);

	// SIZESTR_BEGIN
	void get_SizeStr(CkString &str);
	const char *sizeStr(void);
	// SIZESTR_END
	// UID_BEGIN
	int get_Uid(void);
	// UID_END
	// GID_BEGIN
	int get_Gid(void);
	// GID_END

	// SFTPFILE_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


